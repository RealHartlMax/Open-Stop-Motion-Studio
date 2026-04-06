using System;
using System.IO;
using System.Text;

namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// DebugLogger: Logs all user actions and system events to a file for debugging.
    /// </summary>
    public class DebugLogger
    {
        public static DebugLogger Instance { get; } = new DebugLogger();

        private string _logFilePath = string.Empty;
        private readonly object _lockObject = new();

        private DebugLogger()
        {
            var defaultLogDir = Path.Combine(ProjectRoot.GetPath(), "logs");
            SetLogDirectory(defaultLogDir, isRedirect: false);
            Log($"=== Debug Logger Initialized at {DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} ===");
        }
        
        public void SetLogDirectory(string logDirectory, bool isRedirect = true)
        {
            lock (_lockObject)
            {
                try
                {
                    if (!Directory.Exists(logDirectory))
                        Directory.CreateDirectory(logDirectory);

                    string timestamp = DateTime.Now.ToString("yyyy-MM-dd_HH-mm-ss");
                    _logFilePath = Path.Combine(logDirectory, $"debug_log_{timestamp}.txt");

                    if (isRedirect)
                    {
                        Log($"=== Log output redirected to new file ===");
                    }
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine($"[Logger Error] Failed to set new log directory: {ex.Message}");
                }
            }
        }

        public void Log(string message)
        {
            lock (_lockObject)
            {
                try
                {
                    if (string.IsNullOrEmpty(_logFilePath))
                        return;

                    string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
                    string logEntry = $"[{timestamp}] {message}";
                    
                    File.AppendAllText(_logFilePath, logEntry + Environment.NewLine, Encoding.UTF8);
                    System.Diagnostics.Debug.WriteLine(logEntry);
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine($"[Logger Error] {ex.Message}");
                }
            }
        }

        public void LogKeyDown(string key)
        {
            Log($"[KEY DOWN] {key}");
        }

        public void LogMouseWheel(double deltaY)
        {
            Log($"[MOUSE WHEEL] DeltaY: {deltaY}");
        }

        public void LogFrameNavigation(int fromFrame, int toFrame, string source)
        {
            Log($"[FRAME NAVIGATION] {fromFrame} -> {toFrame} (source: {source})");
        }

        public void LogTimelineCursorMove(int frameNumber)
        {
            Log($"[TIMELINE CURSOR] Moved to frame {frameNumber}");
        }

        public void LogCapture(string details)
        {
            Log($"[CAPTURE] {details}");
        }

        public void LogPlayback(string details)
        {
            Log($"[PLAYBACK] {details}");
        }

        public void LogInfo(string source, string message)
        {
            Log($"[{source}] {message}");
        }

        public void LogError(string source, string message)
        {
            Log($"[{source}] ERROR: {message}");
        }

        public string GetLogFilePath() => _logFilePath;
    }
}
