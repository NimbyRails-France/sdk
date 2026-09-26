#!/bin/sh
set -eu
python3 .woodpecker/check-release.py
# The release image has MinGW, but the JVM client's isolated JNA test fixture
# also needs the host C compiler. This never builds a distributable Linux SDK.
apt-get update
apt-get install -y --no-install-recommends gcc libc6-dev
export JAVA_HOME="$(python3 .woodpecker/toolchain.py java-linux)"
export PATH="$JAVA_HOME/bin:$PATH"
export NRF_KOTLIN_HOME="$(python3 .woodpecker/toolchain.py kotlin-linux)"
chmod +x .woodpecker/wine-run.py
java -version
wine --version
cmake -S . -B build/ci -G Ninja -DCMAKE_TOOLCHAIN_FILE=/opt/nimby-ci/nimby-mingw.cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DCMAKE_INSTALL_PREFIX="$PWD/build/install"
cmake --build build/ci --parallel 2
xvfb-run -a ctest --test-dir build/ci --output-on-failure --timeout 90
cmake --install build/ci
sh gradle-plugin/gradlew -p gradle-plugin build publish -PsdkRepository="$PWD/build/kotlin-kit/gradle-repository" --no-daemon --max-workers=2 --console=plain
sh kotlin-client/gradlew -p kotlin-client test --no-daemon --max-workers=2 --console=plain
python3 .woodpecker/kotlin-kit.py
