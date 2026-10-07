"""Read publisher-image code only. Never execute it or inspect runtime secrets."""

import hashlib
import os
from pathlib import Path


ROOTS = (Path('/usr/local/bin'), Path('/app'), Path('/opt'))
SKIP_DIRECTORIES = {
    '.git', '__pycache__', 'node_modules', 'site-packages', 'dist-packages',
    '.venv', 'venv', 'storage', 'downloads', 'artifacts',
}
SOURCE_EXTENSIONS = {'.py', '.sh', '.bash', '.js', '.mjs', '.cjs'}
MAX_FILES = 2500
MAX_DEPTH = 5
MAX_SOURCE_BYTES = 128 * 1024
MAX_TOTAL_SOURCE_BYTES = 512 * 1024
MAX_SOURCES = 48


def source_candidate(path, root):
    name = path.name.lower()
    relevant = any(word in name for word in (
        'publish', 'entrypoint', 'release', 'distribution', 'github', 'plugin',
    ))
    if root == Path('/usr/local/bin'):
        return relevant and (path.suffix in SOURCE_EXTENSIONS or not path.suffix)
    return path.suffix in SOURCE_EXTENSIONS or (relevant and not path.suffix)


def main():
    candidates = []
    listed = 0
    print('Publisher image code inspection: fixed image directories only.')
    print('No environment, workspace data, credentials, mounts or publisher execution.')
    for root in ROOTS:
        print(f'\nDIRECTORY {root}')
        if not root.is_dir() or root.is_symlink():
            print('Not present as an ordinary directory.')
            continue
        for directory, names, files in os.walk(root, followlinks=False):
            current = Path(directory)
            depth = len(current.relative_to(root).parts)
            names[:] = sorted(name for name in names
                              if name not in SKIP_DIRECTORIES
                              and not (current / name).is_symlink()
                              and depth < MAX_DEPTH)
            for name in sorted(files):
                path = current / name
                if listed >= MAX_FILES:
                    print('File listing limit reached.')
                    names[:] = []
                    break
                listed += 1
                print(f'FILE {path}' + (' [symlink, not read]' if path.is_symlink() else ''))
                if not path.is_symlink() and source_candidate(path, root):
                    candidates.append(path)
            if listed >= MAX_FILES:
                break

    total = 0
    displayed = 0
    for path in candidates:
        if displayed >= MAX_SOURCES:
            print('Source count limit reached.')
            break
        try:
            size = path.stat().st_size
            if size > MAX_SOURCE_BYTES or total + size > MAX_TOTAL_SOURCE_BYTES:
                print(f'SKIP {path}: source size limit.')
                continue
            with path.open('rb') as stream:
                data = stream.read(MAX_SOURCE_BYTES + 1)
            if len(data) > MAX_SOURCE_BYTES or b'\0' in data:
                print(f'SKIP {path}: not a bounded text source.')
                continue
            source = data.decode('utf-8')
        except (OSError, UnicodeError):
            print(f'SKIP {path}: not a readable UTF-8 source.')
            continue
        total += len(data)
        displayed += 1
        print(f'\nBEGIN SOURCE {path} sha256={hashlib.sha256(data).hexdigest()}')
        for number, line in enumerate(source.splitlines(), 1):
            print(f'{number:04d}: {line}')
        print(f'END SOURCE {path}')
    print(f'\nInspection complete: {listed} filenames; {displayed} sources; {total} source bytes.')


if __name__ == '__main__':
    main()
