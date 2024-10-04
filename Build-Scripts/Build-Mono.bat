@echo off

echo Finding VS Install Location

cd /d ".."
for /f "tokens=*" %%i in ('Build-Scripts\vswhere\vswhere.exe -latest -products * -property installationPath') do set VSINSTALL=%%i

if "%VSINSTALL%"=="" (
    echo Visual Studio not found!
    pause
)

rem Echo the Visual Studio installation path
echo Visual Studio is installed at: %VSINSTALL%
set MSBUILD_PATH=%VSINSTALL%\MSBuild\Current\Bin
echo MSBUILD Path is at: %MSBUILD_PATH%
set PATH=%PATH%;%MSBUILD_PATH%

rem Build Mono
echo Building Mono
set MONO_PATH=Libraries\mono\msvc\mono.sln
msbuild.exe %MONO_PATH% /p:Platform=x64 /p:Configuration=Release /p:MONO_TARGET_GC=sgen

pause