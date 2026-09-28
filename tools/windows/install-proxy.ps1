param(
    [ValidateSet('Install','Remove')][string]$Action = 'Install',
    [string]$GameDirectory = 'C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails',
    [string]$SourceDirectory = "$PSScriptRoot/../../build/Release/drop-in"
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
$textureName = 'NimbyRailsFranceTextureBridge-experimental-v4.dll'
$texture = Join-Path $gameRoot $textureName
$textureStage = Join-Path $gameRoot 'NimbyRailsFranceTextureBridge.NimbySDK.tmp'
$uiName = 'NimbySignalUiBridge-experimental-v1.dll'
$drivingName = 'NimbyAutomaticDrivingBridge-v1.dll'
$ui = Join-Path $gameRoot $uiName
$driving = Join-Path $gameRoot $drivingName
$uiStage = Join-Path $gameRoot 'NimbySignalUiBridge.NimbySDK.tmp'
$drivingStage = Join-Path $gameRoot 'NimbyAutomaticDrivingBridge.NimbySDK.tmp'
$constructionName = 'NimbyConstructionBridge-experimental-v1.dll'
$construction = Join-Path $gameRoot $constructionName
$constructionStage = Join-Path $gameRoot 'NimbyConstructionBridge.NimbySDK.tmp'
$manifestStage = Join-Path $gameRoot 'NimbyRailsFranceSDK-install.tmp'
# Optional for older archives, included by current SDK packages. The manifest
# may name only these exact files; ownership still requires their recorded hash.
$extraNames = @('NimbyRailsFranceClockBridge-0.7.1.dll', 'NimbyModMetadataBridge-v1.dll')
$extraBridges = @($extraNames | ForEach-Object { @{
    name=$_; path=(Join-Path $gameRoot $_); stage=(Join-Path $gameRoot ($_+'.NimbySDK.tmp')); hash=$null; copied=$false
} })
$originalExeHash = 'FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE'
$originalSdlHash = '2A2704678BF6C9C6A944270AB35079DF76F5AFE92B780394ED72D9C8218B98D8'
function Hash([string]$path) { (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
function RequireHash([string]$path, [string]$expected) {
    if (!(Test-Path -LiteralPath $path) -or (Hash $path) -ne $expected) { throw "Unexpected file contents: $path. Nothing will be overwritten." }
}
foreach($target in @($exe,$sdl,$backup,$sdk,$manifestPath,$proxyStage,$sdkStage,$manifestStage,$pthread,$pthreadStage,$texture,$textureStage,$ui,$uiStage,$driving,$drivingStage,$construction,$constructionStage)) {
    if([IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($target)) -ne $gameRoot) { throw 'Path outside selected game directory' }
}
foreach($extra in $extraBridges) {
    foreach($target in @($extra.path,$extra.stage)) {
        if([IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($target)) -ne $gameRoot) { throw 'Bridge path outside selected game directory' }
    }
}
$running = Get-Process -Name NimbyRails -ErrorAction SilentlyContinue | Where-Object { !$_.Path -or $_.Path -ieq $exe }
if($running) { throw 'Close NIMBY Rails before installing or removing the proxy. The installer will not close the game.' }
if(Get-Process -Name NimbyRailsFranceLoader -ErrorAction SilentlyContinue) { throw 'Stop NimbyRailsFranceLoader first with --stop.' }

if($Action -eq 'Remove') {
    if(!(Test-Path -LiteralPath $manifestPath)) { throw 'No proxy installation manifest found.' }
    $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
    if($manifest.additionalBridges) {
        foreach($entry in $manifest.additionalBridges.PSObject.Properties) {
            if($entry.Name -notin $extraNames -or $entry.Value -notmatch '^[a-fA-F0-9]{64}$') { throw 'Invalid additional bridge in manifest' }
            $extra = $extraBridges | Where-Object { $_.name -eq $entry.Name }
            $extra.hash=$entry.Value
            RequireHash $extra.path $extra.hash
        }
    }
    if($manifest.textureBridgeSha256) {
        $allowedBridges = 1..4 | ForEach-Object { "NimbyRailsFranceTextureBridge-experimental-v$_.dll" }
        if($manifest.textureBridgeFile) {
            if($manifest.textureBridgeFile -notin $allowedBridges) { throw 'Invalid texture bridge filename in manifest' }
            $texture = Join-Path $gameRoot $manifest.textureBridgeFile
        } else {
            # Older manifests stored only a hash. Locate exactly one known
            # filename with that hash; never remove an unowned neighbouring DLL.
            $matches = @($allowedBridges | ForEach-Object { Join-Path $gameRoot $_ } | Where-Object {
                (Test-Path -LiteralPath $_) -and (Hash $_) -eq $manifest.textureBridgeSha256
            })
            if($matches.Count -ne 1) { throw 'Cannot uniquely identify the installed texture bridge' }
            $texture = $matches[0]
        }
    }
    RequireHash $sdl $manifest.proxySha256
    RequireHash $sdk $manifest.sdkSha256
    RequireHash $backup $originalSdlHash
    if($manifest.pthreadSha256) { RequireHash $pthread $manifest.pthreadSha256 }
    if($manifest.textureBridgeSha256) { RequireHash $texture $manifest.textureBridgeSha256 }
    if($manifest.signalUiBridgeSha256) {
        if($manifest.signalUiBridgeFile -ne $uiName) { throw 'Invalid signal UI bridge filename in manifest' }
        RequireHash $ui $manifest.signalUiBridgeSha256
    }
    if($manifest.automaticDrivingBridgeSha256) {
        if($manifest.automaticDrivingBridgeFile -ne $drivingName) { throw 'Invalid automatic driving bridge filename in manifest' }
        RequireHash $driving $manifest.automaticDrivingBridgeSha256
    }
    if($manifest.constructionBridgeSha256) {
        if($manifest.constructionBridgeFile -ne $constructionName) { throw 'Invalid construction bridge filename in manifest' }
        RequireHash $construction $manifest.constructionBridgeSha256
    }
    # Fixed filenames and verified contents only; no recursive deletion.
    Remove-Item -LiteralPath $sdl
    Move-Item -LiteralPath $backup -Destination $sdl
    Remove-Item -LiteralPath $sdk
    if($manifest.pthreadSha256) { Remove-Item -LiteralPath $pthread }
    if($manifest.textureBridgeSha256) { Remove-Item -LiteralPath $texture }
    if($manifest.signalUiBridgeSha256) { Remove-Item -LiteralPath $ui }
    if($manifest.automaticDrivingBridgeSha256) { Remove-Item -LiteralPath $driving }
    if($manifest.constructionBridgeSha256) { Remove-Item -LiteralPath $construction }
    foreach($extra in $extraBridges) { if($extra.hash) { Remove-Item -LiteralPath $extra.path } }
    Remove-Item -LiteralPath $manifestPath
    RequireHash $sdl $originalSdlHash
    Write-Output 'Original SDL3.dll restored. Proxy and SDK removed.'
    exit 0
}

RequireHash $exe $originalExeHash
RequireHash $sdl $originalSdlHash
foreach($target in @($backup,$sdk,$manifestPath,$proxyStage,$sdkStage,$manifestStage,$pthreadStage,$texture,$textureStage,$ui,$uiStage,$driving,$drivingStage,$construction,$constructionStage)) {
    if(Test-Path -LiteralPath $target) { throw "File already exists; refusing to overwrite: $target" }
}
foreach($extra in $extraBridges) {
    foreach($target in @($extra.path,$extra.stage)) {
        if(Test-Path -LiteralPath $target) { throw "File already exists; refusing to overwrite: $target" }
    }
}
$sourceRoot = (Resolve-Path -LiteralPath $SourceDirectory).Path
foreach($extra in $extraBridges) {
    $extra.source=Join-Path $sourceRoot $extra.name
    if(Test-Path -LiteralPath $extra.source) { $extra.hash=Hash $extra.source }
}
$proxySource = Join-Path $sourceRoot 'SDL3.dll'
$sdkSource = Join-Path $sourceRoot 'NimbyRailsFranceSDK.dll'
$proxyHash = Hash $proxySource
$sdkHash = Hash $sdkSource
$textureSource = Join-Path $sourceRoot $textureName
$textureHash = Hash $textureSource
$uiSource = Join-Path $sourceRoot $uiName
$drivingSource = Join-Path $sourceRoot $drivingName
$uiHash = Hash $uiSource
$drivingHash = Hash $drivingSource
$constructionSource = Join-Path $sourceRoot $constructionName
$constructionHash = $null
if(Test-Path -LiteralPath $constructionSource) { $constructionHash = Hash $constructionSource }
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
$uiCopied = $false
$drivingCopied = $false
$constructionCopied = $false
try {
    $additionalBridges=[ordered]@{}
    foreach($extra in $extraBridges) {
        if($extra.hash) {
            Copy-Item -LiteralPath $extra.source -Destination $extra.stage
            RequireHash $extra.stage $extra.hash
            Move-Item -LiteralPath $extra.stage -Destination $extra.path
            $extra.copied=$true
            $additionalBridges[$extra.name]=$extra.hash
        }
    }
    # Validate staged copies before changing any game dependency.
    Copy-Item -LiteralPath $sdkSource -Destination $sdkStage
    Copy-Item -LiteralPath $proxySource -Destination $proxyStage
    RequireHash $sdkStage $sdkHash
    Copy-Item -LiteralPath $textureSource -Destination $textureStage
    RequireHash $textureStage $textureHash
    Move-Item -LiteralPath $textureStage -Destination $texture
    $textureCopied = $true
    Copy-Item -LiteralPath $uiSource -Destination $uiStage
    Copy-Item -LiteralPath $drivingSource -Destination $drivingStage
    RequireHash $uiStage $uiHash
    RequireHash $drivingStage $drivingHash
    Move-Item -LiteralPath $uiStage -Destination $ui
    $uiCopied = $true
    Move-Item -LiteralPath $drivingStage -Destination $driving
    $drivingCopied = $true
    if($constructionHash) {
        Copy-Item -LiteralPath $constructionSource -Destination $constructionStage
        RequireHash $constructionStage $constructionHash
        Move-Item -LiteralPath $constructionStage -Destination $construction
        $constructionCopied = $true
    }
    RequireHash $proxyStage $proxyHash
    if($pthreadHash) {
        Copy-Item -LiteralPath $pthreadSource -Destination $pthreadStage
        RequireHash $pthreadStage $pthreadHash
        Move-Item -LiteralPath $pthreadStage -Destination $pthread
        $pthreadCopied = $true
    }
    [ordered]@{format=2; installedUtc=[DateTime]::UtcNow.ToString('o'); executableSha256=$originalExeHash;
        additionalBridges=$additionalBridges;
        originalSdlSha256=$originalSdlHash; proxySha256=$proxyHash; sdkSha256=$sdkHash; pthreadSha256=$pthreadHash;
        textureBridgeFile=$textureName; textureBridgeSha256=$textureHash;
        signalUiBridgeFile=$uiName; signalUiBridgeSha256=$uiHash;
        automaticDrivingBridgeFile=$drivingName; automaticDrivingBridgeSha256=$drivingHash;
        constructionBridgeFile=$(if($constructionHash){$constructionName}else{$null}); constructionBridgeSha256=$constructionHash} |
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
    if($uiCopied -and (Test-Path -LiteralPath $ui) -and (Hash $ui) -eq $uiHash) { Remove-Item -LiteralPath $ui }
    if($drivingCopied -and (Test-Path -LiteralPath $driving) -and (Hash $driving) -eq $drivingHash) { Remove-Item -LiteralPath $driving }
    if($constructionCopied -and (Test-Path -LiteralPath $construction) -and (Hash $construction) -eq $constructionHash) { Remove-Item -LiteralPath $construction }
    foreach($extra in $extraBridges) {
        if($extra.copied -and (Test-Path -LiteralPath $extra.path) -and (Hash $extra.path) -eq $extra.hash) { Remove-Item -LiteralPath $extra.path }
        if(Test-Path -LiteralPath $extra.stage) { Remove-Item -LiteralPath $extra.stage }
    }
    foreach($stage in @($proxyStage,$sdkStage,$manifestStage,$pthreadStage,$textureStage,$uiStage,$drivingStage,$constructionStage)) {
        if(Test-Path -LiteralPath $stage) { Remove-Item -LiteralPath $stage }
    }
    throw
}
Write-Output 'Proxy installed. Launch NIMBY Rails normally through Steam; no external loader is required.'
Write-Output "Original SDL preserved at: $backup"
