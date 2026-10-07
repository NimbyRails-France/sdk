"""Offline Docker-state simulation; never talks to a Docker daemon."""
import copy
import importlib.util
import json
import os
from pathlib import Path
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('publisher_upgrade', Path(__file__).with_name('upgrade.py'))
upgrade = importlib.util.module_from_spec(spec)
spec.loader.exec_module(upgrade)
OLD = 'sha256:' + 'a' * 64
NEW = 'sha256:' + 'b' * 64
OTHER = 'sha256:' + 'c' * 64


class Upgrade(unittest.TestCase):
    def setUp(self):
        self.tags = {upgrade.APPROVED: OLD}
        self.receipts = []
        self.calls = []
        self.old_hash_valid = True
        self.build_failure = self.test_failure = self.smoke_failure = False
        self.concurrent_before_promotion = self.concurrent_after_promotion = False
        manifest = upgrade.checked_bundle()
        self.expected = {'/opt/nrf/' + path.removeprefix('image/'): details['sha256'] for path, details in manifest['files'].items() if path.startswith('image/')}
        self.env = {'CI_REPO': 'NimbyRails-France/sdk', 'CI_COMMIT_BRANCH': upgrade.BRANCH,
                    'CI_PIPELINE_EVENT': 'push', 'CI_COMMIT_SHA': 'd' * 40}

    def docker(self, *args, **kwargs):
        self.calls.append(args)
        if args[:2] == ('image', 'inspect'):
            return self.tags.get(args[-1])
        if args[0] == 'build':
            if self.build_failure:
                raise RuntimeError('simulated build failure')
            self.tags[upgrade.CANDIDATE] = NEW
            return
        if args[:2] == ('image', 'tag'):
            self.tags[args[3]] = args[2]
            return
        if args[0] == 'run' and '-c' in args:
            image = args[args.index('-I') - 1]
            values = dict(upgrade.LEGACY) if image == OLD else dict(self.expected)
            if image == OLD and not self.old_hash_valid:
                values['/opt/nrf/publish.py'] = '0' * 64
            return json.dumps(values)
        if args[0] == 'run' and args[-1] == '/opt/nrf/test_publisher.py':
            if self.test_failure:
                raise RuntimeError('simulated candidate test failure')
            if self.concurrent_before_promotion:
                self.tags[upgrade.APPROVED] = OTHER
            return
        if args[0] == 'run':
            if self.concurrent_after_promotion:
                self.tags[upgrade.APPROVED] = OTHER
            return 'wrong output' if self.smoke_failure else 'Ordinary commit: publication skipped.'
        raise AssertionError('Unexpected Docker call')

    def invoke(self):
        with patch.dict(os.environ, self.env, clear=True), patch.object(upgrade, 'docker', side_effect=self.docker), \
                patch.object(upgrade, 'receipt', side_effect=lambda item: self.receipts.append(copy.deepcopy(item))):
            upgrade.run_upgrade()

    def test_success_backs_up_before_promoting_and_records_ids(self):
        self.invoke()
        self.assertEqual(NEW, self.tags[upgrade.APPROVED])
        self.assertEqual(OLD, self.tags[upgrade.BACKUP])
        changes = [call for call in self.calls if call[:2] == ('image', 'tag')]
        self.assertEqual([('image', 'tag', OLD, upgrade.BACKUP), ('image', 'tag', NEW, upgrade.APPROVED)], changes)
        self.assertEqual('success', self.receipts[-1]['status'])
        self.assertEqual(NEW, self.receipts[-1]['promotedImageId'])
        self.assertEqual('d' * 40, self.receipts[-1]['maintenanceCommit'])

    def test_existing_matching_backup_is_preserved(self):
        self.tags[upgrade.BACKUP] = OLD
        self.invoke()
        self.assertEqual(OLD, self.tags[upgrade.BACKUP])
        self.assertNotIn(('image', 'tag', OLD, upgrade.BACKUP), self.calls)

    def test_divergent_backup_refuses_build_and_promotion(self):
        self.tags[upgrade.BACKUP] = OTHER
        with self.assertRaisesRegex(ValueError, 'Backup tag belongs'):
            self.invoke()
        self.assertEqual(OTHER, self.tags[upgrade.BACKUP])
        self.assertEqual(OLD, self.tags[upgrade.APPROVED])
        self.assertFalse(any(call[0] == 'build' for call in self.calls))

    def test_unrecognized_old_code_is_not_replaced(self):
        self.old_hash_valid = False
        with self.assertRaisesRegex(ValueError, 'legacy sources'):
            self.invoke()
        self.assertEqual(OLD, self.tags[upgrade.APPROVED])
        self.assertFalse(any(call[0] == 'build' for call in self.calls))

    def test_failed_build_never_promotes(self):
        self.build_failure = True
        with self.assertRaisesRegex(RuntimeError, 'build failure'):
            self.invoke()
        self.assertEqual(OLD, self.tags[upgrade.APPROVED])

    def test_failed_candidate_tests_never_promote(self):
        self.test_failure = True
        with self.assertRaisesRegex(RuntimeError, 'candidate test failure'):
            self.invoke()
        self.assertEqual(OLD, self.tags[upgrade.APPROVED])
        self.assertNotIn(upgrade.BACKUP, self.tags)

    def test_concurrent_tag_change_prevents_promotion(self):
        self.concurrent_before_promotion = True
        with self.assertRaisesRegex(ValueError, 'changed concurrently'):
            self.invoke()
        self.assertEqual(OTHER, self.tags[upgrade.APPROVED])
        self.assertFalse(any(call[:2] == ('image', 'tag') for call in self.calls))

    def test_failed_smoke_restores_old_image_and_keeps_backup(self):
        self.smoke_failure = True
        with self.assertRaisesRegex(ValueError, 'smoke check failed'):
            self.invoke()
        self.assertEqual(OLD, self.tags[upgrade.APPROVED])
        self.assertEqual(OLD, self.tags[upgrade.BACKUP])
        self.assertEqual('restored-original-image', self.receipts[-1]['rollback'])

    def test_rollback_never_overwrites_another_concurrent_tag(self):
        self.smoke_failure = self.concurrent_after_promotion = True
        with self.assertRaisesRegex(ValueError, 'smoke check failed'):
            self.invoke()
        self.assertEqual(OTHER, self.tags[upgrade.APPROVED])
        self.assertEqual('refused-to-overwrite-concurrent-tag-change', self.receipts[-1]['rollback'])

    def test_wrong_branch_refused_before_docker(self):
        self.env['CI_COMMIT_BRANCH'] = 'alpha'
        with self.assertRaisesRegex(ValueError, 'dedicated SDK maintenance'):
            self.invoke()
        self.assertEqual([], self.calls)


if __name__ == '__main__':
    unittest.main()
