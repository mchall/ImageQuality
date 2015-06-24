using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Text;
using Tesseract;
using System.Linq;

namespace ImageQuality
{
    public class SceneText
    {
        private SceneTextRegionExtractor _extractor;
        private TesseractEngine _ocr;

        public SceneText()
        {
            _extractor = new SceneTextRegionExtractor();
            _ocr = new TesseractEngine("./tessdata", "eng", EngineMode.Default);
        }

        public string DetectRegions(byte[] fileBytes)
        {
            StringBuilder sb = new StringBuilder();

            var regions = _extractor.GetRegions(fileBytes);
            if (regions.Count > 0)
            {
                MemoryStream ms = new MemoryStream(fileBytes);
                var bmp = (Bitmap)Bitmap.FromStream(ms);

                foreach (var region in OrderRegions(regions))
                {
                    using (var page = _ocr.Process(bmp, new Rect(region.X, region.Y, region.Width, region.Height)))
                    {
                        sb.AppendLine(page.GetText());
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