using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace ImageQuality
{
    public class ColorHelper
    {
        private ColorMeasure _measure;

        public ColorHelper()
        {
            _measure = new ColorMeasure();
        }

        public RGB ColorInfo(byte[] fileBytes)
        {
            var info = _measure.GetColorInfo(fileBytes);
            return new RGB()
            {
                R = info.R,
                G = info.G,
                B = info.B,
            };
        }
    }

    public class RGB
    {
        public double R { get; set; }
        public double G { get; set; }
        public double B { get; set; }
    }
}