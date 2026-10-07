"""Prepare verified public GitHub assets, or explicitly apply them on Linux.

prepare, fetch and offline verification write only this staging directory. apply
requires --apply and uses the captured NRF publisher, never rebuilding assets.
No credentials are required or read by this utility.
"""
import argparse
import concurrent.futures
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import sys
import urllib.parse
import urllib.request

ROOT = Path(__file__).resolve().parent
ORIGIN = 'https://releases.nimbyrails-france.fr'
PROJECTS = {
    'sdk': 'sdk', 'hub': 'hub', 'tco': 'tco',
    'signalisationfrancaiserealiste': 'ab-signalisation-lumineuse',
    'signal-placement': 'ba-signal-placement', 'time-change': 'bb-timechange',
}
TAG = re.compile(r'v\d+\.\d+\.\d+(?:-(?:alpha|beta)\.[1-9]\d*)?')
NAME = re.compile(r'[A-Za-z0-9][A-Za-z0-9._-]{0,180}')
HEX = re.compile(r'[a-f0-9]{64}')


def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def write_json(path, value):
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')


def read_json(url):
    request = urllib.request.Request(url, headers={'User-Agent': 'NRF-public-release-resync', 'Accept': 'application/json'})
    with urllib.request.urlopen(request, timeout=30) as response:
        raw = response.read(8 * 1024 * 1024 + 1)
    assert len(raw) <= 8 * 1024 * 1024, 'Oversized JSON response'
    return json.loads(raw)


def release_path(release):
    assert release['project'] in PROJECTS and TAG.fullmatch(release['tag'])
    return ROOT / 'assets' / release['project'] / release['tag']


def asset_path(release, asset):
    name = asset['name']
    assert NAME.fullmatch(name) and name not in ('.', '..')
    assert isinstance(asset['size'], int) and 0 < asset['size'] < 1024 * 1024 * 1024
    assert HEX.fullmatch(asset['sha256'])
    return release_path(release) / name


def download(pair):
    release, asset = pair
    target = asset_path(release, asset)
    target.parent.mkdir(parents=True, exist_ok=True)
    if target.exists():
        assert target.is_file() and not target.is_symlink()
        assert target.stat().st_size == asset['size'] and sha(target) == asset['sha256'], 'Existing staged file differs: ' + str(target)
        return
    url = urllib.parse.urlsplit(asset['url'])
    expected = '/NimbyRails-France/' + release['repository'] + '/releases/download/' + release['tag'] + '/' + asset['name']
    assert url.scheme == 'https' and url.hostname == 'github.com' and url.path == expected and not url.query and not url.fragment
    temporary = target.with_name(target.name + '.part')
    digest = hashlib.sha256()
    size = 0
    try:
        request = urllib.request.Request(asset['url'], headers={'User-Agent': 'NRF-public-release-resync'})
        with urllib.request.urlopen(request, timeout=60) as response, temporary.open('wb') as output:
            final = urllib.parse.urlsplit(response.url)
            assert final.scheme == 'https' and final.hostname in ('github.com', 'release-assets.githubusercontent.com', 'objects.githubusercontent.com')
            while chunk := response.read(1024 * 1024):
                size += len(chunk)
                assert size <= asset['size'], 'Download exceeds declared size'
                digest.update(chunk)
                output.write(chunk)
        assert size == asset['size'] and digest.hexdigest() == asset['sha256'], 'GitHub digest mismatch: ' + str(target)
        os.replace(temporary, target)
    finally:
        if temporary.exists():
            temporary.unlink()


