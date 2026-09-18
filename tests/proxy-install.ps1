param(
 [string]$GameDirectory='C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails',
 [string]$SourceDirectory="$PSScriptRoot/../build/clion-Release/drop-in"
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath("$PSScriptRoot/../build/proxy-install-test-"+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $root | Out-Null
# Read-only copies of installed binaries; never start the copied game or touch the real installation.
Copy-Item -LiteralPath (Join-Path $GameDirectory 'NIMBYRails.exe') -Destination "$root/NIMBYRails.exe"
$sdl=Join-Path $GameDirectory 'NimbyRailsSDL3Original.dll'
if(!(Test-Path -LiteralPath $sdl)){$sdl=Join-Path $GameDirectory 'SDL3.dll'}
Copy-Item -LiteralPath $sdl -Destination "$root/SDL3.dll"
$originalHash=(Get-FileHash -LiteralPath "$root/SDL3.dll").Hash
function Run([string]$action,[bool]$expected=$true){
 $ErrorActionPreference='Continue'
 & powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$PSScriptRoot/../tools/install-proxy.ps1" -Action $action -GameDirectory $root -SourceDirectory $SourceDirectory *> "$root/operation.log"
 $ErrorActionPreference='Stop'
 if(($LASTEXITCODE -eq 0) -ne $expected){Get-Content "$root/operation.log";throw 'Unexpected installer result'}
}
Run 'Install'
$record=Get-Content -LiteralPath "$root/NimbyRailsFranceSDK-install.json" -Raw | ConvertFrom-Json
$bridge=Join-Path $root 'NimbyRailsFranceTextureBridge-experimental-v3.dll'
if(!$record.textureBridgeSha256 -or (Get-FileHash -LiteralPath $bridge).Hash -ne $record.textureBridgeSha256){throw 'Texture bridge installation/hash missing'}
Run 'Remove'
if((Get-FileHash -LiteralPath "$root/SDL3.dll").Hash -ne $originalHash -or (Test-Path -LiteralPath $bridge)){throw 'Proxy removal did not restore SDL and remove the managed bridge'}
[IO.File]::WriteAllText($bridge,'foreign bridge')
Run 'Install' $false
if((Get-FileHash -LiteralPath "$root/SDL3.dll").Hash -ne $originalHash -or [IO.File]::ReadAllText($bridge) -ne 'foreign bridge'){throw 'Installer changed foreign files'}
Write-Output 'PASS: proxy/SDK/texture bridge installation, hash verification, removal, existing bridge protection (copied binaries only)'
