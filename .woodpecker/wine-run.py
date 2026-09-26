#!/usr/bin/env python3
"""Execute a Windows test on the CI worker, translating existing path arguments.

This is explicitly emulated validation, not a claim of testing in the game.
The wrapper accepts argv, never shell text, and preserves the test exit status.
"""
import os
import pathlib
import subprocess
import sys


def windows_path(value):
    path = pathlib.Path(value)
    return 'Z:' + str(path.resolve()).replace('/', '\\') if path.exists() else value


if __name__ == '__main__':
    environment = dict(os.environ, WINEDEBUG='-all')
    # The staged test directory is first; MinGW is only a compiler-runtime fallback.
    directory = pathlib.Path(sys.argv[1]).resolve().parent
    environment['WINEPATH'] = windows_path(str(directory)) + r';Z:\opt\mingw\bin'
    result = subprocess.run(['xvfb-run', '-a', 'wine', *map(windows_path, sys.argv[1:])],
                            env=environment, timeout=180)
    sys.exit(result.returncode)
