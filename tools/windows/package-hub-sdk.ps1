param([Parameter(Mandatory=$true)][string]$SdkRoot,[string]$Version='', [string]$OutputRoot, [switch]$LocalBuild)
$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$currentVersion=(Get-Content -LiteralPath "$root/VERSION" -Raw).Trim()
if(!$Version){$Version=$currentVersion}
if(!$OutputRoot){$OutputRoot=Join-Path $root 'dist'}
if($Version -notmatch '^\d+\.\d+\.\d+(?:-(?:alpha|beta)\.[1-9]\d*)?$' -or $Version -ne $currentVersion){throw 'Package version must match VERSION'}
if(!$LocalBuild -and (Test-Path -LiteralPath "$SdkRoot/bin/NimbyConstructionBridge-experimental-v1.dll")){
    throw 'The construction qualification bridge belongs to local developer kits, not public releases.'
}
$parent=Join-Path $root ('build/hub-sdk-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $parent | Out-Null
$stage=Join-Path $parent "NimbyRailsFranceSDK-$Version"
Copy-Item -LiteralPath $SdkRoot -Destination $stage -Recurse
Copy-Item -LiteralPath "$OutputRoot/NimbyRailsFranceSDK-$Version-drop-in" -Destination "$stage/loader" -Recurse
$asset="$OutputRoot/NimbyRailsFranceSDK-$Version-hub.zip"
Compress-Archive -LiteralPath $stage -DestinationPath $asset -Force
$metadata = [ordered]@{
    url = "https://github.com/NimbyRails-France/sdk/releases/download/v$Version/$([IO.Path]::GetFileName($asset))"
    id = 'sdk'
    kind = 'sdk'
    platform = 'windows-x64'
    channel = $(if($Version -match '-(alpha|beta)\.'){ $Matches[1] }else{ 'stable' })
    loaderApi = 1
    size = (Get-Item -LiteralPath $asset).Length
    version = $Version
    rootFolder = "NimbyRailsFranceSDK-$Version"
    name = 'NimbyRailsFranceSDK + NRF Loader'
    sha256 = (Get-FileHash -LiteralPath $asset -Algorithm SHA256).Hash.ToLowerInvariant()
    gameSha256 = @('fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae')
}
$metadata | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath "$OutputRoot/project.json" -Encoding UTF8
$metadata | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath "$OutputRoot/project-windows-x64.json" -Encoding UTF8
if($LocalBuild) { Write-Output $asset; return }
$releaseFiles = @(
    "$OutputRoot/NimbyRailsFranceSDK-$Version-windows-x64-mingw.zip",
    "$OutputRoot/NimbyRailsFranceSDK-$Version-drop-in-windows-x64.zip",
    $asset,
    "$OutputRoot/project-windows-x64.json",
    "$OutputRoot/project.json"
)
$checksums = foreach($file in $releaseFiles) {
    if(!(Test-Path -LiteralPath $file)) { throw "Release asset missing: $file" }
    "$((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant())  $([IO.Path]::GetFileName($file))"
}
$checksums | Set-Content -LiteralPath "$OutputRoot/SHA256SUMS.txt" -Encoding ascii
Write-Output $asset
