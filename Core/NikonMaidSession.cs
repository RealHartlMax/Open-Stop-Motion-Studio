using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;

namespace OpenStopMotionStudio.Core
{
    internal sealed class NikonMaidSession : IDisposable
    {
        private const int ResultNoError = 0;
        private const int ResultPending = 1;
        private const int ResultUnexpectedError = -118;

        private const uint CommandAsync = 0;
        private const uint CommandOpen = 1;
        private const uint CommandClose = 2;
        private const uint CommandGetCapCount = 3;
        private const uint CommandGetCapInfo = 4;
        private const uint CommandCapStart = 5;
        private const uint CommandCapSet = 6;
        private const uint CommandCapGet = 7;
        private const uint CommandCapGetArray = 9;

        private const uint DataTypeNull = 0;
        private const uint DataTypeUnsigned = 3;
        private const uint DataTypeUnsignedPtr = 6;
        private const uint DataTypeStringPtr = 11;
        private const uint DataTypeCallbackPtr = 13;
        private const uint DataTypeArrayPtr = 15;
        private const uint DataTypeEnumPtr = 16;
        private const uint DataTypeObjectPtr = 17;
        private const uint DataTypeCapInfoPtr = 18;

        private const uint CapOpStart = 0x0001;
        private const uint CapOpGet = 0x0002;
        private const uint CapOpSet = 0x0004;
        private const uint CapOpGetArray = 0x0008;

        private const uint CapChildren = 7;
        private const uint CapName = 9;
        private const uint CapDataTypes = 12;
        private const uint CapCapture = 17;
        private const uint CapAcquire = 20;
        private const uint CapDataProc = 4;

        private const uint NikonCapModuleMode = 0x8101;
        private const uint NikonCapLiveViewStatus = 0x823e;
        private const uint NikonCapGetLiveViewImage = 0x8247;
        private const uint NikonCapSaveMedia = 0x8305;
        private const uint NikonCapCaptureAsync = 0x84d8;

        private const uint NikonModuleModeController = 1;
        private const uint NikonLiveViewOff = 0;
        private const uint NikonLiveViewRemote = 3;
        private const uint NikonSaveMediaSdRam = 1;

        private const uint DataObjImage = 0x00000001;
        private const uint DataObjFile = 0x00000010;

        private readonly IntPtr _moduleHandle;
        private readonly List<IntPtr> _supportHandles = new();
        private readonly MaidEntryPointDelegate _entryPoint;
        private readonly CompletionDelegate _completionDelegate;
        private readonly DataDelegate _dataDelegate;
        private readonly IntPtr _completionPtr;
        private readonly IntPtr _dataPtr;
        private bool _disposed;

        private NikonMaidSession(IntPtr moduleHandle, MaidEntryPointDelegate entryPoint)
        {
            _moduleHandle = moduleHandle;
            _entryPoint = entryPoint;
            _completionDelegate = CompletionCallback;
            _dataDelegate = DataCallback;
            _completionPtr = Marshal.GetFunctionPointerForDelegate(_completionDelegate);
            _dataPtr = Marshal.GetFunctionPointerForDelegate(_dataDelegate);

            Module = OpenRoot();
            TrySetUInt(Module, NikonCapModuleMode, NikonModuleModeController);
            Source = OpenSource(null);
            SourceName = GetString(Source, CapName) ?? "Nikon Kamera";
        }

        private Node Module { get; }
        private Node Source { get; set; }
        public string SourceName { get; private set; }

