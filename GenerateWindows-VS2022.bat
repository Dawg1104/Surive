@echo off
premake5 vs2022

REM Replace this path with your actual MSBuild.exe location
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Survive.sln /p:Configuration=Debug /m

pause