"""Package a Kotlin desktop application for Windows on the Woodpecker VPS.

Gradle compiles JVM code on Linux and resolves Windows Skiko explicitly. The
pinned Windows JDK creates the launcher/runtime with jpackage under Wine.
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

ROOT = pathlib.Path.cwd()
PLAN = json.loads((ROOT / '.release-plan.json').read_text())
VERSION = PLAN['version']
REPO = os.environ['CI_REPO'].split('/')[-1]
assert REPO in ('hub', 'tco')
PRODUCT = 'NRFHub' if REPO == 'hub' else 'NimbyTco'
MAIN = 'fr.nimby.hub.MainKt' if REPO == 'hub' else 'fr.nimby.tco.MainKt'
OUT = ROOT / 'dist/release'
OUT.mkdir(parents=True, exist_ok=True)
if any(OUT.iterdir()):
    raise ValueError('Release directory must be empty')
# Wine's Z: mapping traverses the container's bind mount, where Windows volume
# access queries can report a false read-only state. jpackage must work on its
# own C: drive, then only the verified artifacts are copied into the workspace.
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


if REPO == 'hub':
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
wine(win(JDK / 'bin/jpackage.exe'), '--type', 'app-image', '--name', PRODUCT,
     '--app-version', VERSION.split('-')[0], '--vendor', 'NimbyRails France',
     '--input', win(INPUT), '--dest', win(WORK / 'image'), '--runtime-image', win(runtime),
     '--main-jar', main_jar[0].name, '--main-class', MAIN)
stage = WORK / 'image' / PRODUCT
config = stage / 'app' / (PRODUCT + '.cfg')
content = config.read_text(encoding='utf-8')
# Use explicit classpath entries as Compose's own jpackage task does. This also
# makes missing dependencies reviewable in the installed application.
content = '\n'.join(line for line in content.splitlines() if not line.startswith('app.classpath='))
content = content.replace('[Application]', '[Application]\n' + '\n'.join('app.classpath=$APPDIR\\' + p.name for p in jars))
config.write_text(content + '\n', encoding='utf-8')
for name in ('README.md', 'THIRD_PARTY.md', 'LICENSE', 'LICENSE.txt'):
    if (ROOT / name).is_file():
        shutil.copy2(ROOT / name, stage / name)
if (ROOT / 'build/ci-dependency-notices').is_dir():
    shutil.copytree(ROOT / 'build/ci-dependency-notices', stage / 'licenses/dependencies')
shutil.copytree(JDK / 'legal', stage / 'licenses/Temurin')
# Exercises the packaged Windows JVM, classpath, graphics/JNA libraries and
# resource loading without connecting to the game or opening a window.
wine(win(stage / 'runtime/bin/java.exe'), '-cp', win(stage / 'app') + r'\*', MAIN, '--package-smoke-test')
wine(win(stage / (PRODUCT + '.exe')), '--package-smoke-test')
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
                    sdkMin='0.8.0-alpha.1', sdkMaxExclusive='0.9.0',
                    gameSha256=['fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae'],
                    **metadata(archive))
    for name in ('project.json', 'project-windows-x64.json'):
        write(name, manifest)
assets = sorted(OUT.iterdir())
(OUT / 'SHA256SUMS.txt').write_text(''.join(hash_file(p) + '  ' + p.name + '\n' for p in assets))
PLAN['assets'] = [dict(name=p.name, size=p.stat().st_size, sha256=hash_file(p)) for p in sorted(OUT.iterdir())]
(ROOT / '.release-plan.json').write_text(json.dumps(PLAN, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print('Validated Windows assets: ' + ', '.join(a['name'] for a in PLAN['assets']))
