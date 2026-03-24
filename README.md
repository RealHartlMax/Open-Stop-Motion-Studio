# Open Stop Motion Studio — v0.1

**License:** GPLv3  
**Platform:** Windows 10/11 (.NET 10, WPF)  
**Status:** Early MVP / frueher MVP

`CHANGELOG:` [CHANGELOG.md](CHANGELOG.md)  
`Vendor SDK notes:` [SDKs/README.md](SDKs/README.md)

---

# English

## Overview

**Open Stop Motion Studio** is an open-source stop-motion tool for hobby and professional workflows on Windows.
The current `v0.1` release is an early but usable MVP focused on camera preview, frame capture, timeline playback, onion skinning, and first RAW-import groundwork.

It is not a full Dragonframe replacement yet.
Features such as EXR master output, tethered DSLR control, motion control, and DMX authoring are still planned or in progress.

## Current Feature Set

### Capture and Preview

- Live camera preview through Windows DirectShow
- Camera selection for webcams, USB cameras, capture cards, and virtual cameras
- Webcam driver/settings dialog when the device exposes one
- Live luminance histogram below the camera selector
- Status bar and startup/build launcher via `start.bat`

### Stop-Motion Workflow

- Frame capture with `Space`
- Shot naming with sequential file naming
- Project folder selection
- Onion skin with 1-3 layers and adjustable opacity presets
- Undo support for the last captured frame

### Timeline and Playback

- Dope-sheet style timeline area
- Playback controls with manual FPS input from `1` to `120`
- Mouse-wheel scrubbing over the timeline
- `Shift` + mouse wheel for larger frame jumps
- Playback preview of captured frames

### File Output

- `JPEG sequence` capture mode
- `TIFF + Proxy` capture mode
- Shot-based naming for saved frames
- Internal frame-offset support for film-style sequences such as `1001`

### RAW / Nikon Foundation

- Local Nikon SDK discovery from `SDKs/Nikon`
- Nikon `NEF` / `NRW` folder import through the local Nikon Image SDK
- Import output to:
  - `Raw/<Shot>/...tif`
  - `Proxy/<Shot>/...jpg` or `...png`
- Configurable import start frame, default `1001`

### Hardware Integration

- Stream Deck integration for capture, onion toggle, opacity presets, and undo
- Local vendor SDK folder structure for Nikon and Canon development work

## Quick Start

### Option 1: Start Script

1. Install a current `.NET 10 SDK`
2. Run [`start.bat`](start.bat)
3. Select a camera
4. Click `Kamera starten`
5. Press `Space` to capture a frame

### Option 2: Visual Studio

1. Open [`Open-Stop-Motion-Studio.sln`](Open-Stop-Motion-Studio.sln)
2. Restore NuGet packages
3. Start the project with `F5`

## Requirements

- Windows 10 or Windows 11
- .NET 10 SDK
- Visual Studio 2022 recommended for development
- A webcam, capture card, or other DirectShow-compatible video source for live preview
- Optional: Nikon SDK files placed locally under `SDKs/Nikon` for NEF import

## Keyboard and UI Shortcuts

| Action | Shortcut |
| --- | --- |
| Capture frame | `Space` |
| Scrub timeline | Mouse wheel |
| Scrub faster | `Shift` + mouse wheel |

## Project Structure

```text
OpenStopMotionStudio/
├── Core/
│   ├── CameraManager.cs
│   ├── CaptureManager.cs
│   ├── NikonNefImportService.cs
│   ├── NikonSdkDiscovery.cs
│   ├── OverlayManager.cs
│   └── StreamDeckManager.cs
├── GUI/
│   ├── MainWindow.xaml
│   ├── MainWindow.xaml.cs
│   ├── MainWindow.Timeline.cs
│   ├── MainWindow.Histogram.cs
│   └── MainWindow.RawImport.cs
├── SDKs/
├── CHANGELOG.md
├── README.md
├── start.bat
└── OpenStopMotionStudio.csproj
```

## Roadmap

| Phase | Status | Focus |
| --- | --- | --- |
| 1 | done | Live preview, capture, onion skin, Stream Deck |
| 2 | in progress | TIFF/proxy workflow, RAW import groundwork, EXR later |
| 3 | in progress | Timeline, playback, advanced onion skin |
| 4 | planned | Tethered DSLR control and deeper camera SDK integration |
| 5 | planned | Motion control |
| 6 | planned | DMX lighting and production tools |

## Documentation

- Version changes: [CHANGELOG.md](CHANGELOG.md)
- Local SDK handling: [SDKs/README.md](SDKs/README.md)

## Contributing

Ideas, testing feedback, bug reports, and code contributions are welcome.
The goal is to grow this project into a practical open-source stop-motion production tool.

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.

---

# Deutsch

## Ueberblick

**Open Stop Motion Studio** ist ein Open-Source-Stop-Motion-Tool fuer Hobby- und professionelle Workflows unter Windows.
Die aktuelle Version `v0.1` ist ein frueher, aber bereits nutzbarer MVP mit Fokus auf Kameravorschau, Frame-Aufnahme, Timeline-Playback, Onion Skin und dem ersten Fundament fuer RAW-Import.

