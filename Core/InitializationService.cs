using Avalonia.Controls;
using Avalonia.Threading;
using OpenStopMotionStudio.Core.Startup;
using OpenStopMotionStudio.GUI;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.Core
{
    public class InitializationService
    {
        private readonly List<IStartupTask> _tasks = new();

        public InitializationService()
        {
            _tasks.Add(new DeviceEnumerationTask());
        }

        public async Task<bool> RunAsync(Action<string> reportStatus, Window owner)
        {
            try
            {
                await Task.Run(async () =>
                {
                    foreach (var task in _tasks)
                    {
                        await Dispatcher.UIThread.InvokeAsync(() => reportStatus(task.Description));
                        await task.ExecuteAsync(reportStatus);
                    }
                });

                await Dispatcher.UIThread.InvokeAsync(() => reportStatus("Initialization complete."));
                return true;
            }
            catch (Exception ex)
            {
                DebugLogger.Instance.LogError("Startup", $"Application initialization failed: {ex.Message}");
                await Dispatcher.UIThread.InvokeAsync(async () =>
                {
                    await MessageBox.Show(owner, "Fatal Startup Error", @$"Application initialization failed and the program must close.

Error: {ex.Message}");
                });
                return false;
            }
        }
    }
}