        public static NikonMaidSession Open(NikonMaidSdkLocation location, string? preferredDeviceName)
        {
            string binaryDir = EnsureBinaryDirectory(location);
            List<IntPtr> deps = new();
            foreach (string dllPath in Directory.EnumerateFiles(binaryDir, "*.dll"))
                deps.Add(NativeLibrary.Load(dllPath));

            string modulePath = Path.Combine(binaryDir, Path.GetFileName(location.ModuleRelativePath.Replace('/', Path.DirectorySeparatorChar)));
            IntPtr moduleHandle = NativeLibrary.Load(modulePath);
            if (!NativeLibrary.TryGetExport(moduleHandle, "MAIDEntryPoint", out IntPtr proc) || proc == IntPtr.Zero)
                throw new InvalidOperationException("Die Nikon-Funktion MAIDEntryPoint wurde nicht gefunden.");

            var session = new NikonMaidSession(moduleHandle, Marshal.GetDelegateForFunctionPointer<MaidEntryPointDelegate>(proc));
            session._supportHandles.AddRange(deps);

            if (!string.IsNullOrWhiteSpace(preferredDeviceName))
            {
                Node? preferred = session.TryFindSource(preferredDeviceName);
                if (preferred is not null)
                {
                    session.CloseNode(session.Source);
                    session.Source = preferred;
                    session.SourceName = session.GetString(preferred, CapName) ?? preferredDeviceName;
                }
            }

            return session;
        }

        public void StartLiveView()
        {
            if (!TrySetUInt(Source, NikonCapLiveViewStatus, NikonLiveViewRemote))
                throw new InvalidOperationException("Die Nikon-Kamera unterstützt keinen Remote-Live-View.");
        }

        public void StopLiveView()
        {
            TrySetUInt(Source, NikonCapLiveViewStatus, NikonLiveViewOff);
        }

        public byte[]? TryGetLiveViewJpegFrame()
        {
            if (!HasOps(Source, NikonCapGetLiveViewImage, CapOpGet | CapOpGetArray))
                return null;

            byte[] payload = GetArray(Source, NikonCapGetLiveViewImage);
            for (int i = 0; i < payload.Length - 1; i++)
            {
                if (payload[i] == 0xFF && payload[i + 1] == 0xD8)
                    return payload[i..];
            }

            return null;
        }

        public string CaptureImage()
        {
            IReadOnlyList<uint> beforeChildren = GetChildren(Source);
            TrySetUInt(Source, NikonCapSaveMedia, NikonSaveMediaSdRam);

            uint capId = HasOps(Source, NikonCapCaptureAsync, CapOpStart) ? NikonCapCaptureAsync : CapCapture;
            StartCapability(Source, capId, TimeSpan.FromSeconds(4));

            DateTime timeoutAt = DateTime.UtcNow.AddSeconds(8);
            while (DateTime.UtcNow < timeoutAt)
            {
                Pump(Source.Ptr);
                uint itemId = GetChildren(Source).Except(beforeChildren).FirstOrDefault();
                if (itemId != 0)
                    return AcquireItem(itemId);

                Thread.Sleep(80);
            }

            throw new InvalidOperationException("Die Nikon-Kamera hat kein neues Bildobjekt geliefert.");
        }

        public void Dispose()
        {
            if (_disposed)
                return;

            _disposed = true;
            try { StopLiveView(); } catch { }
            CloseNode(Source);
            CloseNode(Module);
            if (_moduleHandle != IntPtr.Zero)
                NativeLibrary.Free(_moduleHandle);
            foreach (IntPtr handle in _supportHandles)
                if (handle != IntPtr.Zero)
                    NativeLibrary.Free(handle);
        }

        private Node OpenRoot()
        {
            IntPtr objectPtr = AllocObjectPtr();
            EnsureOk(_entryPoint(IntPtr.Zero, CommandOpen, 0, DataTypeObjectPtr, objectPtr, IntPtr.Zero, IntPtr.Zero), "Das Nikon-Modul konnte nicht geöffnet werden.");
            return new Node(objectPtr, GetCapabilities(objectPtr));
        }

        private Node OpenSource(string? preferredName)
        {
            Node? preferred = TryFindSource(preferredName);
            if (preferred is not null)
                return preferred;

            uint firstSourceId = GetChildren(Module).FirstOrDefault();
            if (firstSourceId == 0)
                throw new InvalidOperationException("Im Nikon-Modul wurde keine Kamera gefunden.");

            return OpenChild(Module, firstSourceId);
        }

