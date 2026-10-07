"""Offline contract tests for the image-owned publisher; no network access."""
import base64
import copy
import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import runpy
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

STATIC = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('image_publisher', STATIC / 'publish.py')
plugin = importlib.util.module_from_spec(spec)
spec.loader.exec_module(plugin)


class SourceApi:
    def __init__(self, fixture):
        self.fixture = fixture
        self.calls = []
        self.tag = None

    def request(self, method, path, value=None):
        self.calls.append((method, path, value))
        if path.startswith('/commits/'):
            return {'sha': self.fixture.commit, 'commit': {'message': self.fixture.remote_message}}
        if path.startswith('/git/ref/heads/'):
            return {'object': {'type': 'commit', 'sha': self.fixture.head, 'url': 'ignored'}}
        if path.startswith('/git/ref/tags/'):
            return self.tag
        if method == 'POST' and path == '/git/refs':
            self.tag = {'ref': value['ref'], 'object': {'type': 'commit', 'sha': value['sha']}}
            return self.tag
        raise AssertionError('Unexpected source request')

    def source(self, name, commit):
        assert commit == self.fixture.commit
        return self.fixture.remote[name]


class ReleaseApi:
    def __init__(self, repository):
        self.repository = repository
        self.releases = []
        self.calls = []

    def request(self, method, path, data=None, asset=None, name=None):
        self.calls.append((method, path, name))
        if path.startswith('/releases/tags/'):
            match = next((r for r in self.releases if r['tag_name'] == path.split('/')[-1] and not r['draft']), None)
            if match is None:
                raise plugin.publisher.ApiError(404)
            return copy.deepcopy(match)
        if path.startswith('/releases?'):
            return copy.deepcopy(self.releases)
        if method == 'POST' and path == '/releases':
            item = dict(data, id=len(self.releases) + 1, assets=[], published_at=None)
            self.releases.append(item)
            return copy.deepcopy(item)
        if path.startswith('/releases/assets/'):
            for release in self.releases:
                release['assets'] = [a for a in release['assets'] if a['id'] != int(path.split('/')[-1])]
            return None
        release = next(r for r in self.releases if r['id'] == int(path.split('/')[2]))
        if method == 'GET':
            return copy.deepcopy(release['assets'])
        if method == 'PATCH':
            release.update(data)
            if not release['draft']:
                release['published_at'] = '2026-10-08T00:00:00Z'
            return copy.deepcopy(release)
        item = {'name': name, 'id': 1000 * release['id'] + len(release['assets']), 'state': 'uploaded',
                'size': Path(asset).stat().st_size, 'digest': 'sha256:' + plugin.publisher.digest(asset),
                'browser_download_url': f'https://github.com/{self.repository}/releases/download/{release["tag_name"]}/{name}'}
        release['assets'].append(item)
        return copy.deepcopy(item)


