param([Parameter(Mandatory=$true)][string]$SdkRoot,[string]$Version='0.6.0')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$parent=Join-Path $root ('build/hub-sdk-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $parent | Out-Null
$stage=Join-Path $parent "NimbyRailsSDK-$Version"
Copy-Item -LiteralPath $SdkRoot -Destination $stage -Recurse
Copy-Item -LiteralPath "$root/dist/NimbyRailsSDK-$Version-drop-in" -Destination "$stage/loader" -Recurse
$asset="$root/dist/NimbyRailsSDK-$Version-hub.zip"
Compress-Archive -LiteralPath $stage -DestinationPath $asset -Force
Write-Output $asset
