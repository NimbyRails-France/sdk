#!/bin/sh
# Disposable GitHub runner only. The same Windows build/package scripts remain
# shared with Woodpecker; Wine runs fixtures, never the actual game.
set -eu
sdk_root=$(CDPATH= cd -- "$1" && pwd)
export DEBIAN_FRONTEND=noninteractive
dpkg --add-architecture i386
apt-get update
apt-get install -y --no-install-recommends \
  ca-certificates curl git python3 cmake ninja-build make gcc libc6-dev \
  g++-mingw-w64-x86-64-posix binutils-mingw-w64-x86-64 \
  wine wine64 wine32:i386 xvfb xauth fonts-dejavu-core \
  fontconfig libxi6 libxtst6 libxrender1 libgl1 unzip
mkdir -p /opt/mingw/bin /opt/nimby-ci
for library in libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll; do
  location=$(x86_64-w64-mingw32-g++-posix -print-file-name="$library")
  test -f "$location"
  ln -s "$location" "/opt/mingw/bin/$library"
done
chmod +x "$sdk_root/.woodpecker/wine-run.py"
cat > /opt/nimby-ci/nimby-mingw.cmake <<EOF
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_CROSSCOMPILING_EMULATOR "$sdk_root/.woodpecker/wine-run.py")
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF
export WINEDEBUG=-all
xvfb-run -a wineboot --init
xvfb-run -a winecfg -v win10
# The installer compiler is needed only by the Hub; verify the upstream digest
# before executing it. This installation is confined to the disposable runner.
if [ "${CI_REPO##*/}" = hub ]; then
  curl --fail --location --retry 3 -o /tmp/nrf-inno.exe \
    https://github.com/jrsoftware/issrc/releases/download/is-6_7_3/innosetup-6.7.3.exe
  echo '9c73c3bae7ed48d44112a0f48e66742c00090bdb5bef71d9d3c056c66e97b732  /tmp/nrf-inno.exe' | sha256sum -c -
  xvfb-run -a wine /tmp/nrf-inno.exe /VERYSILENT /SUPPRESSMSGBOXES /NORESTART '/DIR=C:\nrf-inno'
  ln -s "$WINEPREFIX/drive_c/nrf-inno" /opt/inno
fi
wine --version
x86_64-w64-mingw32-g++-posix --version
