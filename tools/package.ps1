param([string]$ClionHome = "$env:LOCALAPPDATA/Programs/CLion", [switch]$SkipBuild, [string]$OutputRoot)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$cmake = Join-Path $ClionHome 'bin/cmake/win/x64/bin/cmake.exe'
$ninja = Join-Path $ClionHome 'bin/ninja/win/x64/ninja.exe'
$compiler = Join-Path $ClionHome 'bin/mingw/bin/c++.exe'
if(!$OutputRoot){$OutputRoot=Join-Path $projectRoot 'dist'}
$prefix = Join-Path $OutputRoot 'NimbyRailsSDK-0.6.0'
if(!$SkipBuild) { & "$PSScriptRoot/build.ps1" -ClionHome $ClionHome -Configuration Release }
& $cmake --install "$projectRoot/build/Release" --prefix $prefix
if($LASTEXITCODE) { throw 'SDK installation failed' }
# Compile only the installed example, outside the SDK source tree and build.
$consumer = Join-Path $projectRoot 'build/sdk-consumer'
New-Item -ItemType Directory -Force -Path "$consumer/source" | Out-Null
Copy-Item -Path "$prefix/share/NimbyRailsSDK/examples/observer/*" -Destination "$consumer/source" -Recurse -Force
& $cmake -S "$consumer/source" -B "$consumer/build" -G Ninja "-DCMAKE_MAKE_PROGRAM=$ninja" "-DCMAKE_CXX_COMPILER=$compiler" "-DCMAKE_BUILD_TYPE=Release" "-DNimbyRailsSDK_DIR=$prefix/lib/cmake/NimbyRailsSDK"
if($LASTEXITCODE) { throw 'External consumer configuration failed' }
& $cmake --build "$consumer/build"
if($LASTEXITCODE) { throw 'External consumer build failed' }
& "$consumer/build/MyNimbyObserver.exe"
if($LASTEXITCODE -ne 2) { throw 'External consumer failed to load its runtime; expected usage exit 2' }
Write-Output "SDK installed: $prefix"
Write-Output "Independent consumer: $consumer/build/MyNimbyObserver.exe <game PID>"

Copy-Item -LiteralPath "$projectRoot/README.md" -Destination $prefix -Force
$zip=Join-Path $projectRoot 'dist/NimbyRailsSDK-0.6.0-windows-x64-mingw.zip'
Compress-Archive -LiteralPath $prefix -DestinationPath $zip -Force
$hash=(Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToLowerInvariant()
"$hash  $([IO.Path]::GetFileName($zip))" | Set-Content "$projectRoot/dist/SHA256SUMS.txt" -Encoding ascii
Write-Output "Release asset: $zip"
