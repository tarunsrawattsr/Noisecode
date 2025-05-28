@echo off
setlocal

:: ✅ Set build directory
set BUILD_DIR=build

:: ✅ Create build directory if it doesn't exist
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

:: ✅ Run CMake with MinGW generator
echo 🛠️ Generating with MinGW Makefiles...
cmake -S . -B %BUILD_DIR% -G "MinGW Makefiles"
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ CMake configuration failed.
    pause
    exit /b 1
)

:: ✅ Build with mingw32-make (not make or nmake!)
echo 🔨 Building project with mingw32-make...
cmake --build %BUILD_DIR% -- -j
IF %ERRORLEVEL% NEQ 0 (
    echo ❌ Build failed.
    pause
    exit /b 1
)

:: ✅ Run your app
cd %BUILD_DIR%
my_app.exe
cd ..

pause
