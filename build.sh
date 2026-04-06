#!/bin/bash

# Rigorose Fehlerprüfung: Bricht bei jedem Fehler, bei ungesetzten Variablen und bei Fehlern in Pipelines ab.
set -euo pipefail

# --- Konfiguration ---
# Version aus dem ersten Kommandozeilenargument übernehmen
# Beispielaufruf: ./build.sh 0.1.1
if [ -z "${1-}" ]; then
  echo "Fehler: Es wurde keine Versionsnummer übergeben."
  echo "Verwendung: $0 <Version>"
  echo "Beispiel:   $0 0.1.1"
  exit 1
fi
VERSION="$1"
PROJECT_FILE="OpenStopMotionStudio.csproj"
PUBLISH_DIR_BASE="./publish-artefacts" # Basis-Veröffentlichungsordner

# --- Hilfsfunktionen ---
# Zeigt eine farbige Log-Nachricht an
log() {
  # BOLD, BLUE
  echo -e "
\033[1;34m--- $1 ---\033[0m"
}

# Führt den Build- und Packprozess für eine gegebene Runtime ID (RID) durch
build_and_package() {
  local RID="$1"
  local PUBLISH_DIR="${PUBLISH_DIR_BASE}/${RID}"
  local ARCHIVE_NAME="OSMS-${RID}-v${VERSION}.tar.gz"

  log "Starte Build für ${RID}"

  # 1. Veröffentlichungsverzeichnis bereinigen und neu erstellen
  log "Bereinige Verzeichnis: ${PUBLISH_DIR}"
  rm -rf "${PUBLISH_DIR}"
  mkdir -p "${PUBLISH_DIR}"

  # 2. App publizieren
  # --self-contained: Alle .NET-Abhängigkeiten werden mitgeliefert
  # /p:PublishSingleFile=true: Erzeugt eine einzige ausführbare Datei
  log "Publiziere App für ${RID}..."
  dotnet publish "${PROJECT_FILE}" 
    -c Release 
    -r "${RID}" 
    --self-contained true 
    /p:PublishSingleFile=true 
    -o "${PUBLISH_DIR}"

  # 3. Artefakt packen
  # -c: In das Zielverzeichnis wechseln, um den Ordnerpfad im Archiv zu vermeiden
  # -z: gzip-Komprimierung verwenden
  # -v: Ausführliche Ausgabe (Dateiliste)
  # -f: Archiv-Dateiname angeben
  log "Packe Artefakt zu ${ARCHIVE_NAME}..."
  tar -czvf "${ARCHIVE_NAME}" -C "${PUBLISH_DIR}" .

  # 4. Veröffentlichungsordner aufräumen
  log "Bereinige temporäres Verzeichnis ${PUBLISH_DIR}."
  rm -rf "${PUBLISH_DIR}"

  echo -e "\033[1;32m✓ Fertig: ${ARCHIVE_NAME}\033[0m"
}

# --- Hauptskript ---
log "Starte Build-Prozess für Version ${VERSION}"

# RIDs für die zu erstellenden Builds
# - linux-x64: Standard für die meisten Linux-Distributionen
# - osx-x64: Für ältere Intel-basierte Macs
# - osx-arm64: Für moderne Apple Silicon Macs (M1, M2, etc.)
build_and_package "linux-x64"
build_and_package "osx-x64"
build_and_package "osx-arm64"

# Aufräumen des Basis-Verzeichnis, falls es leer ist
if [ -d "${PUBLISH_DIR_BASE}" ] && [ -z "$(ls -A "${PUBLISH_DIR_BASE}")" ]; then
    rmdir "${PUBLISH_DIR_BASE}"
fi


log "Alle Builds erfolgreich abgeschlossen!"
