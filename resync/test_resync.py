"""Small offline rejection tests; no real server or network writes."""
import copy
import hashlib
import json
from pathlib import Path
import tempfile
import types
import unittest
from unittest.mock import patch

import resync


class ResyncTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(dir=resync.ROOT)
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.patcher = patch.object(resync, 'ROOT', self.root)
        self.patcher.start()
        self.addCleanup(self.patcher.stop)

    def release(self, manifest_url=None, checksum_override=False):
        release = {'project': 'signal-placement', 'repository': 'ba-signal-placement', 'tag': 'v0.1.0-alpha.3',
                   'notes': 'Historical notes\nuntouched.', 'publishedAt': '2026-09-28T21:49:23Z'}
        folder = resync.release_path(release)
        folder.mkdir(parents=True, exist_ok=True)
        archive = b'fixture bytes, never installed'
        digest = hashlib.sha256(archive).hexdigest()
        url = manifest_url or 'https://github.com/NimbyRails-France/ba-signal-placement/releases/download/v0.1.0-alpha.3/package.zip'
        data = {'id': 'signal-placement', 'version': '0.1.0-alpha.3', 'url': url, 'size': len(archive), 'sha256': digest, 'platform': 'windows-x64'}
        (folder / 'package.zip').write_bytes(archive)
        resync.write_json(folder / 'project.json', data)
        assets = [{'name': name, 'size': (folder / name).stat().st_size, 'sha256': resync.sha(folder / name)} for name in ('package.zip', 'project.json')]
        sums = ''.join(('0' * 64 if checksum_override and a['name'] == 'package.zip' else a['sha256']) + '  ' + a['name'] + '\n' for a in assets)
        (folder / 'SHA256SUMS.txt').write_bytes(sums.encode('utf-8'))
        assets.append({'name': 'SHA256SUMS.txt', 'size': len(sums.encode()), 'sha256': resync.sha(folder / 'SHA256SUMS.txt')})
        release['assets'] = assets
        return release

    def test_canonical_github_manifest(self):
        self.assertEqual(resync.verify_release(self.release())['assets'], 3)

    def test_historical_github_alias(self):
        self.assertEqual(resync.verify_release(self.release('https://github.com/NimbyRails-France/signal-placement/releases/download/v0.1.0-alpha.3/package.zip'))['manifests'], 'verified')

    def test_original_nrf_manifest(self):
        self.assertEqual(resync.verify_release(self.release('https://releases.nimbyrails-france.fr/releases/signal-placement/v0.1.0-alpha.3/package.zip'))['manifests'], 'verified')

    def test_cross_project_manifest_rejected(self):
        with self.assertRaisesRegex(AssertionError, 'another release'):
            resync.verify_release(self.release('https://releases.nimbyrails-france.fr/releases/time-change/v0.1.0-alpha.3/package.zip'))

    def test_wrong_version_manifest_url_rejected(self):
        with self.assertRaisesRegex(AssertionError, 'another release'):
            resync.verify_release(self.release('https://github.com/NimbyRails-France/ba-signal-placement/releases/download/v0.1.0-alpha.2/package.zip'))

    def test_independent_checksum_list_rejected(self):
        with self.assertRaisesRegex(AssertionError, 'SHA256SUMS'):
            resync.verify_release(self.release(checksum_override=True))

    def test_corrupted_asset_rejected(self):
        release = self.release()
        (resync.release_path(release) / 'package.zip').write_bytes(b'corruption')
        with self.assertRaisesRegex(AssertionError, 'Staged digest'):
            resync.verify_release(release)

    def test_path_traversal_rejected(self):
        release = self.release()
        asset = copy.deepcopy(release['assets'][0])
        asset['name'] = '../outside.zip'
        with self.assertRaises(AssertionError):
            resync.asset_path(release, asset)

    def test_uncatalogued_destination_rejected_before_publication(self):
        release = self.release()
        storage = self.root / 'server'
        (storage / 'public/v1').mkdir(parents=True)
        resync.write_json(storage / 'public/v1/catalog.json', {'schema': 1, 'projects': {}})
        (storage / 'public/releases/signal-placement/v0.1.0-alpha.3').mkdir(parents=True)
        with patch.object(resync.sys, 'platform', 'linux'), patch.object(resync, 'verify'):
            with self.assertRaisesRegex(AssertionError, 'already exists'):
                resync.apply({'releases': [release]}, storage)

    def test_catalogued_release_is_skipped(self):
        release = self.release()
        storage = self.root / 'server'
        (storage / 'public/v1').mkdir(parents=True)
        resync.write_json(storage / 'public/v1/catalog.json', {'schema': 1, 'projects': {'signal-placement': [{'tag_name': release['tag']}]}})
        calls = []
        fake = types.ModuleType('distribution')
        fake.publish = lambda *args: calls.append(args)
        with patch.object(resync.sys, 'platform', 'linux'), patch.object(resync, 'verify'), patch.dict(resync.sys.modules, {'distribution': fake}):
            resync.apply({'releases': [release]}, storage)
        self.assertEqual(calls, [])


if __name__ == '__main__':
    unittest.main()