class Publication(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.source = self.root / 'dist/release'
        self.source.mkdir(parents=True)
        (self.source / 'mod.zip').write_bytes(b'verified Windows fixture')
        self.commit = self.head = 'a' * 40
        self.version = '0.9.0-alpha.1'
        self.notes = 'Customer-facing release notes, reviewed before publication.'
        self.remote_message = 'release ' + self.version
        self.env = {'CI_REPO': 'NimbyRails-France/sdk', 'CI_COMMIT_SHA': self.commit, 'CI_COMMIT_BRANCH': 'alpha',
                    'CI_COMMIT_MESSAGE': self.remote_message, 'CI_PIPELINE_EVENT': 'push', 'PLUGIN_TOKEN': 'fixture-token'}
        self.remote = {'VERSION': self.version + '\n', 'release-channels.json': json.dumps({'channels': ['alpha'], 'branches': {'alpha': 'alpha'}}),
                       'CHANGELOG.md': f'# Changelog\n\n## [{self.version}] - 2026-10-08\n\n{self.notes}\n'}
        self.plan = {'publish': True, 'version': self.version, 'channel': 'alpha', 'branch': 'alpha', 'commit': self.commit, 'notes': self.notes,
                     'assets': [{'name': 'mod.zip', 'size': (self.source / 'mod.zip').stat().st_size, 'sha256': plugin.publisher.digest(self.source / 'mod.zip')}]}
        self.write_plan()
        self.source_api = SourceApi(self)
        self.api = ReleaseApi(self.env['CI_REPO'])

    def write_plan(self):
        (self.root / '.release-plan.json').write_text(json.dumps(self.plan), encoding='utf-8')

    def invoke(self, **kwargs):
        with patch('urllib.request.OpenerDirector.open', side_effect=AssertionError('Network is forbidden in unit tests')):
            plugin.main(self.env, self.root, source_factory=lambda repo, token: self.source_api,
                        release_factory=lambda repo, token: self.api, **kwargs)

    def assert_no_mutation(self):
        self.assertTrue(all(method == 'GET' for method, _, _ in self.source_api.calls))
        self.assertTrue(all(method == 'GET' for method, _, _ in self.api.calls))

    def test_six_projects_and_all_historical_aliases_publish_catalogues(self):
        for project, current in plugin.identities.REPOSITORIES.items():
            for slug in {project, current}:
                with self.subTest(repository=slug):
                    self.env['CI_REPO'] = 'NimbyRails-France/' + slug
                    self.source_api = SourceApi(self)
                    self.api = ReleaseApi(self.env['CI_REPO'])
                    self.invoke()
                    self.assertEqual(['v' + self.version, 'catalogue'], [r['tag_name'] for r in self.api.releases])
                    self.assertTrue(all(not r['draft'] for r in self.api.releases))
                    catalog = json.loads((self.root / 'dist/github-releases.json').read_text())
                    self.assertEqual(project, catalog['project'])
                    self.assertEqual(f'https://github.com/NimbyRails-France/{project}/releases/download/v{self.version}/mod.zip', catalog['releases'][0]['assets'][0]['browser_download_url'])

    def test_check_validates_without_tag_or_release_mutation(self):
        self.env['PLUGIN_MODE'] = 'check'
        self.invoke()
        self.assertTrue(self.api.calls)
        self.assert_no_mutation()

    def test_ordinary_commit_never_reads_plan_or_uses_api(self):
        self.env['CI_COMMIT_MESSAGE'] = 'Improve customer-facing documentation'
        (self.root / '.release-plan.json').unlink()
        self.invoke()
        self.assertEqual([], self.source_api.calls + self.api.calls)

    def test_unapproved_event_does_not_publish(self):
        self.env['CI_PIPELINE_EVENT'] = 'pull_request'
        self.invoke()
        self.assertEqual([], self.source_api.calls + self.api.calls)

    def test_github_runner_skips_without_api(self):
        self.env['CI_COMMIT_MESSAGE'] += '\n\nRelease-Runner: github'
        self.invoke()
        self.assertEqual([], self.source_api.calls + self.api.calls)

    def test_github_remote_runner_cannot_be_removed_from_ci_message(self):
        self.remote_message += '\n\nRelease-Runner: github'
        self.invoke()
        self.assert_no_mutation()
        self.assertEqual([], self.api.calls)

    def test_remote_commit_request_mismatch_rejected(self):
        self.remote_message = 'Ordinary change'
        with self.assertRaisesRegex(ValueError, 'GitHub commit'):
            self.invoke()
        self.assert_no_mutation()

    def test_newer_branch_head_rejected(self):
        self.head = 'b' * 40
        with self.assertRaisesRegex(ValueError, 'current branch head'):
            self.invoke()
        self.assert_no_mutation()

    def test_remote_version_mismatch_rejected(self):
        self.remote['VERSION'] = '0.8.0-alpha.8'
        with self.assertRaisesRegex(ValueError, 'Version differs'):
            self.invoke()
        self.assert_no_mutation()

    def test_branch_channel_mismatch_rejected(self):
        self.remote['release-channels.json'] = json.dumps({'channels': ['alpha'], 'branches': {'alpha': 'stable'}})
        with self.assertRaisesRegex(ValueError, 'Branch/channel'):
            self.invoke()
        self.assert_no_mutation()

    def test_remote_notes_mismatch_rejected(self):
        self.remote['CHANGELOG.md'] += 'Additional unbuilt changes.\n'
        with self.assertRaisesRegex(ValueError, 'Build plan'):
            self.invoke()
        self.assert_no_mutation()

    def test_undated_remote_notes_rejected(self):
        self.remote['CHANGELOG.md'] = self.remote['CHANGELOG.md'].replace(' - 2026-10-08', '')
        with self.assertRaises(ValueError):
            self.invoke()
        self.assert_no_mutation()

    def test_local_commit_or_branch_mismatch_rejected(self):
        for change in ({'commit': 'b' * 40}, {'branch': 'main'}, {'publish': 'true'}):
            with self.subTest(change=change):
                original = copy.deepcopy(self.plan)
                self.plan.update(change)
                self.write_plan()
                with self.assertRaisesRegex(ValueError, 'Build plan'):
                    self.invoke()
                self.assert_no_mutation()
                self.plan = original

    def test_existing_tag_on_other_commit_rejected(self):
        self.source_api.tag = {'object': {'type': 'commit', 'sha': 'b' * 40}}
        with self.assertRaisesRegex(ValueError, 'another commit'):
            self.invoke()
        self.assert_no_mutation()

    def test_annotated_tag_is_rejected_like_legacy(self):
        self.source_api.tag = {'object': {'type': 'tag', 'sha': self.commit}}
        with self.assertRaisesRegex(ValueError, 'another commit'):
            self.invoke()
        self.assert_no_mutation()

    def test_modified_artifact_rejected_before_any_write(self):
        (self.source / 'mod.zip').write_bytes(b'changed')
        with self.assertRaisesRegex(ValueError, 'artifact changed'):
            self.invoke()
        self.assert_no_mutation()

    def test_catalogue_output_symlink_rejected_before_any_write(self):
        catalogue = self.root / 'dist/github-releases.json'
        with patch.object(Path, 'is_symlink', autospec=True, side_effect=lambda path: path == catalogue):
            with self.assertRaisesRegex(ValueError, 'Catalogue output'):
                self.invoke()
        self.assert_no_mutation()

    def test_only_plugin_token_is_accepted(self):
        self.env['PLUGIN_TOKEN'] = ''
        self.env['NRF_RELEASE_TOKEN'] = 'must-not-be-used'
        with self.assertRaisesRegex(ValueError, 'credential is missing'):
            plugin.main(self.env, self.root)
        self.assert_no_mutation()

    def test_retry_regenerates_catalogue_without_reuploading_binary(self):
        self.invoke()
        self.invoke()
        self.assertEqual(1, sum(name == 'mod.zip' for _, _, name in self.api.calls))
        self.assertEqual(2, sum(name == 'releases.json' for _, _, name in self.api.calls))

    def test_check_also_rejects_existing_release_from_another_build(self):
        self.invoke()
        self.source_api.calls.clear()
        self.api.calls.clear()
        self.api.releases[0]['target_commitish'] = 'b' * 40
        self.env['PLUGIN_MODE'] = 'check'
        with self.assertRaisesRegex(ValueError, 'another build'):
            self.invoke()
        self.assert_no_mutation()

    def test_unknown_repository_and_mode_are_rejected(self):
        self.env['CI_REPO'] = 'OtherOrg/sdk'
        with self.assertRaisesRegex(ValueError, 'Unsupported repository'):
            self.invoke()
        self.env['CI_REPO'] = 'NimbyRails-France/sdk'
        self.env['PLUGIN_MODE'] = 'chekc'
        with self.assertRaisesRegex(ValueError, 'plugin mode'):
            self.invoke()
        self.assertEqual([], self.source_api.calls + self.api.calls)

    def test_bot_and_site_use_exact_legacy_entrypoint(self):
        for repo in plugin.LEGACY_REPOSITORIES:
            self.env['CI_REPO'] = repo
            calls = []
            self.invoke(legacy_runner=lambda path, run_name: calls.append((path, run_name)))
            self.assertEqual([(str(STATIC / 'legacy/publish.py'), '__main__')], calls)
        self.assertEqual([], self.source_api.calls + self.api.calls)

    def test_legacy_check_still_validates_original_contract(self):
        self.env.update(CI_REPO='NimbyRails-France/website', PLUGIN_MODE='check')
        calls = []
        def api(path, method='GET', value=None, raw=None):
            calls.append((method, path))
            if '/commits/' in path:
                return {'commit': {'message': self.remote_message}}
            if '/git/ref/heads/' in path:
                return {'object': {'sha': self.commit}}
            if '/contents/' in path:
                name = path.split('/contents/')[1].split('?')[0]
                return {'content': base64.b64encode(self.remote[name].encode()).decode()}
            if '/git/ref/tags/' in path or '/releases/tags/' in path:
                return None
            raise AssertionError('Unexpected legacy API request')
        cwd = Path.cwd()
        try:
            os.chdir(self.root)
            with patch.dict(os.environ, self.env, clear=True):
                legacy = runpy.run_path(str(STATIC / 'legacy/publish.py'), run_name='offline_legacy_test')
                legacy['main'].__globals__['api'] = api
                legacy['main']()
        finally:
            os.chdir(cwd)
        self.assertTrue(calls)
        self.assertTrue(all(method == 'GET' for method, _ in calls))

    def test_image_does_not_import_workspace_or_pythonpath_code(self):
        for name in ('project_identities.py', 'github_release.py', 'release-policy.py', 'sitecustomize.py'):
            (self.root / name).write_text('raise RuntimeError("UNTRUSTED WORKSPACE CODE")\n')
        env = dict(os.environ, **self.env, PYTHONPATH=str(self.root))
        env['CI_COMMIT_MESSAGE'] = 'Ordinary commit'
        result = subprocess.run([sys.executable, '-I', str(STATIC / 'publish.py')], cwd=self.root, env=env, capture_output=True, text=True, timeout=10)
        self.assertEqual(0, result.returncode, result.stderr)
        self.assertIn('publication skipped', result.stdout)
        self.assertNotIn('fixture-token', result.stdout + result.stderr)


class Transport(unittest.TestCase):
    def test_credentials_only_reach_fixed_hosts_and_redirects_are_disabled(self):
        source = plugin.SourceGitHub('NimbyRails-France/sdk', 'fixture-token')
        release = plugin.publisher.GitHub('NimbyRails-France/sdk', 'fixture-token')
        urls = []
        def opened(request, timeout):
            urls.append(request.full_url)
            self.assertEqual('Bearer fixture-token', request.get_header('Authorization'))
            return io.BytesIO(b'{}')
        with patch.object(source.opener, 'open', side_effect=opened), patch.object(release.opener, 'open', side_effect=opened):
            source.request('GET', '/commits/' + 'a' * 40)
            release.request('GET', '/releases?per_page=100')
            with tempfile.TemporaryDirectory() as directory:
                asset = Path(directory) / 'fixture.zip'
                asset.write_bytes(b'fixture')
                release.request('POST', '/releases/1/assets', asset=asset, name='fixture.zip')
            for invalid in ('https://attacker.invalid/', '//attacker.invalid/', '/contents/../secret'):
                with self.assertRaises(ValueError):
                    source.request('GET', invalid)
        self.assertEqual(['https://api.github.com/repos/NimbyRails-France/sdk/commits/' + 'a' * 40,
                          'https://api.github.com/repos/NimbyRails-France/sdk/releases?per_page=100',
                          'https://uploads.github.com/repos/NimbyRails-France/sdk/releases/1/assets?name=fixture.zip'], urls)
        self.assertIsNone(plugin.publisher.NoRedirect().redirect_request(None, None, None, None, None, 'https://attacker.invalid/'))

    def test_legacy_sources_are_byte_identical(self):
        for name, expected in {'publish.py': '5927e07b59cc6e2221d56f2db1dc822303aae27bab3334948ee06b12e494ee92',
                               'release-policy.py': '7a5ebbac2352c79b8f71a159438104605c4b06153b43516114fc9bc79fe79e08'}.items():
            self.assertEqual(expected, hashlib.sha256((STATIC / 'legacy' / name).read_bytes()).hexdigest())


if __name__ == '__main__':
    unittest.main()
