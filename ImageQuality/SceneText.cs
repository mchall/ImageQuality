using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ImageQuality
{
    public class SceneText
    {
        private SceneTextRegionExtractor _extractor;

        public SceneText()
        {
            _extractor = new SceneTextRegionExtractor();
        }

        public void DetectRegions(byte[] fileBytes)
        {
            _extractor.GetRegions(fileBytes);
        }
    }
}