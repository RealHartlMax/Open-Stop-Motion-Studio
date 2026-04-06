# Open Stop Motion Studio — v0.1.1

**License:** GPLv3  
**Platform:** Windows, macOS, Linux (.NET 10, Avalonia)  
**Status:** Early MVP / früher MVP

`CHANGELOG:` [CHANGELOG.md](CHANGELOG.md)  
`Vendor SDK notes:` [SDKs/README.md](SDKs/README.md)

---

# English

## Overview

**Open Stop Motion Studio** is an open-source, cross-platform stop-motion application for hobby and professional workflows. This project is in an early but usable MVP state, with a focus on camera preview, frame capture, timeline playback, onion skinning, and initial RAW import capabilities.

It is not yet a full replacement for tools like Dragonframe. Features such as EXR master output, tethered DSLR control, motion control, and DMX authoring are planned for future releases.

## Current Feature Set

### Capture and Preview

- Live camera preview from connected webcams, USB cameras, capture cards, and virtual cameras.
- Camera selection and device-specific settings dialogs.
- Live luminance histogram.
- Status bar and a `start.bat` script for easy launching.

### Stop-Motion Workflow

- Frame capture with `Space`.
- Sequential shot and frame naming.
- Project folder selection.
- Onion skinning with 1-3 layers and adjustable opacity.
- Undo support for the last captured frame.

### Timeline and Playback

- Dope-sheet style timeline.
- Playback controls with adjustable FPS (`1` to `120`).
- Mouse-wheel scrubbing over the timeline (`Shift` for faster scrubbing).
- Playback preview of captured frames.

### File Output

- `JPEG sequence` capture mode.
- `TIFF + Proxy` capture mode for higher quality workflows.
- Shot-based naming for saved frames.
- Internal frame-offset support (e.g., starting sequences at `1001`).

### RAW & Vendor SDKs

- Local discovery of Canon, Nikon, and Sony SDKs from the `SDKs/` directory.
- **Nikon**: `NEF` / `NRW` folder import using the local Nikon Image SDK.
- Import output to `Raw/<Shot>/...tif` and `Proxy/<Shot>/...jpg`.
- Configurable import start frame.

### Hardware Integration

- Elgato Stream Deck support for capture, onion skinning, opacity control, and undo.
- Local vendor SDK structure for Canon, Nikon, and Sony development.

## Quick Start

### Option 1: Start Script

1. Install the **.NET 10 SDK**.
2. Run [`start.bat`](start.bat).
3. Select a camera and click `Kamera starten`.
4. Press `Space` to capture a frame.

### Option 2: Visual Studio

1. Open [`Open-Stop-Motion-Studio.sln`](Open-Stop-Motion-Studio.sln).
2. Restore NuGet packages.
3. Start the project with `F5`.

## Requirements

- **Windows**: Windows 10 or 11
- **macOS**: macOS 11 (Big Sur) or newer
- **Linux**: A modern distribution (e.g., Ubuntu 22.04, Fedora 38)
- **.NET 10 SDK**
- **Development**: Visual Studio 2022, JetBrains Rider, or VS Code.
- A webcam, capture card, or other compatible video source.
- Optional: Nikon/Canon/Sony SDK files placed in `SDKs/` for tethered control.

## Dependencies