def verify_release(release):
    assets = release['assets']
    assert len({asset['name'] for asset in assets}) == len(assets)
    for asset in assets:
        target = asset_path(release, asset)
        assert target.is_file() and not target.is_symlink()
        assert target.stat().st_size == asset['size'] and sha(target) == asset['sha256'], 'Staged digest mismatch: ' + str(target)
    sums = {}
    for line in (release_path(release) / 'SHA256SUMS.txt').read_text(encoding='utf-8-sig').splitlines():
        match = re.fullmatch(r'([a-fA-F0-9]{64}) [ *](.+)', line)
        assert match and NAME.fullmatch(match[2]) and match[2] not in sums, 'Malformed checksum list'
        sums[match[2]] = match[1].lower()
    expected = {asset['name']: asset['sha256'] for asset in assets if asset['name'] != 'SHA256SUMS.txt'}
    assert sums == expected, 'SHA256SUMS does not cover exactly the published assets'
    manifests = [asset for asset in assets if asset['name'].startswith(('project', 'hub-latest')) and asset['name'].endswith('.json')]
    assert manifests, 'Missing install manifest'
    by_name = {asset['name']: asset for asset in assets}
    for asset in manifests:
        data = json.loads(asset_path(release, asset).read_text(encoding='utf-8-sig'))
        assert data['version'] == release['tag'][1:] and data.get('platform', 'windows-x64') == 'windows-x64'
        if 'id' in data:
            assert data['id'] == release['project'], 'Manifest project differs'
        url = urllib.parse.urlsplit(data['url'])
        aliases = {release['project'], release['repository']}
        name = url.path.rsplit('/', 1)[-1]
        assert name in by_name
        allowed = {f'https://github.com/NimbyRails-France/{repo}/releases/download/{release["tag"]}/{name}' for repo in aliases}
        # Existing official manifests may already target the offline NRF mirror.
        allowed.add(f'{ORIGIN}/releases/{release["project"]}/{release["tag"]}/{name}')
        assert data['url'] in allowed, 'Manifest asset URL belongs to another release'
        assert data['size'] == by_name[name]['size'] and data['sha256'].lower() == by_name[name]['sha256']
    return {'project': release['project'], 'tag': release['tag'], 'assets': len(assets), 'bytes': sum(asset['size'] for asset in assets), 'githubDigests': 'verified', 'sha256sums': 'verified', 'manifests': 'verified'}


def prepare(sdk):
    server = read_json(ORIGIN + '/v1/catalog.json')
    assert server['schema'] == 1
    write_json(ROOT / 'server-before.json', server)
    releases = []
    for project, repository in PROJECTS.items():
        existing = {release['tag_name'] for release in server['projects'].get(project, [])}
        published = []
        for page in range(1, 101):
            batch = read_json(f'https://api.github.com/repos/NimbyRails-France/{repository}/releases?per_page=100&page={page}')
            assert isinstance(batch, list)
            published.extend(batch)
            if len(batch) < 100:
                break
        else:
            raise ValueError('GitHub pagination limit reached')
        write_json(ROOT / ('github-' + project + '.json'), published)
        for release in published:
            if release['draft'] or not TAG.fullmatch(release['tag_name']) or release['tag_name'] in existing:
                continue
            assets = []
            for asset in release['assets']:
                assert asset['state'] == 'uploaded' and asset.get('digest', '').startswith('sha256:')
                assets.append({'name': asset['name'], 'size': asset['size'], 'sha256': asset['digest'][7:].lower(), 'url': asset['browser_download_url'], 'githubAssetId': asset['id']})
            releases.append({'project': project, 'repository': repository, 'tag': release['tag_name'], 'githubReleaseId': release['id'], 'publishedAt': release['published_at'], 'notes': release['body'] or '', 'assets': assets})
    releases.sort(key=lambda item: (item['publishedAt'], item['project'], item['tag']))
    publisher = ROOT / 'publisher'
    publisher.mkdir(exist_ok=True)
    for name in ('distribution.py', 'project_identities.py'):
        shutil.copyfile(sdk / '.woodpecker' / name, publisher / name)
    plan = {'schema': 1, 'preparedAt': datetime.datetime.now(datetime.timezone.utc).isoformat(), 'origin': ORIGIN, 'serverGeneratedAt': server.get('generatedAt'), 'releases': releases,
            'publisher': {name: sha(publisher / name) for name in ('distribution.py', 'project_identities.py')}}
    write_json(ROOT / 'plan.json', plan)
    fetch(plan)


