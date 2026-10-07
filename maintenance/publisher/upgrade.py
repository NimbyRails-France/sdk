"""Build/test a static publisher, preserve the old image, then promote its tag.

Executed only by the reviewed maintenance pipeline. No credentials are read or
passed to Docker. Existing image metadata is inspected using its ID only.
"""
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parent
WORKSPACE = ROOT.parents[1]
APPROVED = 'univenanne-ci-nimby-publisher:20260918'
CANDIDATE = 'univenanne-ci-nimby-publisher:20261008'
BACKUP = 'univenanne-ci-nimby-publisher:20260918-before-20261008'
BRANCH = 'maintenance/server-publisher-upgrade-20261008'
SDK_SOURCE_COMMIT = '73fc575e28227efae17134c04358053df4ad3b4f'
LEGACY = {'/opt/nrf/publish.py': '5927e07b59cc6e2221d56f2db1dc822303aae27bab3334948ee06b12e494ee92',
          '/opt/nrf/release-policy.py': '7a5ebbac2352c79b8f71a159438104605c4b06153b43516114fc9bc79fe79e08'}
HASH_PROBE = '''import hashlib,json,sys
from pathlib import Path
print(json.dumps({name:hashlib.sha256(Path(name).read_bytes()).hexdigest() for name in json.loads(sys.argv[1])},sort_keys=True))
'''


def docker(*args, capture=False, timeout=600, missing=False):
    result = subprocess.run(['docker', *args], capture_output=capture, text=True, timeout=timeout)
    if result.returncode:
        if missing and 'No such image' in (result.stderr or ''):
            return None
        raise RuntimeError('Docker operation failed: ' + args[0] + ' (exit ' + str(result.returncode) + ')')
    return (result.stdout or '').strip() if capture else None


def image_id(image, optional=False):
    value = docker('image', 'inspect', '--format', '{{.Id}}', image, capture=True, missing=optional, timeout=30)
    if value is not None and not re.fullmatch('sha256:[a-f0-9]{64}', value):
        raise ValueError('Docker returned an invalid image ID')
    return value


def image_hashes(image, paths):
    value = docker('run', '--rm', '--network', 'none', '--read-only', '--cap-drop', 'ALL',
                   '--security-opt', 'no-new-privileges', '--entrypoint', 'python3', image,
                   '-I', '-B', '-c', HASH_PROBE, json.dumps(paths), capture=True, timeout=60)
    return json.loads(value)


