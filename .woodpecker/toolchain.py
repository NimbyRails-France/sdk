"""Pinned CI compilers. Downloads happen on the VPS, never on a developer PC.

The lock contains upstream SHA-256 values reviewed with each toolchain update.
An existing compiler is accepted only after our verified extraction marker.
These Linux host tools produce Windows artifacts; they are not Linux releases.
"""
import hashlib
import json
import pathlib
import shutil
import sys
import tarfile
import urllib.request
import zipfile

LOCK = json.loads(pathlib.Path(__file__).with_name('toolchains.json').read_text())


def install(name):
    item = LOCK[name]
    root = pathlib.Path('.ci/toolchains').resolve() / name
    target = root / item['directory']
    marker = root / 'verified.sha256'
    if not marker.exists() or marker.read_text() != item['sha256']:
        root.mkdir(parents=True, exist_ok=True)
        archive = root / ('download.zip' if item['url'].endswith('.zip') else 'download.tar.gz')
        print('Downloading pinned toolchain: ' + name, file=sys.stderr, flush=True)
        with urllib.request.urlopen(item['url'], timeout=120) as source, archive.open('wb') as output:
            shutil.copyfileobj(source, output)
        with archive.open('rb') as stream:
            actual = hashlib.file_digest(stream, 'sha256').hexdigest()
        if actual != item['sha256']:
            raise ValueError('Toolchain checksum mismatch: ' + name)
        if archive.suffix == '.zip':
            with zipfile.ZipFile(archive) as content:
                for entry in content.infolist():
                    if not (root / entry.filename).resolve().is_relative_to(root):
                        raise ValueError('Unsafe archive path')
                content.extractall(root)
        else:
            with tarfile.open(archive) as content:
                content.extractall(root, filter='data')
        if not target.is_dir():
            raise ValueError('Unexpected toolchain archive layout')
        marker.write_text(item['sha256'])
        archive.unlink()
    return target


if __name__ == '__main__':
    print(install(sys.argv[1]))
