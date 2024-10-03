@echo off
REM Set variables for paths
chdir ..
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

@echo Project built successfully!
pause