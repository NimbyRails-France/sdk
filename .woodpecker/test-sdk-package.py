"""Exercise the CI SDK packager with inert files; no build, game or publication."""
import contextlib
import hashlib
import io
import json
import os
from pathlib import Path
import runpy
import shutil
import tempfile
import unittest
from unittest.mock import patch
import zipfile


PACKAGER = Path(__file__).with_name('package.py').resolve()
KOTLIN_KIT = Path(__file__).with_name('kotlin-kit.py').resolve()
HOST = 'NimbyRailsFranceModHost.exe'
VERSION = '0.9.0-alpha.1'


class SdkPackageTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix='nrf-sdk-package-test-')
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.payloads = {}
        files = [
            HOST, 'NimbyRailsFranceSDK.dll', 'NimbyRailsFranceTextureBridge-experimental-v4.dll',
            'NimbySignalUiBridge-experimental-v1.dll', 'NimbyAutomaticDrivingBridge-v1.dll',
            'NimbyRailsFranceClockBridge-0.7.1.dll', 'NimbyModMetadataBridge-v1.dll',
            'NimbyConstructionBridge-experimental-v1.dll',
        ]
        for name in files:
            payload = ('inert CI test fixture: ' + name).encode('utf-8')
            self.payloads[name] = payload
            self.write('build/ci/' + name, payload)
        self.write('build/ci/drop-in/SDL3.dll', b'inert proxy fixture')
        self.write('build/kotlin-kit/sdk.json', json.dumps({'sdkVersion': VERSION}))
        self.write('third_party/windows/minhook/LICENSE.txt', 'MinHook test license')
        self.write('docs/install-drop-in.md', 'SDK test documentation')
        self.write('tools/windows/install-proxy.ps1', '$PSScriptRoot/../../build/Release/drop-in')
        self.write('release-channels.json', json.dumps({'developmentStatus': 'in-development'}))
        self.write('.release-plan.json', json.dumps({'version': VERSION, 'channel': 'alpha', 'assets': []}))

    def write(self, relative, value):
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(value if isinstance(value, bytes) else value.encode('utf-8'))
        return path

    def package(self):
        # Only the external CMake installation and fixed Linux toolchain paths
        # are substituted. The actual packager stages, archives and hashes files.
        copy2 = shutil.copy2

        def install(args, **kwargs):
            self.assertEqual(['cmake', '--install', 'build/ci', '--prefix'], args[:4])
            self.assertTrue(kwargs.get('check'))
            stage = Path(args[4])
            (stage / 'bin').mkdir(parents=True)
            for name in [HOST, 'NimbyRailsFranceSDK.dll']:
                source = self.root / 'build/ci' / name
                if source.exists():
                    copy2(source, stage / 'bin' / name)

        def toolchain_copy(source, destination, *args, **kwargs):
            source = Path(source)
            if source.as_posix().startswith(('/opt/mingw/bin/', '/usr/share/')):
                source = self.write('external-fixtures/' + source.name, 'inert toolchain fixture: ' + source.name)
            return copy2(source, destination, *args, **kwargs)

        previous = Path.cwd()
        try:
            os.chdir(self.root)
            with patch.dict(os.environ, {'CI_REPO': 'NimbyRails-France/sdk'}), \
                    patch('subprocess.run', side_effect=install), \
                    patch('shutil.copy2', side_effect=toolchain_copy), \
                    contextlib.redirect_stdout(io.StringIO()):
                runpy.run_path(str(PACKAGER), run_name='__main__')
        finally:
            os.chdir(previous)

    def test_host_is_distributed_in_drop_in_and_hub_loader(self):
        self.package()
        output = self.root / 'dist/release'
        folder = 'NimbyRailsFranceSDK-' + VERSION
        expected = self.payloads[HOST]
        for archive, member in [
            (folder + '-drop-in-windows-x64.zip', folder + '-drop-in/' + HOST),
            (folder + '-hub.zip', folder + '/loader/' + HOST),
        ]:
            with self.subTest(archive=archive), zipfile.ZipFile(output / archive) as content:
                self.assertEqual(expected, content.read(member))
                self.assertIsNone(content.testzip())
        manifest = json.loads((output / 'project.json').read_text(encoding='utf-8'))
        self.assertEqual('in-development', manifest['developmentStatus'])
        self.assertEqual('alpha', manifest['channel'])
        self.assertEqual(manifest, json.loads((output / 'project-windows-x64.json').read_text(encoding='utf-8')))
        archive = output / manifest['url'].rsplit('/', 1)[1]
        self.assertEqual(archive.stat().st_size, manifest['size'])
        self.assertEqual(hashlib.sha256(archive.read_bytes()).hexdigest(), manifest['sha256'])
        plan = json.loads((self.root / '.release-plan.json').read_text(encoding='utf-8'))
        for asset in plan['assets']:
            path = output / asset['name']
            self.assertEqual(path.stat().st_size, asset['size'])
            self.assertEqual(hashlib.sha256(path.read_bytes()).hexdigest(), asset['sha256'])

    def test_missing_host_fails_before_a_hub_package_or_manifest_can_be_published(self):
        (self.root / 'build/ci' / HOST).unlink()
        with self.assertRaises(FileNotFoundError):
            self.package()
        output = self.root / 'dist/release'
        self.assertFalse((output / ('NimbyRailsFranceSDK-' + VERSION + '-hub.zip')).exists())
        self.assertFalse((output / 'project.json').exists())
        self.assertFalse((output / 'project-windows-x64.json').exists())
        plan = json.loads((self.root / '.release-plan.json').read_text(encoding='utf-8'))
        self.assertEqual([], plan['assets'])


