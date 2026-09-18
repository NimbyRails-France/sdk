param(
    [ValidateSet('Install','Remove')][string]$Action = 'Install',
    [string]$GameDirectory = 'C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails',
    [string]$SourceDirectory = "$PSScriptRoot/../build/Release/drop-in"
)
$ErrorActionPreference = 'Stop'
$gameRoot = (Resolve-Path -LiteralPath $GameDirectory).Path
$exe = Join-Path $gameRoot 'NimbyRails.exe'
$sdl = Join-Path $gameRoot 'SDL3.dll'
$backup = Join-Path $gameRoot 'NimbyRailsSDL3Original.dll'
$sdk = Join-Path $gameRoot 'NimbyRailsFranceSDK.dll'
$manifestPath = Join-Path $gameRoot 'NimbyRailsFranceSDK-install.json'
$proxyStage = Join-Path $gameRoot 'SDL3.NimbySDK.tmp'
$sdkStage = Join-Path $gameRoot 'NimbyRailsFranceSDK.tmp'
$pthread = Join-Path $gameRoot 'libwinpthread-1.dll'
$pthreadStage = Join-Path $gameRoot 'libwinpthread-1.NimbySDK.tmp'
$textureName = 'NimbyRailsFranceTextureBridge-experimental-v3.dll'
$texture = Join-Path $gameRoot $textureName
$textureStage = Join-Path $gameRoot 'NimbyRailsFranceTextureBridge.NimbySDK.tmp'
$manifestStage = Join-Path $gameRoot 'NimbyRailsFranceSDK-install.tmp'
$originalExeHash = 'FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE'
$originalSdlHash = '2A2704678BF6C9C6A944270AB35079DF76F5AFE92B780394ED72D9C8218B98D8'
function Hash([string]$path) { (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
function RequireHash([string]$path, [string]$expected) {
    if (!(Test-Path -LiteralPath $path) -or (Hash $path) -ne $expected) { throw "Unexpected file contents: $path. Nothing will be overwritten." }
}
foreach($target in @($exe,$sdl,$backup,$sdk,$manifestPath,$proxyStage,$sdkStage,$manifestStage,$pthread,$pthreadStage,$texture,$textureStage)) {
    if([IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($target)) -ne $gameRoot) { throw 'Path outside selected game directory' }
}
$running = Get-Process -Name NimbyRails -ErrorAction SilentlyContinue | Where-Object { !$_.Path -or $_.Path -ieq $exe }
if($running) { throw 'Close NIMBY Rails before installing or removing the proxy. The installer will not close the game.' }
if(Get-Process -Name NimbyRailsFranceLoader -ErrorAction SilentlyContinue) { throw 'Stop NimbyRailsFranceLoader first with --stop.' }

if($Action -eq 'Remove') {
    if(!(Test-Path -LiteralPath $manifestPath)) { throw 'No proxy installation manifest found.' }
    $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
    RequireHash $sdl $manifest.proxySha256
    RequireHash $sdk $manifest.sdkSha256
    RequireHash $backup $originalSdlHash
    if($manifest.pthreadSha256) { RequireHash $pthread $manifest.pthreadSha256 }
    if($manifest.textureBridgeSha256) { RequireHash $texture $manifest.textureBridgeSha256 }
    # Fixed filenames and verified contents only; no recursive deletion.
    Remove-Item -LiteralPath $sdl
    Move-Item -LiteralPath $backup -Destination $sdl
    Remove-Item -LiteralPath $sdk
    if($manifest.pthreadSha256) { Remove-Item -LiteralPath $pthread }
    if($manifest.textureBridgeSha256) { Remove-Item -LiteralPath $texture }
    Remove-Item -LiteralPath $manifestPath
    RequireHash $sdl $originalSdlHash
    Write-Output 'Original SDL3.dll restored. Proxy and SDK removed.'
    exit 0
}

RequireHash $exe $originalExeHash
RequireHash $sdl $originalSdlHash
foreach($target in @($backup,$sdk,$manifestPath,$proxyStage,$sdkStage,$manifestStage,$pthreadStage,$texture,$textureStage)) {
    if(Test-Path -LiteralPath $target) { throw "File already exists; refusing to overwrite: $target" }
}
$sourceRoot = (Resolve-Path -LiteralPath $SourceDirectory).Path
$proxySource = Join-Path $sourceRoot 'SDL3.dll'
$sdkSource = Join-Path $sourceRoot 'NimbyRailsFranceSDK.dll'
$proxyHash = Hash $proxySource
$sdkHash = Hash $sdkSource
$textureSource = Join-Path $sourceRoot $textureName
$textureHash = Hash $textureSource
$pthreadSource = Join-Path $sourceRoot 'libwinpthread-1.dll'
$pthreadHash = $null
if(Test-Path -LiteralPath $pthreadSource) {
    if(Test-Path -LiteralPath $pthread) { throw "Dependency already exists; refusing to overwrite: $pthread" }
    $pthreadHash = Hash $pthreadSource
}
$renamed = $false
$proxyCopied = $false
$sdkCopied = $false
$pthreadCopied = $false
$textureCopied = $false
try {
    # Validate staged copies before changing any game dependency.
    Copy-Item -LiteralPath $sdkSource -Destination $sdkStage
    Copy-Item -LiteralPath $proxySource -Destination $proxyStage
    RequireHash $sdkStage $sdkHash
    Copy-Item -LiteralPath $textureSource -Destination $textureStage
    RequireHash $textureStage $textureHash
    Move-Item -LiteralPath $textureStage -Destination $texture
    $textureCopied = $true
    RequireHash $proxyStage $proxyHash
    if($pthreadHash) {
        Copy-Item -LiteralPath $pthreadSource -Destination $pthreadStage
        RequireHash $pthreadStage $pthreadHash
        Move-Item -LiteralPath $pthreadStage -Destination $pthread
        $pthreadCopied = $true
    }
    [ordered]@{format=2; installedUtc=[DateTime]::UtcNow.ToString('o'); executableSha256=$originalExeHash;
        originalSdlSha256=$originalSdlHash; proxySha256=$proxyHash; sdkSha256=$sdkHash; pthreadSha256=$pthreadHash; textureBridgeSha256=$textureHash} |
        ConvertTo-Json | Set-Content -LiteralPath $manifestStage -Encoding UTF8
    Move-Item -LiteralPath $sdkStage -Destination $sdk
    $sdkCopied = $true
    RequireHash $sdk $sdkHash
    Move-Item -LiteralPath $sdl -Destination $backup
    $renamed = $true
    Move-Item -LiteralPath $proxyStage -Destination $sdl
    $proxyCopied = $true
    RequireHash $sdl $proxyHash
    RequireHash $backup $originalSdlHash
    RequireHash $exe $originalExeHash
    Move-Item -LiteralPath $manifestStage -Destination $manifestPath
} catch {
    # Restore the original on installation failure, without touching foreign files.
    if($proxyCopied -and (Test-Path -LiteralPath $sdl) -and (Hash $sdl) -eq $proxyHash) { Remove-Item -LiteralPath $sdl }
    if($renamed -and !(Test-Path -LiteralPath $sdl)) { Move-Item -LiteralPath $backup -Destination $sdl }
    if($sdkCopied -and (Test-Path -LiteralPath $sdk) -and (Hash $sdk) -eq $sdkHash) { Remove-Item -LiteralPath $sdk }
    if($pthreadCopied -and (Test-Path -LiteralPath $pthread) -and (Hash $pthread) -eq $pthreadHash) { Remove-Item -LiteralPath $pthread }
    if($textureCopied -and (Test-Path -LiteralPath $texture) -and (Hash $texture) -eq $textureHash) { Remove-Item -LiteralPath $texture }
    foreach($stage in @($proxyStage,$sdkStage,$manifestStage,$pthreadStage,$textureStage)) {
        if(Test-Path -LiteralPath $stage) { Remove-Item -LiteralPath $stage }
    }
    throw
}
Write-Output 'Proxy installed. Launch NIMBY Rails normally through Steam; no external loader is required.'
Write-Output "Original SDL preserved at: $backup"
