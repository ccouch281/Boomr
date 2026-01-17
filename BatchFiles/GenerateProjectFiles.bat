@echo off
echo Generating Project Files...
call "C:\Program Files\Epic Games\UE_4.27\Engine\Binaries\DotNET\UnrealBuildTool.exe" -projectfiles -project="%~dp0..\Boomr.uproject" -game -rocket -progress
if %ERRORLEVEL% NEQ 0 pause
