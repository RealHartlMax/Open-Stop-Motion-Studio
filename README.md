# 🎬 Open Stop Motion Studio — MVP v0.1

**License:** GPLv3
**Platform:** Windows 10/11 (.NET 8, WPF)
**Status:** MVP – Phase 1 (Live Camera + Onion Skin + Stream Deck)

---

# 🇬🇧 English

## Overview

**Open Stop Motion Studio** is an open-source tool for professional and hobby animators.
Its goal is to bridge the gap between simple webcam tools and expensive professional solutions like Dragonframe — modular, extensible, and fully GPLv3.

---

## 🚀 Quick Start (Visual Studio)

### Requirements

* Visual Studio 2022 (Community Edition is sufficient)
* .NET 8 SDK (included with Visual Studio)
* Windows 10 or 11 (WPF is Windows-only)

### Steps

1. Clone the repository or download as ZIP
2. Open `OpenStopMotionStudio.sln` in Visual Studio
3. NuGet will automatically restore dependencies
4. Press **F5** to run the application

---

## 📦 Dependencies Explained

* **DirectShowLib-2005**
  Handles camera communication via Windows DirectShow API.
  Supports webcams, HDMI capture cards, and any video capture device.

* **Emgu.CV**
  OpenCV wrapper for C#.
  Not heavily used in MVP yet, but prepared for:

  * Multi-frame onion skin
  * Difference blending
  * Motion detection (shake warning)

* **OpenMacroBoard.SDK + OpenMacroBoard.StreamDeck**
  Enables integration with Elgato Stream Deck (all models supported).

* **CommunityToolkit.Mvvm**
  Provides MVVM infrastructure.
  Light usage in MVP, foundation for future UI architecture.

---

## 📷 DSLR Cameras (Canon / Sony)

Currently, cameras are accessed via DirectShow (webcams & capture cards only).

Planned:

* **Canon:** EDSDK
* **Sony:** Remote SDK

The `CameraManager` is designed to support this extension without breaking existing code.

---

## ⌨️ Keyboard Shortcuts

| Action                   | Shortcut |
| ------------------------ | -------- |
| Capture Frame            | Space    |
| Undo (Phase 2)           | Ctrl + Z |
| Toggle Overlay (Phase 2) | O        |

---

## 🎛️ Stream Deck Layout (Optional)

```
[ CAPTURE ] [ ONION ] [ 25% ] [ 50% ] [ 75% ]
[         ] [       ] [      ] [      ] [      ]
[  UNDO   ] [       ] [      ] [      ] [      ]
```

Works with all Stream Deck models. Fully optional.

---

## 🧱 Project Structure

```
OpenStopMotionStudio/
├── Core/
│   ├── CameraManager.cs
│   ├── CaptureManager.cs
│   ├── OverlayManager.cs
│   └── StreamDeckManager.cs
├── GUI/
│   ├── MainWindow.xaml
│   └── MainWindow.xaml.cs
├── App.xaml
├── App.xaml.cs
└── OpenStopMotionStudio.csproj
```

---

## 🗺️ Roadmap

| Phase | Focus                                     |
| ----- | ----------------------------------------- |
| 1 ✅   | Live Camera + Onion Skin + Stream Deck    |
| 2     | RAW/TIFF/EXR + Proxy Workflow             |
| 3     | Timeline / Playback + Advanced Onion Skin |
| 4     | Motion Control (Stepper / Arduino)        |
| 5     | DMX Lighting Integration                  |
| 6     | Lip Sync, X-Sheet, Pro Features           |

---

## 📜 License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.

You are free to use, modify, and distribute this software under the same terms.
All derivative work must remain open source.

---

## 🤝 Contributing

Contributions, ideas, and feedback are welcome.
Goal: build a fully-featured open alternative for stop-motion production.

---

# 🇩🇪 Deutsch

## Überblick

**Open Stop Motion Studio** ist ein Open-Source-Tool für professionelle und Hobby-Animatoren.
Ziel ist es, die Lücke zwischen einfachen Webcam-Tools und teurer Profi-Software wie Dragonframe zu schließen — modular, erweiterbar und unter GPLv3.

---

## 🚀 Schnellstart (Visual Studio)

### Voraussetzungen

* Visual Studio 2022 (Community Edition ausreichend)
* .NET 8 SDK
* Windows 10 oder 11 (WPF benötigt Windows)

### Schritte

1. Repository klonen oder ZIP herunterladen
2. `OpenStopMotionStudio.sln` öffnen
3. NuGet lädt automatisch alle Abhängigkeiten
4. **F5 drücken** → Anwendung startet

---

## 📦 Abhängigkeiten erklärt

* **DirectShowLib-2005**
  Kamera-Anbindung über Windows DirectShow API
  Unterstützt Webcams, Capture Cards, etc.

* **Emgu.CV**
  OpenCV-Wrapper für C#
  Vorbereitung für:

  * Multi-Onion Skin
  * Differenz-Blending
  * Motion Detection (Wackelwarnung)

* **OpenMacroBoard.SDK + StreamDeck**
  Integration für Elgato Stream Deck (alle Modelle)

* **CommunityToolkit.Mvvm**
  Grundlage für MVVM-Architektur (späterer Ausbau)

---

## 📷 DSLR-Kameras (Canon / Sony)

Im MVP erfolgt die Anbindung über DirectShow (keine direkte DSLR-Unterstützung).

Geplant:

* Canon → EDSDK
* Sony → Remote SDK

Die Architektur des `CameraManager` ist bereits darauf vorbereitet.

---

## ⌨️ Tastaturkürzel

| Aktion                   | Shortcut  |
| ------------------------ | --------- |
| Frame aufnehmen          | Leertaste |
| Undo (Phase 2)           | Strg + Z  |
| Overlay Toggle (Phase 2) | O         |

---

## 🎛️ Stream Deck Layout (Optional)

```
[ CAPTURE ] [ ONION ] [ 25% ] [ 50% ] [ 75% ]
[         ] [       ] [      ] [      ] [      ]
[  UNDO   ] [       ] [      ] [      ] [      ]
```

Funktioniert mit allen Stream Deck Modellen. Optional.

---

## 🧱 Projektstruktur

(Siehe englischen Abschnitt oben – identisch)

---

## 🗺️ Roadmap

| Phase | Fokus                                  |
| ----- | -------------------------------------- |
| 1 ✅   | Live-Kamera + Onion Skin + Stream Deck |
| 2     | RAW/TIFF/EXR + Proxy Workflow          |
| 3     | Timeline / Playback                    |
| 4     | Motion Control                         |
| 5     | DMX Lighting                           |
| 6     | Profi-Features                         |

---

## 📜 Lizenz

Dieses Projekt steht unter der **GNU General Public License v3 (GPLv3)**.

Alle Weiterentwicklungen müssen ebenfalls Open Source bleiben.

---

## 🤝 Mitwirken

Beiträge, Ideen und Feedback sind willkommen.
Ziel ist eine vollwertige Open-Source-Alternative für Stop-Motion-Produktion.
