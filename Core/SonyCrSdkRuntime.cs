using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace OpenStopMotionStudio.Core
{
    public sealed unsafe class SonyCrSdkRuntime
    {
        private const uint CrErrorNone = 0;
        private const uint CrWarningFrameNotUpdated = 0x00020011;
        private const uint CrErrorConnectFailBusy = 0x820C;
        private const uint CrErrorAdaptorDeviceBusy = 0x8710;
        private const ulong CrCommandIdRelease = 0;
        private const ushort CrCommandParamUp = 0x0000;
        private const ushort CrCommandParamDown = 0x0001;
        private const uint CrSdkControlModeRemote = 0;
        private const uint CrReconnectingOn = 1;
        private const uint SettingKeyEnableLiveView = 0;

        private delegate* unmanaged<uint, byte> _init;
        private delegate* unmanaged<byte> _release;
        private delegate* unmanaged<IntPtr*, byte, uint> _enumCameraObjects;
        private delegate* unmanaged<IntPtr, IntPtr, long*, uint, uint, sbyte*, sbyte*, sbyte*, uint, char*, uint> _connect;
        private delegate* unmanaged<long, uint> _disconnect;
        private delegate* unmanaged<long, uint> _releaseDevice;
        private delegate* unmanaged<long, ulong, ushort, uint> _sendCommand;
        private delegate* unmanaged<long, sbyte*, sbyte*, int, uint> _setSaveInfo;
        private delegate* unmanaged<long, uint, uint, uint> _setDeviceSetting;
        private delegate* unmanaged<long, SonyImageInfo*, uint> _getLiveViewImageInfo;
        private delegate* unmanaged<long, SonyImageDataBlock*, uint> _getLiveViewImage;
        private IntPtr _libraryHandle;
        private string? _libraryPath;
        private int _leaseCount;
        private readonly object _syncRoot = new();

        public static IReadOnlyList<SonySdkDevice> EnumerateConnectedCameras(SonySdkLocation sdkLocation)
        {
            if (!OperatingSystem.IsWindows())
                return Array.Empty<SonySdkDevice>();

            lock (Instance._syncRoot)
            {
                Instance.Acquire(sdkLocation);
                try
                {
                    return Instance.EnumerateConnectedCamerasCore();
                }
                finally
                {
                    Instance.Release();
                }
            }
        }

        internal static SonyConnection ConnectCamera(SonySdkLocation sdkLocation, string? preferredId)
        {
            if (!OperatingSystem.IsWindows())
                throw new PlatformNotSupportedException("Sony CrSDK ist aktuell nur unter Windows verfügbar.");

            lock (Instance._syncRoot)
            {
                Instance.Acquire(sdkLocation);
                try
                {
                    return Instance.ConnectCameraCore(sdkLocation, preferredId);
                }
                catch
                {
                    Instance.Release();
                    throw;
                }
            }
        }

        private static SonyCrSdkRuntime Instance { get; } = new();

        private SonyCrSdkRuntime()
        {
        }

        private void Acquire(SonySdkLocation sdkLocation)
        {
            if (_leaseCount == 0 || !string.Equals(_libraryPath, sdkLocation.LibraryPath, StringComparison.OrdinalIgnoreCase))
            {
                ReleaseLibrary();
                LoadLibrary(sdkLocation);
                if (_init(0) == 0)
                    throw new InvalidOperationException("Sony CrSDK konnte nicht initialisiert werden.");
            }

            _leaseCount++;
        }

        private void Release()
        {
            if (_leaseCount <= 0)
                return;

            _leaseCount--;
            if (_leaseCount > 0)
                return;

            try
            {
                _release();
            }
            finally
            {
                ReleaseLibrary();
            }
        }

        private void LoadLibrary(SonySdkLocation sdkLocation)
        {
            _libraryHandle = NativeLibrary.Load(sdkLocation.LibraryPath);
            _libraryPath = sdkLocation.LibraryPath;
            _init = (delegate* unmanaged<uint, byte>)NativeLibrary.GetExport(_libraryHandle, "Init");
            _release = (delegate* unmanaged<byte>)NativeLibrary.GetExport(_libraryHandle, "Release");
            _enumCameraObjects = (delegate* unmanaged<IntPtr*, byte, uint>)NativeLibrary.GetExport(_libraryHandle, "EnumCameraObjects");
            _connect = (delegate* unmanaged<IntPtr, IntPtr, long*, uint, uint, sbyte*, sbyte*, sbyte*, uint, char*, uint>)NativeLibrary.GetExport(_libraryHandle, "Connect");
            _disconnect = (delegate* unmanaged<long, uint>)NativeLibrary.GetExport(_libraryHandle, "Disconnect");
            _releaseDevice = (delegate* unmanaged<long, uint>)NativeLibrary.GetExport(_libraryHandle, "ReleaseDevice");
            _sendCommand = (delegate* unmanaged<long, ulong, ushort, uint>)NativeLibrary.GetExport(_libraryHandle, "SendCommand");
            _setSaveInfo = (delegate* unmanaged<long, sbyte*, sbyte*, int, uint>)NativeLibrary.GetExport(_libraryHandle, "SetSaveInfo");
            _setDeviceSetting = (delegate* unmanaged<long, uint, uint, uint>)NativeLibrary.GetExport(_libraryHandle, "SetDeviceSetting");
            _getLiveViewImageInfo = (delegate* unmanaged<long, SonyImageInfo*, uint>)NativeLibrary.GetExport(_libraryHandle, "GetLiveViewImageInfo");
            _getLiveViewImage = (delegate* unmanaged<long, SonyImageDataBlock*, uint>)NativeLibrary.GetExport(_libraryHandle, "GetLiveViewImage");
        }

        private void ReleaseLibrary()
        {
            if (_libraryHandle != IntPtr.Zero)
                NativeLibrary.Free(_libraryHandle);

            _libraryHandle = IntPtr.Zero;
            _libraryPath = null;
            _init = null;
            _release = null;
            _enumCameraObjects = null;
            _connect = null;
            _disconnect = null;
            _releaseDevice = null;
            _sendCommand = null;
            _setSaveInfo = null;
            _setDeviceSetting = null;
            _getLiveViewImageInfo = null;
            _getLiveViewImage = null;
        }

        private SonyConnection ConnectCameraCore(SonySdkLocation sdkLocation, string? preferredId)
        {
            IntPtr enumPtr = IntPtr.Zero;
            uint enumResult = _enumCameraObjects(&enumPtr, 3);
            if (enumResult != CrErrorNone || enumPtr == IntPtr.Zero)
                throw new InvalidOperationException($"Sony CrSDK konnte keine Kameras enumerieren (0x{enumResult:X}).");

            IntPtr selectedInfoPtr = IntPtr.Zero;
            string selectedName = "Sony Kamera";

            try
            {
                SonyEnumCameraObjectInfo cameraList = new(enumPtr);
                uint count = cameraList.GetCount();
                for (uint index = 0; index < count; index++)
                {
                    IntPtr infoPtr = cameraList.GetCameraObjectInfo(index);
                    if (infoPtr == IntPtr.Zero)
                        continue;

                    SonyCameraObjectInfo info = new(infoPtr);
                    string connectionType = info.GetConnectionTypeName();
                    string id = string.Equals(connectionType, "IP", StringComparison.OrdinalIgnoreCase)
                        ? info.GetMacAddress()
                        : info.GetId();

                    if (string.IsNullOrWhiteSpace(id))
                        id = info.GetGuid();

                    if (!string.IsNullOrWhiteSpace(preferredId)
                        && !string.Equals(id, preferredId, StringComparison.OrdinalIgnoreCase))
                    {
                        continue;
                    }

                    selectedInfoPtr = CloneCameraObjectInfo(info);
                    selectedName = info.GetModel();
                    break;
                }

                if (selectedInfoPtr == IntPtr.Zero)
                    throw new InvalidOperationException("Die gewählte Sony-Kamera wurde in CrSDK nicht gefunden.");
            }
            finally
            {
                SonyEnumCameraObjectInfo.Release(enumPtr);
            }

            try
            {
                using SonyDeviceCallback callback = new();
                long deviceHandle = 0;
                uint result = _connect(selectedInfoPtr, callback.Pointer, &deviceHandle, CrSdkControlModeRemote, CrReconnectingOn, null, null, null, 0, null);
                if (result != CrErrorNone || deviceHandle == 0)
                    throw new InvalidOperationException($"Sony CrSDK Connect fehlgeschlagen (0x{result:X}).");

                string captureFolder = Path.Combine(Path.GetTempPath(), "OpenStopMotionStudio", "SonyCapture");
                Directory.CreateDirectory(captureFolder);
                ConfigureSaveInfo(deviceHandle, captureFolder);

                return new SonyConnection(this, selectedName, deviceHandle, captureFolder, callback.Detach());
            }
            finally
            {
                SonyCameraObjectInfo.Release(selectedInfoPtr);
            }
        }

        private IntPtr CloneCameraObjectInfo(SonyCameraObjectInfo info)
        {
            sbyte[] name = ToNullTerminatedAnsi(info.GetName());
            sbyte[] model = ToNullTerminatedAnsi(info.GetModel());
            byte[] id = ToRawIdBytes(info.GetId());
            sbyte[] connectionType = ToNullTerminatedAnsi(info.GetConnectionTypeName());
            sbyte[] adaptorName = ToNullTerminatedAnsi(info.GetAdaptorName());
            sbyte[] pairing = ToNullTerminatedAnsi(info.GetPairingNecessity());

            fixed (sbyte* namePtr = name)
            fixed (sbyte* modelPtr = model)
            fixed (byte* idPtr = id)
            fixed (sbyte* connectionTypePtr = connectionType)
            fixed (sbyte* adaptorNamePtr = adaptorName)
            fixed (sbyte* pairingPtr = pairing)
            {
                IntPtr createFn = NativeLibrary.GetExport(_libraryHandle, "CreateCameraObjectInfo");
                var create = (delegate* unmanaged<sbyte*, sbyte*, short, uint, uint, byte*, sbyte*, sbyte*, sbyte*, uint, IntPtr>)createFn;
                return create(namePtr, modelPtr, info.GetUsbPid(), info.GetIdType(), (uint)id.Length, idPtr, connectionTypePtr, adaptorNamePtr, pairingPtr, info.GetSshSupport());
            }
        }

        private void DisconnectCamera(long deviceHandle)
        {
            if (deviceHandle == 0)
                return;

            _disconnect(deviceHandle);
            _releaseDevice(deviceHandle);
        }

        private void SendStillCapture(long deviceHandle)
        {
            uint downResult = SendCommandWithRetry(deviceHandle, CrCommandIdRelease, CrCommandParamDown, "DOWN");
            if (downResult != CrErrorNone)
                throw new InvalidOperationException($"Sony-Auslöser DOWN fehlgeschlagen (0x{downResult:X}).");

            Thread.Sleep(35);

            uint upResult = SendCommandWithRetry(deviceHandle, CrCommandIdRelease, CrCommandParamUp, "UP");
            if (upResult != CrErrorNone)
                throw new InvalidOperationException($"Sony-Auslöser UP fehlgeschlagen (0x{upResult:X}).");
        }

        private uint SendCommandWithRetry(long deviceHandle, ulong commandId, ushort commandParam, string phase)
        {
            uint result = _sendCommand(deviceHandle, commandId, commandParam);
            if (result == CrErrorNone)
                return result;

            if (result is CrErrorConnectFailBusy or CrErrorAdaptorDeviceBusy)
            {
                Thread.Sleep(150);
                result = _sendCommand(deviceHandle, commandId, commandParam);
                if (result == CrErrorNone)
                    return result;
            }

            DebugLogger.Instance.LogInfo("SonyCrSdkRuntime", $"SendCommand {phase} failed with 0x{result:X}");
            return result;
        }

        private void SetLiveViewEnabled(long deviceHandle, bool enabled)
        {
            uint result = _setDeviceSetting(deviceHandle, SettingKeyEnableLiveView, enabled ? 1u : 0u);
            if (result != CrErrorNone)
                throw new InvalidOperationException($"Sony Live-View-Umschaltung fehlgeschlagen (0x{result:X}).");
        }

        private byte[]? TryGetLiveViewJpegFrame(long deviceHandle, out int width, out int height)
        {
            width = 0;
            height = 0;

            SonyImageInfo info = default;
            uint infoResult = _getLiveViewImageInfo(deviceHandle, &info);
            if (infoResult != CrErrorNone)
                return null;

            if (info.BufferSize == 0)
                return null;

            byte[] buffer = new byte[info.BufferSize];
            fixed (byte* dataPtr = buffer)
            {
                SonyImageDataBlock dataBlock = new()
                {
                    Size = info.BufferSize,
                    Data = dataPtr
                };

                uint imageResult = _getLiveViewImage(deviceHandle, &dataBlock);
                if (imageResult == CrWarningFrameNotUpdated)
                    return null;

                if (imageResult != CrErrorNone || dataBlock.ImageSize == 0)
                    return null;

                width = (int)info.Width;
                height = (int)info.Height;
                if (dataBlock.ImageSize == buffer.Length)
                    return buffer;

                byte[] result = new byte[dataBlock.ImageSize];
                Buffer.BlockCopy(buffer, 0, result, 0, (int)dataBlock.ImageSize);
                return result;
            }
        }

        private void ConfigureSaveInfo(long deviceHandle, string captureFolder)
        {
            sbyte[] folder = ToNullTerminatedAnsi(captureFolder);
            sbyte[] prefix = ToNullTerminatedAnsi(string.Empty);

            fixed (sbyte* folderPtr = folder)
            fixed (sbyte* prefixPtr = prefix)
            {
                uint result = _setSaveInfo(deviceHandle, folderPtr, prefixPtr, -1);
                if (result != CrErrorNone)
                    throw new InvalidOperationException($"Sony SetSaveInfo fehlgeschlagen (0x{result:X}).");
            }
        }

        private IReadOnlyList<SonySdkDevice> EnumerateConnectedCamerasCore()
        {
            IntPtr enumPtr = IntPtr.Zero;
            uint result = _enumCameraObjects(&enumPtr, 3);
            if (result != 0 || enumPtr == IntPtr.Zero)
                return Array.Empty<SonySdkDevice>();

            try
            {
                var devices = new List<SonySdkDevice>();
                var seenPorts = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
                SonyEnumCameraObjectInfo cameraList = new(enumPtr);
                uint count = cameraList.GetCount();

                for (uint index = 0; index < count; index++)
                {
                    IntPtr infoPtr = cameraList.GetCameraObjectInfo(index);
                    if (infoPtr == IntPtr.Zero)
                        continue;

                    SonyCameraObjectInfo cameraInfo = new(infoPtr);
                    string model = cameraInfo.GetModel();
                    string connectionType = cameraInfo.GetConnectionTypeName();
                    string id = string.Equals(connectionType, "IP", StringComparison.OrdinalIgnoreCase)
                        ? cameraInfo.GetMacAddress()
                        : cameraInfo.GetId();

                    if (string.IsNullOrWhiteSpace(id))
                        id = cameraInfo.GetGuid();

                    if (string.IsNullOrWhiteSpace(id))
                        id = $"sony-{index}";

                    if (!seenPorts.Add(id))
                        continue;

                    string deviceName = string.IsNullOrWhiteSpace(model)
                        ? "Sony Kamera"
                        : model;

                    devices.Add(new SonySdkDevice(deviceName, id));
                }

                return devices;
            }
            finally
            {
                SonyEnumCameraObjectInfo.Release(enumPtr);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        private readonly struct SonyEnumCameraObjectInfo
        {
            private readonly IntPtr _instance;

            public SonyEnumCameraObjectInfo(IntPtr instance)
            {
                _instance = instance;
            }

            public uint GetCount()
            {
                IntPtr function = GetVtableFunction(_instance, 0);
                var callback = (delegate* unmanaged<IntPtr, uint>)function;
                return callback(_instance);
            }

            public IntPtr GetCameraObjectInfo(uint index)
            {
                IntPtr function = GetVtableFunction(_instance, 1);
                var callback = (delegate* unmanaged<IntPtr, uint, IntPtr>)function;
                return callback(_instance, index);
            }

            public static void Release(IntPtr instance)
            {
                if (instance == IntPtr.Zero)
                    return;

                IntPtr function = GetVtableFunction(instance, 2);
                var callback = (delegate* unmanaged<IntPtr, void>)function;
                callback(instance);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        private readonly struct SonyCameraObjectInfo
        {
            private readonly IntPtr _instance;

            public SonyCameraObjectInfo(IntPtr instance)
            {
                _instance = instance;
            }

            public string GetModel() => ReadString(3, 4);

            public string GetName() => ReadString(0, 1);

            public short GetUsbPid()
            {
                IntPtr function = GetVtableFunction(_instance, 4);
                var callback = (delegate* unmanaged<IntPtr, short>)function;
                return callback(_instance);
            }

            public string GetId() => ReadBytesAsAsciiString(5, 6);

            public uint GetIdType()
            {
                IntPtr function = GetVtableFunction(_instance, 7);
                var callback = (delegate* unmanaged<IntPtr, uint>)function;
                return callback(_instance);
            }

            public string GetConnectionTypeName() => ReadString(9, null);

            public string GetAdaptorName() => ReadString(10, null);

            public string GetGuid() => ReadString(11, null);

            public string GetPairingNecessity() => ReadString(12, null);

            public uint GetSshSupport()
            {
                IntPtr function = GetVtableFunction(_instance, 14);
                var callback = (delegate* unmanaged<IntPtr, uint>)function;
                return callback(_instance);
            }

            public string GetMacAddress() => ReadString(19, 20);

            public static void Release(IntPtr instance)
            {
                if (instance == IntPtr.Zero)
                    return;

                IntPtr function = GetVtableFunction(instance, 21);
                var callback = (delegate* unmanaged<IntPtr, void>)function;
                callback(instance);
            }

            private string ReadString(int stringMethodIndex, int? sizeMethodIndex)
            {
                IntPtr function = GetVtableFunction(_instance, stringMethodIndex);
                var callback = (delegate* unmanaged<IntPtr, IntPtr>)function;
                IntPtr dataPtr = callback(_instance);
                if (dataPtr == IntPtr.Zero)
                    return string.Empty;

                if (sizeMethodIndex.HasValue)
                {
                    IntPtr sizeFunction = GetVtableFunction(_instance, sizeMethodIndex.Value);
                    var sizeCallback = (delegate* unmanaged<IntPtr, uint>)sizeFunction;
                    uint byteCount = sizeCallback(_instance);
                    if (byteCount == 0)
                        return string.Empty;

                    return ReadNullTerminatedUtf8(dataPtr, (int)byteCount);
                }

                return Marshal.PtrToStringAnsi(dataPtr) ?? string.Empty;
            }

            private string ReadBytesAsAsciiString(int dataMethodIndex, int sizeMethodIndex)
            {
                IntPtr function = GetVtableFunction(_instance, dataMethodIndex);
                var callback = (delegate* unmanaged<IntPtr, IntPtr>)function;
                IntPtr dataPtr = callback(_instance);
                if (dataPtr == IntPtr.Zero)
                    return string.Empty;

                IntPtr sizeFunction = GetVtableFunction(_instance, sizeMethodIndex);
                var sizeCallback = (delegate* unmanaged<IntPtr, uint>)sizeFunction;
                int byteCount = checked((int)sizeCallback(_instance));
                if (byteCount <= 0)
                    return string.Empty;

                byte[] buffer = new byte[byteCount];
                Marshal.Copy(dataPtr, buffer, 0, byteCount);
                return BitConverter.ToString(buffer).Replace("-", string.Empty);
            }
        }

        private static IntPtr GetVtableFunction(IntPtr instance, int slotIndex)
        {
            IntPtr vtable = Marshal.ReadIntPtr(instance);
            return Marshal.ReadIntPtr(vtable, slotIndex * IntPtr.Size);
        }

        private static sbyte[] ToNullTerminatedAnsi(string value)
        {
            byte[] bytes = System.Text.Encoding.ASCII.GetBytes(value ?? string.Empty);
            sbyte[] result = new sbyte[bytes.Length + 1];
            Buffer.BlockCopy(bytes, 0, result, 0, bytes.Length);
            return result;
        }

        private static byte[] ToRawIdBytes(string hex)
        {
            if (string.IsNullOrWhiteSpace(hex))
                return Array.Empty<byte>();

            string normalized = hex.Replace("-", string.Empty).Trim();
            if (normalized.Length % 2 != 0)
                return System.Text.Encoding.ASCII.GetBytes(normalized);

            byte[] result = new byte[normalized.Length / 2];
            for (int i = 0; i < result.Length; i++)
            {
                string segment = normalized.Substring(i * 2, 2);
                if (!byte.TryParse(segment, System.Globalization.NumberStyles.HexNumber, null, out byte value))
                    return System.Text.Encoding.ASCII.GetBytes(normalized);

                result[i] = value;
            }

            return result;
        }

        private static string ReadNullTerminatedUtf8(IntPtr dataPtr, int byteCount)
        {
            if (dataPtr == IntPtr.Zero || byteCount <= 0)
                return string.Empty;

            byte[] buffer = new byte[byteCount];
            Marshal.Copy(dataPtr, buffer, 0, byteCount);
            int terminator = Array.IndexOf(buffer, (byte)0);
            if (terminator >= 0)
                byteCount = terminator;

            return byteCount == 0
                ? string.Empty
                : System.Text.Encoding.UTF8.GetString(buffer, 0, byteCount).Trim();
        }

        internal sealed class SonyConnection : IDisposable
        {
            private readonly SonyCrSdkRuntime _owner;
            private readonly SonyDeviceCallback _callback;
            private bool _disposed;

            internal SonyConnection(SonyCrSdkRuntime owner, string deviceName, long deviceHandle, string captureFolder, SonyDeviceCallback callback)
            {
                _owner = owner;
                DeviceName = string.IsNullOrWhiteSpace(deviceName) ? "Sony Kamera" : deviceName;
                DeviceHandle = deviceHandle;
                CaptureFolder = captureFolder;
                _callback = callback;
            }

            public string DeviceName { get; }
            public long DeviceHandle { get; }
            public string CaptureFolder { get; }
            public int LastLiveViewWidth { get; private set; }
            public int LastLiveViewHeight { get; private set; }

            public string TriggerStillCapture()
            {
                ObjectDisposedException.ThrowIf(_disposed, this);
                _callback.PrepareForCapture(CaptureFolder);
                _owner.SendStillCapture(DeviceHandle);
                return _callback.WaitForCapturedFile(TimeSpan.FromSeconds(20), CaptureFolder);
            }

            public void StartLiveView()
            {
                ObjectDisposedException.ThrowIf(_disposed, this);
                _owner.SetLiveViewEnabled(DeviceHandle, true);
            }

            public void StopLiveView()
            {
                if (_disposed)
                    return;

                _owner.SetLiveViewEnabled(DeviceHandle, false);
            }

            public byte[]? TryGetLiveViewJpegFrame()
            {
                ObjectDisposedException.ThrowIf(_disposed, this);
                byte[]? frame = _owner.TryGetLiveViewJpegFrame(DeviceHandle, out int width, out int height);
                if (frame != null)
                {
                    LastLiveViewWidth = width;
                    LastLiveViewHeight = height;
                }

                return frame;
            }

            public void Dispose()
            {
                if (_disposed)
                    return;

                _disposed = true;
                try
                {
                    _owner.SetLiveViewEnabled(DeviceHandle, false);
                }
                catch
                {
                }
                _owner.DisconnectCamera(DeviceHandle);
                _callback.Dispose();
                _owner.Release();
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct SonyImageInfo
        {
            public uint Width;
            public uint Height;
            public uint BufferSize;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct SonyImageDataBlock
        {
            public uint FrameNo;
            public uint Size;
            public byte* Data;
            public uint ImageSize;
            public uint TimeCode;
        }

        internal sealed unsafe class SonyDeviceCallback : IDisposable
        {
            private static readonly Dictionary<IntPtr, SonyDeviceCallback> Callbacks = new();
            private static readonly object CallbackSync = new();

            private readonly IntPtr _instance;
            private readonly IntPtr _vtable;
            private readonly ManualResetEventSlim _captureCompleted = new(false);
            private string? _capturedFile;
            private DateTime _captureStartedUtc;
            private HashSet<string> _knownFiles = new(StringComparer.OrdinalIgnoreCase);
            private bool _disposed;

            public SonyDeviceCallback()
            {
                _vtable = Marshal.AllocHGlobal(IntPtr.Size * 21);
                for (int i = 0; i < 21; i++)
                    Marshal.WriteIntPtr(_vtable, i * IntPtr.Size, IntPtr.Zero);

                Marshal.WriteIntPtr(_vtable, 0 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, ushort, void>)&OnConnected);
                Marshal.WriteIntPtr(_vtable, 1 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, void>)&OnDisconnected);
                Marshal.WriteIntPtr(_vtable, 2 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, void>)&OnPropertyChanged);
                Marshal.WriteIntPtr(_vtable, 3 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, uint*, void>)&OnPropertyChangedCodes);
                Marshal.WriteIntPtr(_vtable, 4 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, void>)&OnLvPropertyChanged);
                Marshal.WriteIntPtr(_vtable, 5 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, uint*, void>)&OnLvPropertyChangedCodes);
                Marshal.WriteIntPtr(_vtable, 6 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, IntPtr, uint, void>)&OnCompleteDownload);
                Marshal.WriteIntPtr(_vtable, 7 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, IntPtr, void>)&OnCompleteOperation);
                Marshal.WriteIntPtr(_vtable, 8 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, uint, IntPtr, void>)&OnNotifyContentsTransfer);
                Marshal.WriteIntPtr(_vtable, 9 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, void>)&OnWarning);
                Marshal.WriteIntPtr(_vtable, 10 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, int, int, int, void>)&OnWarningExt);
                Marshal.WriteIntPtr(_vtable, 11 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, void>)&OnError);
                Marshal.WriteIntPtr(_vtable, 12 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, uint, uint, void>)&OnNotifyFtpTransferResult);
                Marshal.WriteIntPtr(_vtable, 13 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, uint, IntPtr, void>)&OnNotifyRemoteTransferResultByFile);
                Marshal.WriteIntPtr(_vtable, 14 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, uint, byte*, ulong, void>)&OnNotifyRemoteTransferResultByData);
                Marshal.WriteIntPtr(_vtable, 15 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, uint, uint, void>)&OnNotifyRemoteTransferContentsListChanged);
                Marshal.WriteIntPtr(_vtable, 16 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, IntPtr, void>)&OnNotifyRemoteFirmwareUpdateResult);
                Marshal.WriteIntPtr(_vtable, 17 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, void>)&OnReceivePlaybackTimeCode);
                Marshal.WriteIntPtr(_vtable, 18 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, byte, int, byte*, long, long, int, int, void>)&OnReceivePlaybackData);
                Marshal.WriteIntPtr(_vtable, 19 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, uint, uint, void>)&OnNotifyMonitorUpdated);
                Marshal.WriteIntPtr(_vtable, 20 * IntPtr.Size, (IntPtr)(delegate* unmanaged<IntPtr, IntPtr, uint, void>)&OnNotifyPostViewImage);

                _instance = Marshal.AllocHGlobal(IntPtr.Size);
                Marshal.WriteIntPtr(_instance, _vtable);

                lock (CallbackSync)
                {
                    Callbacks[_instance] = this;
                }
            }

            public IntPtr Pointer => _instance;

            public SonyDeviceCallback Detach() => this;

            public void PrepareForCapture(string captureFolder)
            {
                _capturedFile = null;
                _captureStartedUtc = DateTime.UtcNow;
                _knownFiles = Directory.Exists(captureFolder)
                    ? new HashSet<string>(Directory.GetFiles(captureFolder), StringComparer.OrdinalIgnoreCase)
                    : new HashSet<string>(StringComparer.OrdinalIgnoreCase);
                _captureCompleted.Reset();
            }

            public string WaitForCapturedFile(TimeSpan timeout, string captureFolder)
            {
                if (!_captureCompleted.Wait(timeout))
                    throw new TimeoutException("Sony Capture hat keine Datei innerhalb des erwarteten Zeitfensters geliefert.");

                if (string.IsNullOrWhiteSpace(_capturedFile))
                {
                    string[] candidates = Directory.Exists(captureFolder)
                        ? Directory.GetFiles(captureFolder)
                        : Array.Empty<string>();

                    string? newestNewFile = null;
                    DateTime newestWrite = DateTime.MinValue;
                    foreach (string file in candidates)
                    {
                        if (_knownFiles.Contains(file))
                            continue;

                        DateTime writeTime = File.GetLastWriteTimeUtc(file);
                        if (writeTime < _captureStartedUtc.AddSeconds(-1))
                            continue;

                        if (writeTime > newestWrite)
                        {
                            newestWrite = writeTime;
                            newestNewFile = file;
                        }
                    }

                    if (string.IsNullOrWhiteSpace(newestNewFile))
                        throw new InvalidOperationException("Sony Capture wurde ausgelöst, aber es wurde keine Datei geliefert.");

                    return newestNewFile;
                }

                return _capturedFile;
            }

            public void Dispose()
            {
                if (_disposed)
                    return;

                _disposed = true;
                lock (CallbackSync)
                {
                    Callbacks.Remove(_instance);
                }
                Marshal.FreeHGlobal(_instance);
                Marshal.FreeHGlobal(_vtable);
                _captureCompleted.Dispose();
            }

            private static SonyDeviceCallback? Resolve(IntPtr self)
            {
                lock (CallbackSync)
                {
                    return Callbacks.TryGetValue(self, out SonyDeviceCallback? callback) ? callback : null;
                }
            }

            private static void CompleteCapture(IntPtr self, string? filePath)
            {
                SonyDeviceCallback? callback = Resolve(self);
                if (callback == null)
                    return;

                if (!string.IsNullOrWhiteSpace(filePath))
                    callback._capturedFile = filePath;

                callback._captureCompleted.Set();
            }

            [UnmanagedCallersOnly] private static void OnConnected(IntPtr self, ushort version) { }
            [UnmanagedCallersOnly] private static void OnDisconnected(IntPtr self, uint error) { }
            [UnmanagedCallersOnly] private static void OnPropertyChanged(IntPtr self) { }
            [UnmanagedCallersOnly] private static void OnPropertyChangedCodes(IntPtr self, uint num, uint* codes) { }
            [UnmanagedCallersOnly] private static void OnLvPropertyChanged(IntPtr self) { }
            [UnmanagedCallersOnly] private static void OnLvPropertyChangedCodes(IntPtr self, uint num, uint* codes) { }
            [UnmanagedCallersOnly] private static void OnCompleteDownload(IntPtr self, IntPtr filename, uint type)
            {
                CompleteCapture(self, Marshal.PtrToStringAnsi(filename));
            }
            [UnmanagedCallersOnly] private static void OnCompleteOperation(IntPtr self, uint code, IntPtr resultData) { }
            [UnmanagedCallersOnly] private static void OnNotifyContentsTransfer(IntPtr self, uint notify, uint handle, IntPtr filename) { }
            [UnmanagedCallersOnly] private static void OnWarning(IntPtr self, uint warning) { }
            [UnmanagedCallersOnly] private static void OnWarningExt(IntPtr self, uint warning, int p1, int p2, int p3) { }
            [UnmanagedCallersOnly] private static void OnError(IntPtr self, uint error) { }
            [UnmanagedCallersOnly] private static void OnNotifyFtpTransferResult(IntPtr self, uint notify, uint success, uint fail) { }
            [UnmanagedCallersOnly] private static void OnNotifyRemoteTransferResultByFile(IntPtr self, uint notify, uint per, IntPtr filename) { }
            [UnmanagedCallersOnly] private static void OnNotifyRemoteTransferResultByData(IntPtr self, uint notify, uint per, byte* data, ulong size) { }
            [UnmanagedCallersOnly] private static void OnNotifyRemoteTransferContentsListChanged(IntPtr self, uint notify, uint slotNumber, uint addSize) { }
            [UnmanagedCallersOnly] private static void OnNotifyRemoteFirmwareUpdateResult(IntPtr self, uint notify, IntPtr param) { }
            [UnmanagedCallersOnly] private static void OnReceivePlaybackTimeCode(IntPtr self, uint timeCode) { }
            [UnmanagedCallersOnly] private static void OnReceivePlaybackData(IntPtr self, byte mediaType, int dataSize, byte* data, long pts, long dts, int p1, int p2) { }
            [UnmanagedCallersOnly] private static void OnNotifyMonitorUpdated(IntPtr self, uint type, uint frameNo) { }
            [UnmanagedCallersOnly] private static void OnNotifyPostViewImage(IntPtr self, IntPtr filename, uint size)
            {
                CompleteCapture(self, Marshal.PtrToStringAnsi(filename));
            }
        }
    }
}
