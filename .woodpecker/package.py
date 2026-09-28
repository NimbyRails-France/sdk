"""Create distributable assets from the already-tested CI build on the VPS."""
import hashlib
import json
import os
import pathlib
import shutil
import subprocess
import tarfile
import tempfile
import zipfile

ROOT = pathlib.Path.cwd()
PLAN = json.loads((ROOT / '.release-plan.json').read_text(encoding='utf-8'))
VERSION = PLAN['version']
REPO = os.environ['CI_REPO'].split('/')[-1]
OUT = ROOT / 'dist/release'
OUT.mkdir(parents=True, exist_ok=True)
if any(OUT.iterdir()):
    raise ValueError('Release output must be empty before packaging')
WORK = pathlib.Path(tempfile.mkdtemp(prefix='nrf-package-', dir=ROOT / 'build' if (ROOT / 'build').exists() else ROOT))
URL = 'https://releases.nimbyrails-france.fr/releases/' + REPO + '/v' + VERSION + '/'
GAME = ['fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae']
RUNTIME = pathlib.Path('/opt/mingw/bin')

def run(*args, **kwargs):
    subprocess.run([str(a) for a in args], check=True, **kwargs)

def copy(source, dest):
    source, dest = pathlib.Path(source), pathlib.Path(dest)
    dest.parent.mkdir(parents=True, exist_ok=True)
    if source.is_dir():
        shutil.copytree(source, dest, dirs_exist_ok=True)
    else:
        shutil.copy2(source, dest)

def write(name, data):
    (OUT / name).write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')

def digest(path):
    return hashlib.file_digest(path.open('rb'), 'sha256').hexdigest()

def metadata(path):
    return dict(url=URL + path.name, sha256=digest(path), size=path.stat().st_size, version=VERSION)

def archive(stage, name):
    target = OUT / name
    with zipfile.ZipFile(target, 'w', zipfile.ZIP_DEFLATED, compresslevel=9, strict_timestamps=False) as archive:
        for item in sorted(stage.rglob('*')):
            if item.is_file():
                if item.is_symlink():
                    raise ValueError('Symlinks are not distributable')
                archive.write(item, item.relative_to(stage.parent).as_posix())
    with zipfile.ZipFile(target) as archive:
        if archive.testzip():
            raise ValueError('Invalid package archive')
    return target

def runtimes(stage):
    for name in ('libgcc_s_seh-1.dll', 'libstdc++-6.dll', 'libwinpthread-1.dll'):
        copy(RUNTIME / name, stage / name)

def runtime_licenses(stage):
    copy('/usr/share/doc/gcc-mingw-w64-x86-64-posix/copyright', stage / 'licenses/MinGW/copyright.txt')
    copy('/usr/share/common-licenses/GPL-3', stage / 'licenses/MinGW/GPL-3.txt')
    copy('/usr/share/common-licenses/LGPL-2.1', stage / 'licenses/MinGW/LGPL-2.1.txt')

