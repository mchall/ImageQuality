using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace ImageQuality
{
    public class Blur
    {
        private BlurMeasure _measure;

        public Blur()
        {
            _measure = new BlurMeasure();
        }

        public double BlurMeasure(byte[] fileBytes)
        {
            return _measure.Measure(fileBytes);
        }
    }
}