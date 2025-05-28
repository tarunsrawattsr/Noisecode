@echo off
setlocal enabledelayedexpansion

:: Check for administrator privileges
NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ ERROR: Please run this script as Administrator.
    pause
    exit /b 1
)

echo 📦 Starting setup...

:: Install Chocolatey if not already installed
choco -v >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo 🔧 Installing Chocolatey...
    powershell -NoProfile -ExecutionPolicy Bypass -Command ^
     "Set-ExecutionPolicy Bypass -Scope Process -Force; ^
     [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; ^
     iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    
    :: Wait for Chocolatey to be available
    SET /A RETRIES=0
    :WAIT_FOR_CHOCOLATEY
    choco -v >nul 2>&1
    IF %ERRORLEVEL% NEQ 0 (
        SET /A RETRIES+=1
        IF !RETRIES! GEQ 10 (
            echo ❌ Chocolatey installation failed.
            pause
            exit /b 1
        )
        echo ⏳ Waiting for Chocolatey to be available...
        timeout /t 3 >nul
        GOTO WAIT_FOR_CHOCOLATEY
    )
)

:: Optional: Refresh environment
echo 🔄 Refreshing environment...
powershell -NoProfile -ExecutionPolicy Bypass -Command "refreshenv"

:: Install dependencies
echo 🚀 Installing dependencies...

echo 🔧 Installing CMake...
choco install cmake -y

echo 🔐 Installing OpenSSL...
choco install openssl -y

echo 🎨 Installing GTK runtime...
choco install gtk-runtime -y

echo 🛠️ Installing MinGW...
choco install mingw -y

echo ✅ All dependencies installed successfully.
pause