        private Node? TryFindSource(string? preferredName)
        {
            if (string.IsNullOrWhiteSpace(preferredName))
                return null;

            string normalizedPreferred = NormalizeKey(preferredName);
            List<Node> openedNodes = new();
            try
            {
                foreach (uint childId in GetChildren(Module))
                {
                    Node source = OpenChild(Module, childId);
                    openedNodes.Add(source);

                    string normalizedSource = NormalizeKey(GetString(source, CapName));
                    if (normalizedPreferred.Contains(normalizedSource, StringComparison.Ordinal)
                        || normalizedSource.Contains(normalizedPreferred, StringComparison.Ordinal))
                    {
                        openedNodes.Remove(source);
                        return source;
                    }
                }
            }
            finally
            {
                foreach (Node node in openedNodes)
                    CloseNode(node);
            }

            return null;
        }

        private Node OpenChild(Node parent, uint childId)
        {
            IntPtr objectPtr = AllocObjectPtr();
            EnsureOk(_entryPoint(parent.Ptr, CommandOpen, childId, DataTypeObjectPtr, objectPtr, IntPtr.Zero, IntPtr.Zero), "Ein Nikon-Kindobjekt konnte nicht geöffnet werden.");
            return new Node(objectPtr, GetCapabilities(objectPtr));
        }

