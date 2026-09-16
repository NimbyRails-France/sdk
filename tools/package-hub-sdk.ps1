param([Parameter(Mandatory=$true)][string]$SdkRoot,[string]$Version='0.7.1')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$parent=Join-Path $root ('build/hub-sdk-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $parent | Out-Null
$stage=Join-Path $parent "NimbyRailsFranceSDK-$Version"
Copy-Item -LiteralPath $SdkRoot -Destination $stage -Recurse
Copy-Item -LiteralPath "$root/dist/NimbyRailsFranceSDK-$Version-drop-in" -Destination "$stage/loader" -Recurse
$asset="$root/dist/NimbyRailsFranceSDK-$Version-hub.zip"
Compress-Archive -LiteralPath $stage -DestinationPath $asset -Force
$metadata = [ordered]@{
    url = "https://github.com/NimbyRails-France/sdk/releases/download/v$Version/$([IO.Path]::GetFileName($asset))"
    id = 'sdk'
    kind = 'sdk'
    size = (Get-Item -LiteralPath $asset).Length
    version = $Version
    rootFolder = "NimbyRailsFranceSDK-$Version"
    name = 'NimbyRailsFranceSDK + NRF Loader'
    sha256 = (Get-FileHash -LiteralPath $asset -Algorithm SHA256).Hash.ToLowerInvariant()
    gameSha256 = @('fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae')
}
$metadata | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath "$root/dist/project.json" -Encoding UTF8
$releaseFiles = @(
    "$root/dist/NimbyRailsFranceSDK-$Version-windows-x64-mingw.zip",
    "$root/dist/NimbyRailsFranceSDK-$Version-drop-in-windows-x64.zip",
    $asset,
    "$root/dist/project.json"
)
$checksums = foreach($file in $releaseFiles) {
    if(!(Test-Path -LiteralPath $file)) { throw "Release asset missing: $file" }
    "$((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant())  $([IO.Path]::GetFileName($file))"
}
$checksums | Set-Content -LiteralPath "$root/dist/SHA256SUMS.txt" -Encoding ascii
Write-Output $asset
