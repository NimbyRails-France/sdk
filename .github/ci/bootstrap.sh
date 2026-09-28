#!/bin/sh
# Disposable GitHub runner only. The same Windows build/package scripts remain
# shared with Woodpecker; Wine runs fixtures, never the actual game.
set -eu
sdk_root=$(CDPATH= cd -- "$1" && pwd)
export DEBIAN_FRONTEND=noninteractive
dpkg --add-architecture i386
apt-get update
apt-get install -y --no-install-recommends ca-certificates curl gnupg
mkdir -p /etc/apt/keyrings
curl --fail --location --retry 3 -o /etc/apt/keyrings/nrf-winehq.asc \
  https://dl.winehq.org/wine-builds/winehq.key
echo 'd965d646defe94b3dfba6d5b4406900ac6c81065428bf9d9303ad7a72ee8d1b8  /etc/apt/keyrings/nrf-winehq.asc' | sha256sum -c -
echo 'deb [signed-by=/etc/apt/keyrings/nrf-winehq.asc] https://dl.winehq.org/wine-builds/ubuntu/ noble main' > /etc/apt/sources.list.d/nrf-winehq.list
apt-get update
apt-get install -y --no-install-recommends \
  ca-certificates curl git python3 cmake ninja-build make gcc libc6-dev \
  g++-mingw-w64-x86-64-posix binutils-mingw-w64-x86-64 \
  winehq-stable=11.0.0.0~noble-1 xvfb xauth fonts-dejavu-core \
  fontconfig libxi6 libxtst6 libxrender1 libgl1 unzip
mkdir -p /opt/mingw/bin /opt/nimby-ci
# CMake stages compiler runtimes next to the chosen compiler. Mirror the same
# directory contract as the Woodpecker image instead of assuming /usr/bin DLLs.
ln -s /usr/bin/x86_64-w64-mingw32-gcc-posix /opt/mingw/bin/gcc
ln -s /usr/bin/x86_64-w64-mingw32-g++-posix /opt/mingw/bin/g++
ln -s /usr/bin/x86_64-w64-mingw32-windres /opt/mingw/bin/windres
for library in libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll; do
  location=$(x86_64-w64-mingw32-g++-posix -print-file-name="$library")
  test -f "$location"
  # Install the actual DLL bytes: CMake preserves symlinks during install,
  # while distributable Windows ZIPs deliberately reject symbolic links.
  cp -L "$location" "/opt/mingw/bin/$library"
done
chmod +x "$sdk_root/.woodpecker/wine-run.py"
cat > /opt/nimby-ci/nimby-mingw.cmake <<EOF
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER /opt/mingw/bin/gcc)
set(CMAKE_CXX_COMPILER /opt/mingw/bin/g++)
set(CMAKE_RC_COMPILER /opt/mingw/bin/windres)
set(CMAKE_CROSSCOMPILING_EMULATOR "$sdk_root/.woodpecker/wine-run.py")
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF
export WINEDEBUG=-all
# WineHQ offers optional Mono/Gecko installers in a fresh prefix. These native
# fixtures and JVM applications use neither; no interactive installer may stall
# a headless runner. Bound initialization separately from compilation.
export WINEDLLOVERRIDES='mscoree,mshtml='
timeout 90 xvfb-run -a wineboot --init
timeout 60 xvfb-run -a winecfg -v win10
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