Es ist noch kein vollstaendiger Dragonframe-Ersatz.
Funktionen wie EXR-Master-Ausgabe, tethered DSLR-Steuerung, Motion Control und DMX-Authoring sind noch geplant oder im Ausbau.

## Aktueller Funktionsstand

### Aufnahme und Vorschau

- Live-Kameravorschau ueber Windows DirectShow
- Kameraauswahl fuer Webcams, USB-Kameras, Capture Cards und virtuelle Kameras
- Webcam-/Treiberdialog, wenn das Geraet ihn anbietet
- Live-Luminanz-Histogramm unter der Kameraauswahl
- Statusleiste und Build-/Start-Launcher ueber `start.bat`

### Stop-Motion-Workflow

- Frame-Aufnahme mit `Leertaste`
- Shot-Namen mit fortlaufender Dateibenennung
- Projektordner-Auswahl
- Onion Skin mit 1-3 Layern und einstellbaren Transparenz-Presets
- Undo fuer das zuletzt aufgenommene Frame

### Timeline und Playback

- Dope-Sheet-aehnliche Timeline-Ansicht
- Playback-Steuerung mit direkter FPS-Eingabe von `1` bis `120`
- Scrubbing per Mausrad ueber der Timeline
- `Shift` + Mausrad fuer groessere Frame-Spruenge
- Playback-Vorschau der aufgenommenen Frames

### Dateiausgabe

- `JPEG sequence` als Capture-Modus
- `TIFF + Proxy` als Capture-Modus
- Shot-basierte Dateinamen
- Interner Frame-Offset fuer filmtypische Sequenzen wie `1001`

### RAW- / Nikon-Grundlage

- Lokale Nikon-SDK-Erkennung aus `SDKs/Nikon`
- Nikon-`NEF`- / `NRW`-Ordnerimport ueber das lokale Nikon Image SDK
- Import-Ausgabe nach:
  - `Raw/<Shot>/...tif`
  - `Proxy/<Shot>/...jpg` oder `...png`
- Konfigurierbarer Import-Startframe, standardmaessig `1001`

### Hardware-Integration

- Stream-Deck-Anbindung fuer Capture, Onion-Toggle, Transparenz-Presets und Undo
- Lokale Vendor-SDK-Struktur fuer Nikon- und Canon-Entwicklung

## Schnellstart

### Option 1: Start-Skript

1. Aktuelles `.NET 10 SDK` installieren
2. [`start.bat`](start.bat) ausfuehren
3. Kamera auswaehlen
4. `Kamera starten` klicken
5. Mit `Leertaste` ein Frame aufnehmen

### Option 2: Visual Studio

1. [`Open-Stop-Motion-Studio.sln`](Open-Stop-Motion-Studio.sln) oeffnen
2. NuGet-Pakete wiederherstellen
3. Projekt mit `F5` starten

## Voraussetzungen

- Windows 10 oder Windows 11
- .NET 10 SDK
- Visual Studio 2022 empfohlen fuer die Entwicklung
- Eine Webcam, Capture Card oder andere DirectShow-kompatible Videoquelle fuer die Live-Vorschau
- Optional: lokal abgelegte Nikon-SDK-Dateien unter `SDKs/Nikon` fuer den NEF-Import

## Tastatur- und UI-Shortcuts

| Aktion | Shortcut |
| --- | --- |
| Frame aufnehmen | `Leertaste` |
| Timeline scrubbing | Mausrad |
| Schneller scrubbing | `Shift` + Mausrad |

## Projektstruktur

```text
OpenStopMotionStudio/
├── Core/
├── GUI/
├── SDKs/
├── CHANGELOG.md
├── README.md
├── start.bat
└── OpenStopMotionStudio.csproj
```

## Roadmap

| Phase | Status | Fokus |
| --- | --- | --- |
| 1 | fertig | Live-Vorschau, Capture, Onion Skin, Stream Deck |
| 2 | in Arbeit | TIFF/Proxy-Workflow, RAW-Import-Fundament, EXR spaeter |
| 3 | in Arbeit | Timeline, Playback, Advanced Onion Skin |
| 4 | geplant | Tethered DSLR-Steuerung und tiefere Kamera-SDK-Integration |
| 5 | geplant | Motion Control |
| 6 | geplant | DMX-Licht und Produktionstools |

## Dokumentation

- Versionsaenderungen: [CHANGELOG.md](CHANGELOG.md)
- Lokale SDK-Handhabung: [SDKs/README.md](SDKs/README.md)

## Mitwirken

Ideen, Test-Feedback, Bugreports und Code-Beitraege sind willkommen.
Ziel ist es, das Projekt zu einem praxistauglichen Open-Source-Tool fuer Stop-Motion-Produktion weiterzuentwickeln.

## Lizenz

Dieses Projekt steht unter der **GNU General Public License v3.0 (GPLv3)**.
