@echo off
REM Set variables for paths
set PROJECT_DIR="..\y3-gam"
set BUILD_DIR=%PROJECT_DIR%build
set CONFIGURATION=Release

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM Navigate to the build directory
cd "%BUILD_DIR%"

REM Run CMake to generate build files
cmake .. -DCMAKE_BUILD_TYPE=%CONFIGURATION%

REM Build the project
cmake --build . --config %CONFIGURATION%

REM Go back to the project root
cd "%PROJECT_DIR%"

@echo Project built successfully!
pause