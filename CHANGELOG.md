# Changelog / Aenderungsprotokoll

All notable changes to this project are documented here.  
Alle wichtigen Aenderungen an diesem Projekt werden hier dokumentiert.

---
## v0.2.1 - 2026-04-11

### English

#### Fixed
- **Cross-Platform Build**: Fixed a compilation error on Linux and macOS caused by the missing `CameraDeviceEnumerator.Other.cs` non-Windows partial method implementation.
- **CI Pipeline Sequencing**: Auto-tag workflow now waits for the CI build to complete successfully before creating a version tag, preventing tags on broken builds.

### Deutsch

#### Behoben
- **Plattformuebergreifender Build**: Einen Kompilierfehler unter Linux und macOS behoben, der durch die fehlende Nicht-Windows-Partial-Method-Implementierung in `CameraDeviceEnumerator.Other.cs` verursacht wurde.
- **CI-Pipeline-Reihenfolge**: Der Auto-Tag-Workflow wartet jetzt auf den erfolgreichen Abschluss des CI-Builds, bevor ein Versions-Tag erstellt wird.

---
## v0.2.0 - 2026-04-11

### English

#### Added
- **Reference Overlay Workflow**: Added reference overlays for single images, image sequences, and video files, including loop/hold playback behavior and separate transparency control.
- **Loop Review Overlay**: Added a start/end loop comparison overlay with edge crossfade support for checking seamless animation loops.
- **Sony CrSDK Path**: Added Sony CrSDK camera enumeration, connection, live view, and in-app capture file delivery for Windows-based Sony SDK setups.
- **In-App Updater**: Added `versions.json` based update checks and a startup popup with a direct update/download action when a newer version is available.

#### Changed
- **Timeline Interaction**: The dope sheet now supports direct click-and-drag playhead scrubbing in addition to wheel and keyboard navigation.
- **Camera Integration UI**: The hardware view now shows the active backend, SDK/capture mode, and SDK-aware resolution behavior for DSLR-backed cameras.
- **RAW Import UX**: Nikon RAW import now provides modal progress, cancellation, completion states, and continued processing after per-file failures.
- **Update Manifest Metadata**: `versions.json` now includes historical release changelog entries and uses a GitHub-hosted raw manifest URL for live update checks.

#### Fixed
- **Timeline Scrubbing**: Fixed duplicate wheel-event handling that could skip every second frame during timeline navigation.
- **Onion Performance**: Improved onion-skin rendering with multithreaded tint generation and separate alpha controls for onion, reference, and loop overlays.
- **Sony Reliability**: Hardened Sony CrSDK handling with safer ID parsing fallback, retry logic for busy shutter commands, and more robust capture-file timeout resolution.

### Deutsch

#### Hinzugefuegt
- **Referenz-Overlay-Workflow**: Referenz-Overlays fuer Einzelbilder, Bildsequenzen und Videodateien hinzugefuegt, inklusive Loop-/Hold-Wiedergabeverhalten und getrennter Transparenzsteuerung.
- **Loop-Vergleichs-Overlay**: Ein Overlay zum Vergleich von Start und Ende mit Rand-Crossfade hinzugefuegt, um nahtlose Animations-Loops zu pruefen.
- **Sony-CrSDK-Pfad**: Sony-CrSDK-Kameraerkennung, Verbindung, Live-View und die Dateiübergabe von Captures in die App fuer Windows-basierte Sony-SDK-Setups hinzugefuegt.
- **In-App-Updater**: Update-Pruefung auf Basis von `versions.json` und ein Startup-Popup mit direkter Update-/Download-Aktion bei einer neueren Version hinzugefuegt.

#### Geaendert
- **Timeline-Interaktion**: Das Dope Sheet unterstuetzt jetzt direktes Klick-und-Drag-Scrubbing des Playheads zusaetzlich zu Mausrad- und Tastatur-Navigation.
- **Kamera-Integrations-UI**: Die Hardware-Ansicht zeigt jetzt aktives Backend, SDK-/Capture-Modus und SDK-bewusstes Aufloesungsverhalten fuer DSLR-Kameras an.
- **RAW-Import-UX**: Der Nikon-RAW-Import bietet jetzt modalen Fortschritt, Abbruch, Abschlusszustaende und fortgesetzte Verarbeitung trotz Einzeldatei-Fehlern.
- **Update-Manifest-Metadaten**: `versions.json` enthaelt jetzt historische Release-ChangeLogs und verwendet eine gehostete GitHub-Raw-Manifest-URL fuer Live-Update-Pruefungen.

#### Behoben
- **Timeline-Scrubbing**: Doppelte Mausrad-Ereignisse behoben, die beim Navigieren durch die Timeline jedes zweite Frame ueberspringen konnten.
- **Onion-Performance**: Onion-Skin-Rendering durch multithreaded Tint-Erzeugung und getrennte Alpha-Regler fuer Onion-, Referenz- und Loop-Overlays verbessert.
- **Sony-Zuverlaessigkeit**: Sony-CrSDK-Behandlung gehaertet durch robustere ID-Parsing-Fallbacks, Retry-Logik bei Device-Busy-Ausloeserkommandos und stabilere Capture-Datei-Zuordnung bei Timeouts.

---
## v0.1.1 - 2026-04-06

### English

