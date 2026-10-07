"""Package a Kotlin desktop application for Windows on the Woodpecker VPS.

Gradle compiles JVM code on Linux and resolves Windows Skiko explicitly. The
pinned Windows JDK supplies its official jpackage launcher and jlink runtime.
Only Windows artifacts enter dist/release; host test libraries never enter it.
The existing publisher owns GitHub credentials and uploads after this succeeds.
"""
import hashlib
import json
import os
import pathlib
import shutil
import subprocess
import sys
import zipfile

from toolchain import install
from release_metadata import development_metadata

ROOT = pathlib.Path.cwd()
PLAN = json.loads((ROOT / '.release-plan.json').read_text())
VERSION = PLAN['version']
# An explicit release-commit trailer can skip execution tests for one alpha.
# Packaging/integrity checks remain mandatory, and ordinary builds still test.
SKIP_TESTS = (PLAN['publish'] and PLAN['channel'] == 'alpha' and
              'Release-Validation: skip-tests' in os.environ.get('CI_COMMIT_MESSAGE', '').splitlines())
REPO = os.environ['CI_REPO'].split('/')[-1]
assert REPO in ('hub', 'tco')
PROJECT_METADATA = development_metadata(ROOT) if REPO == 'tco' else {}
PRODUCT = 'NRFHub' if REPO == 'hub' else 'NimbyTco'
MAIN = 'fr.nimby.hub.MainKt' if REPO == 'hub' else 'fr.nimby.tco.MainKt'
OUT = ROOT / 'dist/release'
OUT.mkdir(parents=True, exist_ok=True)
if any(OUT.iterdir()):
    raise ValueError('Release directory must be empty')
# Keep Windows tool outputs within Wine's own C: drive. Only verified artifacts
# are copied into the workspace consumed by the release publisher.
WINE_C = pathlib.Path(os.environ.get('WINEPREFIX', str(pathlib.Path.home() / '.wine'))).resolve() / 'drive_c'
WORK = WINE_C / 'nrf-packaging' / REPO
WORK.mkdir(parents=True, exist_ok=True)
INPUT = ROOT / 'build/ci-windows-runtime'
JDK = install('java-windows')


def win(path):
    path = pathlib.Path(path).resolve()
    if path.is_relative_to(WINE_C):
        return 'C:\\' + str(path.relative_to(WINE_C)).replace('/', '\\')
    return 'Z:' + str(path).replace('/', '\\')


def run(*args, **kwargs):
    subprocess.run(list(map(str, args)), check=True, **kwargs)


def wine(*args):
    run('xvfb-run', '-a', 'wine', *args, env=dict(os.environ, WINEDEBUG='-all'), timeout=300)


def hash_file(path):
    with path.open('rb') as source:
        return hashlib.file_digest(source, 'sha256').hexdigest()


def metadata(path):
    return dict(version=VERSION, platform='windows-x64', channel=PLAN['channel'],
                url=f'https://github.com/NimbyRails-France/{REPO}/releases/download/v{VERSION}/{path.name}',
                size=path.stat().st_size, sha256=hash_file(path))


def write(name, data):
    (OUT / name).write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')


if REPO == 'hub' and not SKIP_TESTS:
    test_root = ROOT / 'build/ci-windows-tests'
    if not (test_root / 'fr/nimby/hub/SdkPromotionTest.class').exists():
        raise ValueError('Windows transaction tests were not compiled')
    test_classpath = ';'.join((win(test_root), win(test_root) + r'\*', win(INPUT) + r'\*'))
    wine(win(JDK / 'bin/java.exe'), '-cp', test_classpath, 'org.junit.runner.JUnitCore',
         'fr.nimby.hub.SdkPromotionTest')

jars = sorted(INPUT.glob('*.jar'))
if not any('skiko-awt-runtime-windows-x64' in p.name for p in jars):
    raise ValueError('Windows Skiko runtime missing')
if any('skiko-awt-runtime-linux' in p.name or 'skiko-awt-runtime-macos' in p.name for p in jars):
    raise ValueError('Host graphics runtime leaked into the Windows package')
main_jar = [p for p in jars if any(x in p.name for x in ('nrf-hub-desktop-', 'nimby-tco-desktop-'))]
if len(main_jar) != 1:
    raise ValueError('Expected exactly one application JAR: ' + str([p.name for p in jars]))
# jlink reads only the verified Windows JDK. The resulting runtime contains
# java.desktop (Compose/AWT), HTTPS, JNA support and diagnostics, not a full JDK.
runtime = WORK / 'runtime'
wine(win(JDK / 'bin/jlink.exe'), '--add-modules',
     'java.base,java.desktop,java.logging,java.management,java.naming,java.net.http,java.sql,jdk.unsupported,jdk.crypto.ec',
     '--strip-debug', '--no-header-files', '--no-man-pages', '--output', win(runtime))
