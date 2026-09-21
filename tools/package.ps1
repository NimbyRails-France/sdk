param([string]$ClionHome = "$env:LOCALAPPDATA/Programs/CLion", [switch]$SkipBuild, [string]$OutputRoot)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$version=(Get-Content -LiteralPath "$projectRoot/VERSION" -Raw).Trim()
if($version -notmatch '^\d+\.\d+\.\d+(?:-(?:alpha|beta)\.[1-9]\d*)?$'){throw 'Invalid VERSION'}
$cmake = Join-Path $ClionHome 'bin/cmake/win/x64/bin/cmake.exe'
$ninja = Join-Path $ClionHome 'bin/ninja/win/x64/ninja.exe'
$compiler = Join-Path $ClionHome 'bin/mingw/bin/c++.exe'
if(!$OutputRoot){$OutputRoot=Join-Path $projectRoot 'dist'}
$prefix = Join-Path $OutputRoot "NimbyRailsFranceSDK-$version"
if(!$SkipBuild) { & "$PSScriptRoot/build.ps1" -ClionHome $ClionHome -Configuration Release }
& $cmake --install "$projectRoot/build/Release" --prefix $prefix
if($LASTEXITCODE) { throw 'SDK installation failed' }
foreach($removed in @('include/nimby/sdk.h','include/nimby/observation.h','bin/NimbyRailsSDK.dll','lib/cmake/NimbyRailsSDK')) {
    if(Test-Path -LiteralPath (Join-Path $prefix $removed)) { throw "Old SDK files in package: $removed. Use a fresh output directory." }
}
# Verify the installed Kotlin client example with the Gradle wrapper.
& "$projectRoot/gradle-plugin/gradlew.bat" -p "$prefix/share/NimbyRailsFranceSDK/examples/kotlin-observer" build '--console=plain'
if($LASTEXITCODE) { throw 'Installed Kotlin client example build failed' }
Copy-Item -LiteralPath "$projectRoot/README.md" -Destination $prefix -Force
$zip=Join-Path $projectRoot "dist/NimbyRailsFranceSDK-$version-windows-x64-mingw.zip"
Compress-Archive -LiteralPath $prefix -DestinationPath $zip -Force
$hash=(Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToLowerInvariant()
"$hash  $([IO.Path]::GetFileName($zip))" | Set-Content "$projectRoot/dist/SHA256SUMS.txt" -Encoding ascii
Write-Output "Release asset: $zip"
