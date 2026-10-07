"""Hermetic deployment tests: isolated files and injected HTTP, no external network."""
import importlib.util
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
from urllib.parse import urlsplit

spec = importlib.util.spec_from_file_location("wiki_deploy", Path(__file__).with_name("deploy.py"))
deploy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(deploy)


class DeploymentTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="wiki-deploy-test-")
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.base = self.root / "live"
        self.storage = self.base / "storage"
        self.releases = self.storage / "releases"
        self.releases.mkdir(parents=True)
        self.source = self.root / "source"
        self.old = self.releases / "old"
        self.older = self.releases / "older"
        for root, label in [(self.source, "new"), (self.old, "old"), (self.older, "older")]:
            self.pages(root, label)
        os.symlink("releases/old", self.storage / "current", target_is_directory=True)
        self.marker = self.base / "Caddyfile"
        self.marker.write_text("must stay unchanged", encoding="utf-8")
        self.requests = []

    def pages(self, root, label):
        for name in set(deploy.CHECK_PAGES.values()) | {"404.html", "_nuxt/test.js"}:
            path = root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(label + " " + name, encoding="utf-8")

    def fetch(self, url, timeout, limit):
        route = urlsplit(url).path
        self.requests.append(route)
        self.assertGreater(timeout, 0)
        if route == "/healthz":
            return b"ok"
        self.assertIn(route, deploy.CHECK_PAGES)
        return (self.storage / "current" / deploy.CHECK_PAGES[route]).read_bytes()

    def run_deploy(self, fetch=None, **kwargs):
        return deploy.deploy(self.source, self.base, "new-release", fetch=fetch or self.fetch,
                             attempts=kwargs.pop("attempts", 1), delay=0, **kwargs)

    def assert_old(self, previous=None):
        self.assertEqual(os.readlink(self.storage / "current").replace("\\", "/"), "releases/old")
        self.assertEqual(self.marker.read_text(encoding="utf-8"), "must stay unchanged")
        if previous is None:
            self.assertFalse(os.path.lexists(self.storage / "previous"))
        else:
            self.assertEqual(os.readlink(self.storage / "previous").replace("\\", "/"), previous)

    def test_publish_exact_copy_checks_all_pages_and_keeps_previous(self):
        result = self.run_deploy()
        self.assertEqual(result["status"], "published")
        self.assertEqual(result["checkedPages"], 8)
        self.assertEqual(set(self.requests), set(deploy.CHECK_PAGES) | {"/healthz"})
        self.assertEqual(deploy.file_map(self.source), deploy.file_map(self.releases / "new-release"))
        self.assertEqual(os.readlink(self.storage / "previous").replace("\\", "/"), "releases/old")
        self.assertEqual(self.marker.read_text(encoding="utf-8"), "must stay unchanged")
        if os.name != "nt":
            self.assertEqual((self.releases / "new-release/index.html").stat().st_mode & 0o777, 0o644)

    def test_wrong_public_bytes_roll_back_and_check_previous_content(self):
        def wrong_new(url, timeout, limit):
            if "rollback-" not in url:
                return b"different bytes"
            return self.fetch(url, timeout, limit)
        with self.assertRaisesRegex(RuntimeError, "original pointers restored"):
            self.run_deploy(wrong_new)
        self.assert_old()
        self.assertEqual(set(self.requests), set(deploy.CHECK_PAGES) | {"/healthz"})

    def test_transport_failure_preserves_older_previous_pointer(self):
        os.symlink("releases/older", self.storage / "previous", target_is_directory=True)
        def unavailable_new(url, timeout, limit):
            if "rollback-" not in url:
                raise TimeoutError("simulated connection failure")
            return self.fetch(url, timeout, limit)
        with self.assertRaisesRegex(RuntimeError, "original pointers restored"):
            self.run_deploy(unavailable_new)
        self.assert_old("releases/older")

    def test_rollback_http_failure_still_restores_original_pointer(self):
        def unavailable(*_args):
            raise TimeoutError("simulated persistent outage")
        with self.assertRaisesRegex(RuntimeError, "pointer restored, but public rollback verification failed"):
            self.run_deploy(unavailable)
        self.assert_old()

    def test_transient_public_mismatch_is_retried(self):
        called = False
        def transient(url, timeout, limit):
            nonlocal called
            if not called:
                called = True
                return b"old"
            return self.fetch(url, timeout, limit)
        self.assertEqual(self.run_deploy(transient, attempts=2)["status"], "published")

    def test_source_symlink_is_rejected_before_switch(self):
        os.symlink(self.old / "index.html", self.source / "link.html")
        with self.assertRaisesRegex(ValueError, "contains a link"):
            self.run_deploy()
        self.assert_old()

    def test_current_pointer_cannot_escape_release_directory(self):
        (self.storage / "current").unlink()
        os.symlink(str(self.source), self.storage / "current", target_is_directory=True)
        with self.assertRaisesRegex(ValueError, "leaves the release directory"):
            self.run_deploy()
        self.assertEqual(deploy.file_map(self.source)["index.html"][1], len(b"new index.html"))

    def test_previous_release_symlink_is_rejected(self):
        os.symlink(str(self.old), self.releases / "redirect", target_is_directory=True)
        (self.storage / "current").unlink()
        os.symlink("releases/redirect", self.storage / "current", target_is_directory=True)
        with self.assertRaisesRegex(ValueError, "real directory"):
            self.run_deploy()

    def test_missing_required_english_page_is_rejected(self):
        (self.source / "en/version/0.8/index.html").unlink()
        with self.assertRaisesRegex(ValueError, "missing or empty"):
            self.run_deploy()
        self.assert_old()

    def test_existing_release_is_never_replaced(self):
        target = self.releases / "new-release"
        target.mkdir()
        (target / "marker").write_text("preserve", encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "already exists"):
            self.run_deploy()
        self.assertEqual((target / "marker").read_text(), "preserve")
        self.assert_old()

    def test_invalid_release_identifier_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "Invalid release identifier"):
            deploy.deploy(self.source, self.base, "../outside", fetch=self.fetch)
        self.assert_old()

    def test_lock_prevents_concurrent_deployment(self):
        with deploy.deploy_lock(self.base):
            with self.assertRaises(OSError):
                self.run_deploy()
        self.assert_old()

    def test_failed_atomic_switch_restores_previous_pointer(self):
        original = deploy.replace_link
        def fail_current(storage, name, target):
            if name == "current":
                raise OSError("simulated switch failure")
            return original(storage, name, target)
        with patch.object(deploy, "replace_link", side_effect=fail_current):
            with self.assertRaisesRegex(RuntimeError, "original pointers restored"):
                self.run_deploy()
        self.assert_old()

    def test_copy_corruption_is_rejected_before_switch(self):
        original = deploy.shutil.copytree
        def corrupted(source, destination, *args, **kwargs):
            result = original(source, destination, *args, **kwargs)
            if Path(source) == self.source:
                (Path(destination) / "index.html").write_text("corrupted", encoding="utf-8")
            return result
        with patch.object(deploy.shutil, "copytree", side_effect=corrupted):
            with self.assertRaisesRegex(ValueError, "differ"):
                self.run_deploy()
        self.assert_old()


if __name__ == "__main__":
    unittest.main(verbosity=2)
