namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// OverlayManager: Verwaltet den Zustand des Onion Skin Overlays.
    ///
    /// Warum eine eigene Klasse für so wenig Logik?
    /// Weil "wenig Logik heute" oft "viel Logik morgen" bedeutet. In Phase 3
    /// wird der OverlayManager deutlich komplexer:
    ///
    ///   - Multi-Frame Onion Skin: nicht nur das letzte, sondern die letzten
    ///     N Frames werden übereinander gelegt, mit abnehmender Transparenz
    ///     (ähnlich wie in DragonFrame oder Monkey Jam).
    ///
    ///   - Farbkodierung: Frame -1 in Rot, Frame -2 in Blau (professioneller Standard).
    ///
    ///   - Loop-Playback-Overlay: Überblendung zwischen erstem und letztem Frame
    ///     für nahtlose Loop-Animationen.
    ///
    /// All das wäre schwer nachträglich in das MainWindow einzubauen. Eine saubere
    /// Klasse von Anfang an macht diese Erweiterungen trivial.
    /// </summary>
    public class OverlayManager
    {
        // ── Öffentliche Properties ───────────────────────────────────────────

        /// <summary>
        /// Ob das Onion Skin Overlay aktuell aktiv ist.
        /// Wird vom MainWindow bei Toggle-Änderung gesetzt.
        /// </summary>
        public bool IsEnabled { get; set; } = false;

        /// <summary>
        /// Transparenz des Onion Skin Overlays als WPF-Opacity-Wert (0.0 – 1.0).
        ///
        /// Standard: 0.4 (40%) – das ist in der Praxis ein guter Startpunkt.
        /// Genug um die Referenz zu sehen, aber nicht so stark, dass der
        /// Live-Feed unlesbar wird.
        /// </summary>
        public double AlphaValue { get; set; } = 0.4;

        // ── Zukünftige Erweiterung: Multi-Onion-Skin ────────────────────────

        /// <summary>
        /// Anzahl der vorherigen Frames, die im Onion Skin angezeigt werden.
        /// MVP: immer 1. Phase 3: konfigurierbar 1–5.
        /// </summary>
        public int OnionLayers { get; set; } = 1;

        /// <summary>
        /// Farbmodus: false = normal (transparent-weiß), true = farbkodiert
        /// (letzter Frame Rot, vorletzter Blau – Profi-Workflow).
        /// MVP: immer false. Phase 3: konfigurierbar.
        /// </summary>
        public bool ColorCodedMode { get; set; } = false;

        // ── Hilfsmethode: Alpha für einen Layer berechnen ───────────────────

        /// <summary>
        /// Berechnet den Alpha-Wert für einen bestimmten Onion-Skin-Layer.
        ///
        /// Bei Multi-Layer-Onion-Skin nimmt die Transparenz mit jedem älteren
        /// Frame ab. Layer 1 (letzter Frame) bekommt den vollen AlphaValue,
        /// Layer 2 bekommt die Hälfte, Layer 3 ein Drittel usw.
        ///
        /// Formel: alpha = AlphaValue / layerIndex
        ///
        /// Das entspricht dem Verhalten in professionellen Stop-Motion-Tools
        /// und sorgt für eine natürliche visuelle Hierarchie.
        /// </summary>
        /// <param name="layerIndex">1 = letzter Frame, 2 = vorletzter, usw.</param>
        public double GetAlphaForLayer(int layerIndex)
        {
            if (layerIndex < 1) return 0;
            return AlphaValue / layerIndex;
        }
    }
}
