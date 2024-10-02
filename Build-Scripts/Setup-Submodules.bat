@echo off
echo Initializing and updating Git submodules...

REM Ensure the user is in the correct directory (optional)
echo Navigating to repository root...
if exist "..\y3-gam" (
    cd /d "..\y3-gam"
) else (
    echo Directory ..\y3-gam does not exist.
    pause
    exit /b 1
)

REM Run the Git submodule command to initialize and update
git submodule update --init --recursive

echo Submodules have been initialized and updated.
pause