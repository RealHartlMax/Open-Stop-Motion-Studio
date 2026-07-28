using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.Core
{
    public sealed class GPhotoCameraAdapter : CameraAdapterBase
    {
        private readonly CameraDeviceDescriptor _descriptor;
        private CancellationTokenSource? _liveViewCancellation;
        private Task? _liveViewTask;
        private readonly string _captureTempFolder = Path.Combine(Path.GetTempPath(), "OpenStopMotionStudio", "GPhotoCapture");

        public GPhotoCameraAdapter(CameraDeviceDescriptor descriptor)
        {
            _descriptor = descriptor;
        }

        public override string Name => "gphoto2 Adapter";

        public override bool Connect(int deviceIndex)
        {
            Disconnect();

            if (!TryGetGPhoto2Availability(out string? error))
            {
                RaiseStatusChanged(error ?? "gphoto2 ist auf diesem System nicht verfügbar.");
                return false;
            }

            IsConnected = true;
            RaiseStatusChanged($"gphoto2 verbunden: {_descriptor.Name}");
            return true;
        }

        public override void Disconnect()
        {
            bool hadConnection = IsConnected;
            StopLiveView();
            IsConnected = false;

            if (hadConnection)
                RaiseDisconnected();
        }

        public override void StartLiveView()
        {
            if (!IsConnected)
                return;

            if (_liveViewTask != null && !_liveViewTask.IsCompleted)
                return;

            _liveViewCancellation = new CancellationTokenSource();
            _liveViewTask = Task.Run(() => LiveViewLoop(_liveViewCancellation.Token), _liveViewCancellation.Token);
        }

        public override void StopLiveView()
        {
            if (_liveViewCancellation == null)
                return;

            try
            {
                _liveViewCancellation.Cancel();
                _liveViewTask?.Wait(1500);
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"StopLiveView fehlgeschlagen: {ex.Message}");
            }
            finally
            {
                _liveViewTask = null;
                _liveViewCancellation.Dispose();
                _liveViewCancellation = null;
            }
        }

        public override void CaptureImage()
        {
            if (!IsConnected)
                return;

            try
            {
                Directory.CreateDirectory(_captureTempFolder);
                string filePath = Path.Combine(_captureTempFolder, $"capture_{DateTime.UtcNow:yyyyMMdd_HHmmssfff}.jpg");

                if (!RunGPhotoCommand(out _, out _, "--capture-image-and-download", "--filename", filePath))
                {
                    RaiseStatusChanged("gphoto2 konnte kein Bild erfassen.");
                    return;
                }

                if (File.Exists(filePath))
                {
                    RaiseImageCaptured(filePath);
                    RaiseStatusChanged($"Bild gespeichert: {Path.GetFileName(filePath)}");
                }
                else
                {
                    RaiseStatusChanged("gphoto2 hat keine Bilddatei erzeugt.");
                }
            }
            catch (Exception ex)
            {
                RaiseStatusChanged($"CaptureImage fehlgeschlagen: {ex.Message}");
            }
        }

        public override void SetProperty(string property, object value)
        {
            RaiseStatusChanged($"gphoto2 unterstützt die Eigenschaft '{property}' derzeit nicht.");
        }

        public override CameraResolution GetCurrentResolution() => new(0, 0);

        private void LiveViewLoop(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                try
                {
                    if (TryCapturePreviewFrame(out byte[]? frameData) && frameData != null && frameData.Length > 0)
                        RaiseLiveViewFrame(frameData);
                }
                catch (Exception ex)
                {
                    RaiseStatusChanged($"gphoto2 Live-View fehlgeschlagen: {ex.Message}");
                    return;
                }

                Thread.Sleep(500);
            }
        }

        private bool TryCapturePreviewFrame(out byte[]? frameData)
        {
            frameData = null;

            if (!TryStartGPhotoProcess(out Process? process, "--capture-preview", "--stdout"))
                return false;

            try
            {
                using (process)
                {
                    if (process == null)
                        return false;

                    string stderr = process.StandardError.ReadToEnd();
                    byte[] output = System.Text.Encoding.UTF8.GetBytes(process.StandardOutput.ReadToEnd());
                    process.WaitForExit(10000);

                    if (process.ExitCode != 0 || output.Length == 0)
                        return false;

                    frameData = output;
                    return true;
                }
            }
            catch
            {
                return false;
            }
        }

        private bool TryGetGPhoto2Availability(out string? error)
        {
            error = null;

            if (!RunGPhotoCommand(out _, out string stderr, "--version"))
            {
                error = string.IsNullOrWhiteSpace(stderr) ? "gphoto2 ist nicht verfügbar." : stderr.Trim();
                return false;
            }

            return true;
        }

        private bool RunGPhotoCommand(out string stdout, out string stderr, params string[] arguments)
        {
            stdout = string.Empty;
            stderr = string.Empty;

            if (!TryStartGPhotoProcess(out Process? process, arguments))
                return false;

            using (process)
            {
                if (process == null)
                    return false;

                stdout = process.StandardOutput.ReadToEnd();
                stderr = process.StandardError.ReadToEnd();
                process.WaitForExit(20000);
                return process.ExitCode == 0;
            }
        }

        private bool TryStartGPhotoProcess(out Process? process, params string[] arguments)
        {
            process = null;

            try
            {
                var startInfo = new ProcessStartInfo
                {
                    FileName = "gphoto2",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                };

                foreach (string argument in arguments)
                    startInfo.ArgumentList.Add(argument);

                startInfo.ArgumentList.Add("--quiet");

                if (!string.IsNullOrWhiteSpace(_descriptor.ConnectionToken))
                {
                    startInfo.ArgumentList.Add("--port");
                    startInfo.ArgumentList.Add(_descriptor.ConnectionToken);
                }

                process = Process.Start(startInfo);
                return process != null;
            }
            catch
            {
                return false;
            }
        }
    }
}
