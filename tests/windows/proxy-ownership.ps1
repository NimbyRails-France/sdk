# Self-contained transaction tests. Text fixtures only: no installed game, DLL
# execution, compilation, or release installation on the developer's machine.
$ErrorActionPreference = 'Stop'
$root = Join-Path ([IO.Path]::GetTempPath()) ('nrf-ownership-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $root | Out-Null
$source = Join-Path $root 'source'
New-Item -ItemType Directory -Path $source | Out-Null
$dlls = @('NimbyRailsFranceSDK.dll','libwinpthread-1.dll',
    'NimbyRailsFranceTextureBridge-experimental-v4.dll','NimbySignalUiBridge-experimental-v1.dll',
    'NimbyAutomaticDrivingBridge-v1.dll','NimbyConstructionBridge-experimental-v1.dll',
    'NimbyRailsFranceClockBridge-0.7.1.dll','NimbyModMetadataBridge-v1.dll')
$packaging = [IO.File]::ReadAllText("$PSScriptRoot/../../tools/windows/package-drop-in.ps1")
$packagedNames = @([regex]::Matches($packaging, "'([A-Za-z0-9_.-]+\.dll)'") | ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
foreach($name in $packagedNames) {
    if($name -ne 'SDL3.dll' -and $name -notin $dlls) { throw "Add ownership/recovery tests for newly packaged DLL: $name" }
}
foreach($name in $dlls + @('SDL3.dll')) { [IO.File]::WriteAllText((Join-Path $source $name), "fixture $name") }
$exeFixture = Join-Path $root 'exe.fixture'
$sdlFixture = Join-Path $root 'sdl.fixture'
[IO.File]::WriteAllText($exeFixture,'fake game executable')
[IO.File]::WriteAllText($sdlFixture,'original SDL fixture')
$legacyFixture = Join-Path $root 'legacy.fixture'
[IO.File]::WriteAllText($legacyFixture,'legacy dependency fixture')
$script = [IO.File]::ReadAllText("$PSScriptRoot/../../tools/windows/install-proxy.ps1")
$script = $script.Replace('FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE',(Get-FileHash $exeFixture).Hash)
$script = $script.Replace('2A2704678BF6C9C6A944270AB35079DF76F5AFE92B780394ED72D9C8218B98D8',(Get-FileHash $sdlFixture).Hash)
$script = $script.Replace('1179C0C0ED77ABB4AA92A14DB97F369CDF364D810167D251B0DFE466DB004C21',(Get-FileHash $legacyFixture).Hash)
$installer = Join-Path $root 'install.ps1'
[IO.File]::WriteAllText($installer,$script)
$failing = Join-Path $root 'fail.ps1'
[IO.File]::WriteAllText($failing,$script.Replace('    Move-Item -LiteralPath $manifestStage -Destination $manifestPath', "    throw 'injected failure before commit'"))
function NewGame {
    $path = Join-Path $root ([guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $path | Out-Null
    Copy-Item $exeFixture (Join-Path $path 'NimbyRails.exe')
    Copy-Item $sdlFixture (Join-Path $path 'SDL3.dll')
    [IO.File]::WriteAllText((Join-Path $path 'unrelated.dll'),'unrelated')
    return $path
}
function Snapshot($game) {
    return ((Get-ChildItem -LiteralPath $game -File | Sort-Object Name | ForEach-Object { $_.Name + ':' + (Get-FileHash -LiteralPath $_.FullName).Hash }) -join "`n")
}
function Run($game, $action, $success = $true, $runner = $installer) {
    $ErrorActionPreference = 'Continue'
    $output = & powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File $runner -Action $action -GameDirectory $game -SourceDirectory $source 2>&1
    $ErrorActionPreference = 'Stop'
    if(($LASTEXITCODE -eq 0) -ne $success) { throw "Unexpected $action result: $output" }
}
# Fresh installs own their DLLs; shared installs retain all pre-existing bytes.
foreach($shared in @($false,$true)) {
    $game = NewGame
    if($shared) { foreach($name in $dlls) { Copy-Item (Join-Path $source $name) (Join-Path $game $name) } }
    $before = Snapshot $game
    Run $game Install
    $manifest = Get-Content (Join-Path $game 'NimbyRailsFranceSDK-install.json') -Raw | ConvertFrom-Json
    if($shared -and ($manifest.format -ne 3 -or @($manifest.sharedFiles.PSObject.Properties).Count -ne $dlls.Count)) { throw 'Shared ownership not recorded' }
    Run $game Remove
    if((Snapshot $game) -ne $before) { throw 'Uninstall did not restore exact initial files' }
    Run $game Install $false $failing
    if((Snapshot $game) -ne $before) { throw 'Rollback did not preserve initial files' }
    # A second install/remove cycle covers same-version updates and ownership retention.
    Run $game Install
    Run $game Remove
    if((Snapshot $game) -ne $before) { throw 'Reinstall changed shared files' }
}
foreach($name in $dlls) {
    $game = NewGame
    [IO.File]::WriteAllText((Join-Path $game $name),'foreign bytes')
    $before = Snapshot $game
    Run $game Install $false
    if((Snapshot $game) -ne $before) { throw "Foreign DLL modified: $name" }
    # Reusing just one DLL must preserve it while removing all SDK-owned files.
    Copy-Item (Join-Path $source $name) (Join-Path $game $name) -Force
    $before = Snapshot $game
    Run $game Install
    [IO.File]::WriteAllText((Join-Path $game $name),'changed after install')
    $modified = Snapshot $game
    Run $game Remove $false
    if((Snapshot $game) -ne $modified) { throw "Removal changed files despite modified $name" }
    Copy-Item (Join-Path $source $name) (Join-Path $game $name) -Force
    Run $game Remove
    if((Snapshot $game) -ne $before) { throw "Shared DLL removed: $name" }
}
foreach($missing in $dlls + @('SDL3.dll','steam-restored')) {
    $game = NewGame
    $before = Snapshot $game
    Run $game Install
    if($missing -eq 'steam-restored') {
        Copy-Item $sdlFixture (Join-Path $game 'SDL3.dll') -Force
        Remove-Item -LiteralPath (Join-Path $game 'NimbyRailsSDL3Original.dll')
    } else { Remove-Item -LiteralPath (Join-Path $game $missing) }
    Run $game Remove
    if((Snapshot $game) -ne $before) { throw "Failed to recover missing $missing" }
}
# A process exit bypasses catch: durable intent must already exist.
$interrupted = Join-Path $root 'interrupt.ps1'
[IO.File]::WriteAllText($interrupted,$script.Replace('try {', "exit 91`ntry {"))
$game = NewGame
Run $game Install $false $interrupted
$pending = Join-Path $game 'NimbyRailsFranceSDK-install.tmp'
if(!(Test-Path $pending)) { throw 'No durable installation intent before mutation' }
$intent = Get-Content $pending -Raw | ConvertFrom-Json
if(!$intent.sdkSha256 -or !$intent.additionalBridges) { throw 'Incomplete recovery intent' }
# Upgrade of owned files, then rollback to the previous payload.
$game = NewGame
$before = Snapshot $game
Run $game Install
Run $game Remove
foreach($name in $dlls) { [IO.File]::WriteAllText((Join-Path $source $name), "updated $name") }
Run $game Install
foreach($name in $dlls) {
    if((Get-FileHash (Join-Path $game $name)).Hash -ne (Get-FileHash (Join-Path $source $name)).Hash) { throw "Update not installed: $name" }
}
Run $game Remove
foreach($name in $dlls) { [IO.File]::WriteAllText((Join-Path $source $name), "fixture $name") }
Run $game Install
Run $game Remove
if((Snapshot $game) -ne $before) { throw 'Update/rollback changed original files' }
# Removal interrupted immediately after SDL restoration remains resumable.
$partialRemove = Join-Path $root 'partial-remove.ps1'
[IO.File]::WriteAllText($partialRemove,$script.Replace('    RemoveOwned $sdk', "    exit 92`n    RemoveOwned `$sdk"))
$game = NewGame
$before = Snapshot $game
Run $game Install
Run $game Remove $false $partialRemove
Run $game Remove
if((Snapshot $game) -ne $before) { throw 'Interrupted removal cannot resume' }
# A recognized legacy dependency is replaced, but its original bytes remain
# recoverable on uninstall, failed install, and repeated update/rollback cycles.
$game = NewGame
Copy-Item $legacyFixture (Join-Path $game 'libwinpthread-1.dll')
$before = Snapshot $game
Run $game Install
if((Get-FileHash (Join-Path $game 'libwinpthread-1.dll')).Hash -ne (Get-FileHash (Join-Path $source 'libwinpthread-1.dll')).Hash) { throw 'Legacy dependency was not upgraded' }
if((Get-FileHash (Join-Path $game 'libwinpthread-1.dll.nrf-before-sdk')).Hash -ne (Get-FileHash $legacyFixture).Hash) { throw 'Original legacy dependency not backed up' }
Run $game Remove
if((Snapshot $game) -ne $before) { throw 'Legacy dependency not restored on removal' }
Run $game Install $false $failing
if((Snapshot $game) -ne $before) { throw 'Legacy dependency not restored on failure' }
Run $game Install
Run $game Remove $false $partialRemove
Run $game Remove
if((Snapshot $game) -ne $before) { throw 'Legacy restoration cannot resume' }
Write-Output 'PASS: all 8 payload DLLs, ownership, reuse, mismatches, rollback, reinstall, removal and unrelated files'
Write-Output "Fixtures and evidence: $root"
