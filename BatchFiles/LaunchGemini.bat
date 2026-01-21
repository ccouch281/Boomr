@echo off
title Gemini CLI
echo Launching Gemini CLI...

REM Try to launch gemini. If 'gemini' is not in your PATH, replace it with the full path to the executable.
call gemini

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Error: 'gemini' command not found or failed to run.
    echo Please ensure Gemini CLI is installed and in your PATH,
    echo or edit this script to point to the correct executable.
    pause
)