class KotlinKitConsumerTests(unittest.TestCase):
    def test_consumer_range_tracks_the_kit_minor_version(self):
        for version, maximum in [('0.9.0-alpha.1', '0.10.0'), ('0.10.0-beta.2', '0.11.0')]:
            with self.subTest(version=version), tempfile.TemporaryDirectory(prefix='nrf-kotlin-kit-test-') as directory:
                root = Path(directory)
                for relative, text in {
                    'VERSION': version,
                    'kotlin/src/nimby/Api.kt': 'package nimby',
                    'kotlin/native/Exports.kt': '// inert exports fixture',
                    'kotlin/native/Package.kt': '// inert package fixture',
                    'compiler/licenses/LICENSE.txt': 'test compiler license',
                    'third_party/nlohmann-json-LICENSE.MIT': 'test JSON license',
                    'docs/README.md': 'test documentation',
                    'README.md': 'test SDK',
                    'verification/packaged-mod/src/main/kotlin/Entry.kt': '// inert consumer fixture',
                }.items():
                    path = root / relative
                    path.parent.mkdir(parents=True, exist_ok=True)
                    path.write_text(text, encoding='utf-8')
                calls = []
                previous = Path.cwd()
                try:
                    os.chdir(root)
                    with patch.dict(os.environ, {'NRF_KOTLIN_HOME': str(root / 'compiler')}), \
                            patch('subprocess.run', side_effect=lambda args, **kwargs: calls.append(args)):
                        runpy.run_path(str(KOTLIN_KIT), run_name='__main__')
                finally:
                    os.chdir(previous)
                manifest = json.loads((root / 'build/ci-consumer/mod.json').read_text(encoding='utf-8'))
                kit = json.loads((root / 'build/kotlin-kit/sdk.json').read_text(encoding='utf-8'))
                self.assertEqual(version, kit['sdkVersion'])
                self.assertEqual(version, kit['gradlePluginVersion'])
                self.assertEqual(version, manifest['sdkMin'])
                self.assertEqual(maximum, manifest['sdkMaxExclusive'])
                self.assertEqual(4, len(calls))
                self.assertIn('packageMod', calls[-1])
                self.assertIn('-PnrfSdkDir=' + str(root / 'build/kotlin-kit'), calls[-1])
                with zipfile.ZipFile(root / 'build/kotlin-kit/sources/nimby-mod-api-sources.jar') as sources:
                    self.assertEqual(b'package nimby', sources.read('nimby/Api.kt'))


if __name__ == '__main__':
    unittest.main()
