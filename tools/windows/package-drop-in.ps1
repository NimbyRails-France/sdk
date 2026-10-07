param([string]$ClionHome="$env:LOCALAPPDATA/Programs/CLion",[switch]$SkipBuild,[string]$BuildDirectory,[string]$OutputRoot,[switch]$DevelopmentConstruction)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$version=(Get-Content -LiteralPath "$root/VERSION" -Raw).Trim()
if($version -notmatch '^\d+\.\d+\.\d+(?:-(?:alpha|beta)\.[1-9]\d*)?$'){throw 'Invalid VERSION'}
if($BuildDirectory -and !$SkipBuild){throw 'BuildDirectory requires SkipBuild'}
if(!$BuildDirectory){$BuildDirectory=Join-Path $root 'build/Release'}
if(!$SkipBuild){ & "$PSScriptRoot/build.ps1" -ClionHome $ClionHome -Configuration Release }
if(!$OutputRoot){$OutputRoot=Join-Path $root 'dist'}
$stage=Join-Path $OutputRoot "NimbyRailsFranceSDK-$version-drop-in"
New-Item -ItemType Directory -Force -Path $stage | Out-Null
if($DevelopmentConstruction){
 Copy-Item -LiteralPath "$BuildDirectory/NimbyConstructionBridge-experimental-v1.dll" -Destination $stage -Force
}elseif(Test-Path -LiteralPath "$stage/NimbyConstructionBridge-experimental-v1.dll"){
 throw 'A production package cannot reuse a stage containing the development construction bridge.'
}
foreach($name in @('SDL3.dll','NimbyRailsFranceSDK.dll','NimbyRailsFranceModHost.exe','libwinpthread-1.dll','NimbyRailsFranceTextureBridge-experimental-v4.dll','NimbySignalUiBridge-experimental-v1.dll','NimbyAutomaticDrivingBridge-v1.dll','NimbyRailsFranceClockBridge-0.7.1.dll','NimbyModMetadataBridge-v1.dll')){
 $source=if($name -eq 'SDL3.dll'){"$BuildDirectory/drop-in/$name"}else{"$BuildDirectory/$name"}
 Copy-Item -LiteralPath $source -Destination $stage -Force
}
$installer=[IO.File]::ReadAllText("$PSScriptRoot/install-proxy.ps1").Replace('$PSScriptRoot/../../build/Release/drop-in','$PSScriptRoot')
[IO.File]::WriteAllText("$stage/install-proxy.ps1",$installer,(New-Object Text.UTF8Encoding($false)))
Copy-Item -LiteralPath "$root/docs/install-drop-in.md" -Destination "$stage/README.md" -Force
New-Item -ItemType Directory -Force -Path "$stage/licenses" | Out-Null
Copy-Item -LiteralPath "$root/third_party/windows/minhook/LICENSE.txt" -Destination "$stage/licenses/MinHook.txt" -Force
Copy-Item -LiteralPath "$ClionHome/bin/mingw/licenses" -Destination "$stage/licenses/MinGW" -Recurse -Force
$zip=Join-Path $OutputRoot "NimbyRailsFranceSDK-$version-drop-in-windows-x64.zip"
Compress-Archive -LiteralPath $stage -DestinationPath $zip -Force
$files=@($zip,"$stage/SDL3.dll","$stage/NimbyRailsFranceSDK.dll","$stage/libwinpthread-1.dll")
$lines=foreach($file in $files){"$((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant())  $([IO.Path]::GetFileName($file))"}
$lines | Set-Content "$OutputRoot/SHA256SUMS-drop-in.txt" -Encoding ascii
Write-Output "Drop-in release: $zip"
