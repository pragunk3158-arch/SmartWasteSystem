@echo off
echo ====================================================================
echo   Compiling SmartWaste System (C++17)
echo ====================================================================

g++ -std=c++17 -Wall -Wextra -Iinclude src/Bin.cpp src/Vehicle.cpp src/Graph.cpp src/Route.cpp src/WasteManager.cpp src/main.cpp -o SmartWaste.exe

if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b %errorlevel%
)

echo [SUCCESS] Compilation successful! Generated SmartWaste.exe
pause
