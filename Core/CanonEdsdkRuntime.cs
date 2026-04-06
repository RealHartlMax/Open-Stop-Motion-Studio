using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace OpenStopMotionStudio.Core
{
    internal sealed class CanonEdsdkLease : IDisposable
    {
        private bool _disposed;

        public CanonEdsdkLease(CanonSdkLocation location)
        {
            CanonEdsdkRuntime.Acquire(location);
        }

        public void Dispose()
        {
            if (_disposed)
                return;

            _disposed = true;
            CanonEdsdkRuntime.Release();
        }
    }

    internal static class CanonEdsdkRuntime
    {
        private const uint EDS_ERR_OK = 0x00000000;
        private const uint EDS_ERR_DEVICE_BUSY = 0x00000081;
        private const uint EDS_ERR_OBJECT_NOTREADY = 0x000000A3;
        private const uint kEdsPropID_SaveTo = 0x0000000b;
        private const uint kEdsPropID_Evf_OutputDevice = 0x00000500;
        private const uint kEdsPropID_Evf_Mode = 0x00000501;
        private const uint kEdsSaveTo_Host = 2;
        private const uint kEdsEvfOutputDevice_PC = 2;
        private const uint kEdsCameraCommand_PressShutterButton = 0x00000004;
        private const uint kEdsCameraCommand_ShutterButton_OFF = 0x00000000;
        private const uint kEdsCameraCommand_ShutterButton_Completely = 0x00000003;
        private const uint kEdsObjectEvent_All = 0x00000200;
        private const uint kEdsStateEvent_All = 0x00000300;
        private const uint kEdsPropertyEvent_All = 0x00000100;
        private const uint kEdsObjectEvent_DirItemRequestTransfer = 0x00000208;
        private const int LiveViewBufferSize = 2 * 1024 * 1024;
        private const uint kEdsCameraStatusCommand_UILock = 0x00000000;
        private const uint kEdsCameraStatusCommand_UIUnLock = 0x00000001;
        private const uint kEdsFileCreateDisposition_CreateAlways = 0;
        private const uint kEdsAccess_ReadWrite = 3;

        private static readonly object SyncRoot = new();
        private static CanonSdkLocation? _currentLocation;
        private static IntPtr _edsImageHandle;
        private static IntPtr _edsdkHandle;
        private static int _leaseCount;

        static CanonEdsdkRuntime()
        {
            NativeLibrary.SetDllImportResolver(typeof(CanonEdsdkRuntime).Assembly, ResolveLibrary);
        }

        public static IReadOnlyList<CanonSdkDevice> EnumerateConnectedCameras(CanonSdkLocation location)
        {
            using var lease = new CanonEdsdkLease(location);
            IntPtr cameraListRef = IntPtr.Zero;
            var devices = new List<CanonSdkDevice>();

            try
            {
                EnsureSuccess(EdsGetCameraList(out cameraListRef), "Canon EDSDK konnte keine Kameraliste laden.");
                EnsureSuccess(EdsGetChildCount(cameraListRef, out uint count), "Canon EDSDK konnte die Kameraanzahl nicht lesen.");

                for (uint i = 0; i < count; i++)
                {
                    IntPtr cameraRef = IntPtr.Zero;
                    try
                    {
                        EnsureSuccess(EdsGetChildAtIndex(cameraListRef, (int)i, out cameraRef), "Canon EDSDK konnte eine Kamera nicht lesen.");
                        EnsureSuccess(EdsGetDeviceInfo(cameraRef, out CanonDeviceInfo info), "Canon EDSDK konnte Geräteinformationen nicht lesen.");

                        string name = string.IsNullOrWhiteSpace(info.DeviceDescription)
                            ? "Canon Kamera"
                            : info.DeviceDescription.Trim();
                        string port = string.IsNullOrWhiteSpace(info.PortName)
                            ? $"canon-{i}"
                            : info.PortName.Trim();

                        devices.Add(new CanonSdkDevice(name, port));
                    }
                    finally
                    {
                        ReleaseRef(cameraRef);
                    }
                }
            }
            finally
            {
                ReleaseRef(cameraListRef);
            }

            return devices;
        }

        public static CanonCameraSession OpenSession(CanonSdkLocation location, string? preferredPortName)
        {
            var lease = new CanonEdsdkLease(location);

            try
            {
                IntPtr cameraListRef = IntPtr.Zero;
                IntPtr selectedCameraRef = IntPtr.Zero;

                try
                {
                    EnsureSuccess(EdsGetCameraList(out cameraListRef), "Canon EDSDK konnte keine Kameraliste laden.");
                    EnsureSuccess(EdsGetChildCount(cameraListRef, out uint count), "Canon EDSDK konnte die Kameraanzahl nicht lesen.");

                    for (uint i = 0; i < count; i++)
                    {
                        IntPtr cameraRef = IntPtr.Zero;
                        try
                        {
                            EnsureSuccess(EdsGetChildAtIndex(cameraListRef, (int)i, out cameraRef), "Canon EDSDK konnte eine Kamera nicht lesen.");
                            EnsureSuccess(EdsGetDeviceInfo(cameraRef, out CanonDeviceInfo info), "Canon EDSDK konnte Geräteinformationen nicht lesen.");

                            string portName = string.IsNullOrWhiteSpace(info.PortName)
                                ? string.Empty
                                : info.PortName.Trim();

                            if (!string.IsNullOrWhiteSpace(preferredPortName)
                                && !string.Equals(portName, preferredPortName, StringComparison.OrdinalIgnoreCase))
                            {
                                continue;
                            }

                            selectedCameraRef = cameraRef;
                            cameraRef = IntPtr.Zero;
                            break;
                        }
                        finally
                        {
                            ReleaseRef(cameraRef);
                        }
                    }

                    if (selectedCameraRef == IntPtr.Zero)
                        throw new InvalidOperationException("Keine passende Canon-Kamera im EDSDK gefunden.");

                    OpenSessionCore(selectedCameraRef);
                    return new CanonCameraSession(lease, selectedCameraRef);
                }
                catch
                {
                    lease.Dispose();
                    throw;
                }
                finally
                {
                    ReleaseRef(cameraListRef);
                }
            }
            catch
            {
                lease.Dispose();
                throw;
            }
        }

        public static void StartEvf(IntPtr cameraRef)
        {
            uint evfMode = 0;
            TryGetUInt32Property(cameraRef, kEdsPropID_Evf_Mode, out evfMode);
            if (evfMode == 0)
            {
                uint enabled = 1;
                EnsureSuccess(EdsSetPropertyData(cameraRef, kEdsPropID_Evf_Mode, 0, sizeof(uint), ref enabled), "Canon Live-View konnte nicht aktiviert werden.");
            }

            uint outputDevice = 0;
            TryGetUInt32Property(cameraRef, kEdsPropID_Evf_OutputDevice, out outputDevice);
            outputDevice |= kEdsEvfOutputDevice_PC;
            EnsureSuccess(EdsSetPropertyData(cameraRef, kEdsPropID_Evf_OutputDevice, 0, sizeof(uint), ref outputDevice), "Canon Live-View-Ausgabegerät konnte nicht gesetzt werden.");
        }

        public static void StopEvf(IntPtr cameraRef)
        {
            if (cameraRef == IntPtr.Zero)
                return;

            if (!TryGetUInt32Property(cameraRef, kEdsPropID_Evf_OutputDevice, out uint outputDevice))
                return;

            if ((outputDevice & kEdsEvfOutputDevice_PC) == 0)
                return;

            outputDevice &= ~kEdsEvfOutputDevice_PC;
            EdsSetPropertyData(cameraRef, kEdsPropID_Evf_OutputDevice, 0, sizeof(uint), ref outputDevice);
        }

        public static byte[]? TryDownloadEvfFrame(IntPtr cameraRef)
        {
            IntPtr streamRef = IntPtr.Zero;
            IntPtr evfImageRef = IntPtr.Zero;

            try
            {
                uint result = EdsCreateMemoryStream(LiveViewBufferSize, out streamRef);
                if (result != EDS_ERR_OK)
                    throw CreateException(result, "Canon Live-View-Speicher konnte nicht angelegt werden.");

                result = EdsCreateEvfImageRef(streamRef, out evfImageRef);
                if (result != EDS_ERR_OK)
                    throw CreateException(result, "Canon Live-View-Container konnte nicht angelegt werden.");

                result = EdsDownloadEvfImage(cameraRef, evfImageRef);
                if (result == EDS_ERR_OBJECT_NOTREADY || result == EDS_ERR_DEVICE_BUSY)
                    return null;

                EnsureSuccess(result, "Canon Live-View-Bild konnte nicht geladen werden.");
                EnsureSuccess(EdsGetPointer(streamRef, out IntPtr pointer), "Canon Live-View-Speicheradresse konnte nicht gelesen werden.");
                EnsureSuccess(EdsGetLength(streamRef, out ulong length), "Canon Live-View-Länge konnte nicht gelesen werden.");

                if (pointer == IntPtr.Zero || length == 0 || length > int.MaxValue)
                    return null;

                byte[] data = new byte[(int)length];
                Marshal.Copy(pointer, data, 0, data.Length);
                return data;
            }
            finally
            {
                ReleaseRef(evfImageRef);
                ReleaseRef(streamRef);
            }
        }

        public static void PumpEvents()
        {
            uint result = EdsGetEvent();
            if (result != EDS_ERR_OK && result != EDS_ERR_OBJECT_NOTREADY)
                throw CreateException(result, "Canon EDSDK-Ereignisse konnten nicht gelesen werden.");
        }

        public static void TriggerStillCapture(IntPtr cameraRef)
        {
            uint result = EdsSendCommand(cameraRef, kEdsCameraCommand_PressShutterButton, (int)kEdsCameraCommand_ShutterButton_Completely);
            if (result == EDS_ERR_DEVICE_BUSY)
            {
                Thread.Sleep(200);
                result = EdsSendCommand(cameraRef, kEdsCameraCommand_PressShutterButton, (int)kEdsCameraCommand_ShutterButton_Completely);
            }

            EnsureSuccess(result, "Canon-Ausloeser konnte nicht betaetigt werden.");
            EdsSendCommand(cameraRef, kEdsCameraCommand_PressShutterButton, (int)kEdsCameraCommand_ShutterButton_OFF);
        }

        public static bool IsTransferRequestEvent(uint eventCode) =>
            eventCode == kEdsObjectEvent_DirItemRequestTransfer;

        public static string DownloadDirectoryItem(IntPtr directoryItemRef, string outputFolder)
        {
            EnsureSuccess(EdsGetDirectoryItemInfo(directoryItemRef, out EdsDirectoryItemInfo info), "Canon Directory-Info konnte nicht gelesen werden.");

            string fileName = string.IsNullOrWhiteSpace(info.FileName)
                ? $"canon_{DateTime.UtcNow:yyyyMMdd_HHmmssfff}.jpg"
                : info.FileName.Trim();

            Directory.CreateDirectory(outputFolder);
            string outputPath = BuildUniqueFilePath(outputFolder, fileName);

            IntPtr streamRef = IntPtr.Zero;
            try
            {
                EnsureSuccess(
                    EdsCreateFileStream(outputPath, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, out streamRef),
                    "Canon Zieldatei konnte nicht angelegt werden.");

                uint result = EdsDownload(directoryItemRef, info.Size, streamRef);
                if (result == EDS_ERR_DEVICE_BUSY)
                {
                    Thread.Sleep(200);
                    result = EdsDownload(directoryItemRef, info.Size, streamRef);
                }

                EnsureSuccess(result, "Canon-Still konnte nicht heruntergeladen werden.");
                EnsureSuccess(EdsDownloadComplete(directoryItemRef), "Canon-Still konnte nicht abgeschlossen werden.");
                return outputPath;
            }
            finally
            {
                ReleaseRef(streamRef);
            }
        }

        public static uint RegisterHandlers(
            IntPtr cameraRef,
            CanonObjectEventHandler objectEventHandler,
            CanonPropertyEventHandler propertyEventHandler,
            CanonStateEventHandler stateEventHandler)
        {
            EnsureSuccess(EdsSetObjectEventHandler(cameraRef, kEdsObjectEvent_All, objectEventHandler, IntPtr.Zero), "Canon Objekt-Events konnten nicht registriert werden.");
            EnsureSuccess(EdsSetPropertyEventHandler(cameraRef, kEdsPropertyEvent_All, propertyEventHandler, IntPtr.Zero), "Canon Property-Events konnten nicht registriert werden.");
            EnsureSuccess(EdsSetCameraStateEventHandler(cameraRef, kEdsStateEvent_All, stateEventHandler, IntPtr.Zero), "Canon State-Events konnten nicht registriert werden.");
            return EDS_ERR_OK;
        }

        public static void CloseSession(IntPtr cameraRef)
        {
            if (cameraRef == IntPtr.Zero)
                return;

            EdsCloseSession(cameraRef);
        }

        public static void ReleaseRef(IntPtr reference)
        {
            if (reference != IntPtr.Zero)
                EdsRelease(reference);
        }

        public static bool TryGetUInt32Property(IntPtr reference, uint propertyId, out uint value)
        {
            value = 0;
            uint result = EdsGetPropertyData(reference, propertyId, 0, sizeof(uint), out value);
            return result == EDS_ERR_OK;
        }

        private static void OpenSessionCore(IntPtr cameraRef)
        {
            EnsureSuccess(EdsOpenSession(cameraRef), "Canon Session konnte nicht geoeffnet werden.");

            uint saveTo = kEdsSaveTo_Host;
            EnsureSuccess(EdsSetPropertyData(cameraRef, kEdsPropID_SaveTo, 0, sizeof(uint), ref saveTo), "Canon Speicherziel konnte nicht auf Host gestellt werden.");

            bool uiLocked = false;
            try
            {
                EnsureSuccess(EdsSendStatusCommand(cameraRef, kEdsCameraStatusCommand_UILock, 0), "Canon UI-Lock konnte nicht gesetzt werden.");
                uiLocked = true;

                var capacity = new CanonCapacity
                {
                    NumberOfFreeClusters = int.MaxValue,
                    BytesPerSector = 0x1000,
                    Reset = 1
                };

                EnsureSuccess(EdsSetCapacity(cameraRef, capacity), "Canon Kapazitaet konnte nicht gesetzt werden.");
            }
            finally
            {
                if (uiLocked)
                    EdsSendStatusCommand(cameraRef, kEdsCameraStatusCommand_UIUnLock, 0);
            }
        }

        internal static void Acquire(CanonSdkLocation location)
        {
            lock (SyncRoot)
            {
                if (_currentLocation == null || !string.Equals(_currentLocation.LibraryPath, location.LibraryPath, StringComparison.OrdinalIgnoreCase))
                {
                    LoadLibraries(location);
                    _currentLocation = location;
                }

                if (_leaseCount == 0)
                    EnsureSuccess(EdsInitializeSDK(), "Canon EDSDK konnte nicht initialisiert werden.");

                _leaseCount++;
            }
        }

        internal static void Release()
        {
            lock (SyncRoot)
            {
                if (_leaseCount <= 0)
                    return;

                _leaseCount--;
                if (_leaseCount == 0)
                    EdsTerminateSDK();
            }
        }

        private static void LoadLibraries(CanonSdkLocation location)
        {
            _edsImageHandle = NativeLibrary.Load(location.ImageLibraryPath);
            _edsdkHandle = NativeLibrary.Load(location.LibraryPath);
        }

        private static IntPtr ResolveLibrary(string libraryName, System.Reflection.Assembly assembly, DllImportSearchPath? searchPath)
        {
            if (!string.Equals(libraryName, "EDSDK.dll", StringComparison.OrdinalIgnoreCase))
                return IntPtr.Zero;

            return _edsdkHandle;
        }

        private static void EnsureSuccess(uint result, string message)
        {
            if (result == EDS_ERR_OK)
                return;

            throw CreateException(result, message);
        }

        private static InvalidOperationException CreateException(uint result, string message) =>
            new($"{message} Canon-Code: 0x{result:X8}");

        private static string BuildUniqueFilePath(string folder, string fileName)
        {
            string sanitizedFileName = fileName;
            foreach (char invalidChar in Path.GetInvalidFileNameChars())
                sanitizedFileName = sanitizedFileName.Replace(invalidChar, '_');

            string outputPath = Path.Combine(folder, sanitizedFileName);
            if (!File.Exists(outputPath))
                return outputPath;

            string stem = Path.GetFileNameWithoutExtension(sanitizedFileName);
            string extension = Path.GetExtension(sanitizedFileName);
            int suffix = 1;

            do
            {
                outputPath = Path.Combine(folder, $"{stem}_{suffix++}{extension}");
            }
            while (File.Exists(outputPath));

            return outputPath;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        internal struct CanonDeviceInfo
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string PortName;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string DeviceDescription;

            public uint DeviceSubType;
            public uint Reserved;
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct CanonCapacity
        {
            public int NumberOfFreeClusters;
            public int BytesPerSector;
            public int Reset;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        internal struct EdsDirectoryItemInfo
        {
            public ulong Size;
            public int IsFolder;
            public uint GroupId;
            public uint Option;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string FileName;

            public uint Format;
            public uint DateTime;
        }

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        internal delegate uint CanonPropertyEventHandler(uint inEvent, uint inPropertyId, uint inParam, IntPtr inContext);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        internal delegate uint CanonObjectEventHandler(uint inEvent, IntPtr inRef, IntPtr inContext);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        internal delegate uint CanonStateEventHandler(uint inEvent, uint inEventData, IntPtr inContext);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsInitializeSDK();

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsTerminateSDK();

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetCameraList(out IntPtr outCameraListRef);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetChildCount(IntPtr inRef, out uint outCount);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetChildAtIndex(IntPtr inRef, int index, out IntPtr outRef);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetDeviceInfo(IntPtr inCameraRef, out CanonDeviceInfo outDeviceInfo);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetDirectoryItemInfo(IntPtr inDirItemRef, out EdsDirectoryItemInfo outDirItemInfo);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsOpenSession(IntPtr inCameraRef);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsCloseSession(IntPtr inCameraRef);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsRelease(IntPtr inRef);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsSetPropertyData(IntPtr inRef, uint inPropertyId, int inParam, uint inPropertySize, ref uint inPropertyData);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsSetCapacity(IntPtr inCameraRef, CanonCapacity inCapacity);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsSendStatusCommand(IntPtr inCameraRef, uint inStatusCommand, int inParam);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsSendCommand(IntPtr inCameraRef, uint inCommand, int inParam);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsCreateMemoryStream(uint inBufferSize, out IntPtr outStream);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern uint EdsCreateFileStream(string inFileName, uint inCreateDisposition, uint inDesiredAccess, out IntPtr outStream);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsCreateEvfImageRef(IntPtr inStreamRef, out IntPtr outEvfImageRef);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsDownloadEvfImage(IntPtr inCameraRef, IntPtr inEvfImageRef);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsDownload(IntPtr inDirItemRef, ulong inReadSize, IntPtr outStream);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsDownloadComplete(IntPtr inDirItemRef);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetPointer(IntPtr inStream, out IntPtr outPointer);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetLength(IntPtr inStreamRef, out ulong outLength);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetPropertyData(IntPtr inRef, uint inPropertyId, int inParam, uint inPropertySize, out uint outPropertyData);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsSetPropertyEventHandler(IntPtr inCameraRef, uint inEvent, CanonPropertyEventHandler inPropertyEventHandler, IntPtr inContext);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsSetObjectEventHandler(IntPtr inCameraRef, uint inEvent, CanonObjectEventHandler inObjectEventHandler, IntPtr inContext);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsSetCameraStateEventHandler(IntPtr inCameraRef, uint inEvent, CanonStateEventHandler inStateEventHandler, IntPtr inContext);

        [DllImport("EDSDK.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern uint EdsGetEvent();
    }

    internal sealed class CanonCameraSession : IDisposable
    {
        private readonly CanonEdsdkLease _lease;
        private bool _disposed;

        public CanonCameraSession(CanonEdsdkLease lease, IntPtr cameraRef)
        {
            _lease = lease;
            CameraRef = cameraRef;
        }

        public IntPtr CameraRef { get; }

        public void Dispose()
        {
            if (_disposed)
                return;

            _disposed = true;
            CanonEdsdkRuntime.StopEvf(CameraRef);
            CanonEdsdkRuntime.CloseSession(CameraRef);
            CanonEdsdkRuntime.ReleaseRef(CameraRef);
            _lease.Dispose();
        }
    }
}