def receipt(value):
    folder = WORKSPACE / '.ci'
    if folder.is_symlink():
        raise ValueError('Receipt directory must stay in the maintenance workspace')
    folder.mkdir(exist_ok=True)
    target = folder / 'publisher-upgrade.json'
    temporary = folder / 'publisher-upgrade.tmp'
    if target.is_symlink() or temporary.is_symlink():
        raise ValueError('Invalid receipt path')
    value['updatedAt'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    temporary.write_text(json.dumps(value, indent=2) + '\n', encoding='utf-8')
    temporary.replace(target)


def checked_bundle():
    manifest = json.loads((ROOT / 'bundle-manifest.json').read_text(encoding='utf-8'))
    for relative, expected in manifest['files'].items():
        path = ROOT / relative
        if not path.resolve().is_relative_to(ROOT.resolve()) or path.is_symlink() or not path.is_file():
            raise ValueError('Invalid bundle path')
        raw = path.read_bytes()
        if len(raw) != expected['bytes'] or hashlib.sha256(raw).hexdigest() != expected['sha256']:
            raise ValueError('Reviewed bundle changed: ' + relative)
    return manifest


def run_upgrade():
    commit = os.environ.get('CI_COMMIT_SHA', '')
    if (os.environ.get('CI_REPO') != 'NimbyRails-France/sdk' or os.environ.get('CI_COMMIT_BRANCH') != BRANCH or
            os.environ.get('CI_PIPELINE_EVENT') not in ('push', 'manual') or not re.fullmatch('[a-f0-9]{40}', commit)):
        raise ValueError('Use the dedicated SDK maintenance branch and pipeline')
    manifest = checked_bundle()
    state = {'schema': 1, 'status': 'checking', 'maintenanceCommit': commit, 'sdkSourceCommit': SDK_SOURCE_COMMIT,
             'approvedTag': APPROVED, 'candidateTag': CANDIDATE, 'backupTag': BACKUP,
             'bundleManifestSha256': hashlib.sha256((ROOT / 'bundle-manifest.json').read_bytes()).hexdigest(),
             'secretFilterChanged': False, 'credentialsUsed': False, 'legacySourceHashes': LEGACY}
    receipt(state)
    old = candidate = None
    promotion_attempted = False
    try:
        old = image_id(APPROVED)
        state['oldImageId'] = old
        if image_hashes(old, list(LEGACY)) != LEGACY:
            raise ValueError('Current approved image does not match the inspected legacy sources')
        backup = image_id(BACKUP, optional=True)
        if backup is not None and backup != old:
            raise ValueError('Backup tag belongs to another image; refusing to overwrite it')
        state['status'] = 'building-candidate'
        receipt(state)
        # The Dockerfile itself runs all 26 offline tests before producing an image.
        docker('build', '--pull=false', '--network', 'none', '-t', CANDIDATE, str(ROOT), timeout=1200)
        candidate = image_id(CANDIDATE)
        state['candidateImageId'] = candidate
        docker('run', '--rm', '--network', 'none', '--read-only', '--cap-drop', 'ALL',
               '--security-opt', 'no-new-privileges', '--tmpfs', '/tmp:rw,nosuid,size=64m',
               '--entrypoint', 'python3', candidate, '-I', '-B', '/opt/nrf/test_publisher.py', timeout=120)
        expected = {'/opt/nrf/' + path.removeprefix('image/'): details['sha256']
                    for path, details in manifest['files'].items() if path.startswith('image/')}
        if image_hashes(candidate, list(expected)) != expected:
            raise ValueError('Candidate image differs from the reviewed static sources')
        state.update(status='candidate-verified', offlineTests=26, candidateSourcesVerified=True, legacyByteIdentityVerified=True)
        receipt(state)
        if image_id(APPROVED) != old:
            raise ValueError('Approved tag changed concurrently; refusing promotion')
        backup = image_id(BACKUP, optional=True)
        if backup is not None and backup != old:
            raise ValueError('Backup tag changed concurrently; refusing to overwrite it')
        if backup is None:
            docker('image', 'tag', old, BACKUP, timeout=30)
        if image_id(BACKUP) != old:
            raise ValueError('Backup verification failed')
        state.update(status='backed-up', backupImageId=old)
        receipt(state)
        if image_id(APPROVED) != old:
            raise ValueError('Approved tag changed concurrently immediately before promotion')
        promotion_attempted = True
        docker('image', 'tag', candidate, APPROVED, timeout=30)
        if image_id(APPROVED) != candidate:
            raise ValueError('Promotion verification failed')
        # Default image entrypoint, ordinary commit, no token and no network.
        smoke = docker('run', '--rm', '--network', 'none', '--read-only', '--cap-drop', 'ALL',
                       '--security-opt', 'no-new-privileges',
                       '-e', 'CI_PIPELINE_EVENT=push', '-e', 'CI_COMMIT_MESSAGE=maintenance validation',
                       '-e', 'CI_REPO=NimbyRails-France/sdk', APPROVED, capture=True, timeout=60)
        if smoke != 'Ordinary commit: publication skipped.' or image_id(APPROVED) != candidate:
            raise ValueError('Promoted image ordinary-commit smoke check failed')
        state.update(status='success', smoke='ordinary-commit-without-token-or-network', promotedImageId=candidate)
        receipt(state)
        print(json.dumps(state, sort_keys=True))
    except Exception as failure:
        state.update(status='failed', error=str(failure))
        if promotion_attempted:
            try:
                current = image_id(APPROVED)
                if current == candidate:
                    docker('image', 'tag', old, APPROVED, timeout=30)
                    if image_id(APPROVED) != old:
                        raise ValueError('Rollback image ID verification failed')
                    state['rollback'] = 'restored-original-image'
                elif current == old:
                    state['rollback'] = 'original-image-already-active'
                else:
                    state['rollback'] = 'refused-to-overwrite-concurrent-tag-change'
            except Exception as rollback_error:
                state['rollback'] = 'failed: ' + str(rollback_error)
        receipt(state)
        print(json.dumps(state, sort_keys=True))
        raise


if __name__ == '__main__':
    run_upgrade()
