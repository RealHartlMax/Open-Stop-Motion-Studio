using System;
using System.IO;
using System.Windows.Media.Imaging;

namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// CaptureManager: Verantwortlich für das Speichern von Frames und
    /// die Verwaltung der Bildsequenz.
    ///
    /// Designentscheidungen erklärt:
    ///
    /// 1. DATEINAMEN-SCHEMA: Frames werden als "frame_0001.jpg", "frame_0002.jpg"
    ///    usw. gespeichert. Das führende Null-Padding ist wichtig, weil Dateisysteme
    ///    und NLEs (Premiere, DaVinci) Bildsequenzen alphabetisch sortieren.
    ///    Ohne Padding käme "frame_10" vor "frame_2".
    ///
    /// 2. LETZTER FRAME: LastFrame wird nicht aus der Datei zurückgeladen,
    ///    sondern direkt im Speicher gehalten. Das ist schneller und vermeidet
    ///    Datei-I/O im heißen Pfad (Onion Skin muss bei ~30fps verfügbar sein).
    ///
    /// 3. OUTPUT-ORDNER: Wenn kein Ordner gewählt wurde, speichern wir in einem
    ///    temporären Unterordner im Benutzer-Dokumente-Verzeichnis. So wird der
    ///    User nicht überrascht, aber verliert auch nichts.
    /// </summary>
    public class CaptureManager
    {
        // ── Öffentliche Properties ───────────────────────────────────────────
        /// <summary>
        /// Anzahl der bisher aufgenommenen Frames (für den Frame-Zähler in der UI).
        /// </summary>
        public int FrameCount { get; private set; }

        /// <summary>
        /// Das zuletzt gespeicherte Frame – wird vom OverlayManager als
        /// Onion Skin verwendet. Im Speicher gehalten für schnellen Zugriff.
        /// </summary>
        public BitmapSource? LastFrame { get; private set; }

        // ── Interner Zustand ────────────────────────────────────────────────
        private string _outputFolder;

        public CaptureManager()
        {
            // Standardordner: Dokumente/OpenStopMotionStudio/Untitled
            _outputFolder = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                "OpenStopMotionStudio", "Untitled");

            EnsureOutputFolderExists();
        }

        // ════════════════════════════════════════════════════════════════════
        //  FRAME SPEICHERN
        // ════════════════════════════════════════════════════════════════════

        /// <summary>
        /// Speichert einen Frame als JPEG-Datei und gibt den Dateipfad zurück.
        ///
        /// Warum JPEG als Standard für den MVP?
        /// JPEG bietet einen guten Kompromiss: klein genug, um bei langen
        /// Animationen nicht gigantische Ordner zu erzeugen, aber gut genug
        /// für Preview und direkten Export. Phase 2 ergänzt TIFF/EXR für
        /// professionelle Workflows mit echten RAW-Daten und voller Farbtiefe.
        /// </summary>
        public string SaveFrame(BitmapSource frame)
        {
            FrameCount++;

            // Dateinamen mit führenden Nullen: 0001, 0002, ... 9999
            // Das ermöglicht bis zu 9999 Frames (~6,5 Minuten bei 25fps) ohne
            // Sortierprobleme – für Stop-Motion-Projekte mehr als ausreichend.
            string fileName    = $"frame_{FrameCount:D4}.jpg";
            string fullPath    = Path.Combine(_outputFolder, fileName);

            SaveBitmapAsJpeg(frame, fullPath);

            // Im Speicher halten für Onion Skin (kein erneutes Laden nötig)
            LastFrame = frame;

            System.Diagnostics.Debug.WriteLine($"[CaptureManager] Saved: {fullPath}");
            return fullPath;
        }

        /// <summary>
        /// Speichert eine BitmapSource als JPEG-Datei mit 90% Qualität.
        ///
        /// Technische Anmerkung: WPF's JpegBitmapEncoder arbeitet direkt mit
        /// BitmapSource – kein Umweg über System.Drawing.Bitmap nötig. Das spart
        /// eine Konvertierung und ist damit schneller.
        /// </summary>
        private static void SaveBitmapAsJpeg(BitmapSource bitmap, string path)
        {
            var encoder = new JpegBitmapEncoder
            {
                QualityLevel = 90 // 90% = gute Qualität, ~3–5MB pro Bild je nach Auflösung
            };
            encoder.Frames.Add(BitmapFrame.Create(bitmap));

            using var fileStream = new FileStream(path, FileMode.Create, FileAccess.Write);
            encoder.Save(fileStream);
        }

        // ════════════════════════════════════════════════════════════════════
        //  PROJEKT-VERWALTUNG
        // ════════════════════════════════════════════════════════════════════

        /// <summary>
        /// Setzt den Ausgabeordner für neue Frames.
        /// Wichtig: Der FrameCount wird NICHT zurückgesetzt, weil der User
        /// vielleicht nur den Speicherort ändert, nicht das Projekt neu beginnt.
        /// Für ein neues Projekt gibt es <see cref="NewProject"/>.
        /// </summary>
        public void SetOutputFolder(string folder)
        {
            _outputFolder = folder;
            EnsureOutputFolderExists();
        }

        /// <summary>
        /// Startet ein neues Projekt: Zähler zurücksetzen, LastFrame löschen.
        /// Der Ordner bleibt erhalten – der User hat ihn bewusst gewählt.
        /// </summary>
        public void NewProject()
        {
            FrameCount = 0;
            LastFrame  = null;
        }

        /// <summary>
        /// Letzten Frame rückgängig machen: Datei löschen und Zähler dekrementieren.
        ///
        /// Diese "Undo-Capture" Funktion ist in Stop-Motion-Software essentiell –
        /// Fehler passieren, und kein Animator will dafür extra ein Dateibrowser
        /// öffnen müssen.
        /// </summary>
        public bool UndoLastCapture()
        {
            if (FrameCount == 0) return false;

            string fileName = $"frame_{FrameCount:D4}.jpg";
            string fullPath = Path.Combine(_outputFolder, fileName);

            if (File.Exists(fullPath))
                File.Delete(fullPath);

            FrameCount--;
            // LastFrame wird auf das neue letzte Frame gesetzt (Datei laden)
            LastFrame = FrameCount > 0 ? LoadFrame(FrameCount) : null;

            return true;
        }

        /// <summary>
        /// Lädt ein Frame anhand seines Index aus dem Dateisystem.
        /// Wird nur bei Undo benötigt – im normalen Betrieb arbeiten
        /// wir immer mit dem im Speicher gehaltenen LastFrame.
        /// </summary>
        private BitmapSource? LoadFrame(int frameIndex)
        {
            string path = Path.Combine(_outputFolder, $"frame_{frameIndex:D4}.jpg");
            if (!File.Exists(path)) return null;

            var image = new BitmapImage();
            image.BeginInit();
            image.CacheOption = BitmapCacheOption.OnLoad;
            image.UriSource   = new Uri(path, UriKind.Absolute);
            image.EndInit();
            image.Freeze(); // Freeze für Thread-Sicherheit
            return image;
        }

        private void EnsureOutputFolderExists()
        {
            if (!Directory.Exists(_outputFolder))
                Directory.CreateDirectory(_outputFolder);
        }
    }
}
