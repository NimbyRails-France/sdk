param([string]$ProofDirectory)
$ErrorActionPreference = 'Stop'
. "$PSScriptRoot/../../tools/windows/resolve-clion.ps1"

$sandbox = Join-Path ([IO.Path]::GetTempPath()) ('nrf-clion-detection-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $sandbox | Out-Null
$results = New-Object 'System.Collections.Generic.List[object]'
$tools = @('bin/cmake/win/x64/bin/cmake.exe', 'bin/cmake/win/x64/bin/ctest.exe', 'bin/mingw/bin/gcc.exe', 'bin/mingw/bin/g++.exe', 'bin/ninja/win/x64/ninja.exe')

function New-Toolchain([string]$HomeDirectory, [string]$Missing = '') {
    foreach ($tool in $tools) {
        if ($tool -eq $Missing) { continue }
        $file = Join-Path $HomeDirectory $tool
        New-Item -ItemType Directory -Path (Split-Path -Parent $file) -Force | Out-Null
        New-Item -ItemType File -Path $file -Force | Out-Null
    }
    return [IO.Path]::GetFullPath($HomeDirectory)
}
function Assert-Equal($Expected, $Actual) {
    if ($Expected -cne $Actual) { throw "Expected <$Expected>, got <$Actual>" }
}
function Assert-Fails([scriptblock]$Action, [string[]]$Contains) {
    $failure = $null
    try { & $Action | Out-Null } catch { $failure = $_.Exception.Message }
    if (!$failure) { throw 'Expected a failed toolchain lookup' }
    foreach ($fragment in $Contains) {
        if (!$failure.Contains($fragment)) { throw "Missing diagnostic <$fragment> in <$failure>" }
    }
}
function Test-Case([string]$Name, [scriptblock]$Action) {
    & $Action
    $results.Add([pscustomobject]@{ name = $Name; passed = $true })
    Write-Output "PASS $Name"
}

try {
    Test-Case 'explicit root with spaces returns one string' {
        $installationRoot = New-Toolchain (Join-Path $sandbox 'explicit/CLion with spaces')
        $actual = @(Resolve-NrfClionHome -PreferredHome $installationRoot -LocalAppData '' -ProgramFilesRoots @() -RegistryRoots @())
        Assert-Equal 1 $actual.Count
        Assert-Equal $installationRoot $actual[0]
    }
    Test-Case 'invalid explicit root never falls back to a valid automatic root' {
        $local = Join-Path $sandbox 'explicit-invalid/local'
        $null = New-Toolchain (Join-Path $local 'Programs/CLion')
        $invalid = Join-Path $sandbox 'explicit-invalid/no-such-installation'
        Assert-Fails { Resolve-NrfClionHome -PreferredHome $invalid -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @() } @($invalid, 'NRF_CLION_HOME', 'cmake.exe')
    }
    Test-Case 'standard per-user installation' {
        $local = Join-Path $sandbox 'per-user/local'
        $installationRoot = New-Toolchain (Join-Path $local 'Programs/CLion')
        Assert-Equal $installationRoot (Resolve-NrfClionHome -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @())
    }
    Test-Case 'version comparison is numeric: 2026.10 beats 2026.9' {
        $local = Join-Path $sandbox 'versions/local'
        $null = New-Toolchain (Join-Path $local 'Programs/CLion 2026.9')
        $installationRoot = New-Toolchain (Join-Path $local 'Programs/CLion 2026.10')
        Assert-Equal $installationRoot (Resolve-NrfClionHome -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @())
    }
    Test-Case 'versioned Program Files installation' {
        $programFiles = Join-Path $sandbox 'Program Files'
        $installationRoot = New-Toolchain (Join-Path $programFiles 'JetBrains/CLion 2026.2.3')
        Assert-Equal $installationRoot (Resolve-NrfClionHome -LocalAppData '' -ProgramFilesRoots @($programFiles) -RegistryRoots @())
    }
    Test-Case 'legacy Toolbox channel and version directories' {
        $local = Join-Path $sandbox 'toolbox-legacy/local'
        $installationRoot = New-Toolchain (Join-Path $local 'JetBrains/Toolbox/apps/CLion/ch-0/262.12345.1')
        Assert-Equal $installationRoot (Resolve-NrfClionHome -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @())
    }
    Test-Case 'current Toolbox direct application directory' {
        $local = Join-Path $sandbox 'toolbox-current/local'
        $installationRoot = New-Toolchain (Join-Path $local 'JetBrains/Toolbox/apps/clion')
        Assert-Equal $installationRoot (Resolve-NrfClionHome -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @())
    }
    Test-Case 'incomplete newest installation falls back to a complete one' {
        $local = Join-Path $sandbox 'incomplete/local'
        $null = New-Toolchain (Join-Path $local 'Programs/CLion 2026.10') 'bin/mingw/bin/gcc.exe'
        $installationRoot = New-Toolchain (Join-Path $local 'Programs/CLion 2026.9')
        Assert-Equal $installationRoot (Resolve-NrfClionHome -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @())
    }
    Test-Case 'missing gcc is reported as an incomplete explicit toolchain' {
        $installationRoot = New-Toolchain (Join-Path $sandbox 'no-gcc/CLion') 'bin/mingw/bin/gcc.exe'
        Assert-Fails { Resolve-NrfClionHome -PreferredHome $installationRoot -LocalAppData '' -ProgramFilesRoots @() -RegistryRoots @() } @('bin/mingw/bin/gcc.exe', 'NRF_CLION_HOME')
    }
    Test-Case 'no installation produces a useful searched-path diagnostic' {
        $local = Join-Path $sandbox 'absent/local'
        Assert-Fails { Resolve-NrfClionHome -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @() } @('No complete CLion toolchain', (Join-Path $local 'Programs/CLion'), 'NRF_CLION_HOME')
    }
    Test-Case 'empty search roots produce a useful absence diagnostic' {
        Assert-Fails { Resolve-NrfClionHome -LocalAppData '' -ProgramFilesRoots @() -RegistryRoots @() } @('no installation candidates', 'NRF_CLION_HOME')
    }
    Test-Case 'product metadata supplies the version of an unversioned directory' {
        $local = Join-Path $sandbox 'metadata/local'
        $installationRoot = New-Toolchain (Join-Path $local 'Programs/CLion')
        $null = New-Toolchain (Join-Path $local 'Programs/CLion 2026.9')
        '{"version":"2026.10.1"}' | Set-Content -LiteralPath (Join-Path $installationRoot 'product-info.json') -Encoding UTF8
        Assert-Equal $installationRoot (Resolve-NrfClionHome -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @())
    }
    Test-Case 'Toolbox search does not traverse arbitrary trees' {
        $local = Join-Path $sandbox 'bounded/local'
        $null = New-Toolchain (Join-Path $local 'JetBrains/Toolbox/apps/clion/plugins/arbitrary/2026.10')
        Assert-Fails { Resolve-NrfClionHome -LocalAppData $local -ProgramFilesRoots @() -RegistryRoots @() } @('No complete CLion toolchain', 'NRF_CLION_HOME')
    }
    if ($ProofDirectory) {
        New-Item -ItemType Directory -Path $ProofDirectory -Force | Out-Null
        [ordered]@{ tests = $results.Count; failed = 0; cases = @($results.ToArray()); syntheticToolsOnly = $true; hostRegistryRead = $false } |
            ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $ProofDirectory 'clion-toolchain-tests.json') -Encoding UTF8
    }
    Write-Output "$($results.Count) CLion toolchain tests passed. No compiler was executed."
} finally {
    # The target was created above from a fixed prefix and a GUID beneath Temp.
    $temporaryRoot = [IO.Path]::GetFullPath([IO.Path]::GetTempPath()).TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
    $resolvedSandbox = [IO.Path]::GetFullPath($sandbox)
    if (!$resolvedSandbox.StartsWith($temporaryRoot, [StringComparison]::OrdinalIgnoreCase) -or
        (Split-Path -Leaf $resolvedSandbox) -notmatch '^nrf-clion-detection-[0-9a-f]{32}$') {
        throw "Refusing to remove unexpected test directory: $resolvedSandbox"
    }
    Remove-Item -LiteralPath $resolvedSandbox -Recurse -Force
}
