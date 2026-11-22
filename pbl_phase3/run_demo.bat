@echo off
if not exist SmartWaste.exe (
    echo SmartWaste.exe not found. Compiling first...
    call build.bat
)
SmartWaste.exe --demo
pause
