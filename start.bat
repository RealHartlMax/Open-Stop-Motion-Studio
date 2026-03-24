@echo off
setlocal
title Open Stop Motion Studio - Launcher
color 0A

set "PROJECT_DIR=%~dp0"
set "PROJECT_FILE=%PROJECT_DIR%OpenStopMotionStudio.csproj"
set "ARTIFACTS_DIR=%PROJECT_DIR%.artifacts"
set "ARTIFACTS_OBJ=%ARTIFACTS_DIR%\obj"
set "ARTIFACTS_BIN=%ARTIFACTS_DIR%\bin"
set "DOTNET_CLI_HOME=%PROJECT_DIR%.dotnet-cli"
set "DOTNET_SKIP_FIRST_TIME_EXPERIENCE=1"
set "DOTNET_CLI_TELEMETRY_OPTOUT=1"

if not exist "%DOTNET_CLI_HOME%" mkdir "%DOTNET_CLI_HOME%"

echo.
echo  ============================================================
echo   Open Stop Motion Studio - MVP v0.1
echo  ============================================================
echo.

echo  [1/5] Check .NET SDK...
dotnet --version >nul 2>&1
if errorlevel 1 (
    color 0C
    echo.
    echo  [ERROR] .NET SDK not found.
    echo.
    echo  Please install a current .NET SDK:
    echo  https://dotnet.microsoft.com/download
    echo.
    pause
    exit /b 1
)

for /f "tokens=*" %%v in ('dotnet --version 2^>nul') do set "DOTNET_VER=%%v"
echo  [OK] .NET SDK found: v%DOTNET_VER%

echo.
echo  [2/5] Check project file...
if not exist "%PROJECT_FILE%" (
    color 0C
    echo.
    echo  [ERROR] OpenStopMotionStudio.csproj not found.
    echo.
    echo  Expected path:
    echo  %PROJECT_FILE%
    echo.
    pause
    exit /b 1
)
echo  [OK] Project file found.

echo.
echo  [3/5] Restore packages if needed...
if exist "%ARTIFACTS_OBJ%\project.assets.json" (
    echo  [OK] Existing restore data found.
) else (
    echo        No restore cache found. Restoring packages...
    dotnet restore "%PROJECT_FILE%" --nologo -v minimal
    if errorlevel 1 (
        color 0C
        echo.
        echo  [ERROR] Package restore failed.
        echo.
        echo  Common causes:
        echo    - No internet connection for NuGet package download
        echo    - nuget.org is temporarily unavailable
        echo.
        echo  Full output is shown above.
        echo.
        pause
        exit /b 1
    )
    echo  [OK] Package restore completed.
)

echo.
echo  [4/5] Build project...
echo        (First run can take 1-2 minutes)
echo.

dotnet build "%PROJECT_FILE%" --configuration Debug --nologo -v minimal --no-restore
if errorlevel 1 (
    echo.
    echo  Build failed on the first attempt.
    echo  Trying once more with a clean build cache...
    if exist "%ARTIFACTS_BIN%" rmdir /s /q "%ARTIFACTS_BIN%"
    if exist "%ARTIFACTS_OBJ%" rmdir /s /q "%ARTIFACTS_OBJ%"
    dotnet restore "%PROJECT_FILE%" --nologo -v minimal
    if errorlevel 1 (
        color 0C
        echo.
        echo  [ERROR] Package restore failed after cache cleanup.
        echo.
        echo  Full output is shown above.
        echo.
        pause
        exit /b 1
    )
    dotnet build "%PROJECT_FILE%" --configuration Debug --nologo -v minimal --no-restore
    if errorlevel 1 (
        color 0C
        echo.
        echo  [ERROR] Build failed.
        echo.
        echo  Common causes:
        echo    - No internet connection for NuGet package download
        echo    - No .NET SDK installed
        echo    - Old build cache in bin/obj
        echo    - Missing source files in the project folder
        echo.
        echo  Full output is shown above.
        echo.
        pause
        exit /b 1
    )
)

echo.
echo  [OK] Build succeeded.

echo.
echo  [5/5] Start Open Stop Motion Studio...
echo.
echo  Tips for the first launch:
echo    - Connect a webcam or USB camera
echo    - Select the camera in the dropdown
echo    - Click "Kamera starten"
echo    - Press SPACE to capture a frame
echo    - Connect a Stream Deck if available
echo.

dotnet run --project "%PROJECT_FILE%" --configuration Debug --no-build

echo.
echo  Application closed.
pause
