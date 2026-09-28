param(
 [string]$GameDirectory='C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails',
 [string]$SourceDirectory="$PSScriptRoot/../../build/clion-Release/drop-in"
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath("$PSScriptRoot/../../build/proxy-install-test-"+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $root | Out-Null
# Read-only copies of installed binaries; never start the copied game or touch the real installation.
Copy-Item -LiteralPath (Join-Path $GameDirectory 'NIMBYRails.exe') -Destination "$root/NIMBYRails.exe"
$sdl=Join-Path $GameDirectory 'NimbyRailsSDL3Original.dll'
if(!(Test-Path -LiteralPath $sdl)){$sdl=Join-Path $GameDirectory 'SDL3.dll'}
Copy-Item -LiteralPath $sdl -Destination "$root/SDL3.dll"
$originalHash=(Get-FileHash -LiteralPath "$root/SDL3.dll").Hash
function Run([string]$action,[bool]$expected=$true){
 $ErrorActionPreference='Continue'
 & powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$PSScriptRoot/../../tools/windows/install-proxy.ps1" -Action $action -GameDirectory $root -SourceDirectory $SourceDirectory *> "$root/operation.log"
 $ErrorActionPreference='Stop'
 if(($LASTEXITCODE -eq 0) -ne $expected){Get-Content "$root/operation.log";throw 'Unexpected installer result'}
}
Run 'Install'
$record=Get-Content -LiteralPath "$root/NimbyRailsFranceSDK-install.json" -Raw | ConvertFrom-Json
$bridge=Join-Path $root 'NimbyRailsFranceTextureBridge-experimental-v4.dll'
if(!$record.textureBridgeSha256 -or (Get-FileHash -LiteralPath $bridge).Hash -ne $record.textureBridgeSha256){throw 'Texture bridge installation/hash missing'}
$uiBridge=Join-Path $root 'NimbySignalUiBridge-experimental-v1.dll'
if(!$record.signalUiBridgeSha256 -or (Get-FileHash -LiteralPath $uiBridge).Hash -ne $record.signalUiBridgeSha256){throw 'UI bridge installation/hash missing'}
$drivingBridge=Join-Path $root 'NimbyAutomaticDrivingBridge-v1.dll'
if(!$record.automaticDrivingBridgeSha256 -or (Get-FileHash -LiteralPath $drivingBridge).Hash -ne $record.automaticDrivingBridgeSha256){throw 'Automatic driving bridge installation/hash missing'}
$constructionBridge=Join-Path $root 'NimbyConstructionBridge-experimental-v1.dll'
if(Test-Path -LiteralPath (Join-Path $SourceDirectory 'NimbyConstructionBridge-experimental-v1.dll')){
 if(!$record.constructionBridgeSha256 -or (Get-FileHash -LiteralPath $constructionBridge).Hash -ne $record.constructionBridgeSha256){throw 'Construction bridge installation/hash missing'}
}
Run 'Remove'
if(Test-Path -LiteralPath $constructionBridge){throw 'Construction bridge not removed'}
if(Test-Path -LiteralPath $drivingBridge){throw 'Automatic driving bridge not removed'}
if(Test-Path -LiteralPath $uiBridge){throw 'UI bridge not removed'}
if((Get-FileHash -LiteralPath "$root/SDL3.dll").Hash -ne $originalHash -or (Test-Path -LiteralPath $bridge)){throw 'Proxy removal did not restore SDL and remove the managed bridge'}
[IO.File]::WriteAllText($bridge,'foreign bridge')
Run 'Install' $false
if((Get-FileHash -LiteralPath "$root/SDL3.dll").Hash -ne $originalHash -or [IO.File]::ReadAllText($bridge) -ne 'foreign bridge'){throw 'Installer changed foreign files'}
Remove-Item -LiteralPath $bridge
[IO.File]::WriteAllText($uiBridge,'foreign UI bridge')
Run 'Install' $false
if((Get-FileHash -LiteralPath "$root/SDL3.dll").Hash -ne $originalHash -or [IO.File]::ReadAllText($uiBridge) -ne 'foreign UI bridge'){throw 'Installer changed foreign UI bridge'}
Remove-Item -LiteralPath $uiBridge
[IO.File]::WriteAllText($constructionBridge,'foreign construction bridge')
Run 'Install' $false
if((Get-FileHash -LiteralPath "$root/SDL3.dll").Hash -ne $originalHash -or [IO.File]::ReadAllText($constructionBridge) -ne 'foreign construction bridge'){throw 'Installer changed foreign construction bridge'}
Write-Output 'PASS: proxy/SDK/texture bridge installation, hash verification, removal, existing bridge protection (copied binaries only)'
