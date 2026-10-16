# CLion installations may be per-user, versioned, or managed by Toolbox.
# Resolve a complete toolchain without executing tools or changing the registry.
function Resolve-NrfClionHome {
    [CmdletBinding()]
    param(
        [AllowEmptyString()][string]$PreferredHome,
        [AllowEmptyString()][string]$LocalAppData = $env:LOCALAPPDATA,
        [AllowEmptyCollection()][string[]]$ProgramFilesRoots = @($env:ProgramFiles, ${env:ProgramFiles(x86)}, $env:ProgramW6432),
        [AllowEmptyCollection()][string[]]$RegistryRoots = @(
            'HKCU:/Software/Microsoft/Windows/CurrentVersion/Uninstall',
            'HKLM:/Software/Microsoft/Windows/CurrentVersion/Uninstall',
            'HKLM:/Software/WOW6432Node/Microsoft/Windows/CurrentVersion/Uninstall'
        )
    )

    $requiredTools = @(
        'bin/cmake/win/x64/bin/cmake.exe',
        'bin/cmake/win/x64/bin/ctest.exe',
        'bin/mingw/bin/gcc.exe',
        'bin/mingw/bin/g++.exe',
        'bin/ninja/win/x64/ninja.exe'
    )

    function Get-MissingTools([string]$HomeDirectory) {
        foreach ($relative in $requiredTools) {
            if (!(Test-Path -LiteralPath (Join-Path $HomeDirectory $relative) -PathType Leaf -ErrorAction SilentlyContinue)) {
                $relative
            }
        }
    }

    # An explicit choice must not silently select a different installation.
    if (![string]::IsNullOrWhiteSpace($PreferredHome)) {
        try { $resolved = [IO.Path]::GetFullPath($PreferredHome) }
        catch { throw "Invalid CLion path: $PreferredHome. Set NRF_CLION_HOME to the CLion installation directory." }
        $missing = @(Get-MissingTools $resolved)
        if ($missing.Count) {
            throw "Incomplete CLion toolchain: $resolved. Missing: $($missing -join ', '). Set NRF_CLION_HOME to the CLion installation directory containing CMake, CTest, MinGW (gcc/g++) and Ninja."
        }
        return $resolved
    }

    $candidates = New-Object 'System.Collections.Generic.List[string]'
    $seen = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    function Add-Candidate([string]$HomeDirectory) {
        if ([string]::IsNullOrWhiteSpace($HomeDirectory)) { return }
        try { $absolute = [IO.Path]::GetFullPath($HomeDirectory) }
        catch { return }
        if ($seen.Add($absolute)) { $candidates.Add($absolute) }
    }
    function Get-ChildDirectories([string]$Directory) {
        if (![string]::IsNullOrWhiteSpace($Directory)) {
            Get-ChildItem -LiteralPath $Directory -Directory -ErrorAction SilentlyContinue
        }
    }

    if (![string]::IsNullOrWhiteSpace($LocalAppData)) {
        $programs = Join-Path $LocalAppData 'Programs'
        Add-Candidate (Join-Path $programs 'CLion')
        foreach ($directory in @(Get-ChildDirectories $programs)) {
            if ($directory.Name -match '(?i)^clion(?:$|[ .\d-])') { Add-Candidate $directory.FullName }
        }

        # Current Toolbox can install directly in apps/clion. Older layouts use
        # apps/CLion/ch-0/<version>. Inspect only these bounded application trees.
        $apps = Join-Path $LocalAppData 'JetBrains/Toolbox/apps'
        foreach ($application in @(Get-ChildDirectories $apps)) {
            if ($application.Name -notmatch '(?i)^clion(?:$|[ .\d-])') { continue }
            Add-Candidate $application.FullName
            $level = @($application)
            for ($depth = 0; $depth -lt 3; $depth++) {
                $next = New-Object 'System.Collections.Generic.List[object]'
                foreach ($parent in $level) {
                    foreach ($directory in @(Get-ChildDirectories $parent.FullName)) {
                        if ($directory.Name -match '(?i)^(?:ch-\d+|\d[^\\/]*|clion(?:$|[ .\d-]).*)$') {
                            Add-Candidate $directory.FullName
                            $next.Add($directory)
                        }
                    }
                }
                $level = @($next.ToArray())
                if (!$level.Count) { break }
            }
        }
    }

    foreach ($programFiles in $ProgramFilesRoots) {
        if ([string]::IsNullOrWhiteSpace($programFiles)) { continue }
        $jetbrains = Join-Path $programFiles 'JetBrains'
        Add-Candidate (Join-Path $jetbrains 'CLion')
        foreach ($directory in @(Get-ChildDirectories $jetbrains)) {
            if ($directory.Name -match '(?i)^clion(?:$|[ .\d-])') { Add-Candidate $directory.FullName }
        }
    }

    # Only uninstall entries positively identified as CLion are considered.
    # Tests pass an empty RegistryRoots array to avoid accessing the host registry.
    foreach ($registryRoot in $RegistryRoots) {
        if ([string]::IsNullOrWhiteSpace($registryRoot)) { continue }
        foreach ($entry in @(Get-ChildItem -LiteralPath $registryRoot -ErrorAction SilentlyContinue)) {
            $properties = Get-ItemProperty -LiteralPath $entry.PSPath -ErrorAction SilentlyContinue
            if ($null -ne $properties -and $properties.DisplayName -match '(?i)^(?:JetBrains\s+)?CLion(?:\s|$)') {
                Add-Candidate $properties.InstallLocation
            }
        }
    }

    function Get-InstallationVersion([string]$HomeDirectory) {
        $text = $null
        $info = Join-Path $HomeDirectory 'product-info.json'
        if (Test-Path -LiteralPath $info -PathType Leaf -ErrorAction SilentlyContinue) {
            try {
                if ((Get-Item -LiteralPath $info -ErrorAction Stop).Length -le 65536) {
                    $text = (Get-Content -LiteralPath $info -Raw -ErrorAction Stop | ConvertFrom-Json -ErrorAction Stop).version
                }
            } catch { $text = $null }
        }
        if (!$text -or $text -notmatch '^(\d+(?:\.\d+){1,3})') {
            if ($HomeDirectory -match '(?i)(?:^|[\\/])CLion[ .-]*(\d+(?:\.\d+){1,3})(?:[\\/]|$)') {
                $text = $Matches[1]
            } elseif ((Split-Path -Leaf $HomeDirectory) -match '^(\d+(?:\.\d+){1,3})') {
                $text = $Matches[1]
            } else { $text = '0.0' }
        } else { $text = $Matches[1] }
        try { return [version]$text }
        catch { return [version]'0.0' }
    }

    $complete = New-Object 'System.Collections.Generic.List[object]'
    $diagnostics = New-Object 'System.Collections.Generic.List[string]'
    foreach ($candidate in $candidates) {
        $missing = @(Get-MissingTools $candidate)
        if (!$missing.Count) {
            $complete.Add([pscustomobject]@{ Home = $candidate; Version = Get-InstallationVersion $candidate })
        } else {
            $diagnostics.Add("$candidate (missing: $($missing -join ', '))")
        }
    }
    if ($complete.Count) {
        $selected = $complete | Sort-Object -Property @{ Expression = 'Version'; Descending = $true }, @{ Expression = 'Home'; Descending = $false } | Select-Object -First 1
        return $selected.Home
    }
    $searched = if ($diagnostics.Count) { $diagnostics -join '; ' } else { '(no installation candidates)' }
    throw "No complete CLion toolchain found. Searched: $searched. Set NRF_CLION_HOME to the CLion installation directory containing CMake, CTest, MinGW (gcc/g++) and Ninja."
}