#### Added
- **Cross-Platform Support**: Migrated the entire application from WPF to the cross-platform Avalonia UI framework, enabling support for Windows, macOS, and Linux.
- **Advanced SDK Discovery**: Implemented a new service to automatically find Nikon and Canon camera SDKs, including searching in local folders and ZIP archives. This allows for direct hardware control of supported cameras.
- **Hardware Capture**: Added support for triggering still image capture directly from connected Nikon cameras, with the captured image being imported into the timeline automatically.
- **Project Loading & Migration**: The application can now load existing projects from disk. An automatic migration service updates older project file structures to the latest version.
- **Composition Overlays**: Added new visual aids for the live preview, including a Rule of Thirds grid, 4x4 grid, center cross, and Action/Title Safe overlays.
- **Enhanced Timeline Navigation**: The timeline can now be navigated frame-by-frame using the mouse wheel and left/right arrow keys.

#### Changed
- **UI Framework**: Replaced all WPF UI components with their Avalonia equivalents for cross-platform compatibility.
- **Camera Backend**: The camera manager was refactored to use a new `ICameraAdapter` abstraction, replacing the Windows-specific DirectShow implementation.
- **Image Format**: The `TIFF + Proxy` capture mode has been changed to `PNG + Proxy`, using PNG for master files.
- **Stream Deck Integration**: The image generation for Stream Deck keys now uses the cross-platform `SixLabors.ImageSharp` library.

#### Fixed
- **Startup Crash**: Fixed a fatal error during application startup caused by a file system scan encountering corrupted files in unrelated directories (e.g., Android SDK). The application is now more resilient to these errors.
- **Startup Performance**: Significantly improved startup time by restricting the search for camera SDKs to the project's own `SDKs` directory, avoiding a slow and unnecessary scan of the entire file system.
- **Histogram Performance**: The histogram calculation and rendering have been rewritten for better performance and cross-platform compatibility.
- **Keyboard Input**: Implemented debouncing for keyboard shortcuts (like the spacebar for capture) to prevent accidental multiple triggers from auto-repeating keys.
- **UI Responsiveness**: Improved overall UI performance and responsiveness due to the framework migration and code optimizations.

### Deutsch

#### Hinzugefuegt
- **Plattformuebergreifende Unterstuetzung**: Die gesamte Anwendung wurde von WPF auf das plattformuebergreifende Avalonia UI Framework migriert, was die Unterstuetzung fuer Windows, macOS und Linux ermoeglicht.
- **Erweiterte SDK-Erkennung**: Ein neuer Dienst wurde implementiert, um Kamera-SDKs von Nikon und Canon automatisch zu finden, einschliesslich der Suche in lokalen Ordnern und ZIP-Archiven. Dies ermoeglicht die direkte Hardware-Steuerung von unterstuetzten Kameras.
- **Hardware-Aufnahme**: Unterstuetzung fuer das Ausloesen von Standbildaufnahmen direkt von angeschlossenen Nikon-Kameras hinzugefuegt. Das aufgenommene Bild wird automatisch in die Timeline importiert.
- **Projekt-Laden & Migration**: Die Anwendung kann jetzt bestehende Projekte von der Festplatte laden. Ein automatischer Migrationsdienst aktualisiert aeltere Projektstrukturen auf die neueste Version.
- **Kompositions-Overlays**: Neue visuelle Hilfslinien fuer die Live-Vorschau hinzugefuegt, darunter ein Drittel-Regel-Gitter, ein 4x4-Gitter, ein Fadenkreuz in der Mitte sowie Action/Title-Safe-Overlays.
- **Verbesserte Timeline-Navigation**: Die Timeline kann jetzt mit dem Mausrad und den Links/Rechts-Pfeiltasten bildweise navigiert werden.

#### Geaendert
- **UI-Framework**: Alle WPF-UI-Komponenten wurden fuer plattformuebergreifende Kompatibilitaet durch ihre Avalonia-Aequivalente ersetzt.
- **Kamera-Backend**: Der Kamera-Manager wurde umgestaltet und verwendet nun eine neue `ICameraAdapter`-Abstraktion, die die Windows-spezifische DirectShow-Implementierung ersetzt.
- **Bildformat**: Der Aufnahmemodus `TIFF + Proxy` wurde in `PNG + Proxy` geaendert, wobei PNG fuer Master-Dateien verwendet wird.
- **Stream-Deck-Integration**: Die Bilderzeugung fuer die Stream-Deck-Tasten verwendet jetzt die plattformuebergreifende `SixLabors.ImageSharp`-Bibliothek.

#### Behoben
- **Absturz beim Start**: Ein fataler Fehler beim Anwendungsstart wurde behoben, der durch das Scannen des Dateisystems und das Auffinden beschaedigter Dateien in nicht zusammenhaengenden Verzeichnissen (z. B. Android SDK) verursacht wurde. Die Anwendung ist jetzt widerstandsfaehiger gegen solche Fehler.
- **Startperformance**: Die Startzeit wurde erheblich verbessert, indem die Suche nach Kamera-SDKs auf das projekteigene `SDKs`-Verzeichnis beschraenkt wurde, wodurch ein langsames und unnoetiges Scannen des gesamten Dateisystems vermieden wird.
- **Histogramm-Performance**: Die Berechnung und das Rendern des Histogramms wurden fuer bessere Leistung und plattformuebergreifende Kompatibilitaet neu geschrieben.
- **Tastatureingabe**: Ein Debouncing fuer Tastaturkuerzel (wie die Leertaste fuer die Aufnahme) wurde implementiert, um versehentliche Mehrfachausloesungen durch sich wiederholende Tasten zu verhindern.
- **UI-Reaktivitaet**: Die allgemeine UI-Leistung und -Reaktivitaet wurde durch die Framework-Migration und Code-Optimierungen verbessert.

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
