using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Tesseract;

namespace ImageQuality
{
    public class SceneText
    {
        private SceneTextRegionExtractor _extractor;

        public SceneText()
        {
            _extractor = new SceneTextRegionExtractor();
        }

        public string DetectRegions(byte[] fileBytes)
        {
            StringBuilder sb = new StringBuilder();

            var regions = _extractor.GetRegions(fileBytes);
            foreach (var region in OrderRegions(regions))
            {
                using (var pix = Pix.LoadTiffFromMemory(region.Tiff))
                {
                    PageSegMode mode = PageSegMode.SingleLine;
                    using (var page = OcrEngine.Instance.Process(pix, mode))
                    {
                        if (page.GetMeanConfidence() > OcrEngine.MinConfidence)
                        {
                            var text = page.GetText().Trim();
                            if (!String.IsNullOrEmpty(text))
                            {
                                sb.AppendLine(text);
                            }
                        }
                    }
                }
            }

            return sb.ToString();
        }

        private List<Region> OrderRegions(IList<Region> regions)
        {
            var sort = regions.ToList();
            sort.Sort((l, r) =>
                {
                    if (l.Y == r.Y)
                        return l.X.CompareTo(r.X);
                    return l.Y.CompareTo(r.Y);
                });
            return sort;
        }
    }
}