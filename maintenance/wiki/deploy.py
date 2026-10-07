"""Deploy verified static wiki files only. No Docker or Caddy configuration changes."""
from __future__ import annotations

import argparse
import contextlib
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import stat
import time
import urllib.request
import uuid

PUBLIC_ORIGIN = "https://wiki.nimbyrails-france.fr"
DEFAULT_RELEASE = "20261008-5948781"
CHECK_PAGES = {
    "/": "index.html",
    "/en/": "en/index.html",
    "/version/0.9/": "version/0.9/index.html",
    "/en/version/0.9/": "en/version/0.9/index.html",
    "/version/0.8/": "version/0.8/index.html",
    "/en/version/0.8/": "en/version/0.8/index.html",
    "/commencer/installation/": "commencer/installation/index.html",
    "/en/commencer/installation/": "en/commencer/installation/index.html",
}


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def is_link(path: Path) -> bool:
    return path.is_symlink() or (hasattr(path, "is_junction") and path.is_junction())


def directory(path: Path) -> Path:
    if is_link(path) or not path.is_dir():
        raise ValueError(f"Expected a real directory: {path}")
    return path.resolve(strict=True)


def file_map(root: Path) -> dict[str, tuple[str, int]]:
    directory(root)
    files = {}
    for parent, dirs, names in os.walk(root, followlinks=False):
        for name in dirs + names:
            path = Path(parent) / name
            if is_link(path):
                raise ValueError(f"Static tree contains a link: {path}")
            mode = path.stat(follow_symlinks=False).st_mode
            if stat.S_ISDIR(mode):
                continue
            if not stat.S_ISREG(mode):
                raise ValueError(f"Static tree contains a special file: {path}")
            data = path.read_bytes()
            files[path.relative_to(root).as_posix()] = (digest(data), len(data))
    if not files:
        raise ValueError("Static tree is empty")
    return files


def current_target(storage: Path, name: str, required: bool) -> str | None:
    link = storage / name
    if not os.path.lexists(link):
        if required:
            raise ValueError(f"Existing deployment link is required: {link}")
        return None
    if not link.is_symlink():
        raise ValueError(f"Deployment pointer is not a symlink: {link}")
    target = os.readlink(link).replace("\\", "/")
    if not re.fullmatch(r"releases/[a-zA-Z0-9-]+", target):
        raise ValueError(f"Deployment pointer leaves the release directory: {name}")
    resolved = directory(storage / target)
    if not resolved.is_relative_to((storage / "releases").resolve(strict=True)):
        raise ValueError("Release path leaves storage")
    return target


def replace_link(storage: Path, name: str, target: str) -> None:
    temporary = storage / f".{name}-{uuid.uuid4().hex}"
    os.symlink(target, temporary, target_is_directory=True)
    try:
        os.replace(temporary, storage / name)
    finally:
        if temporary.is_symlink():
            temporary.unlink()


@contextlib.contextmanager
def deploy_lock(base: Path):
    lock = base / ".deploy.lock"
    if is_link(lock):
        raise ValueError("Deployment lock must not be a symlink")
    fd = os.open(lock, os.O_CREAT | os.O_RDWR | getattr(os, "O_NOFOLLOW", 0), 0o600)
    try:
        if not stat.S_ISREG(os.fstat(fd).st_mode):
            raise ValueError("Deployment lock is not a regular file")
        if os.name == "nt":
            # Local fixtures only; production uses the same flock as install.sh.
            import msvcrt
            if os.fstat(fd).st_size == 0:
                os.write(fd, b"\0")
            os.lseek(fd, 0, os.SEEK_SET)
            msvcrt.locking(fd, msvcrt.LK_NBLCK, 1)
        else:
            import fcntl
            fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
        yield
    finally:
        os.close(fd)


def http_fetch(url: str, timeout: float, limit: int) -> bytes:
    request = urllib.request.Request(url, headers={
        "Accept-Encoding": "identity", "Cache-Control": "no-cache",
        "User-Agent": "NRF-Wiki-Deployment/1",
    })
    with urllib.request.urlopen(request, timeout=timeout) as response:
        if response.status != 200:
            raise RuntimeError(f"Unexpected HTTP status: {response.status}")
        data = response.read(limit + 1)
    if len(data) > limit:
        raise RuntimeError("HTTP response exceeds expected size")
    return data


