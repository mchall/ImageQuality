using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace ImageQuality
{
    public class Intensity
    {
        private IntensityMeasure _measure;

        public Intensity()
        {
            _measure = new IntensityMeasure();
        }

        public double IntensityMeasure(byte[] fileBytes)
        {
            return _measure.IntensityTest(fileBytes);
        }
    }
}