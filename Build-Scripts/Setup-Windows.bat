@echo off

echo Initializing and updating Git submodules...

REM Ensure the user is in the correct directory
echo Navigating to repository root...
if exist ".." (
    cd /d ".."
) else (
    echo Directory does not exist.
    pause
    exit /b 1
)

REM Run the Git submodule command to initialize and update
git submodule update --init --recursive

echo Submodules have been initialized and updated.
echo -----------------------------------------------------------------------

rem Mono Setup
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
rem echo Building Mono
rem set MONO_PATH=Libraries\mono\msvc\mono.sln
rem msbuild.exe %MONO_PATH% /p:Platform=x64 /p:Configuration=Release /p:MONO_TARGET_GC=sgen

rem Build DirectXTex
echo Building DirectXTex
set DIRECTX_TEX_PATH=Libraries\DirectXTex\DirectXTex_Desktop_2022_Win10.sln
msbuild.exe %DIRECTX_TEX_PATH% /p:Platform=x64 /p:Configuration=Release
msbuild.exe %DIRECTX_TEX_PATH% /p:Platform=x64 /p:Configuration=Debug

echo -----------------------------------------------------------------------

set PROJECT_DIR=%cd%
set BUILD_DIR=Libraries\Built-Libraries\Debug
set CONFIGURATION=Debug

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM Run CMake to generate build files
cmake -S %PROJECT_DIR% -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%CONFIGURATION% -DBUILD_RTTR=OFF

REM Build the project using the specified configuration
cmake --build %BUILD_DIR% --config %CONFIGURATION%

REM Build RTTR
cmake -S Libraries\rttr -B Libraries\rttr\Built-Libraries\Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build Libraries\rttr\Built-Libraries\Debug --config Debug

@echo Project Debug Libraries built successfully!
echo -----------------------------------------------------------------------

set PROJECT_DIR=%cd%
set BUILD_DIR=Libraries\Built-Libraries\Release
set CONFIGURATION=Release

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM Run CMake to generate build files
cmake -S %PROJECT_DIR% -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%CONFIGURATION% -DBUILD_RTTR=OFF

REM Build the project using the specified configuration
cmake --build %BUILD_DIR% --config %CONFIGURATION%

REM Build RTTR
cmake -S Libraries\rttr -B Libraries\rttr\Built-Libraries\Release -DCMAKE_BUILD_TYPE=Release
cmake --build Libraries\rttr\Built-Libraries\Release --config Release

@echo Project Release Libraries built successfully!
echo -----------------------------------------------------------------------

Build-Scripts\premake\Windows\premake5.exe --file=Build.lua vs2022

pause