@echo off
powershell -ExecutionPolicy Bypass -File "%~dp0..\TimeCard\TimeTracker.ps1" -Action Auto
echo Launching Boomr Editor (ES3.1)...
start "" "C:\Program Files\Epic Games\UE_4.27\Engine\Binaries\Win64\UE4Editor.exe" "%~dp0..\Boomr.uproject" -featureleveles31