# jpackage's Files.isWritable preflight is not implemented correctly by the
# worker's Wine filesystem provider, even though actual writes succeed. Use
# the unchanged Windows launcher that jpackage itself copies from this pinned
# JDK. Its relative runtime/app layout and configuration are smoke-tested below.
# Upstream contract: jdk.jpackage.internal.WindowsAppImageBuilder.
stage = WORK / 'image' / PRODUCT
stage.mkdir(parents=True)
shutil.copytree(runtime, stage / 'runtime')
shutil.copytree(INPUT, stage / 'app')
with zipfile.ZipFile(JDK / 'jmods/jdk.jpackage.jmod') as module:
    launcher = module.read('classes/jdk/jpackage/internal/resources/jpackageapplauncherw.exe')
    if launcher[:2] != b'MZ':
        raise ValueError('Pinned JDK does not contain the expected Windows launcher')
    (stage / (PRODUCT + '.exe')).write_bytes(launcher)
config = stage / 'app' / (PRODUCT + '.cfg')
# Use explicit classpath entries as Compose's own jpackage task does. This also
# makes missing dependencies reviewable in the installed application.
content = '[Application]\napp.mainclass=' + MAIN + '\n'
content += '\n'.join('app.classpath=$APPDIR\\' + p.name for p in jars)
content += '\n[JavaOptions]\njava-options=-Djpackage.app-version=' + VERSION + '\n'
content += 'java-options=-Dcompose.application.resources.dir=$APPDIR\\resources\n'
content += 'java-options=-Djava.library.path=$APPDIR\n'
config.write_text(content, encoding='utf-8')
for name in ('README.md', 'THIRD_PARTY.md', 'LICENSE', 'LICENSE.txt'):
    if (ROOT / name).is_file():
        shutil.copy2(ROOT / name, stage / name)
if (ROOT / 'build/ci-dependency-notices').is_dir():
    shutil.copytree(ROOT / 'build/ci-dependency-notices', stage / 'licenses/dependencies')
shutil.copytree(JDK / 'legal', stage / 'licenses/Temurin')
# Exercises the packaged Windows JVM, classpath, graphics/JNA libraries and
# resource loading without connecting to the game or opening a window.
if not SKIP_TESTS:
    wine(win(stage / 'runtime/bin/java.exe'), '-cp', win(stage / 'app') + r'\*', MAIN, '--package-smoke-test')
    wine(win(stage / (PRODUCT + '.exe')), '--package-smoke-test')
else:
    print('Execution tests skipped by explicit alpha release request', flush=True)
if REPO == 'hub':
    wine('/opt/inno/ISCC.exe', '/Qp', '/DStage=' + win(stage), '/DOutput=' + win(OUT),
         '/DVersion=' + VERSION, '/DNativeVersion=' + VERSION.split('-')[0],
         win(ROOT / 'tools/windows/installer.iss'))
    installer = OUT / f'NRFHub-{VERSION}-windows-x64-Setup.exe'
    if not installer.is_file() or installer.read_bytes()[:2] != b'MZ':
        raise ValueError('Windows installer missing or invalid')
    manifest = dict(schema=1, product=PRODUCT, **metadata(installer))
    for name in ('hub-latest.json', 'hub-latest-windows-x64.json'):
        write(name, manifest)
else:
    root_folder = PRODUCT + '-' + VERSION
    archive = OUT / (root_folder + '-windows-x64.zip')
    with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED, compresslevel=9, strict_timestamps=False) as output:
        for path in sorted(stage.rglob('*')):
            if path.is_file():
                output.write(path, root_folder + '/' + path.relative_to(stage).as_posix())
    with zipfile.ZipFile(archive) as content:
        if content.testzip():
            raise ValueError('Corrupt TCO archive')
    manifest = dict(id='tco', kind='tco', name='Nimby TCO', rootFolder=root_folder,
                    sdkMin='0.9.0-alpha.1', sdkMaxExclusive='0.10.0',
                    gameSha256=['fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae'],
                    **PROJECT_METADATA, **metadata(archive))
    for name in ('project.json', 'project-windows-x64.json'):
        write(name, manifest)
assets = sorted(OUT.iterdir())
(OUT / 'SHA256SUMS.txt').write_text(''.join(hash_file(p) + '  ' + p.name + '\n' for p in assets))
PLAN['assets'] = [dict(name=p.name, size=p.stat().st_size, sha256=hash_file(p)) for p in sorted(OUT.iterdir())]
(ROOT / '.release-plan.json').write_text(json.dumps(PLAN, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print('Validated Windows assets: ' + ', '.join(a['name'] for a in PLAN['assets']))
