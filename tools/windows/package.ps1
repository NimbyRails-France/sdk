param([string]$ClionHome = "$env:LOCALAPPDATA/Programs/CLion", [switch]$SkipBuild, [string]$OutputRoot, [string]$BuildDirectory)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$version=(Get-Content -LiteralPath "$projectRoot/VERSION" -Raw).Trim()
if($version -notmatch '^\d+\.\d+\.\d+(?:-(?:alpha|beta)\.[1-9]\d*)?$'){throw 'Invalid VERSION'}
$cmake = Join-Path $ClionHome 'bin/cmake/win/x64/bin/cmake.exe'
$ninja = Join-Path $ClionHome 'bin/ninja/win/x64/ninja.exe'
$compiler = Join-Path $ClionHome 'bin/mingw/bin/c++.exe'
if(!$OutputRoot){$OutputRoot=Join-Path $projectRoot 'dist'}
$prefix = Join-Path $OutputRoot "NimbyRailsFranceSDK-$version"
# An explicit build tree is already configured by the workspace task.
# Require SkipBuild so this script cannot rebuild another tree by accident.
if($BuildDirectory -and !$SkipBuild){throw 'BuildDirectory requires SkipBuild'}
if(!$BuildDirectory){$BuildDirectory=Join-Path $projectRoot 'build/Release'}
if(!$SkipBuild) { & "$PSScriptRoot/build.ps1" -ClionHome $ClionHome -Configuration Release }
& $cmake --install $BuildDirectory --prefix $prefix
if($LASTEXITCODE) { throw 'SDK installation failed' }
foreach($removed in @('include/nimby/client.hpp','include/nimby/sdk.h','include/nimby/observation.h','bin/NimbyRailsSDK.dll','lib/cmake/NimbyRailsSDK','share/NimbyRailsFranceSDK/examples')) {
    if(Test-Path -LiteralPath (Join-Path $prefix $removed)) { throw "Old SDK files in package: $removed. Use a fresh output directory." }
}
# Verify a copy of the installed consumer layout. Gradle writes build/cache
# directories: running it inside the distributable would leak local build state.
$validation=Join-Path $projectRoot ('build/package-consumer-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $validation -Force | Out-Null
Copy-Item -LiteralPath "$projectRoot/verification/kotlin-consumer" -Destination "$validation/consumer" -Recurse
Copy-Item -LiteralPath "$prefix/share/NimbyRailsFranceSDK/kotlin-client" -Destination "$validation/kotlin-client" -Recurse
& "$projectRoot/gradle-plugin/gradlew.bat" -p "$validation/consumer" build '--console=plain' '--no-daemon'
if($LASTEXITCODE) { throw 'Installed Kotlin client consumer compilation failed' }
$generated=Get-ChildItem -LiteralPath $prefix -Directory -Recurse -Force | Where-Object { $_.Name -in @('.gradle','.kotlin','build') }
if($generated){throw 'Generated build/cache directory in SDK package; use a fresh output prefix'}
Copy-Item -LiteralPath "$projectRoot/README.md" -Destination $prefix -Force
$zip=Join-Path $projectRoot "dist/NimbyRailsFranceSDK-$version-windows-x64-mingw.zip"
Compress-Archive -LiteralPath $prefix -DestinationPath $zip -Force
$hash=(Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToLowerInvariant()
"$hash  $([IO.Path]::GetFileName($zip))" | Set-Content "$projectRoot/dist/SHA256SUMS.txt" -Encoding ascii
Write-Output "Release asset: $zip"
