using Avalonia;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenStopMotionStudio.Core
{
    public sealed class NikonNefImportService
    {
        private readonly NikonSdkDiscovery _sdkDiscovery;

        public NikonNefImportService()
            : this(new NikonSdkDiscovery())
        {
        }

        public NikonNefImportService(NikonSdkDiscovery sdkDiscovery)
        {
            _sdkDiscovery = sdkDiscovery;
        }

        public NefImportSummary ImportFolder(
            string sourceFolder,
            NefImportSettings settings,
            IProgress<NefImportProgress>? progress = null)
        {
            if (string.IsNullOrWhiteSpace(sourceFolder) || !Directory.Exists(sourceFolder))
                throw new DirectoryNotFoundException("Der gewählte RAW-Ordner wurde nicht gefunden.");

            if (string.IsNullOrWhiteSpace(settings.ProjectFolder))
                throw new InvalidOperationException("Es wurde kein Projektordner für den Import gesetzt.");

            NikonImageSdkLocation sdkLocation = _sdkDiscovery.FindImageSdk()
                ?? throw new InvalidOperationException("Kein lokales Nikon Image SDK für NEF-Import gefunden.");

            List<string> sourceFiles = Directory
                .EnumerateFiles(sourceFolder)
                .Where(IsSupportedRawFile)
                .OrderBy(path => path, StringComparer.OrdinalIgnoreCase)
                .ToList();

            if (sourceFiles.Count == 0)
                throw new InvalidOperationException("Im gewählten Ordner wurden keine NEF- oder NRW-Dateien gefunden.");

            string shotName = CaptureManager.NormalizeShotName(settings.ShotName);
            int frameStart = Math.Max(1, settings.FrameStart);
            
            // Get folders and ensure they exist
            string masterFolder = ProjectPaths.GetOriginalsFolder(settings.ProjectFolder, shotName, ".png");
            string proxyFolder = ProjectPaths.GetProxyFolder(settings.ProjectFolder, shotName);
            Directory.CreateDirectory(masterFolder);
            Directory.CreateDirectory(proxyFolder);

            List<CapturedFrame> importedFrames = new(sourceFiles.Count);

            using var sdk = new NikonImageSdkRuntime(sdkLocation);
            for (int i = 0; i < sourceFiles.Count; i++)
            {
                string sourcePath = sourceFiles[i];
                int frameNumber = frameStart + i;

                progress?.Report(new NefImportProgress(i + 1, sourceFiles.Count, Path.GetFileName(sourcePath)));

                string masterPath = ProjectPaths.BuildOriginalPath(settings.ProjectFolder, shotName, frameNumber, ".png");
                string proxyPath = ProjectPaths.BuildProxyPath(settings.ProjectFolder, shotName, frameNumber);

                ImportedFrameData imported = sdk.ImportFile(sourcePath, masterPath, proxyPath, settings);
                importedFrames.Add(new CapturedFrame(frameNumber, shotName, imported.MasterPath, imported.ProxyPath, imported.PreviewFrame));
            }

            return new NefImportSummary(shotName, frameStart, masterFolder, proxyFolder, importedFrames);
        }

        private static bool IsSupportedRawFile(string path)
        {
            string extension = Path.GetExtension(path);
            return extension.Equals(".nef", StringComparison.OrdinalIgnoreCase)
                || extension.Equals(".nrw", StringComparison.OrdinalIgnoreCase);
        }

        private static string GetProxyExtension(RawImportProxyFormat proxyFormat)
        {
            return proxyFormat switch
            {
                RawImportProxyFormat.Png => ".png",
                _ => ".jpg"
            };
        }

        private sealed class ImportedFrameData
        {
            public ImportedFrameData(string masterPath, string proxyPath, Bitmap previewFrame)
            {
                MasterPath = masterPath;
                ProxyPath = proxyPath;
                PreviewFrame = previewFrame;
            }

            public string MasterPath { get; }
            public string ProxyPath { get; }
            public Bitmap PreviewFrame { get; }
        }

        private sealed class NikonImageSdkRuntime : IDisposable
        {
            private const int LoadWithAlteredSearchPath = 0x00000008;
            private const int MaxPathLength = 260;

            private const uint CommandOpenLibrary = 0x0001;
            private const uint CommandCloseLibrary = 0x0002;
            private const uint CommandOpenSession = 0x0003;
            private const uint CommandCloseSession = 0x0004;
            private const uint CommandGetImageInfo = 0x0011;
            private const uint CommandGetImageData = 0x0012;
            private const uint CommandSetDevelopColorMode = 0x0101;

            private const uint SourceFileNameUtf8 = 0x0008;
            private const uint NikonCodeNone = 0x0000;
            private const uint NikonColorRgb = 0x0020;
            private const int DevelopColorModeAppliedInCamera = 0;

            private readonly IntPtr _libraryHandle;
            private readonly NkflEntryDelegate _entry;
            private bool _disposed;

            public NikonImageSdkRuntime(NikonImageSdkLocation sdkLocation)
            {
                _libraryHandle = LoadLibraryEx(sdkLocation.LibraryPath, IntPtr.Zero, LoadWithAlteredSearchPath);
                if (_libraryHandle == IntPtr.Zero)
                    throw new InvalidOperationException($"NkImgSDK.dll konnte nicht geladen werden: {sdkLocation.LibraryPath}");

                try
                {
                    IntPtr entryProc = GetProcAddress(_libraryHandle, "Nkfl_Entry");
                    if (entryProc == IntPtr.Zero)
                        throw new InvalidOperationException("Die Funktion Nkfl_Entry wurde im Nikon Image SDK nicht gefunden.");

                    _entry = Marshal.GetDelegateForFunctionPointer<NkflEntryDelegate>(entryProc);

                    OpenLibrary();
                    ApplyDevelopColorMode();
                }
                catch
                {
                    FreeLibrary(_libraryHandle);
                    throw;
                }
            }

            public ImportedFrameData ImportFile(string sourcePath, string masterPath, string proxyPath, NefImportSettings settings)
            {
                uint sessionId = OpenSession(sourcePath);

                try
                {
                    NkflImageInfoParam imageInfo = GetImageInfo(sessionId);
                    if (imageInfo.ulColor != NikonColorRgb)
                        throw new InvalidOperationException($"Nicht unterstützter Nikon-Farbraum 0x{imageInfo.ulColor:X} in {Path.GetFileName(sourcePath)}.");

                    if (imageInfo.ulByteDepth is not 1 and not 2)
                        throw new InvalidOperationException($"Nicht unterstützte Byte-Tiefe {imageInfo.ulByteDepth} in {Path.GetFileName(sourcePath)}.");

                    byte[] rawBuffer = GetImageData(sessionId, imageInfo);
                    Bitmap masterBitmap = CreateBitmap(imageInfo, rawBuffer);
                    Bitmap proxyBitmap = CreateProxyBitmap(masterBitmap);

                    SaveMaster(masterBitmap, masterPath);
                    SaveProxy(proxyBitmap, proxyPath, settings);

                    return new ImportedFrameData(masterPath, proxyPath, proxyBitmap);
                }
                finally
                {
                    CloseSession(sessionId);
                }
            }

            public void Dispose()
            {
                if (_disposed)
                    return;

                _disposed = true;

                try
                {
                    ExecuteCommand(CommandCloseLibrary, IntPtr.Zero);
                }
                catch
                {
                    // Beim Beenden wollen wir kein Cleanup-Problem nach oben propagieren.
                }

                if (_libraryHandle != IntPtr.Zero)
                    FreeLibrary(_libraryHandle);
            }

            private void OpenLibrary()
            {
                string tempDirectory = Path.GetTempPath();
                string tempFile = Path.Combine(tempDirectory, $"OpenStopMotionStudio_{Guid.NewGuid():N}.tmp");

                NkflLibraryParam libraryParam = new()
                {
                    ulSize = (uint)Marshal.SizeOf<NkflLibraryParam>(),
                    ulVersion = 0x01000000,
                    ulVMMemorySize = 256,
                    pNkflPtr = IntPtr.Zero,
                    VMFileInfo = CreateFixedUtf8Buffer(tempFile),
                    DefProfPath = CreateFixedUtf8Buffer(tempDirectory)
                };

                uint code = ExecuteStructCommand(CommandOpenLibrary, ref libraryParam);
                EnsureSuccess(code, "Das Nikon Image SDK konnte nicht initialisiert werden.");
            }

            private void ApplyDevelopColorMode()
            {
                NkflDevelopColorMode developColorMode = new()
                {
                    ulSize = (uint)Marshal.SizeOf<NkflDevelopColorMode>(),
                    lDevelopColorMode = DevelopColorModeAppliedInCamera
                };

                uint code = ExecuteStructCommand(CommandSetDevelopColorMode, ref developColorMode);
                EnsureSuccess(code, "Der Nikon-Entwicklungsmodus konnte nicht gesetzt werden.");
            }

            private uint OpenSession(string sourcePath)
            {
                IntPtr pathPtr = IntPtr.Zero;

                try
                {
                    byte[] utf8Path = Encoding.UTF8.GetBytes(sourcePath + '\0');
                    pathPtr = Marshal.AllocHGlobal(utf8Path.Length);
                    Marshal.Copy(utf8Path, 0, pathPtr, utf8Path.Length);

                    NkflSessionParam sessionParam = new()
                    {
                        ulSize = (uint)Marshal.SizeOf<NkflSessionParam>(),
                        ulType = SourceFileNameUtf8,
                        pFileInfo = pathPtr,
                        ulFileSize = 0,
                        bImageLoadSkip = false
                    };

                    uint code = ExecuteStructCommand(CommandOpenSession, ref sessionParam);
                    EnsureSuccess(code, $"Die NEF-Datei konnte nicht geöffnet werden: {Path.GetFileName(sourcePath)}");
                    return sessionParam.ulSessionID;
                }
                finally
                {
                    if (pathPtr != IntPtr.Zero)
                        Marshal.FreeHGlobal(pathPtr);
                }
            }

            private void CloseSession(uint sessionId)
            {
                if (sessionId == 0)
                    return;

                NkflSessionParam sessionParam = new()
                {
                    ulSize = (uint)Marshal.SizeOf<NkflSessionParam>(),
                    ulSessionID = sessionId
                };

                uint code = ExecuteStructCommand(CommandCloseSession, ref sessionParam);
                EnsureSuccess(code, "Die Nikon-Session konnte nicht sauber geschlossen werden.");
            }

            private NkflImageInfoParam GetImageInfo(uint sessionId)
            {
                NkflImageInfoParam imageInfo = new()
                {
                    ulSize = (uint)Marshal.SizeOf<NkflImageInfoParam>(),
                    ulSessionID = sessionId
                };

                uint code = ExecuteStructCommand(CommandGetImageInfo, ref imageInfo);
                EnsureSuccess(code, "Die Bildinformationen der NEF-Datei konnten nicht gelesen werden.");
                return imageInfo;
            }

            private byte[] GetImageData(uint sessionId, NkflImageInfoParam imageInfo)
            {
                int width = checked((int)imageInfo.ulWidth);
                int height = checked((int)imageInfo.ulHeight);
                int byteDepth = checked((int)imageInfo.ulByteDepth);
                int stride = checked(width * 3 * byteDepth);
                byte[] buffer = new byte[checked(height * stride)];

                GCHandle pinnedBuffer = GCHandle.Alloc(buffer, GCHandleType.Pinned);

                try
                {
                    NkflImageParam imageParam = new()
                    {
                        ulSize = (uint)Marshal.SizeOf<NkflImageParam>(),
                        ulSessionID = sessionId,
                        ulImageID = 0,
                        rectArea = new NativeRect { left = 0, top = 0, right = width, bottom = height },
                        ulDataSize = (uint)buffer.Length,
                        pData = pinnedBuffer.AddrOfPinnedObject(),
                        pFunc = IntPtr.Zero,
                        pProgressParam = IntPtr.Zero
                    };

                    uint code = ExecuteStructCommand(CommandGetImageData, ref imageParam);
                    EnsureSuccess(code, "Die NEF-Bilddaten konnten nicht entwickelt werden.");
                    return buffer;
                }
                finally
                {
                    pinnedBuffer.Free();
                }
            }

            private uint ExecuteCommand(uint command, IntPtr parameter)
            {
                return _entry(command, parameter);
            }

            private uint ExecuteStructCommand<T>(uint command, ref T value)
                where T : struct
            {
                IntPtr buffer = Marshal.AllocHGlobal(Marshal.SizeOf<T>());

                try
                {
                    Marshal.StructureToPtr(value, buffer, false);
                    uint code = ExecuteCommand(command, buffer);
                    value = Marshal.PtrToStructure<T>(buffer);
                    return code;
                }
                finally
                {
                    Marshal.FreeHGlobal(buffer);
                }
            }

            private static void EnsureSuccess(uint code, string message)
            {
                if (code == NikonCodeNone || IsWarning(code))
                    return;

                throw new InvalidOperationException($"{message} Nikon-Code: 0x{code:X4}");
            }

            private static bool IsWarning(uint code)
            {
                return (code & 0x0F00) != 0;
            }

            private static byte[] CreateFixedUtf8Buffer(string path)
            {
                byte[] utf8 = Encoding.UTF8.GetBytes(path + '\0');
                if (utf8.Length > MaxPathLength)
                    throw new InvalidOperationException($"Pfad ist für das Nikon SDK zu lang: {path}");

                byte[] buffer = new byte[MaxPathLength];
                Array.Copy(utf8, buffer, utf8.Length);
                return buffer;
            }

            private static unsafe Bitmap CreateBitmap(NkflImageInfoParam imageInfo, byte[] rawBuffer)
            {
                int width = checked((int)imageInfo.ulWidth);
                int height = checked((int)imageInfo.ulHeight);
                int byteDepth = checked((int)imageInfo.ulByteDepth);
                
                var format = global::Avalonia.Platform.PixelFormat.Rgba8888;
                int stride = width * 4;
                byte[] convertedBuffer = new byte[height * stride];

                if (byteDepth == 2) // 48bpp -> 32bpp RGBA
                {
                    for (int i = 0; i < width * height; i++)
                    {
                        convertedBuffer[i * 4 + 0] = rawBuffer[i * 6 + 1]; // R (high byte)
                        convertedBuffer[i * 4 + 1] = rawBuffer[i * 6 + 3]; // G (high byte)
                        convertedBuffer[i * 4 + 2] = rawBuffer[i * 6 + 5]; // B (high byte)
                        convertedBuffer[i * 4 + 3] = 255;                  // A
                    }
                }
                else // 24bpp -> 32bpp RGBA
                {
                    for (int i = 0; i < width * height; i++)
                    {
                        convertedBuffer[i * 4 + 0] = rawBuffer[i * 3 + 0]; // R
                        convertedBuffer[i * 4 + 1] = rawBuffer[i * 3 + 1]; // G
                        convertedBuffer[i * 4 + 2] = rawBuffer[i * 3 + 2]; // B
                        convertedBuffer[i * 4 + 3] = 255;                  // A
                    }
                }
                
                var dpi = new Vector(72, 72);
                var bitmap = new WriteableBitmap(new PixelSize(width, height), dpi, format, AlphaFormat.Unpremul);
                
                using (var lockedBitmap = bitmap.Lock())
                {
                    Marshal.Copy(convertedBuffer, 0, lockedBitmap.Address, convertedBuffer.Length);
                }

                return bitmap;
            }

            private static Bitmap CreateProxyBitmap(Bitmap masterBitmap)
            {
                // Already in a usable format from CreateBitmap
                return masterBitmap;
            }

            private static void SaveMaster(Bitmap masterBitmap, string masterPath)
            {
                masterBitmap.Save(masterPath);
            }

            private static void SaveProxy(Bitmap proxyBitmap, string proxyPath, NefImportSettings settings)
            {
                if (settings.ProxyFormat == RawImportProxyFormat.Png)
                {
                    proxyBitmap.Save(proxyPath);
                }
                else
                {
                    using var fileStream = new FileStream(proxyPath, FileMode.Create, FileAccess.Write);
                    proxyBitmap.Save(fileStream, Math.Clamp(settings.JpegQuality, 1, 100));
                }
            }

            [DllImport("kernel32", CharSet = CharSet.Ansi, SetLastError = true)]
            private static extern IntPtr LoadLibraryEx(string lpFileName, IntPtr hFile, int dwFlags);

            [DllImport("kernel32", CharSet = CharSet.Ansi, SetLastError = true)]
            private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

            [DllImport("kernel32", SetLastError = true)]
            [return: MarshalAs(UnmanagedType.Bool)]
            private static extern bool FreeLibrary(IntPtr hModule);

            [UnmanagedFunctionPointer(CallingConvention.Winapi)]
            private delegate uint NkflEntryDelegate(uint command, IntPtr parameter);

            [StructLayout(LayoutKind.Sequential)]
            private struct NkflLibraryParam
            {
                public uint ulSize;
                public uint ulVersion;
                public uint ulVMMemorySize;
                public IntPtr pNkflPtr;

                [MarshalAs(UnmanagedType.ByValArray, SizeConst = MaxPathLength)]
                public byte[] VMFileInfo;

                [MarshalAs(UnmanagedType.ByValArray, SizeConst = MaxPathLength)]
                public byte[] DefProfPath;
            }

            [StructLayout(LayoutKind.Sequential)]
            private struct NkflSessionParam
            {
                public uint ulSize;
                public uint ulSessionID;
                public uint ulType;
                public IntPtr pFileInfo;
                public uint ulFileSize;

                [MarshalAs(UnmanagedType.I1)]
                public bool bImageLoadSkip;
            }

            [StructLayout(LayoutKind.Sequential)]
            private struct NkflImageInfoParam
            {
                public uint ulSize;
                public uint ulSessionID;
                public uint ulImageID;
                public uint ulWidth;
                public uint ulHeight;
                public uint ulByteDepth;
                public uint ulColor;
                public uint ulOrientation;
                public double dbResolution;
            }

            [StructLayout(LayoutKind.Sequential)]
            private struct NkflImageParam
            {
                public uint ulSize;
                public uint ulSessionID;
                public uint ulImageID;
                public NativeRect rectArea;
                public uint ulDataSize;
                public IntPtr pData;
                public IntPtr pFunc;
                public IntPtr pProgressParam;
            }

            [StructLayout(LayoutKind.Sequential)]
            private struct NkflDevelopColorMode
            {
                public uint ulSize;
                public int lDevelopColorMode;
            }

            [StructLayout(LayoutKind.Sequential)]
            private struct NativeRect
            {
                public int left;
                public int top;
                public int right;
                public int bottom;
            }
        }
    }
}
