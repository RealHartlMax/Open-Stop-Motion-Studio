using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using OpenMacroBoard.SDK;
using StreamDeckSharp;

namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// StreamDeckManager: Verwaltet die Verbindung zum Elgato Stream Deck.
    ///
    /// API-Hinweis: StreamDeckSharp v6 hat die Verbindungs-API gegenüber
    /// älteren Versionen geändert. Statt StreamDeck.OpenDevice() wird nun
    /// StreamDeck.EnumerateDevices() + device.Open() verwendet.
    ///
    /// BUTTON-LAYOUT (5x3 Grid):
    ///   [ CAPTURE ] [ ONION  ] [  25%  ] [  50%  ] [  75%  ]
    ///   [          ] [        ] [       ] [       ] [       ]
    ///   [  UNDO   ] [        ] [       ] [       ] [       ]
    /// </summary>
    public class StreamDeckManager : IDisposable
    {
        // ── Öffentliche Events ───────────────────────────────────────────────
        public event Action? CapturePressed;
        public event Action? OnionTogglePressed;
        public event Action<int>? AlphaPresetPressed;
        public event Action? UndoPressed;

        // ── Verbindung ───────────────────────────────────────────────────────
        private IMacroBoard? _deck;
        public bool IsConnected => _deck != null;

        // ── Button-Index Konstanten (5x3 Grid) ──────────────────────────────
        private const int ButtonCapture     = 0;
        private const int ButtonOnionToggle = 1;
        private const int ButtonAlpha25     = 2;
        private const int ButtonAlpha50     = 3;
        private const int ButtonAlpha75     = 4;
        private const int ButtonUndo        = 10;

        // ════════════════════════════════════════════════════════════════════
        //  VERBINDUNG
        // ════════════════════════════════════════════════════════════════════

        /// <summary>
        /// Versucht, das erste gefundene Stream Deck zu verbinden.
        /// StreamDeckSharp v6: Geräteliste via StreamDeck.EnumerateDevices().
        /// </summary>
        public bool Connect()
        {
            try
            {
                StreamDeckDeviceReference? firstDevice = null;

                foreach (var device in StreamDeck.EnumerateDevices())
                {
                    firstDevice = device;
                    break;
                }

                if (firstDevice == null) return false;

                _deck = firstDevice.Open();
                _deck.KeyStateChanged += OnKeyStateChanged;

                InitializeButtonDisplay();
                return true;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[StreamDeckManager] Connect failed: {ex.Message}");
                _deck = null;
                return false;
            }
        }

        public void Disconnect()
        {
            if (_deck == null) return;
            _deck.KeyStateChanged -= OnKeyStateChanged;
            _deck.Dispose();
            _deck = null;
        }

        // ════════════════════════════════════════════════════════════════════
        //  BUTTON EVENTS
        // ════════════════════════════════════════════════════════════════════

        private void OnKeyStateChanged(object? sender, KeyEventArgs e)
        {
            if (!e.IsDown) return;

            switch (e.Key)
            {
                case ButtonCapture:
                    CapturePressed?.Invoke();
                    FlashButton(ButtonCapture);
                    break;
                case ButtonOnionToggle:
                    OnionTogglePressed?.Invoke();
                    break;
                case ButtonAlpha25:
                    AlphaPresetPressed?.Invoke(25);
                    break;
                case ButtonAlpha50:
                    AlphaPresetPressed?.Invoke(50);
                    break;
                case ButtonAlpha75:
                    AlphaPresetPressed?.Invoke(75);
                    break;
                case ButtonUndo:
                    UndoPressed?.Invoke();
                    break;
            }
        }

        // ════════════════════════════════════════════════════════════════════
        //  BUTTON-DISPLAY
        // ════════════════════════════════════════════════════════════════════

        private void InitializeButtonDisplay()
        {
            if (_deck == null) return;
            SetButtonLabel(ButtonCapture,     "CAPTURE", 0xE9, 0x45, 0x60);
            SetButtonLabel(ButtonOnionToggle, "ONION",   0x4C, 0xAF, 0x50);
            SetButtonLabel(ButtonAlpha25,     "25%",     0x2D, 0x2D, 0x44);
            SetButtonLabel(ButtonAlpha50,     "50%",     0x2D, 0x2D, 0x44);
            SetButtonLabel(ButtonAlpha75,     "75%",     0x2D, 0x2D, 0x44);
            SetButtonLabel(ButtonUndo,        "UNDO",    0x88, 0x44, 0x00);
        }

        private void SetButtonLabel(int buttonIndex, string label, byte r, byte g, byte b)
        {
            if (_deck == null) return;

            try
            {
                int size = _deck.Keys.KeySize;

                using var bmp = new Bitmap(size, size);
                using var gfx = Graphics.FromImage(bmp);
                gfx.Clear(Color.FromArgb(r, g, b));

                using var font  = new Font("Arial", size * 0.14f, FontStyle.Bold);
                using var brush = new SolidBrush(Color.White);

                var textSize = gfx.MeasureString(label, font);
                gfx.DrawString(label, font, brush,
                    (size - textSize.Width)  / 2,
                    (size - textSize.Height) / 2);

                using var imageStream = new MemoryStream();
                bmp.Save(imageStream, ImageFormat.Png);
                imageStream.Position = 0;

                _deck.SetKeyBitmap(buttonIndex, KeyBitmap.Create.FromStream(imageStream));
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"[StreamDeckManager] SetButtonLabel failed: {ex.Message}");
            }
        }

        private async void FlashButton(int buttonIndex)
        {
            if (_deck == null) return;
            SetButtonLabel(buttonIndex, "OK", 0xFF, 0xFF, 0xFF);
            await System.Threading.Tasks.Task.Delay(150);
            SetButtonLabel(buttonIndex, "CAPTURE", 0xE9, 0x45, 0x60);
        }

        public void Dispose() => Disconnect();
    }
}
