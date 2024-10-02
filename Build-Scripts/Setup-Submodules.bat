@echo off
echo Initializing and updating Git submodules...

REM Ensure the user is in the correct directory (optional)
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
pause