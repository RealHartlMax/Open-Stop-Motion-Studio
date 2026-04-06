@echo off
setlocal

rem --- Konfiguration ---
if "%~1"=="" (
    echo Fehler: Es wurde keine Versionsnummer übergeben.
    echo Verwendung: build.bat ^<Version^>
    echo Beispiel:   build.bat 0.1.1
    exit /b 1
)
set "VERSION=%~1"
set "PROJECT_FILE=OpenStopMotionStudio.csproj"
set "PUBLISH_DIR_BASE=.\publish-artefacts"

rem --- Hilfsfunktionen ---
:log
    echo.
    echo --- %~1 ---
    goto :eof

rem --- Hauptskript ---
call :log "Starte Build-Prozess für Version %VERSION%"

set "RID=win-x64"
set "PUBLISH_DIR=%PUBLISH_DIR_BASE%\%RID%"
set "ARCHIVE_NAME=OSMS-%RID%-v%VERSION%.zip"

call :log "Starte Build für %RID%"

rem 1. Veröffentlichungsverzeichnis bereinigen und neu erstellen
call :log "Bereinige Verzeichnis: %PUBLISH_DIR%"
if exist "%PUBLISH_DIR%" (
    rmdir /s /q "%PUBLISH_DIR%"
)
mkdir "%PUBLISH_DIR%"

rem 2. App publizieren
call :log "Publiziere App für %RID%..."
dotnet publish "%PROJECT_FILE%" ^
    -c Release ^
    -r "%RID%" ^
    --self-contained true ^
    /p:PublishSingleFile=true ^
    -o "%PUBLISH_DIR%"

rem 3. Artefakt packen
call :log "Packe Artefakt zu %ARCHIVE_NAME%..."
powershell -NoProfile -Command "Compress-Archive -Path '%PUBLISH_DIR%\*' -DestinationPath '%ARCHIVE_NAME%' -Force"


rem 4. Veröffentlichungsordner aufräumen
call :log "Bereinige temporäres Verzeichnis %PUBLISH_DIR%."
rmdir /s /q "%PUBLISH_DIR%"

if exist "%PUBLISH_DIR_BASE%" (
    rmdir "%PUBLISH_DIR_BASE%"
)

echo.
echo ✓ Fertig: %ARCHIVE_NAME%
echo.

call :log "Alle Builds erfolgreich abgeschlossen!"

endlocal
