# Changelog / Aenderungsprotokoll

All notable changes to this project are documented here.  
Alle wichtigen Aenderungen an diesem Projekt werden hier dokumentiert.

---

## v0.1 - 2026-03-24

### English

#### Added

- Initial Windows WPF application on `.NET 10`
- Live camera preview through DirectShow
- Camera selection, start/stop controls, and status bar feedback
- Frame capture workflow with project folders and shot naming
- Onion skin overlay with 1-3 layers and adjustable opacity presets
- Stream Deck integration for capture, onion toggle, opacity presets, and undo
- Timeline and playback area with scrubbing and manual `1-120 fps` input
- Live luminance histogram in the camera section
- Webcam settings button for devices that expose a driver/property dialog
- `TIFF + Proxy` capture workflow alongside JPEG sequence capture
- Project-side shot naming and preview of upcoming output files
- Local SDK folder structure for Nikon and Canon development
- Nikon SDK discovery from `SDKs/Nikon`
- Nikon `NEF` / `NRW` folder import to TIFF masters plus JPG/PNG proxies
- Film-style frame-start support such as `1001`
- Dedicated `CHANGELOG.md`

#### Changed

- Build outputs were moved away from the default `bin/obj` layout into `.artifacts`
- `start.bat` now uses a safer local CLI setup and cleaner restore/build flow
- README was updated to reflect the real `v0.1` feature set and current roadmap state
- Timeline display now follows actual frame numbers instead of only raw capture counts

#### Fixed

- WPF UI issues around invalid layout properties and unreadable controls
- Camera preview handling by switching to a more reliable DirectShow sample callback flow
- Startup/build failures caused by stale or inaccessible generated build files
- Stream Deck integration mismatches with the currently installed SDK version
- Global exception reporting now surfaces inner exceptions and writes an error log

#### Notes / Known limitations

- The timeline display currently defaults to a `250` frame shot range and is not yet user-configurable per shot
- This does not limit the final captured result itself, but it should become editable for better timeline planning and future motion-control / DMX workflows

### Deutsch

#### Hinzugefuegt

- Erste Windows-WPF-Anwendung auf Basis von `.NET 10`
- Live-Kameravorschau ueber DirectShow
- Kameraauswahl, Start/Stopp-Steuerung und Statusleisten-Feedback
- Frame-Aufnahme-Workflow mit Projektordnern und Shot-Namen
- Onion-Skin-Overlay mit 1-3 Layern und einstellbaren Transparenz-Presets
- Stream-Deck-Integration fuer Capture, Onion-Toggle, Transparenz-Presets und Undo
- Timeline- und Playback-Bereich mit Scrubbing und direkter `1-120 fps`-Eingabe
- Live-Luminanz-Histogramm im Kamerabereich
- Webcam-Einstellungsbutton fuer Geraete mit Treiber-/Property-Dialog
- `TIFF + Proxy`-Workflow zusaetzlich zur JPEG-Sequenz-Aufnahme
- Shot-Namensvergabe im Projektbereich mit Vorschau der naechsten Ausgabedateien
- Lokale SDK-Ordnerstruktur fuer Nikon- und Canon-Entwicklung
- Nikon-SDK-Erkennung aus `SDKs/Nikon`
- Nikon-`NEF`- / `NRW`-Ordnerimport zu TIFF-Mastern plus JPG/PNG-Proxys
- Unterstuetzung fuer filmtypische Startframes wie `1001`
- Eigenes `CHANGELOG.md`

#### Geaendert

- Build-Ausgaben wurden aus dem Standardlayout `bin/obj` nach `.artifacts` verschoben
- `start.bat` nutzt jetzt ein sichereres lokales CLI-Setup und einen aufgeraeumten Restore-/Build-Ablauf
- Die README wurde auf den realen `v0.1`-Funktionsstand und die aktuelle Roadmap aktualisiert
- Die Timeline orientiert sich jetzt an echten Framenummern statt nur an rohen Capture-Zaehlern

#### Behoben

- WPF-UI-Probleme rund um ungueltige Layout-Eigenschaften und schlecht lesbare Controls
- Kameravorschau durch Umstellung auf einen stabileren DirectShow-Sample-Callback-Flow
- Start-/Buildfehler durch veraltete oder unzugaengliche generierte Build-Dateien
- Stream-Deck-Integrationsprobleme mit der aktuell installierten SDK-Version
- Globale Fehlerbehandlung zeigt jetzt auch innere Exceptions und schreibt ein Fehlerlog

#### Hinweise / Bekannte Einschraenkungen

- Die Timeline-Anzeige verwendet aktuell standardmaessig einen Shot-Bereich von `250` Frames und ist pro Shot noch nicht frei konfigurierbar
- Das begrenzt das finale Capture-Ergebnis nicht direkt, sollte aber fuer bessere Timeline-Planung und spaetere Motion-Control- / DMX-Workflows editierbar werden
