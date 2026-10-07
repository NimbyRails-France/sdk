param([string]$OutputRoot)
$ErrorActionPreference='Stop'
$sdkSource=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
if(!$OutputRoot){$OutputRoot=Join-Path $sdkSource ('build/metadata-tests-'+[guid]::NewGuid().ToString('N'))}
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
$version='0.9.0-alpha.1'
$cases=@(
    @{name='absent';policy=@{};expected=$null;valid=$true},
    @{name='stable';policy=@{developmentStatus='stable'};expected='stable';valid=$true},
    @{name='development';policy=@{developmentStatus='in-development'};expected='in-development';valid=$true},
    @{name='null';policy=@{developmentStatus=$null};valid=$false},
    @{name='boolean';policy=@{developmentStatus=$true};valid=$false},
    @{name='number';policy=@{developmentStatus=1};valid=$false},
    @{name='unknown';policy=@{developmentStatus='beta'};valid=$false},
    @{name='capitalized';policy=@{developmentStatus='Stable'};valid=$false},
    @{name='array';policy=@{developmentStatus=@('stable')};valid=$false}
)
foreach($case in $cases){
    $fixture=Join-Path $OutputRoot ($case.name+'-'+[guid]::NewGuid().ToString('N'))
    $tools=Join-Path $fixture 'tools/windows'
    $source=Join-Path $fixture 'input'
    $output=Join-Path $fixture 'dist'
    $drop=Join-Path $output "NimbyRailsFranceSDK-$version-drop-in"
    New-Item -ItemType Directory -Path $tools,$source,$drop -Force | Out-Null
    Copy-Item -LiteralPath "$sdkSource/tools/windows/package-hub-sdk.ps1" -Destination "$tools/package-hub-sdk.ps1"
    Set-Content -LiteralPath "$fixture/VERSION" -Value $version -Encoding ASCII
    $case.policy | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath "$fixture/release-channels.json" -Encoding UTF8
    Set-Content -LiteralPath "$source/fixture.txt" -Value 'Metadata test, not an SDK binary.' -Encoding ASCII
    Set-Content -LiteralPath "$drop/fixture.txt" -Value 'Metadata test, not a loader.' -Encoding ASCII
    $failure=$null
    try{ & "$tools/package-hub-sdk.ps1" -SdkRoot $source -OutputRoot $output -LocalBuild | Out-Null }
    catch{$failure=$_}
    if(!$case.valid){
        if(!$failure -or $failure.Exception.Message -notmatch 'developmentStatus'){throw "Invalid status accepted: $($case.name)"}
        if(Test-Path -LiteralPath "$output/project.json"){throw 'Invalid metadata produced a manifest'}
        if(Test-Path -LiteralPath "$output/NimbyRailsFranceSDK-$version-hub.zip"){throw 'Invalid metadata produced an archive'}
    }else{
        if($failure){throw $failure}
        $legacy=Get-Content -LiteralPath "$output/project.json" -Raw
        $canonical=Get-Content -LiteralPath "$output/project-windows-x64.json" -Raw
        if($legacy -ne $canonical){throw 'Legacy and platform metadata differ'}
        $manifest=$legacy | ConvertFrom-Json
        if($manifest.channel -cne 'alpha' -or $manifest.version -cne $version){throw 'Development status changed version/channel'}
        if($case.expected){
            if($manifest.developmentStatus -cne $case.expected){throw 'Development status was not preserved'}
        }elseif($null -ne $manifest.PSObject.Properties['developmentStatus']){throw 'Absent development status was invented'}
        $asset=Join-Path $output "NimbyRailsFranceSDK-$version-hub.zip"
        if($manifest.sha256 -cne (Get-FileHash -LiteralPath $asset -Algorithm SHA256).Hash.ToLowerInvariant()){throw 'Archive checksum differs'}
    }
    Write-Output "PASS SDK project metadata: $($case.name)"
}
