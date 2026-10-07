"""Offline tests for optional author metadata; no build or publication."""
import json
from pathlib import Path
import tempfile
import unittest

from release_metadata import development_metadata


class DevelopmentMetadataTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)

    def write(self, value):
        (self.root / 'release-channels.json').write_text(json.dumps(value), encoding='utf-8')

    def test_missing_file_and_absent_status_remain_compatible(self):
        self.assertEqual({}, development_metadata(self.root))
        self.write(dict(channels=['alpha', 'beta', 'stable']))
        self.assertEqual({}, development_metadata(self.root))

    def test_status_is_explicit_and_independent_of_channel(self):
        for status in ('stable', 'in-development'):
            for channel in ('alpha', 'beta', 'stable'):
                with self.subTest(status=status, channel=channel):
                    self.write(dict(developmentStatus=status, channels=[channel], default=channel))
                    self.assertEqual(dict(developmentStatus=status), development_metadata(self.root))

    def test_invalid_author_values_fail_instead_of_becoming_badges(self):
        for status in (None, True, 1, '', 'Stable', 'alpha', 'development', ['stable'], dict(value='stable')):
            with self.subTest(status=status):
                self.write(dict(developmentStatus=status))
                with self.assertRaisesRegex(ValueError, 'developmentStatus'):
                    development_metadata(self.root)

    def test_non_object_policy_fails(self):
        for policy in (None, ['stable'], 'stable', True):
            with self.subTest(policy=policy):
                self.write(policy)
                with self.assertRaisesRegex(ValueError, 'JSON object'):
                    development_metadata(self.root)


if __name__ == '__main__':
    unittest.main()