        private List<CapInfo> GetCapabilities(IntPtr objectPtr)
        {
            IntPtr countPtr = Marshal.AllocHGlobal(sizeof(uint));
            try
            {
                Marshal.WriteInt32(countPtr, 0);
                Call(objectPtr, CommandGetCapCount, 0, DataTypeUnsignedPtr, countPtr, TimeSpan.FromSeconds(2));
                uint count = unchecked((uint)Marshal.ReadInt32(countPtr));
                if (count == 0)
                    return new List<CapInfo>();

                int capSize = Marshal.SizeOf<CapInfo>();
                IntPtr capPtr = Marshal.AllocHGlobal(capSize * checked((int)count));
                try
                {
                    Call(objectPtr, CommandGetCapInfo, count, DataTypeCapInfoPtr, capPtr, TimeSpan.FromSeconds(2));
                    List<CapInfo> caps = new(checked((int)count));
                    for (int i = 0; i < count; i++)
                        caps.Add(Marshal.PtrToStructure<CapInfo>(IntPtr.Add(capPtr, i * capSize)));
                    return caps;
                }
                finally
                {
                    Marshal.FreeHGlobal(capPtr);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(countPtr);
            }
        }

        private bool HasOps(Node node, uint capId, uint ops)
        {
            CapInfo? cap = node.Caps.FirstOrDefault(info => info.Id == capId);
            return cap.HasValue && (cap.Value.Operations & ops) == ops;
        }

        private IReadOnlyList<uint> GetChildren(Node node)
        {
            if (!HasOps(node, CapChildren, CapOpGet | CapOpGetArray))
                return Array.Empty<uint>();

            IntPtr enumPtr = Marshal.AllocHGlobal(Marshal.SizeOf<EnumValue>());
            try
            {
                Marshal.StructureToPtr(new EnumValue(), enumPtr, false);
                Call(node.Ptr, CommandCapGet, CapChildren, DataTypeEnumPtr, enumPtr, TimeSpan.FromSeconds(2));
                EnumValue info = Marshal.PtrToStructure<EnumValue>(enumPtr);
                if (info.Elements <= 0)
                    return Array.Empty<uint>();

                IntPtr dataPtr = Marshal.AllocHGlobal(info.Elements * Math.Max(info.PhysicalBytes, (short)4));
                try
                {
                    info.Data = dataPtr;
                    Marshal.StructureToPtr(info, enumPtr, false);
                    Call(node.Ptr, CommandCapGetArray, CapChildren, DataTypeEnumPtr, enumPtr, TimeSpan.FromSeconds(2));

                    int[] raw = new int[info.Elements];
                    Marshal.Copy(dataPtr, raw, 0, raw.Length);
                    return raw.Select(value => unchecked((uint)value)).ToList();
                }
                finally
                {
                    Marshal.FreeHGlobal(dataPtr);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(enumPtr);
            }
        }

        private string? GetString(Node node, uint capId)
        {
            if (!HasOps(node, capId, CapOpGet))
                return null;

            IntPtr stringPtr = Marshal.AllocHGlobal(Marshal.SizeOf<MaidString>());
            try
            {
                Marshal.StructureToPtr(new MaidString(), stringPtr, false);
                Call(node.Ptr, CommandCapGet, capId, DataTypeStringPtr, stringPtr, TimeSpan.FromSeconds(2));
                return Marshal.PtrToStructure<MaidString>(stringPtr).Value?.TrimEnd('\0').Trim();
            }
            finally
            {
                Marshal.FreeHGlobal(stringPtr);
            }
        }

        private uint? GetUInt(Node node, uint capId)
        {
            if (!HasOps(node, capId, CapOpGet))
                return null;

            IntPtr valuePtr = Marshal.AllocHGlobal(sizeof(uint));
            try
            {
                Marshal.WriteInt32(valuePtr, 0);
                Call(node.Ptr, CommandCapGet, capId, DataTypeUnsignedPtr, valuePtr, TimeSpan.FromSeconds(2));
                return unchecked((uint)Marshal.ReadInt32(valuePtr));
            }
            finally
            {
                Marshal.FreeHGlobal(valuePtr);
            }
        }

        private bool TrySetUInt(Node node, uint capId, uint value)
        {
            if (!HasOps(node, capId, CapOpSet))
                return false;

            Call(node.Ptr, CommandCapSet, capId, DataTypeUnsigned, new IntPtr(unchecked((long)value)), TimeSpan.FromSeconds(2));
            return true;
        }

        private byte[] GetArray(Node node, uint capId)
        {
            IntPtr arrayPtr = Marshal.AllocHGlobal(Marshal.SizeOf<ArrayValue>());
            try
            {
                Marshal.StructureToPtr(new ArrayValue(), arrayPtr, false);
                Call(node.Ptr, CommandCapGet, capId, DataTypeArrayPtr, arrayPtr, TimeSpan.FromSeconds(2));
                ArrayValue info = Marshal.PtrToStructure<ArrayValue>(arrayPtr);
                int byteCount = checked((int)(info.Elements * (uint)Math.Max(info.PhysicalBytes, (short)1)));
                if (byteCount <= 0)
                    return Array.Empty<byte>();

                IntPtr dataPtr = Marshal.AllocHGlobal(byteCount);
                try
                {
                    info.Data = dataPtr;
                    Marshal.StructureToPtr(info, arrayPtr, false);
                    Call(node.Ptr, CommandCapGetArray, capId, DataTypeArrayPtr, arrayPtr, TimeSpan.FromSeconds(2));
                    byte[] buffer = new byte[byteCount];
                    Marshal.Copy(dataPtr, buffer, 0, byteCount);
                    return buffer;
                }
                finally
                {
                    Marshal.FreeHGlobal(dataPtr);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(arrayPtr);
            }
        }

        private string AcquireItem(uint itemId)
        {
            Node itemNode = OpenChild(Source, itemId);
            try
            {
                uint dataTypes = GetUInt(itemNode, CapDataTypes) ?? 0;
                if ((dataTypes & DataObjImage) == 0)
                    throw new InvalidOperationException("Das Nikon-Bildobjekt enthält kein Image-Data-Objekt.");

                Node imageNode = OpenChild(itemNode, DataObjImage);
                try
                {
                    return AcquireData(imageNode);
                }
                finally
                {
                    CloseNode(imageNode);
                }
            }
            finally
            {
                CloseNode(itemNode);
            }
        }

        private string AcquireData(Node imageNode)
        {
            if (!HasOps(imageNode, CapDataProc, CapOpSet))
                throw new InvalidOperationException("Das Nikon-Image-Objekt erlaubt keinen DataProc.");

            if (!HasOps(imageNode, CapAcquire, CapOpStart))
                throw new InvalidOperationException("Das Nikon-Image-Objekt erlaubt keinen Acquire-Aufruf.");

            using var completion = new CompletionState();
            using var dataState = new DataState(Path.Combine(Path.GetTempPath(), "OpenStopMotionStudio", "NikonCapture"));

            IntPtr callbackPtr = Marshal.AllocHGlobal(Marshal.SizeOf<CallbackValue>());
            try
            {
                dataState.Attach();
                Marshal.StructureToPtr(new CallbackValue
                {
                    Proc = _dataPtr,
                    RefProc = dataState.RefPtr
                }, callbackPtr, false);

                Call(imageNode.Ptr, CommandCapSet, CapDataProc, DataTypeCallbackPtr, callbackPtr, TimeSpan.FromSeconds(2));
                StartCapability(imageNode, CapAcquire, TimeSpan.FromSeconds(10), completion);
                if (dataState.Error is not null)
                    throw new InvalidOperationException("Das Nikon-Bild konnte nicht empfangen werden.", dataState.Error);

                if (string.IsNullOrWhiteSpace(dataState.CompletedFilePath))
                    throw new InvalidOperationException("Das Nikon-Bild wurde nicht als Datei bereitgestellt.");

                return dataState.CompletedFilePath;
            }
            finally
            {
                try
                {
                    Call(imageNode.Ptr, CommandCapSet, CapDataProc, DataTypeNull, IntPtr.Zero, TimeSpan.FromSeconds(2));
                }
                catch
                {
                    // Best effort cleanup.
                }

                Marshal.FreeHGlobal(callbackPtr);
            }
        }

        private void StartCapability(Node node, uint capId, TimeSpan timeout, CompletionState? state = null)
        {
            using var ownedState = state is null ? new CompletionState() : null;
            CompletionState waitState = state ?? ownedState!;
            waitState.Attach();

            int result = _entryPoint(node.Ptr, CommandCapStart, capId, DataTypeNull, IntPtr.Zero, _completionPtr, waitState.RefPtr);
            EnsureOk(result, $"Die Nikon-Fähigkeit 0x{capId:X} konnte nicht gestartet werden.");
            Wait(node.Ptr, waitState, timeout);
        }

        private void Call(IntPtr objectPtr, uint command, uint param, uint dataType, IntPtr data, TimeSpan timeout)
        {
            using var state = new CompletionState();
            state.Attach();
            int result = _entryPoint(objectPtr, command, param, dataType, data, _completionPtr, state.RefPtr);
            EnsureOk(result, "Ein Nikon-SDK-Aufruf ist fehlgeschlagen.");
            Wait(objectPtr, state, timeout);
        }

        private void Wait(IntPtr objectPtr, CompletionState state, TimeSpan timeout)
        {
            DateTime timeoutAt = DateTime.UtcNow.Add(timeout);
            while (state.DoneCount == 0 && DateTime.UtcNow < timeoutAt)
            {
                Pump(objectPtr);
                Thread.Sleep(10);
            }

            if (state.DoneCount == 0)
                throw new InvalidOperationException("Das Nikon-SDK hat das Zeitlimit überschritten.");

            EnsureOk(state.Result, "Der Nikon-SDK-Aufruf wurde mit einem Fehler beendet.");
        }

        private void Pump(IntPtr objectPtr)
        {
            if (objectPtr == IntPtr.Zero)
                return;

            _entryPoint(objectPtr, CommandAsync, 0, DataTypeNull, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);
        }

        private void CloseNode(Node node)
        {
            if (node.Ptr == IntPtr.Zero)
                return;

            try
            {
                _entryPoint(node.Ptr, CommandClose, 0, DataTypeNull, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);
            }
            catch
            {
                // Suppress cleanup errors.
            }
            finally
            {
                Marshal.FreeHGlobal(node.Ptr);
            }
        }

        private static IntPtr AllocObjectPtr()
        {
            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf<ObjectValue>());
            Marshal.StructureToPtr(new ObjectValue(), ptr, false);
            return ptr;
        }

        private static void EnsureOk(int result, string message)
        {
            if (result < 0)
                throw new InvalidOperationException($"{message} Nikon-Code: {result}");
        }

        private static string EnsureBinaryDirectory(NikonMaidSdkLocation location)
        {
            if (!location.IsArchive)
                return Path.Combine(location.SourcePath, location.BinaryDirectoryRelativePath.Replace('/', Path.DirectorySeparatorChar));

            string cacheRoot = Path.Combine(Path.GetTempPath(), "OpenStopMotionStudio", "NikonSdkCache", Path.GetFileNameWithoutExtension(location.SourcePath));
            string modulePath = Path.Combine(cacheRoot, location.ModuleRelativePath.Replace('/', Path.DirectorySeparatorChar));
            if (File.Exists(modulePath))
                return Path.Combine(cacheRoot, location.BinaryDirectoryRelativePath.Replace('/', Path.DirectorySeparatorChar));

            using ZipArchive archive = ZipFile.OpenRead(location.SourcePath);
            string prefix = location.BinaryDirectoryRelativePath.TrimEnd('/') + "/";

            foreach (ZipArchiveEntry entry in archive.Entries.Where(entry =>
                         entry.FullName.Replace('\\', '/').StartsWith(prefix, StringComparison.OrdinalIgnoreCase)))
            {
                if (string.IsNullOrEmpty(entry.Name))
                    continue;

                string targetPath = Path.Combine(cacheRoot, entry.FullName.Replace('/', Path.DirectorySeparatorChar));
                Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
                entry.ExtractToFile(targetPath, overwrite: true);
            }

            return Path.Combine(cacheRoot, location.BinaryDirectoryRelativePath.Replace('/', Path.DirectorySeparatorChar));
        }

        private static string NormalizeKey(string? value)
        {
            if (string.IsNullOrWhiteSpace(value))
                return string.Empty;

            return new string(value.Where(char.IsLetterOrDigit).Select(char.ToLowerInvariant).ToArray());
        }

        private static void CompletionCallback(IntPtr objectPtr, uint command, uint param, uint dataType, IntPtr data, IntPtr refComplete, int result)
        {
            if (refComplete == IntPtr.Zero)
                return;

            var handle = GCHandle.FromIntPtr(refComplete);
            if (handle.Target is CompletionState state)
            {
                state.DoneCount++;
                state.Result = result;
            }
        }

        private static int DataCallback(IntPtr refClient, IntPtr infoPtr, IntPtr dataPtr)
        {
            if (refClient == IntPtr.Zero)
                return ResultNoError;

            var handle = GCHandle.FromIntPtr(refClient);
            if (handle.Target is not DataState state)
                return ResultUnexpectedError;

            try
            {
                DataInfo info = Marshal.PtrToStructure<DataInfo>(infoPtr);
                if ((info.Type & DataObjFile) != 0)
                    state.AppendFile(Marshal.PtrToStructure<FileInfoValue>(infoPtr), dataPtr);
                else
                    state.AppendRaw(Marshal.PtrToStructure<ImageInfoValue>(infoPtr), dataPtr);

                return ResultNoError;
            }
            catch (Exception ex)
            {
                state.Error = ex;
                return ResultUnexpectedError;
            }
        }

        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        private delegate int MaidEntryPointDelegate(IntPtr objectPtr, uint command, uint param, uint dataType, IntPtr data, IntPtr completionProc, IntPtr refComplete);

        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        private delegate void CompletionDelegate(IntPtr objectPtr, uint command, uint param, uint dataType, IntPtr data, IntPtr refComplete, int result);

        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        private delegate int DataDelegate(IntPtr refClient, IntPtr infoPtr, IntPtr dataPtr);

        [StructLayout(LayoutKind.Sequential)]
        private struct ObjectValue
        {
            public uint Type;
            public uint Id;
            public IntPtr RefClient;
            public IntPtr RefModule;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct CapInfo
        {
            public uint Id;
            public uint Type;
            public uint Visibility;
            public uint Operations;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string Description;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct MaidString
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string Value;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct EnumValue
        {
            public uint Type;
            public int Elements;
            public uint Value;
            public uint DefaultValue;
            public short PhysicalBytes;
            public IntPtr Data;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct ArrayValue
        {
            public uint Type;
            public uint Elements;
            public uint Dim1;
            public uint Dim2;
            public uint Dim3;
            public short PhysicalBytes;
            public short LogicalBits;
            public IntPtr Data;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct CallbackValue
        {
            public IntPtr Proc;
            public IntPtr RefProc;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct DataInfo
        {
            public uint Type;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct SizeValue
        {
            public uint Width;
            public uint Height;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct RectValue
        {
            public int X;
            public int Y;
            public uint Width;
            public uint Height;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct ImageInfoValue
        {
            public DataInfo Base;
            public SizeValue TotalPixels;
            public uint ColorSpace;
            public RectValue DataRect;
            public uint RowBytes;
            public ushort Bits0;
            public ushort Bits1;
            public ushort Bits2;
            public ushort Bits3;
            public ushort Plane;
            public int RemoveObject;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct FileInfoValue
        {
            public DataInfo Base;
            public uint FileDataType;
            public uint TotalLength;
            public uint Start;
            public uint Length;
            public int DiskFile;
            public int RemoveObject;
        }

        private sealed class Node
        {
            public Node(IntPtr ptr, List<CapInfo> caps)
            {
                Ptr = ptr;
                Caps = caps;
            }

            public IntPtr Ptr { get; }
            public List<CapInfo> Caps { get; }
        }

        private sealed class CompletionState : IDisposable
        {
            private GCHandle? _handle;

            public int DoneCount { get; set; }
            public int Result { get; set; }
            public IntPtr RefPtr => _handle.HasValue ? GCHandle.ToIntPtr(_handle.Value) : IntPtr.Zero;

            public void Attach()
            {
                if (_handle.HasValue)
                    return;

                _handle = GCHandle.Alloc(this);
            }

            public void Dispose()
            {
                if (_handle.HasValue)
                {
                    _handle.Value.Free();
                    _handle = null;
                }
            }
        }

        private sealed class DataState : IDisposable
        {
            private GCHandle? _handle;
            private byte[]? _buffer;
            private int _rawOffset;

            public DataState(string tempFolder)
            {
                TempFolder = tempFolder;
            }

            public string TempFolder { get; }
            public string? CompletedFilePath { get; private set; }
            public Exception? Error { get; set; }
            public IntPtr RefPtr => _handle.HasValue ? GCHandle.ToIntPtr(_handle.Value) : IntPtr.Zero;

            public void Attach()
            {
                if (_handle.HasValue)
                    return;

                _handle = GCHandle.Alloc(this);
            }

            public void AppendFile(FileInfoValue info, IntPtr dataPtr)
            {
                EnsureBuffer(checked((int)info.TotalLength));
                Marshal.Copy(dataPtr, _buffer!, checked((int)info.Start), checked((int)info.Length));
                if (info.Start + info.Length < info.TotalLength)
                    return;

                CompletedFilePath = Save(GetExtension(info.FileDataType));
            }

            public void AppendRaw(ImageInfoValue info, IntPtr dataPtr)
            {
                int totalSize = checked((int)(info.RowBytes * info.TotalPixels.Height));
                int chunkSize = checked((int)(info.RowBytes * info.DataRect.Height));
                EnsureBuffer(totalSize);
                Marshal.Copy(dataPtr, _buffer!, _rawOffset, chunkSize);
                _rawOffset += chunkSize;

                if (_rawOffset < totalSize)
                    return;

                CompletedFilePath = Save(".raw");
            }

            public void Dispose()
            {
                if (_handle.HasValue)
                {
                    _handle.Value.Free();
                    _handle = null;
                }
            }

            private void EnsureBuffer(int size)
            {
                if (_buffer == null || _buffer.Length != size)
                    _buffer = new byte[size];
            }

            private string Save(string extension)
            {
                Directory.CreateDirectory(TempFolder);
                string path = Path.Combine(TempFolder, $"nikon_{DateTime.UtcNow:yyyyMMdd_HHmmssfff}{extension}");
                File.WriteAllBytes(path, _buffer!);
                return path;
            }

            private static string GetExtension(uint fileType)
            {
                return fileType switch
                {
                    1 => ".jpg",
                    2 => ".tif",
                    4 => ".nef",
                    _ => ".dat"
                };
            }
        }
    }
}
