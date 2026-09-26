param(
    [Parameter(Mandatory=$true)][string]$BuildDirectory,
    [string]$ClionHome = "$env:LOCALAPPDATA/Programs/CLion"
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$version=(Get-Content -LiteralPath "$root/VERSION" -Raw).Trim()
# These scripts only create local artifacts. They never install into the game
# or upload a release. Every DLL comes from the same tested Release build.
& "$PSScriptRoot/package.ps1" -SkipBuild -BuildDirectory $BuildDirectory -ClionHome $ClionHome
& "$PSScriptRoot/package-drop-in.ps1" -SkipBuild -BuildDirectory $BuildDirectory -ClionHome $ClionHome
& "$PSScriptRoot/package-hub-sdk.ps1" -SdkRoot "$root/dist/NimbyRailsFranceSDK-$version"
