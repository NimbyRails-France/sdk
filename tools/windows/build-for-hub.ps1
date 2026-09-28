param(
    [Parameter(Mandatory=$true)][string]$OutputDirectory,
    [string]$ClionHome = $env:NRF_CLION_HOME,
    [string]$KotlinHome = $env:NRF_KOTLIN_HOME,
    [string]$JavaHome = $env:NRF_JAVA_HOME
)
$ErrorActionPreference='Stop'
[Console]::OutputEncoding = New-Object Text.UTF8Encoding($false)
$OutputEncoding = [Console]::OutputEncoding
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$version=(Get-Content -LiteralPath "$root/VERSION" -Raw).Trim()
if($version -notmatch '^\d+\.\d+\.\d+(?:-(?:alpha|beta)\.[1-9]\d*)?$'){throw 'Invalid VERSION'}
if(!$ClionHome){$ClionHome="$env:LOCALAPPDATA/Programs/CLion"}
if(!$KotlinHome){$KotlinHome="$env:USERPROFILE/.konan/kotlin-native-prebuilt-windows-x86_64-2.2.20"}
if(!$JavaHome){$JavaHome=$env:JAVA_HOME}
if(!$JavaHome -or !(Test-Path -LiteralPath "$JavaHome/bin/javac.exe")){
    $JavaHome=Get-ChildItem -LiteralPath "$env:ProgramFiles/Eclipse Adoptium" -Directory -Filter 'jdk-21*' -ErrorAction SilentlyContinue |
        Sort-Object Name -Descending | Select-Object -First 1 -ExpandProperty FullName
}
$cmake="$ClionHome/bin/cmake/win/x64/bin/cmake.exe"
$ctest="$ClionHome/bin/cmake/win/x64/bin/ctest.exe"
foreach($tool in @($cmake,$ctest,"$ClionHome/bin/mingw/bin/g++.exe","$ClionHome/bin/ninja/win/x64/ninja.exe","$KotlinHome/bin/konanc.bat","$JavaHome/bin/javac.exe")){
    if(!(Test-Path -LiteralPath $tool -PathType Leaf)){throw "Missing build tool: $tool. Configure NRF_CLION_HOME, NRF_KOTLIN_HOME or NRF_JAVA_HOME."}
}
$env:JAVA_HOME=$JavaHome
$env:PATH="$ClionHome/bin/mingw/bin;$JavaHome/bin;$env:PATH"
$env:GRADLE_OPTS='-Dorg.gradle.daemon=false -Dfile.encoding=UTF-8'
$output=[IO.Path]::GetFullPath($OutputDirectory)
if(Test-Path -LiteralPath $output){
    if(@(Get-ChildItem -LiteralPath $output -Force).Count){throw 'OutputDirectory must be empty. A previous working kit is never overwritten.'}
}else{New-Item -ItemType Directory -Path $output | Out-Null}
# One incremental native build cache. The lock also excludes another Hub or CLI
# invocation; output kits are unique and are selected only after full success.
$build=Join-Path $root 'build/hub-native'
New-Item -ItemType Directory -Path $build -Force | Out-Null
$lock=[IO.File]::Open("$build/build.lock",'OpenOrCreate','ReadWrite','None')
try {
    Write-Output "SDK $version | Windows Release | CMake: $cmake | Kotlin: $KotlinHome | JDK: $JavaHome"
    & $cmake -S $root -B $build -G Ninja '-DCMAKE_BUILD_TYPE=Release' '-DBUILD_TESTING=ON' '-DNIMBY_DEVELOPMENT_CONSTRUCTION=ON' `
        "-DCMAKE_CXX_COMPILER=$ClionHome/bin/mingw/bin/g++.exe" "-DCMAKE_C_COMPILER=$ClionHome/bin/mingw/bin/gcc.exe" `
        "-DCMAKE_MAKE_PROGRAM=$ClionHome/bin/ninja/win/x64/ninja.exe"
    if($LASTEXITCODE){throw 'SDK configuration failed'}
    & $cmake --build $build --parallel 4
    if($LASTEXITCODE){throw 'SDK compilation failed'}
    & $ctest --test-dir $build --output-on-failure
    if($LASTEXITCODE){throw 'SDK tests failed'}
    $nativeInstall=Join-Path $output "NimbyRailsFranceSDK-$version"
    & $cmake --install $build --prefix $nativeInstall
    if($LASTEXITCODE){throw 'SDK staging failed'}
    & "$PSScriptRoot/package-kotlin-sdk.ps1" -KotlinHome $KotlinHome -ClionHome $ClionHome -SdkInstall $nativeInstall `
        -Destination "$output/kotlin-kit" -BuildDirectory "$build/kotlin-adapter" -ArchiveDirectory $output
    & "$PSScriptRoot/package-drop-in.ps1" -ClionHome $ClionHome -SkipBuild -BuildDirectory $build -OutputRoot $output -DevelopmentConstruction
    & "$PSScriptRoot/package-hub-sdk.ps1" -SdkRoot $nativeInstall -Version $version -OutputRoot $output -LocalBuild
    if((Get-Content -LiteralPath "$root/VERSION" -Raw).Trim() -ne $version){throw 'VERSION changed during build. Rebuild the SDK.'}
    # Written LAST: a failed/cancelled build cannot be mistaken for a ready kit.
    [ordered]@{project='project.json';archive="NimbyRailsFranceSDK-$version-hub.zip";kotlinKit='kotlin-kit'} |
        ConvertTo-Json | Set-Content -LiteralPath "$output/hub-result.json" -Encoding UTF8
    Write-Output "SDK $version ready for Hub import: $output"
}finally{$lock.Dispose()}
