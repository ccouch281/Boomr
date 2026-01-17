@echo off
echo Cleaning Boomr project...
pushd "%~dp0.."
if exist "Binaries" rd /s /q "Binaries"
if exist "Intermediate" rd /s /q "Intermediate"
if exist "Saved" rd /s /q "Saved"
if exist "DerivedDataCache" rd /s /q "DerivedDataCache"
if exist "Boomr.sln" del /q "Boomr.sln"
echo Done.
if %ERRORLEVEL% NEQ 0 pause
