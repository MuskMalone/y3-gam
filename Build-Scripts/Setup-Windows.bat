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

set PROJECT_DIR=%cd%
set BUILD_DIR=Libraries\Built-Libraries
set CONFIGURATION=Release

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM Run CMake to generate build files
cmake -S %PROJECT_DIR% -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%CONFIGURATION%

REM Build the project using the specified configuration
cmake --build %BUILD_DIR% --config %CONFIGURATION%

@echo Project Libraries built successfully!

Build-Scripts\premake\Windows\premake5.exe --file=Build.lua vs2022
pause