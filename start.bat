@echo off
title Open Stop Motion Studio - Launcher
color 0A

echo.
echo  ============================================================
echo   🎬  Open Stop Motion Studio - MVP v0.1
echo  ============================================================
echo.

:: ── Schritt 1: .NET 8 SDK prüfen ────────────────────────────────────────────
echo  [1/4] Prüfe .NET 8 SDK...
dotnet --version >nul 2>&1
if %errorlevel% neq 0 (
    color 0C
    echo.
    echo  ❌  FEHLER: .NET SDK nicht gefunden!
    echo.
    echo  Bitte .NET 8 SDK installieren:
    echo  https://dotnet.microsoft.com/download/dotnet/8.0
    echo.
    pause
    exit /b 1
)

for /f "tokens=*" %%v in ('dotnet --version 2^>nul') do set DOTNET_VER=%%v
echo  ✔  .NET SDK gefunden: v%DOTNET_VER%

:: ── Schritt 2: Projektdatei prüfen ──────────────────────────────────────────
echo.
echo  [2/4] Suche Projektdatei...

if not exist "%~dp0OpenStopMotionStudio.csproj" (
    color 0C
    echo.
    echo  ❌  FEHLER: OpenStopMotionStudio.csproj nicht gefunden!
    echo.
    echo  Bitte sicherstellen, dass start.bat im selben Ordner
    echo  wie die .csproj Datei liegt.
    echo.
    echo  Erwarteter Pfad: %~dp0OpenStopMotionStudio.csproj
    echo.
    pause
    exit /b 1
)
echo  ✔  Projektdatei gefunden.

:: ── Schritt 3: NuGet-Pakete wiederherstellen und bauen ──────────────────────
echo.
echo  [3/4] NuGet-Pakete laden und Projekt bauen...
echo        (Beim ersten Start kann das 1-2 Minuten dauern)
echo.

dotnet build "%~dp0OpenStopMotionStudio.csproj" --configuration Debug --nologo -v minimal
if %errorlevel% neq 0 (
    color 0C
    echo.
    echo  ❌  BUILD FEHLGESCHLAGEN!
    echo.
    echo  Mögliche Ursachen:
    echo    - Keine Internetverbindung für NuGet-Download
    echo    - .NET 8 SDK fehlt (nur Runtime installiert)
    echo    - Fehlende Quelldateien im Projektordner
    echo.
    echo  Vollständige Fehlerausgabe steht oben.
    echo.
    pause
    exit /b 1
)

echo.
echo  ✔  Build erfolgreich.

:: ── Schritt 4: Anwendung starten ────────────────────────────────────────────
echo.
echo  [4/4] Starte Open Stop Motion Studio...
echo.
echo  ┌─────────────────────────────────────────┐
echo  │  Tipps für den ersten Start:            │
echo  │                                         │
echo  │  • Webcam / USB-Kamera anschließen      │
echo  │  • Kamera im Dropdown auswählen         │
echo  │  • "Kamera starten" klicken             │
echo  │  • LEERTASTE = Frame aufnehmen          │
echo  │  • Stream Deck optional verbinden       │
echo  └─────────────────────────────────────────┘
echo.

dotnet run --project "%~dp0OpenStopMotionStudio.csproj" --configuration Debug --no-build

:: ── Beendet ──────────────────────────────────────────────────────────────────
echo.
echo  Anwendung beendet.
pause
