@echo off
setlocal enabledelayedexpansion

:: Ensure dependencies are installed
if not exist "%ProgramData%\chocolatey\bin\choco.exe" (
echo [!] Please run setup.bat first to install dependencies.
pause
exit /b 1
)

:: Set build directory
set BUILD_DIR=build

:: Create build directory
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

:: Run CMake and build the project
echo [*] Configuring with CMake...
cmake -S . -B "%BUILD_DIR%"
IF %ERRORLEVEL% NEQ 0 (
    echo [X] CMake configuration failed.
    pause
    exit /b 1
)

echo [*] Building project...
cmake --build "%BUILD_DIR%"
IF %ERRORLEVEL% NEQ 0 (
    echo [X] Build failed.
    pause
    exit /b 1
)

:: Run the compiled executable
echo [>] Running the application...
cd "%BUILD_DIR%"
my_app.exe
cd ..

pause
