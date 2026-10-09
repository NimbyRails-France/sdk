param(
    [Parameter(Mandatory=$true)][string]$KotlinHome,
    [string]$SdkInstall,
    [string]$Destination,
    [string]$BuildDirectory,
    [string]$ArchiveDirectory,
    [string]$ClionHome = "$env:LOCALAPPDATA/Programs/CLion"
)
$ErrorActionPreference='Stop'
$sdkRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$sdkVersion=(Get-Content -LiteralPath "$sdkRoot/VERSION" -Raw).Trim()
if(!$SdkInstall){$SdkInstall=Join-Path $sdkRoot 'install/development'}
if(!$Destination){$Destination=Join-Path $sdkRoot 'install/kotlin'}
$Destination=[IO.Path]::GetFullPath($Destination)
$compiler=Join-Path $KotlinHome 'bin/konanc.bat'
foreach($folder in @('klib','bridge','bin','sources','licenses')){
    New-Item -ItemType Directory -Path "$Destination/$folder" -Force | Out-Null
}
$sources=@(Get-ChildItem -LiteralPath "$sdkRoot/kotlin/src" -Recurse -Filter *.kt | ForEach-Object FullName)
& $compiler -target mingw_x64 -produce library -o "$Destination/klib/nimby-mod-api" @sources
if($LASTEXITCODE){throw 'Kotlin SDK API compilation failed'}
Copy-Item -LiteralPath "$sdkRoot/kotlin/native/Exports.kt" -Destination "$Destination/bridge/Exports.kt" -Force
Copy-Item -LiteralPath "$sdkRoot/kotlin/native/Package.kt" -Destination "$Destination/bridge/Package.kt" -Force
$clion=Join-Path $ClionHome 'bin'
$cmake=Join-Path $clion 'cmake/win/x64/bin/cmake.exe'
$nativeBuild=if($BuildDirectory){$BuildDirectory}else{Join-Path $sdkRoot 'build/kotlin-devkit-adapter'}
& $cmake -S "$sdkRoot/kotlin/native" -B $nativeBuild -G Ninja '-DCMAKE_BUILD_TYPE=Release' `
    "-DCMAKE_CXX_COMPILER=$clion/mingw/bin/g++.exe" "-DCMAKE_MAKE_PROGRAM=$clion/ninja/win/x64/ninja.exe" `
    "-DNimbyRailsFranceSDK_DIR=$SdkInstall/lib/cmake/NimbyRailsFranceSDK" `
    '-DNRF_MODULE_NAME=NimbyKotlinMod' '-DNRF_KOTLIN_LIBRARY=NimbyKotlinModKotlin.dll' "-DNRF_OUTPUT_DIRECTORY=$Destination/bin"
if($LASTEXITCODE){throw 'Kotlin SDK adapter configuration failed'}
& $cmake --build $nativeBuild --target kotlin_mod kotlin_loader_test
if($LASTEXITCODE){throw 'Kotlin SDK adapter compilation failed'}
New-Item -ItemType Directory -Force -Path "$Destination/licenses/Kotlin-Native" | Out-Null
Copy-Item -Path "$KotlinHome/licenses/*" -Destination "$Destination/licenses/Kotlin-Native" -Recurse -Force
Copy-Item -LiteralPath "$SdkInstall/share/licenses/nlohmann-json" -Destination "$Destination/licenses" -Recurse -Force
if(Test-Path -LiteralPath "$SdkInstall/share/licenses/MinGW"){
    Copy-Item -LiteralPath "$SdkInstall/share/licenses/MinGW" -Destination "$Destination/licenses" -Recurse -Force
}
Add-Type -AssemblyName System.IO.Compression.FileSystem
$sourceArchive=Join-Path $Destination 'sources/nimby-mod-api-sources.jar'
if(Test-Path -LiteralPath $sourceArchive){Remove-Item -LiteralPath $sourceArchive}
[IO.Compression.ZipFile]::CreateFromDirectory("$sdkRoot/kotlin/src",$sourceArchive)
& "$sdkRoot/gradle-plugin/gradlew.bat" --project-dir "$sdkRoot/gradle-plugin" build publish "-PsdkRepository=$Destination/gradle-repository" --console=plain --no-daemon
if($LASTEXITCODE){throw 'Gradle plugin build or SDK repository publication failed'}
[ordered]@{format=1;sdkVersion=$sdkVersion;kotlinVersion='2.2.20';gradlePluginVersion=$sdkVersion;gradleVersion='8.14.3';target='mingw_x64';api='klib/nimby-mod-api.klib';gameSha256=@('fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae')} |
    ConvertTo-Json | Set-Content -LiteralPath "$Destination/sdk.json" -Encoding UTF8
# Reusing an old kit must not silently republish retired sample projects.
if(Test-Path -LiteralPath "$Destination/examples") { throw 'Old examples directory in kit. Choose a fresh Destination.' }
New-Item -ItemType Directory -Path "$Destination/docs" -Force | Out-Null
Get-ChildItem -LiteralPath "$sdkRoot/docs" -Recurse -File -Filter *.md | ForEach-Object {
    $relative=$_.FullName.Substring((Join-Path $sdkRoot 'docs').Length+1)
    $target=Join-Path "$Destination/docs" $relative
    New-Item -ItemType Directory -Path (Split-Path $target) -Force | Out-Null
    Copy-Item -LiteralPath $_.FullName -Destination $target -Force
}
$kitReadme = @'
# NRF Kotlin SDK

Create your own project with [the official Kotlin guide](https://wiki-dev.nimbyrails-france.fr/version/{SDK_EDITION}/commencer/installation).
Configure `NRF_KOTLIN_SDK` or `nrfSdkDir` to point at this kit.
The kit includes the Gradle plugin, Kotlin API, native adapter and diagnostics.
All tutorials and Kotlin API documentation live in [this SDK's wiki edition](https://wiki-dev.nimbyrails-france.fr/version/{SDK_EDITION}).
'@
$sdkEdition = ($sdkVersion.Split('-')[0].Split('.')[0..1] -join '.')
$kitReadme.Replace('{SDK_EDITION}', $sdkEdition) | Set-Content -LiteralPath "$Destination/README.md" -Encoding UTF8
if(!$ArchiveDirectory){$ArchiveDirectory=Join-Path $sdkRoot 'dist'}
$archive=Join-Path $ArchiveDirectory "NimbyRailsFranceSDK-kotlin-$sdkVersion-windows-x64.zip"
New-Item -ItemType Directory -Path (Split-Path $archive) -Force | Out-Null
Compress-Archive -Path "$Destination/*" -DestinationPath $archive -Force
Write-Output "Kotlin developer SDK: $Destination"
Write-Output "Redistributable archive: $archive"