The project is built on the following core technologies:
- **UI:** [Avalonia](https://avaloniaui.net/) (cross-platform UI framework)
- **Image Processing:** [Emgu.CV](http://www.emgu.com/wiki/index.php/Main_Page) (OpenCV wrapper) and [ImageSharp](https://sixlabors.com/products/imagesharp/)
- **Hardware:** [StreamDeckSharp](https://github.com/OpenMacroBoard/StreamDeckSharp) for Elgato Stream Deck integration.
- **Architecture:** MVVM using the [CommunityToolkit.Mvvm](https://learn.microsoft.com/en-us/dotnet/communitytoolkit/mvvm/) library.

## Project Structure

```text
OpenStopMotionStudio/
├── Core/
│   ├── CameraAdapterBase.cs      // Base classes for camera implementations
│   ├── CameraManager.cs          // Handles camera selection and lifecycle
│   ├── CaptureManager.cs         // Manages frame capture and file output
│   ├── NikonNefImportService.cs  // Logic for importing Nikon RAW files
│   ├── OverlayManager.cs         // Handles onion skinning
│   ├── StreamDeckManager.cs      // Integrates with Elgato Stream Deck
│   └── Startup/
│       └── DeviceEnumerationTask.cs // Background task for finding cameras
├── GUI/
│   ├── MainWindow.axaml          // Main application window layout
│   ├── MainWindow.axaml.cs       // Code-behind for the main window
│   ├── MainWindow.Timeline.cs    // Partial class for timeline logic
│   ├── MainWindow.Histogram.cs   // Partial class for histogram logic
│   ├── MainWindow.RawImport.cs   // Partial class for RAW import UI
│   ├── ProjectWindow.axaml       // The main window for a project
│   └── SplashWindow.axaml        // Splash screen shown on startup
├── SDKs/                           // Folder for local vendor SDKs (see SDKs/README.md)
├── CHANGELOG.md
├── README.md
├── start.bat
└── OpenStopMotionStudio.csproj
```

## Roadmap

| Phase | Status | Focus |
| --- | --- | --- |
| 1 | done | Live preview, capture, onion skin, Stream Deck |
| 2 | in progress | TIFF/proxy workflow, RAW import groundwork |
| 3 | in progress | Timeline, playback, advanced onion skin |
| 4 | planned | Tethered DSLR control and deeper camera SDK integration |
| 5 | planned | Motion control (e.g., for camera sliders) |
| 6 | planned | DMX lighting and other production tools |

---

# Deutsch

## Überblick

**Open Stop Motion Studio** ist eine Open-Source, plattformübergreifende Stop-Motion-Anwendung für Hobby- und professionelle Workflows. Das Projekt befindet sich in einem frühen, aber nutzbaren MVP-Zustand, mit Fokus auf Kameravorschau, Frame-Aufnahme, Timeline-Playback, Onion Skinning und ersten RAW-Import-Fähigkeiten.

Es ist noch kein vollständiger Ersatz für Werkzeuge wie Dragonframe. Funktionen wie EXR-Master-Ausgabe, angebundene DSLR-Steuerung, Motion Control und DMX-Authoring sind für zukünftige Versionen geplant.

## Aktueller Funktionsstand

### Aufnahme und Vorschau

- Live-Kameravorschau von angeschlossenen Webcams, USB-Kameras, Capture Cards und virtuellen Kameras.
- Kameraauswahl und gerätespezifische Einstellungsdialoge.
- Live-Luminanz-Histogramm.
- Statusleiste und ein `start.bat`-Skript für einfaches Starten.

### Stop-Motion-Workflow

- Frame-Aufnahme mit der `Leertaste`.
- Fortlaufende Benennung von Shots und Frames.
- Auswahl des Projektordners.
- Onion Skinning mit 1-3 Layern und einstellbarer Deckkraft.
- Undo-Funktion für das zuletzt aufgenommene Frame.

### Timeline und Playback

- Zeitleiste im Dope-Sheet-Stil.
- Playback-Steuerung mit einstellbaren FPS (`1` bis `120`).
- Scrubbing per Mausrad über die Timeline (`Shift` für schnelleres Scrubbing).
- Playback-Vorschau der aufgenommenen Frames.

### Dateiausgabe

- `JPEG sequence`-Aufnahmemodus.
- `TIFF + Proxy`-Aufnahmemodus für Workflows mit höherer Qualität.
- Shot-basierte Benennung für gespeicherte Frames.
- Interner Frame-Offset (z.B. um Sequenzen bei `1001` zu starten).

### RAW & Hersteller-SDKs

- Lokale Erkennung von Canon-, Nikon- und Sony-SDKs aus dem `SDKs/`-Verzeichnis.
- **Nikon**: `NEF`- / `NRW`-Ordnerimport über das lokale Nikon Image SDK.
- Import-Ausgabe nach `Raw/<Shot>/...tif` und `Proxy/<Shot>/...jpg`.
- Konfigurierbarer Startframe für den Import.

### Hardware-Integration

- Elgato Stream Deck-Unterstützung für Aufnahme, Onion Skinning, Deckkraft-Steuerung und Undo.
- Lokale Ordnerstruktur für die Entwicklung mit Canon-, Nikon- und Sony-SDKs.

## Schnellstart

### Option 1: Start-Skript

1. Das **.NET 10 SDK** installieren.
2. [`start.bat`](start.bat) ausführen.
3. Eine Kamera auswählen und auf `Kamera starten` klicken.
4. `Leertaste` drücken, um ein Frame aufzunehmen.

### Option 2: Visual Studio

1. [`Open-Stop-Motion-Studio.sln`](Open-Stop-Motion-Studio.sln) öffnen.
2. NuGet-Pakete wiederherstellen.
3. Das Projekt mit `F5` starten.

## Voraussetzungen

- **Windows**: Windows 10 oder 11
- **macOS**: macOS 11 (Big Sur) oder neuer
- **Linux**: Eine moderne Distribution (z.B. Ubuntu 22.04, Fedora 38)
- **.NET 10 SDK**
- **Entwicklung**: Visual Studio 2022, JetBrains Rider oder VS Code.
- Eine Webcam, Capture Card oder eine andere kompatible Videoquelle.
- Optional: Nikon/Canon/Sony SDK-Dateien im Ordner `SDKs/` für die angebundene Steuerung.

## Abhängigkeiten

Das Projekt basiert auf den folgenden Kerntechnologien:
- **UI:** [Avalonia](https://avaloniaui.net/) (plattformübergreifendes UI-Framework)
- **Bildverarbeitung:** [Emgu.CV](http://www.emgu.com/wiki/index.php/Main_Page) (OpenCV-Wrapper) und [ImageSharp](https://sixlabors.com/products/imagesharp/)
- **Hardware:** [StreamDeckSharp](https://github.com/OpenMacroBoard/StreamDeckSharp) für die Elgato Stream Deck-Integration.
- **Architektur:** MVVM mit dem [CommunityToolkit.Mvvm](https://learn.microsoft.com/en-us/dotnet/communitytoolkit/mvvm/)

## Projektstruktur

```text
OpenStopMotionStudio/
├── Core/
│   ├── CameraAdapterBase.cs      // Basisklassen für Kamera-Implementierungen
│   ├── CameraManager.cs          // Verwaltet Kameraauswahl und Lebenszyklus
│   ├── CaptureManager.cs         // Steuert die Frame-Aufnahme und Dateiausgabe
│   ├── NikonNefImportService.cs  // Logik für den Import von Nikon-RAW-Dateien
│   ├── OverlayManager.cs         // Zuständig für Onion Skinning
│   ├── StreamDeckManager.cs      // Integriert das Elgato Stream Deck
│   └── Startup/
│       └── DeviceEnumerationTask.cs // Hintergrund-Task zum Finden von Kameras
├── GUI/
│   ├── MainWindow.axaml          // Layout des Hauptanwendungsfensters
│   ├── MainWindow.axaml.cs       // Code-Behind für das Hauptfenster
│   ├── MainWindow.Timeline.cs    // Partielle Klasse für die Timeline-Logik
│   ├── MainWindow.Histogram.cs   // Partielle Klasse für die Histogramm-Logik
│   ├── MainWindow.RawImport.cs   // Partielle Klasse für die RAW-Import-UI
│   ├── ProjectWindow.axaml       // Das Hauptfenster für ein Projekt
│   └── SplashWindow.axaml        // Splash-Screen beim Start
├── SDKs/                           // Ordner für lokale Hersteller-SDKs (siehe SDKs/README.md)
├── CHANGELOG.md
├── README.md
├── start.bat
└── OpenStopMotionStudio.csproj
```

## Roadmap

| Phase | Status | Fokus |
| --- | --- | --- |
| 1 | fertig | Live-Vorschau, Capture, Onion Skin, Stream Deck |
| 2 | in Arbeit | TIFF/Proxy-Workflow, RAW-Import-Fundament |
| 3 | in Arbeit | Timeline, Playback, Advanced Onion Skin |
| 4 | geplant | Tethered DSLR-Steuerung und tiefere Kamera-SDK-Integration |
| 5 | geplant | Motion Control (z.B. für Kamera-Slider) |
| 6 | geplant | DMX-Licht und weitere Produktionstools |

## Mitwirken

Ideen, Test-Feedback, Bugreports und Code-Beiträge sind willkommen. Ziel ist es, das Projekt zu einem praxistauglichen Open-Source-Tool für die Stop-Motion-Produktion zu entwickeln.

## Lizenz

Dieses Projekt steht unter der **GNU General Public License v3.0 (GPLv3)**.
