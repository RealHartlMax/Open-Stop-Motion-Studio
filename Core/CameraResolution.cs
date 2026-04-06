namespace OpenStopMotionStudio.Core
{
    /// <summary>
    /// Represents a camera resolution with aspect ratio information.
    /// </summary>
    public class CameraResolution
    {
        public int Width { get; }
        public int Height { get; }
        public string AspectRatio { get; }

        public CameraResolution(int width, int height)
        {
            Width = width;
            Height = height;
            AspectRatio = CalculateAspectRatio(width, height);
        }

        /// <summary>
        /// Calculates the aspect ratio string (e.g., "16:9", "4:3", "21:9")
        /// </summary>
        private static string CalculateAspectRatio(int width, int height)
        {
            if (width <= 0 || height <= 0)
                return "?:?";

            int gcd = GCD(width, height);
            int ratioW = width / gcd;
            int ratioH = height / gcd;
            return $"{ratioW}:{ratioH}";
        }

        /// <summary>
        /// Calculates the greatest common divisor for aspect ratio calculation.
        /// </summary>
        private static int GCD(int a, int b)
        {
            while (b != 0)
            {
                int temp = b;
                b = a % b;
                a = temp;
            }
            return a;
        }

        /// <summary>
        /// Returns display format: "1920x1080 (16:9)"
        /// </summary>
        public override string ToString()
        {
            return $"{Width}x{Height} ({AspectRatio})";
        }

        /// <summary>
        /// Equality check.
        /// </summary>
        public override bool Equals(object? obj)
        {
            if (obj is CameraResolution other)
                return Width == other.Width && Height == other.Height;
            return false;
        }

        /// <summary>
        /// Hash code for collections.
        /// </summary>
        public override int GetHashCode()
        {
            return HashCode.Combine(Width, Height);
        }
    }
}
