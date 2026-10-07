"""Build the Windows Kotlin SDK and test a real consumer on Woodpecker."""
import json
import os
import pathlib
import shutil
import subprocess
import zipfile

ROOT = pathlib.Path.cwd()
KIT = ROOT / 'build/kotlin-kit'
VERSION = (ROOT / 'VERSION').read_text().strip()
major, minor, _ = VERSION.split('-', 1)[0].split('.')
SDK_MAX_EXCLUSIVE = f'{major}.{int(minor) + 1}.0'
KOTLIN = pathlib.Path(os.environ['NRF_KOTLIN_HOME'])


def run(*args):
    subprocess.run(list(map(str, args)), check=True)


def copy(source, dest):
    source, dest = pathlib.Path(source), pathlib.Path(dest)
    dest.parent.mkdir(parents=True, exist_ok=True)
    if source.is_dir():
        shutil.copytree(source, dest, dirs_exist_ok=True,
                        ignore=shutil.ignore_patterns('build', '.gradle', '.kotlin', '.idea'))
    else:
        shutil.copy2(source, dest)


for folder in ('klib', 'bridge', 'bin', 'sources', 'licenses'):
    (KIT / folder).mkdir(parents=True, exist_ok=True)
run(KOTLIN / 'bin/konanc', '-target', 'mingw_x64', '-produce', 'library',
    '-o', KIT / 'klib/nimby-mod-api', *sorted((ROOT / 'kotlin/src').rglob('*.kt')))
copy(ROOT / 'kotlin/native/Exports.kt', KIT / 'bridge/Exports.kt')
copy(ROOT / 'kotlin/native/Package.kt', KIT / 'bridge/Package.kt')
run('cmake', '-S', 'kotlin/native', '-B', 'build/ci-kotlin', '-G', 'Ninja',
    '-DCMAKE_TOOLCHAIN_FILE=/opt/nimby-ci/nimby-mingw.cmake', '-DCMAKE_BUILD_TYPE=Release',
    '-DNimbyRailsFranceSDK_DIR=' + str(ROOT / 'build/install/lib/cmake/NimbyRailsFranceSDK'),
    '-DNRF_MODULE_NAME=NimbyKotlinMod', '-DNRF_KOTLIN_LIBRARY=NimbyKotlinModKotlin.dll',
    '-DNRF_OUTPUT_DIRECTORY=' + str(KIT / 'bin'))
run('cmake', '--build', 'build/ci-kotlin', '--parallel', '2')
copy(KOTLIN / 'licenses', KIT / 'licenses/Kotlin-Native')
copy(ROOT / 'third_party/nlohmann-json-LICENSE.MIT', KIT / 'licenses/nlohmann-json/LICENSE.MIT')
copy(ROOT / 'docs', KIT / 'docs')
copy(ROOT / 'README.md', KIT / 'README.md')
with zipfile.ZipFile(KIT / 'sources/nimby-mod-api-sources.jar', 'w', zipfile.ZIP_DEFLATED) as archive:
    for path in sorted((ROOT / 'kotlin/src').rglob('*.kt')):
        archive.write(path, path.relative_to(ROOT / 'kotlin/src'))
(KIT / 'sdk.json').write_text(json.dumps(dict(format=1, sdkVersion=VERSION,
    kotlinVersion='2.2.20', gradlePluginVersion=VERSION, gradleVersion='8.14.3',
    target='mingw_x64', api='klib/nimby-mod-api.klib',
    gameSha256=['fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae']), indent=2) + '\n')
# A separate consumer directory keeps caches and build outputs out of the SDK ZIP.
consumer = ROOT / 'build/ci-consumer'
copy(ROOT / 'verification/packaged-mod', consumer)
(consumer / 'mod.json').write_text(json.dumps(dict(id='sdk-contract', name='SDK package contract', modId='SdkContract',
    module='SdkContractMod', language='kotlin-native', version='1.0.0', sdkMin=VERSION,
    sdkMaxExclusive=SDK_MAX_EXCLUSIVE, gameSha256=['fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae'])))
run('sh', ROOT / 'gradle-plugin/gradlew', '-p', consumer, 'packageMod',
    '-PnrfSdkDir=' + str(KIT), '-PnrfWineRunner=' + str(ROOT / '.woodpecker/wine-run.py'),
    '-Pkotlin.native.home=' + str(KOTLIN), '--no-daemon', '--max-workers=2', '--console=plain')
