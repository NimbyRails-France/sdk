"""Publish validated assets after all build/test/deploy steps succeed."""
import base64
import hashlib
import importlib.util
import json
import os
import pathlib
import re
import urllib.error
import urllib.parse
import urllib.request

spec = importlib.util.spec_from_file_location('policy', pathlib.Path(__file__).with_name('release-policy.py'))
policy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(policy)
TOKEN = os.environ.get('PLUGIN_TOKEN', '')
NAMES = {'hub': 'NimbyRails France Hub', 'tco': 'Nimby TCO', 'sdk': 'NimbyRails France SDK',
         'website': 'NimbyRails France — Site web', 'signalisationfrancaiserealiste': 'Signalisation française réaliste',
         'nimbyrailsfrance-bot': 'NimbyRails France — Notifications'}

def api(path, method='GET', value=None, raw=None):
    # Credentials only ever go to the two fixed GitHub API hosts; redirects disabled.
    url = path if path.startswith('https://') else 'https://api.github.com' + path
    if urllib.parse.urlparse(url).netloc not in ('api.github.com', 'uploads.github.com'):
        raise ValueError('Invalid GitHub API host')
    data = raw if raw is not None else json.dumps(value).encode() if value is not None else None
    class NoRedirect(urllib.request.HTTPRedirectHandler):
        def redirect_request(self, *args):
            return None
    req = urllib.request.Request(url, data=data, method=method, headers={
        'Authorization': 'Bearer ' + TOKEN, 'Accept': 'application/vnd.github+json',
        'User-Agent': 'NimbyRails-France-CI', 'Content-Type': 'application/octet-stream' if raw is not None else 'application/json'})
    try:
        with urllib.request.build_opener(NoRedirect).open(req, timeout=180) as response:
            body = response.read()
            return json.loads(body) if body else None
    except urllib.error.HTTPError as error:
        if error.code == 404 and method == 'GET':
            return None
        raise RuntimeError('GitHub ' + method + ' failed: HTTP ' + str(error.code)) from None

def main():
    event = os.environ.get('CI_PIPELINE_EVENT', '')
    request = policy.requested_version(os.environ.get('CI_COMMIT_MESSAGE', '')) if event in ('push', 'manual') else None
    if not request:
        print('Ordinary commit: publication skipped.')
        return
    repository = os.environ.get('CI_REPO', '')
    if repository not in ['NimbyRails-France/' + n for n in NAMES]:
        raise ValueError('Repository is not an enabled NimbyRails France project')
    if not TOKEN:
        raise ValueError('Publishing credential is missing')
    prefix = '/repos/' + repository
    sha = os.environ['CI_COMMIT_SHA']
    if not re.fullmatch('[a-f0-9]{40}', sha):
        raise ValueError('Invalid commit')
    branch = os.environ['CI_COMMIT_BRANCH']
    commit = api(prefix + '/commits/' + sha)
    if policy.requested_version(commit['commit']['message']) != request:
        raise ValueError('GitHub commit does not request this release')
    head = api(prefix + '/git/ref/heads/' + urllib.parse.quote(branch, safe=''))
    if not head or head['object']['sha'] != sha:
        raise ValueError('A newer commit exists; publish from the current branch head')
    def source(name):
        item = api(prefix + '/contents/' + name + '?ref=' + sha)
        return base64.b64decode(item['content']).decode('utf-8')
    if source('VERSION').strip() != request:
        raise ValueError('Version differs from GitHub source')
    channels = json.loads(source('release-channels.json'))
    channel = request.split('-')[1].split('.')[0] if '-' in request else 'stable'
    if channel not in channels['channels'] or channels['branches'].get(branch) != channel:
        raise ValueError('Branch/channel mismatch')
    notes = policy.notes_for(source('CHANGELOG.md'), request, True)
    plan = json.loads(pathlib.Path('.release-plan.json').read_text(encoding='utf-8'))
    if not plan['publish'] or plan['version'] != request or plan['commit'] != sha or plan['notes'] != notes:
        raise ValueError('Build plan does not match GitHub source')
    assets = plan['assets']
    if not assets or len(assets) != len({a['name'] for a in assets}):
        raise ValueError('Missing or duplicate release assets')
    for asset in assets:
        if not re.fullmatch(r'[A-Za-z0-9_.-]+', asset['name']):
            raise ValueError('Invalid asset name')
        path = pathlib.Path('dist/release') / asset['name']
        if path.is_symlink() or path.stat().st_size != asset['size'] or hashlib.sha256(path.read_bytes()).hexdigest() != asset['sha256']:
            raise ValueError('Asset validation failed: ' + asset['name'])
    tag = 'v' + request
    ref = api(prefix + '/git/ref/tags/' + tag)
    if ref and (ref['object']['type'] != 'commit' or ref['object']['sha'] != sha):
        raise ValueError('Version already belongs to another commit; choose a new version')
    release = api(prefix + '/releases/tags/' + tag)
    if release and not release['draft']:
        if not ref:
            raise ValueError('Published release has no matching tag')
        print('This commit is already published; existing release remains unchanged.')
        return
    if os.environ.get('PLUGIN_MODE') == 'check':
        print('Release version and artifacts validated before deployment.')
        return
    if not ref:
        api(prefix + '/git/refs', 'POST', {'ref': 'refs/tags/' + tag, 'sha': sha})
    if not release:
        # Include existing drafts: the tag endpoint may omit drafts depending on the API.
        release = next((r for r in api(prefix + '/releases?per_page=100') if r['tag_name'] == tag), None)
    if not release:
        release = api(prefix + '/releases', 'POST', {'tag_name': tag, 'target_commitish': sha,
            'name': NAMES[repository.split('/')[1]] + ' ' + request, 'body': notes,
            'draft': True, 'prerelease': channel != 'stable'})
    if not release['draft']:
        raise ValueError('Release is already published')
    expected = {a['name']: a for a in assets}
    for old in api(prefix + '/releases/' + str(release['id']) + '/assets?per_page=100'):
        if old['name'] not in expected or old.get('digest') != 'sha256:' + expected[old['name']]['sha256']:
            api(prefix + '/releases/assets/' + str(old['id']), 'DELETE')
        else:
            del expected[old['name']]
    for name in expected:
        content = (pathlib.Path('dist/release') / name).read_bytes()
        result = api('https://uploads.github.com/repos/' + repository + '/releases/' + str(release['id']) + '/assets?name=' + urllib.parse.quote(name), 'POST', raw=content)
        if result['size'] != len(content) or result.get('digest') != 'sha256:' + hashlib.sha256(content).hexdigest():
            raise ValueError('Uploaded artifact verification failed')
    uploaded = api(prefix + '/releases/' + str(release['id']) + '/assets?per_page=100')
    if {a['name'] for a in uploaded} != {a['name'] for a in assets}:
        raise ValueError('Release asset set is incomplete')
    api(prefix + '/releases/' + str(release['id']), 'PATCH', {'body': notes, 'draft': False,
        'prerelease': channel != 'stable', 'make_latest': 'true' if channel == 'stable' else 'false'})
    print('Published ' + repository + ' ' + tag + ' (' + channel + ').')

if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        # Never serialize HTTP bodies, request headers or environment values.
        print('Publication failed: ' + str(error).replace(TOKEN, '[redacted]') if TOKEN else 'Publication failed: ' + str(error))
        raise SystemExit(1)
