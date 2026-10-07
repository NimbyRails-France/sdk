"""Static Woodpecker plugin: validate GitHub source before publishing builds.

Only data is read from the CI workspace. All executable Python code lives in
this image and the entrypoint uses Python's isolated mode (-I).
"""
import base64
import importlib.util
import json
import os
from pathlib import Path
import re
import runpy
import sys
import urllib.error
import urllib.parse
import urllib.request

STATIC = Path(__file__).resolve().parent
LEGACY_REPOSITORIES = frozenset(('NimbyRails-France/website', 'NimbyRails-France/nimbyrailsfrance-bot'))


def embedded(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


# Explicit absolute paths prevent a workspace module with the same name from
# becoming credentialed code. The SDK publisher's only non-stdlib import is
# inserted explicitly, before loading that publisher.
identities = embedded('project_identities', STATIC / 'runtime/project_identities.py')
publisher = embedded('nrf_embedded_github_release', STATIC / 'runtime/github_release.py')
policy = embedded('nrf_embedded_release_policy', STATIC / 'legacy/release-policy.py')


def github_runner(message):
    return 'Release-Runner: github' in message.splitlines()


class SourceGitHub:
    """Fixed-host, bounded source reads and exact tag creation; no redirects."""
    def __init__(self, repository, token):
        identities.project_id(repository)
        if not token:
            raise ValueError('Publishing credential is missing')
        self.repository = repository
        self.token = token
        self.opener = urllib.request.build_opener(publisher.NoRedirect)

    def request(self, method, path, value=None):
        read = method == 'GET' and path.startswith(('/commits/', '/git/ref/heads/', '/contents/', '/git/ref/tags/'))
        tag = method == 'POST' and path == '/git/refs'
        if not (read or tag) or not re.fullmatch(r'/[A-Za-z0-9/%?=._-]+', path) or '..' in path:
            raise ValueError('Unexpected source API request')
        url = 'https://api.github.com/repos/' + self.repository + path
        headers = {'Authorization': 'Bearer ' + self.token, 'Accept': 'application/vnd.github+json',
                   'User-Agent': 'NRF-Static-Publisher', 'X-GitHub-Api-Version': '2026-03-10'}
        payload = json.dumps(value).encode('utf-8') if value is not None else None
        if payload is not None:
            headers['Content-Type'] = 'application/json'
        request = urllib.request.Request(url, data=payload, headers=headers, method=method)
        try:
            with self.opener.open(request, timeout=180) as response:
                raw = response.read(8 * 1024 * 1024 + 1)
                if len(raw) > 8 * 1024 * 1024:
                    raise ValueError('GitHub source response too large')
                return json.loads(raw) if raw else None
        except urllib.error.HTTPError as error:
            status = error.code
            error.close()
            if status == 404 and method == 'GET':
                return None
            raise publisher.ApiError(status) from None

    def source(self, name, commit):
        if name not in ('VERSION', 'release-channels.json', 'CHANGELOG.md') or not re.fullmatch('[a-f0-9]{40}', commit):
            raise ValueError('Unexpected source file')
        item = self.request('GET', '/contents/' + name + '?ref=' + commit)
        if not item or item.get('type') != 'file' or item.get('encoding') != 'base64' or not 0 <= item.get('size', -1) <= 2 * 1024 * 1024:
            raise ValueError('GitHub source file unavailable: ' + name)
        raw = base64.b64decode(''.join(item['content'].splitlines()), validate=True)
        if len(raw) != item['size']:
            raise ValueError('GitHub source file size mismatch')
        return raw.decode('utf-8')


def load_plan(workspace):
    plan_path = workspace / '.release-plan.json'
    source = workspace / 'dist/release'
    if plan_path.is_symlink() or not plan_path.is_file() or plan_path.stat().st_size > 4 * 1024 * 1024:
        raise ValueError('Missing or invalid build plan')
    if (workspace / 'dist').is_symlink() or source.is_symlink() or not source.is_dir():
        raise ValueError('Build artifacts must be an ordinary workspace directory')
    catalogue = workspace / 'dist/github-releases.json'
    if catalogue.is_symlink() or (catalogue.exists() and not catalogue.is_file()):
        raise ValueError('Catalogue output must stay in the workspace')
    return json.loads(plan_path.read_text(encoding='utf-8')), source


def validate_source(env, request, source_api, workspace):
    commit = env.get('CI_COMMIT_SHA', '')
    branch = env.get('CI_COMMIT_BRANCH', '')
    if not re.fullmatch('[a-f0-9]{40}', commit):
        raise ValueError('Invalid CI commit')
    if not re.fullmatch(r'[A-Za-z0-9_/-][A-Za-z0-9._/-]{0,127}', branch) or '..' in branch:
        raise ValueError('Invalid CI branch')
    remote = source_api.request('GET', '/commits/' + commit)
    if not remote or remote.get('sha') != commit or policy.requested_version(remote['commit']['message']) != request:
        raise ValueError('GitHub commit does not request this release')
    if github_runner(remote['commit']['message']):
        return None
    head = source_api.request('GET', '/git/ref/heads/' + urllib.parse.quote(branch, safe=''))
    if not head or head.get('object', {}).get('type') != 'commit' or head['object'].get('sha') != commit:
        raise ValueError('A newer commit exists; publish from the current branch head')
    if source_api.source('VERSION', commit).strip() != request:
        raise ValueError('Version differs from GitHub source')
    channels = json.loads(source_api.source('release-channels.json', commit))
    channel = request.split('-')[1].split('.')[0] if '-' in request else 'stable'
    if channel not in channels['channels'] or channels['branches'].get(branch) != channel:
        raise ValueError('Branch/channel mismatch')
    notes = policy.notes_for(source_api.source('CHANGELOG.md', commit), request, True)
    plan, source = load_plan(workspace)
    if (plan.get('publish') is not True or plan.get('version') != request or plan.get('commit') != commit or
            plan.get('notes') != notes or plan.get('channel') != channel or plan.get('branch') != branch):
        raise ValueError('Build plan does not match GitHub source')
    publisher.validate_inputs(plan, source, env['CI_REPO'], commit)
    tag = source_api.request('GET', '/git/ref/tags/v' + request)
    if tag and (tag['object']['type'] != 'commit' or tag['object']['sha'] != commit):
        raise ValueError('Version already belongs to another commit; choose a new version')
    return plan, source, tag


def validate_existing(api, plan, commit, tag):
    release = publisher.find_release(api, 'v' + plan['version'])
    if release is None:
        return
    if (release.get('target_commitish') != commit or release['prerelease'] != (plan['channel'] != 'stable') or
            release.get('body') != plan['notes']):
        raise ValueError('Release already belongs to another build; increment the version')
    if not release['draft'] and not tag:
        raise ValueError('Published release has no matching tag')
    expected = {asset['name']: asset for asset in plan['assets']}
    uploaded = publisher.assets_for(api, release)
    seen = set()
    for asset in uploaded:
        if asset['name'] not in expected or asset['name'] in seen:
            raise ValueError('Existing release assets do not match the build plan')
        publisher.verify_asset(asset, expected[asset['name']])
        seen.add(asset['name'])
    if not release['draft'] and seen != set(expected):
        raise ValueError('Published release is incomplete; do not modify it')


def main(env=None, workspace=None, source_factory=SourceGitHub, release_factory=publisher.GitHub, legacy_runner=runpy.run_path):
    env = os.environ if env is None else env
    message = env.get('CI_COMMIT_MESSAGE', '')
    event = env.get('CI_PIPELINE_EVENT', '')
    if github_runner(message):
        print('GitHub runner selected: publication skipped.')
        return
    request = policy.requested_version(message) if event in ('push', 'manual') else None
    if not request:
        print('Ordinary commit: publication skipped.')
        return
    repository = env.get('CI_REPO', '')
    if repository in LEGACY_REPOSITORIES:
        # The bot/site keep the inspected legacy implementation byte-for-byte.
        legacy_runner(str(STATIC / 'legacy/publish.py'), run_name='__main__')
        return
    identities.project_id(repository)
    mode = env.get('PLUGIN_MODE', '')
    if mode not in ('', 'publish', 'check'):
        raise ValueError('Unsupported plugin mode')
    token = env.get('PLUGIN_TOKEN', '')
    source_api = source_factory(repository, token)
    validated = validate_source(env, request, source_api, Path.cwd().resolve() if workspace is None else Path(workspace).resolve())
    if validated is None:
        print('GitHub source selects the GitHub runner: publication skipped.')
        return
    plan, source, tag = validated
    api = release_factory(repository, token)
    commit = env['CI_COMMIT_SHA']
    validate_existing(api, plan, commit, tag)
    if mode == 'check':
        print('Release source, version and artifacts validated; no publication performed.')
        return
    if not tag:
        result = source_api.request('POST', '/git/refs', {'ref': 'refs/tags/v' + request, 'sha': commit})
        if (not result or result.get('ref') != 'refs/tags/v' + request or result.get('object', {}).get('type') != 'commit' or
                result['object'].get('sha') != commit):
            raise ValueError('Created tag does not match the verified commit')
    publisher.publish(plan, source, api, commit)


if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        token = os.environ.get('PLUGIN_TOKEN', '')
        message = str(error).replace(token, '[redacted]') if token else str(error)
        print('Publication failed: ' + message)
        raise SystemExit(1)
