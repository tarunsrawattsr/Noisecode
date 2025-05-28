@echo off
setlocal enabledelayedexpansion

:: ✅ Ensure running as Administrator
NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ ERROR: This script must be run as Administrator.
    pause
    exit /b 1
)

:: ✅ Install Chocolatey if not already installed
where choco >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo 🚀 Installing Chocolatey...
    powershell -NoProfile -ExecutionPolicy Bypass -Command ^
     "Set-ExecutionPolicy Bypass -Scope Process -Force; ^
     [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; ^
     iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    
    IF %ERRORLEVEL% NEQ 0 (
        echo ❌ Chocolatey installation failed. Check your internet connection or proxy settings.
        pause
        exit /b 1
    )
)

:: ✅ Make sure choco command is available
SET CHOCO_PATH=%ALLUSERSPROFILE%\chocolatey\bin\choco.exe
IF NOT EXIST "%CHOCO_PATH%" (
    echo ❌ Chocolatey not found after installation.
    pause
    exit /b 1
)

:: ✅ Add Chocolatey to PATH for current session
set PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin

:: ✅ Install CMake
echo 🔧 Installing CMake...
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ Failed to install CMake
    pause
    exit /b 1
)

:: ✅ Install OpenSSL
echo 🔐 Installing OpenSSL...
choco install openssl -y
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ Failed to install OpenSSL
    pause
    exit /b 1
)

:: ✅ Install GTK
echo 🎨 Installing GTK runtime...
choco install gtk-runtime -y
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ Failed to install GTK runtime
    pause
    exit /b 1
)

:: ✅ Install MinGW
echo 🛠️ Installing MinGW...
choco install mingw -y
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ Failed to install MinGW
    pause
    exit /b 1
)

:: ✅ All done
echo ✅ All dependencies installed successfully!
pause
