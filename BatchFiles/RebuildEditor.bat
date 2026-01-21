@echo off
echo Rebuilding BoomrEditor Development Win64...
call "C:\Program Files\Epic Games\UE_4.27\Engine\Build\BatchFiles\Rebuild.bat" BoomrEditor Win64 Development "%~dp0..\Boomr.uproject" -WaitMutex -FromMsBuild
if %ERRORLEVEL% NEQ 0 pause
