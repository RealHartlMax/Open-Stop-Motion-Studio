using System;
using System.Threading.Tasks;

namespace OpenStopMotionStudio.Core
{
    public interface IStartupTask
    {
        string Description { get; }
        Task ExecuteAsync(Action<string> reportStatus);
    }
}
