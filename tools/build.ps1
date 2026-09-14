param(
    [string]$ClionHome = "$env:LOCALAPPDATA/Programs/CLion",
    [ValidateSet('Debug','Release')][string]$Configuration = 'Debug'
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$cmake = Join-Path $ClionHome 'bin/cmake/win/x64/bin/cmake.exe'
$ctest = Join-Path $ClionHome 'bin/cmake/win/x64/bin/ctest.exe'
$ninja = Join-Path $ClionHome 'bin/ninja/win/x64/ninja.exe'
$compiler = Join-Path $ClionHome 'bin/mingw/bin/c++.exe'
$cCompiler = Join-Path $ClionHome 'bin/mingw/bin/gcc.exe'
foreach ($tool in @($cmake,$ctest,$ninja,$compiler,$cCompiler)) {
    if (!(Test-Path -LiteralPath $tool)) { throw "Tool missing: $tool (set -ClionHome)" }
}
$build = Join-Path $projectRoot "build/$Configuration"
& $cmake -S $projectRoot -B $build -G Ninja "-DCMAKE_MAKE_PROGRAM=$ninja" "-DCMAKE_CXX_COMPILER=$compiler" "-DCMAKE_C_COMPILER=$cCompiler" "-DCMAKE_BUILD_TYPE=$Configuration"
if ($LASTEXITCODE) { throw 'CMake configuration failed' }
& $cmake --build $build
if ($LASTEXITCODE) { throw 'Build failed' }
& $ctest --test-dir $build --output-on-failure
if ($LASTEXITCODE) { throw 'Tests failed' }
