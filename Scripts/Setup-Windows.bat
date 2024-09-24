@echo off

pushd ..
Libraries\premake\Windows\premake5.exe --file=Build.lua vs2022
popd
pause