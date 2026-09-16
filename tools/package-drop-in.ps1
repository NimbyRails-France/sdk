param([string]$ClionHome="$env:LOCALAPPDATA/Programs/CLion",[switch]$SkipBuild)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
if(!$SkipBuild){ & "$PSScriptRoot/build.ps1" -ClionHome $ClionHome -Configuration Release }
$stage=Join-Path $root 'dist/NimbyRailsFranceSDK-0.7.1-drop-in'
New-Item -ItemType Directory -Force -Path $stage | Out-Null
foreach($name in @('SDL3.dll','NimbyRailsFranceSDK.dll','libwinpthread-1.dll')){
 $source=if($name -eq 'SDL3.dll'){"$root/build/Release/drop-in/$name"}else{"$root/build/Release/$name"}
 Copy-Item -LiteralPath $source -Destination $stage -Force
}
$installer=[IO.File]::ReadAllText("$PSScriptRoot/install-proxy.ps1").Replace('$PSScriptRoot/../build/Release/drop-in','$PSScriptRoot')
[IO.File]::WriteAllText("$stage/install-proxy.ps1",$installer,(New-Object Text.UTF8Encoding($false)))
Copy-Item -LiteralPath "$root/docs/install-drop-in.md" -Destination "$stage/README.md" -Force
New-Item -ItemType Directory -Force -Path "$stage/licenses" | Out-Null
Copy-Item -LiteralPath "$root/third_party/minhook/LICENSE.txt" -Destination "$stage/licenses/MinHook.txt" -Force
Copy-Item -LiteralPath "$ClionHome/bin/mingw/licenses" -Destination "$stage/licenses/MinGW" -Recurse -Force
$zip=Join-Path $root 'dist/NimbyRailsFranceSDK-0.7.1-drop-in-windows-x64.zip'
Compress-Archive -LiteralPath $stage -DestinationPath $zip -Force
$files=@($zip,"$stage/SDL3.dll","$stage/NimbyRailsFranceSDK.dll","$stage/libwinpthread-1.dll")
$lines=foreach($file in $files){"$((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant())  $([IO.Path]::GetFileName($file))"}
$lines | Set-Content "$root/dist/SHA256SUMS-drop-in.txt" -Encoding ascii
Write-Output "Drop-in release: $zip"
