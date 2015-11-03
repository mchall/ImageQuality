using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace ImageQuality
{
    public class HistogramHelper
    {
        private Histogram _histogram;

        public HistogramHelper()
        {
            _histogram = new Histogram();
        }

        public double CompareHistograms(byte[] left, byte[] right)
        {
            return _histogram.Compare(left, right);
        }
    }
}