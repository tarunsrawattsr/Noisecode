@echo off
setlocal

:: Check if running as admin
NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ Please run this script as Administrator.
    pause
    exit /b 1
)

echo 🔍 Checking for Chocolatey...
choco -v >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo 🔧 Installing Chocolatey...
    powershell -NoProfile -ExecutionPolicy Bypass -Command ^
     "Set-ExecutionPolicy Bypass -Scope Process -Force; ^
     [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; ^
     iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"
)

:: Refresh environment
refreshenv

:: Install dependencies
echo 🚀 Installing CMake...
choco install cmake -y

echo 🔐 Installing OpenSSL...
choco install openssl -y

echo 🎨 Installing GTK runtime...
choco install gtk-runtime -y

echo 🛠️ Installing MinGW...
choco install mingw -y

echo ✅ All dependencies installed!
pause