def fetch(plan):
    releases = plan['releases']
    assert plan['schema'] == 1 and plan['origin'] == ORIGIN
    assert len({(r['project'], r['tag']) for r in releases}) == len(releases), 'Duplicate release'
    for release in releases:
        assert PROJECTS[release['project']] == release['repository'], 'Unexpected repository'
    print(f'Download plan: {len(releases)} releases, {sum(len(r["assets"]) for r in releases)} files, {sum(a["size"] for r in releases for a in r["assets"])} bytes', flush=True)
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        futures = [pool.submit(download, (release, asset)) for release in releases for asset in release['assets']]
        for index, future in enumerate(concurrent.futures.as_completed(futures), 1):
            future.result()
            if index % 10 == 0:
                print(f'Verified GitHub downloads: {index}/{len(futures)}', flush=True)
    verify(plan)


def verify(plan):
    assert plan['schema'] == 1 and plan['origin'] == ORIGIN
    for name, digest in plan['publisher'].items():
        assert name in ('distribution.py', 'project_identities.py') and sha(ROOT / 'publisher' / name) == digest
    reports = [verify_release(release) for release in plan['releases']]
    result = {'verifiedAt': datetime.datetime.now(datetime.timezone.utc).isoformat(), 'releases': reports, 'releaseCount': len(reports), 'assetCount': sum(r['assets'] for r in reports), 'bytes': sum(r['bytes'] for r in reports), 'planSha256': sha(ROOT / 'plan.json'), 'publisher': plan['publisher']}
    write_json(ROOT / 'verification.json', result)
    print(json.dumps({k: result[k] for k in ('releaseCount', 'assetCount', 'bytes', 'planSha256')}))


def apply(plan, storage):
    assert sys.platform == 'linux', 'Server publication requires Linux'
    verify(plan)
    catalogue = storage / 'public/v1/catalog.json'
    current = json.loads(catalogue.read_text(encoding='utf-8'))
    assert current['schema'] == 1
    pending = [r for r in plan['releases'] if not any(item['tag_name'] == r['tag'] for item in current['projects'].get(r['project'], []))]
    # A directory without a catalogue entry needs inspection, never replacement.
    for release in pending:
        target = storage / 'public/releases' / release['project'] / release['tag']
        assert not target.exists(), 'Uncatalogued destination already exists; inspect without overwriting: ' + str(target)
    sys.path.insert(0, str(ROOT / 'publisher'))
    from distribution import publish
    for release in pending:
        publish(storage, release['project'], release['tag'][1:], release_path(release), release['assets'], release['notes'], release['publishedAt'])
    print(f'Applied {len(pending)} previously missing releases; existing catalogue entries were skipped.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=('prepare', 'fetch', 'verify', 'apply'))
    parser.add_argument('--sdk', type=Path, default=ROOT.parents[1] / 'sdk')
    parser.add_argument('--storage', type=Path)
    parser.add_argument('--apply', action='store_true')
    parser.add_argument('--expected-plan-sha256')
    args = parser.parse_args()
    if args.command == 'prepare':
        prepare(args.sdk.resolve())
    else:
        if args.expected_plan_sha256:
            assert HEX.fullmatch(args.expected_plan_sha256) and sha(ROOT / 'plan.json') == args.expected_plan_sha256, 'Unexpected import plan'
        plan = json.loads((ROOT / 'plan.json').read_text(encoding='utf-8'))
        if args.command == 'fetch':
            fetch(plan)
        elif args.command == 'verify':
            verify(plan)
        else:
            assert args.apply and args.storage and args.storage.is_absolute(), 'apply requires --apply and an absolute --storage path'
            apply(plan, args.storage.resolve())
