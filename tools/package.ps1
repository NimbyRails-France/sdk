param([string]$ClionHome = "$env:LOCALAPPDATA/Programs/CLion", [switch]$SkipBuild, [string]$OutputRoot)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$cmake = Join-Path $ClionHome 'bin/cmake/win/x64/bin/cmake.exe'
$ninja = Join-Path $ClionHome 'bin/ninja/win/x64/ninja.exe'
$compiler = Join-Path $ClionHome 'bin/mingw/bin/c++.exe'
if(!$OutputRoot){$OutputRoot=Join-Path $projectRoot 'dist'}
$prefix = Join-Path $OutputRoot 'NimbyRailsFranceSDK-0.7.2'
if(!$SkipBuild) { & "$PSScriptRoot/build.ps1" -ClionHome $ClionHome -Configuration Release }
& $cmake --install "$projectRoot/build/Release" --prefix $prefix
if($LASTEXITCODE) { throw 'SDK installation failed' }
foreach($removed in @('include/nimby/sdk.h','include/nimby/observation.h','bin/NimbyRailsSDK.dll','lib/cmake/NimbyRailsSDK')) {
    if(Test-Path -LiteralPath (Join-Path $prefix $removed)) { throw "Old SDK files in package: $removed. Use a fresh output directory." }
}
# Compile only the installed example, outside the SDK source tree and build.
$consumer = Join-Path $projectRoot 'build/sdk-consumer'
New-Item -ItemType Directory -Force -Path "$consumer/source" | Out-Null
Copy-Item -Path "$prefix/share/NimbyRailsFranceSDK/examples/observer/*" -Destination "$consumer/source" -Recurse -Force
& $cmake -S "$consumer/source" -B "$consumer/build" -G Ninja "-DCMAKE_MAKE_PROGRAM=$ninja" "-DCMAKE_CXX_COMPILER=$compiler" "-DCMAKE_BUILD_TYPE=Release" "-DNimbyRailsFranceSDK_DIR=$prefix/lib/cmake/NimbyRailsFranceSDK"
if($LASTEXITCODE) { throw 'External consumer configuration failed' }
& $cmake --build "$consumer/build"
if($LASTEXITCODE) { throw 'External consumer build failed' }
& "$consumer/build/MyNimbyObserver.exe" --check-sdk
if($LASTEXITCODE) { throw 'External consumer runtime/version check failed' }
Write-Output "SDK installed: $prefix"
Write-Output "Independent consumer: $consumer/build/MyNimbyObserver.exe <game PID>"

# Validate the tutorial from the installed package, with no running game.
$tutorial = Join-Path $projectRoot 'build/sdk-first-consumer'
New-Item -ItemType Directory -Force -Path "$tutorial/source" | Out-Null
Copy-Item -Path "$prefix/share/NimbyRailsFranceSDK/examples/first-observer/*" -Destination "$tutorial/source" -Recurse -Force
& $cmake -S "$tutorial/source" -B "$tutorial/build" -G Ninja "-DCMAKE_MAKE_PROGRAM=$ninja" "-DCMAKE_CXX_COMPILER=$compiler" "-DCMAKE_BUILD_TYPE=Release" "-DNimbyRailsFranceSDK_DIR=$prefix/lib/cmake/NimbyRailsFranceSDK"
if($LASTEXITCODE) { throw 'Tutorial configuration failed' }
& $cmake --build "$tutorial/build"
if($LASTEXITCODE) { throw 'Tutorial build failed' }
& "$tutorial/build/MyFirstNimbyTool.exe" --check-sdk
if($LASTEXITCODE) { throw 'Tutorial runtime/version check failed' }
Write-Output "Tutorial: $tutorial/build/MyFirstNimbyTool.exe <game PID>"

# Build the high-level client using only the installed header and C ABI library.
$autoConsumer = Join-Path $projectRoot 'build/sdk-auto-consumer'
New-Item -ItemType Directory -Force -Path "$autoConsumer/source" | Out-Null
Copy-Item -Path "$prefix/share/NimbyRailsFranceSDK/examples/auto-observer/*" -Destination "$autoConsumer/source" -Recurse -Force
& $cmake -S "$autoConsumer/source" -B "$autoConsumer/build" -G Ninja "-DCMAKE_MAKE_PROGRAM=$ninja" "-DCMAKE_CXX_COMPILER=$compiler" "-DCMAKE_BUILD_TYPE=Release" "-DNimbyRailsFranceSDK_DIR=$prefix/lib/cmake/NimbyRailsFranceSDK"
if($LASTEXITCODE) { throw 'C++ client configuration failed' }
& $cmake --build "$autoConsumer/build"
if($LASTEXITCODE) { throw 'C++ client build failed' }
& "$autoConsumer/build/MyNimbyClient.exe" --check-sdk
if($LASTEXITCODE) { throw 'C++ client runtime check failed' }

Copy-Item -LiteralPath "$projectRoot/README.md" -Destination $prefix -Force
$zip=Join-Path $projectRoot 'dist/NimbyRailsFranceSDK-0.7.2-windows-x64-mingw.zip'
Compress-Archive -LiteralPath $prefix -DestinationPath $zip -Force
$hash=(Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToLowerInvariant()
"$hash  $([IO.Path]::GetFileName($zip))" | Set-Content "$projectRoot/dist/SHA256SUMS.txt" -Encoding ascii
Write-Output "Release asset: $zip"