def verify_public(expected, release, *, fetch=http_fetch, attempts=8, delay=2.0,
                  deadline_seconds=60.0):
    deadline = time.monotonic() + deadline_seconds
    error = None
    for attempt in range(attempts):
        try:
            for route, (sha, size) in {"/healthz": (digest(b"ok"), 2), **expected}.items():
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    raise TimeoutError("Public verification deadline exceeded")
                body = fetch(PUBLIC_ORIGIN + route + "?nrf-deploy=" + release,
                             min(5.0, remaining), size)
                if len(body) != size or digest(body) != sha:
                    raise RuntimeError(f"Published content differs: {route}")
            return
        except Exception as exc:
            error = exc
            remaining = deadline - time.monotonic()
            if attempt + 1 >= attempts or remaining <= 0:
                break
            time.sleep(min(delay, remaining))
    raise RuntimeError(f"Public verification failed: {error}") from error


def deploy(source: Path, base: Path, release: str, *, fetch=http_fetch,
           attempts=8, delay=2.0, deadline_seconds=60.0):
    if not re.fullmatch(r"[a-zA-Z0-9-]+", release):
        raise ValueError("Invalid release identifier")
    base = directory(base)
    source = directory(source)
    storage = directory(base / "storage")
    releases = directory(storage / "releases")
    if source.is_relative_to(storage) or storage.is_relative_to(source):
        raise ValueError("Source must be separate from live storage")
    with deploy_lock(base):
        previous = current_target(storage, "current", required=True)
        old_previous = current_target(storage, "previous", required=False)
        target = releases / release
        if os.path.lexists(target):
            raise ValueError("Release already exists; never overwrite a deployed release")
        source_files = file_map(source)
        required = set(CHECK_PAGES.values()) | {"404.html"}
        if not required.issubset(source_files) or any(source_files[p][1] == 0 for p in required):
            raise ValueError("Generated pages required for deployment are missing or empty")
        expected = {route: source_files[path] for route, path in CHECK_PAGES.items()}
        prior_files = file_map(storage / previous)
        rollback_expected = {route: prior_files[path] for route, path in CHECK_PAGES.items()
                             if path in prior_files}
        if "/" not in rollback_expected:
            raise ValueError("Previous release has no home page to verify on rollback")
        stage = releases / f".incoming-{release}-{uuid.uuid4().hex}"
        shutil.copytree(source, stage, symlinks=True)
        if file_map(stage) != source_files:
            raise ValueError("Copied static files differ from the verified source")
        for parent, dirs, names in os.walk(stage):
            Path(parent).chmod(0o755)
            for name in names:
                (Path(parent) / name).chmod(0o644)
        os.rename(stage, target)
        new_target = "releases/" + release
        switched = False
        try:
            replace_link(storage, "previous", previous)
            replace_link(storage, "current", new_target)
            switched = True
            verify_public(expected, release, fetch=fetch, attempts=attempts,
                          delay=delay, deadline_seconds=deadline_seconds)
        except BaseException as exc:
            # Do not overwrite an unexpected replacement made outside our lock.
            if switched:
                if current_target(storage, "current", required=True) != new_target:
                    raise RuntimeError("Deployment failed and current changed externally; rollback refused") from exc
                replace_link(storage, "current", previous)
            if old_previous is None:
                (storage / "previous").unlink(missing_ok=True)
            else:
                replace_link(storage, "previous", old_previous)
            if switched:
                try:
                    verify_public(rollback_expected, "rollback-" + release, fetch=fetch,
                                  attempts=attempts, delay=delay,
                                  deadline_seconds=deadline_seconds)
                except Exception as rollback_error:
                    raise RuntimeError("Previous pointer restored, but public rollback verification failed: "
                                       + str(rollback_error)) from exc
            raise RuntimeError("Deployment failed; original pointers restored: " + str(exc)) from exc
        return {"status": "published", "release": release, "previous": previous,
                "files": len(source_files), "checkedPages": len(expected),
                "pageHashes": {route: sha for route, (sha, _) in expected.items()}}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--release", default=DEFAULT_RELEASE)
    args = parser.parse_args()
    # Production destination is fixed; tests inject an isolated root directly.
    print(json.dumps(deploy(args.source, Path("/wiki"), args.release), indent=2))


if __name__ == "__main__":
    main()
