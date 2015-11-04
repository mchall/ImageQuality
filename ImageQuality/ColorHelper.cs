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

        public ColorInfoResult ColorInfo(byte[] fileBytes)
        {
            var info = _measure.GetColorInfo(fileBytes);
            return new ColorInfoResult()
            {
                Red = info.Red,
                Green = info.Green,
                Blue = info.Blue,
                Yellow = info.Yellow,
            };
        }
    }

    public class ColorInfoResult
    {
        public double Red { get; set; }
        public double Green { get; set; }
        public double Blue { get; set; }
        public double Yellow { get; set; }
    }
}