if REPO == 'sdk':
    folder = 'NimbyRailsFranceSDK-' + VERSION
    stage = WORK / folder
    run('cmake', '--install', 'build/ci', '--prefix', stage)
    runtime_licenses(stage)
    archive(stage, folder + '-windows-x64-mingw.zip')
    drop = WORK / (folder + '-drop-in')
    drop.mkdir()
    for name in ('SDL3.dll', 'NimbyRailsFranceSDK.dll', 'NimbyRailsFranceTextureBridge-experimental-v4.dll', 'NimbySignalUiBridge-experimental-v1.dll', 'NimbyAutomaticDrivingBridge-v1.dll', 'NimbyRailsFranceClockBridge-0.7.1.dll', 'NimbyModMetadataBridge-v1.dll'):
        source = ROOT / 'build/ci' / ('drop-in/' + name if name == 'SDL3.dll' else name)
        copy(source, drop / name)
    runtimes(drop)
    if PLAN['channel'] == 'alpha':
        copy(ROOT / 'build/ci/NimbyConstructionBridge-experimental-v1.dll', drop / 'NimbyConstructionBridge-experimental-v1.dll')
    runtime_licenses(drop)
    copy(ROOT / 'third_party/windows/minhook/LICENSE.txt', drop / 'licenses/MinHook.txt')
    copy(ROOT / 'docs/install-drop-in.md', drop / 'README.md')
    content = (ROOT / 'tools/windows/install-proxy.ps1').read_text(encoding='utf-8').replace('$PSScriptRoot/../../build/Release/drop-in', '$PSScriptRoot')
    (drop / 'install-proxy.ps1').write_text(content, encoding='utf-8')
    archive(drop, folder + '-drop-in-windows-x64.zip')
    copy(drop, stage / 'loader')
    asset = archive(stage, folder + '-hub.zip')
    write('project.json', dict(id='sdk', name='NimbyRailsFranceSDK + NRF Loader', kind='sdk', loaderApi=1,
        platform='windows-x64', channel=PLAN['channel'], rootFolder=folder, gameSha256=GAME, **metadata(asset)))
    copy(OUT / 'project.json', OUT / 'project-windows-x64.json')
    kit = ROOT / 'build/kotlin-kit'
    runtime_licenses(kit)
    # Kotlin consumers expect sdk.json at the ZIP root (NRF_KOTLIN_SDK).
    target = OUT / ('NimbyRailsFranceSDK-kotlin-' + VERSION + '-windows-x64.zip')
    # Upstream Kotlin license files use the Unix epoch. ZIP's earliest date
    # is 1980; clamp the metadata instead of dropping required license files.
    with zipfile.ZipFile(target, 'w', zipfile.ZIP_DEFLATED, compresslevel=9, strict_timestamps=False) as content:
        for item in sorted(kit.rglob('*')):
            if item.is_file():
                relative = item.relative_to(kit)
                if any(p in ('.gradle', '.kotlin', 'build', '.git') for p in relative.parts):
                    raise ValueError('Generated build files leaked into the SDK kit')
                content.write(item, relative.as_posix())
    with zipfile.ZipFile(target) as content:
        if content.testzip():
            raise ValueError('Invalid Kotlin SDK archive')
elif REPO == 'signalisationfrancaiserealiste':
    stage = WORK / ('SignalisationFrancaiseRealiste-' + VERSION)
    run('cmake', '--install', 'build/ci', '--prefix', stage)
    runtime_licenses(stage)
    asset = archive(stage, stage.name + '-windows-x64.zip')
    write('project.json', dict(id=REPO, name='Signalisation française réaliste', kind='native-mod',
        modId='SignalisationFrancaiseRealiste', loaderApi=1, sdkMin='0.7.2', sdkMaxExclusive='0.8.0',
        module='SignalisationFrancaiseRealisteMod.dll', rootFolder=stage.name, gameSha256=GAME, **metadata(asset)))
elif REPO in ('website', 'nimbyrailsfrance-bot'):
    files = subprocess.check_output(['git', 'ls-files', '-z']).decode('utf-8').split('\0')
    if REPO == 'website':
        if not (ROOT / 'public/build/manifest.json').exists():
            raise ValueError('Website frontend has not been built')
        files += [str(p.relative_to(ROOT)) for p in (ROOT / 'public/build').rglob('*') if p.is_file()]
    with tarfile.open(OUT / (REPO + '-' + VERSION + '.tar.gz'), 'w:gz') as archive:
        for name in sorted(set(files)):
            if not name:
                continue
            path = pathlib.Path(name)
            if path.is_symlink() or (path.name.startswith('.env') and path.name != '.env.example'):
                raise ValueError('Private environment files or symlinks must not be packaged')
            archive.add(path, arcname=REPO + '-' + VERSION + '/' + path.as_posix(), recursive=False)
else:
    raise ValueError('Unsupported project')

assets = sorted(p for p in OUT.iterdir() if p.is_file())
(OUT / 'SHA256SUMS.txt').write_text(''.join(digest(p) + '  ' + p.name + '\n' for p in assets), encoding='ascii')
PLAN['assets'] = [dict(name=p.name, size=p.stat().st_size, sha256=digest(p)) for p in sorted(OUT.iterdir())]
(ROOT / '.release-plan.json').write_text(json.dumps(PLAN, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print('Validated release packages: ' + ', '.join(a['name'] for a in PLAN['assets']))
