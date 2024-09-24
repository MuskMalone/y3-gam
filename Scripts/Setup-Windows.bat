@echo off

pushd ..
ImaGE-Core\lib\premake\Windows\premake5.exe --file=Build.lua vs2022
popd
pause