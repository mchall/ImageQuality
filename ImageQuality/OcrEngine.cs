using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Text;
using Tesseract;
using System.Linq;

namespace ImageQuality
{
    internal static class OcrEngine
    {
        private static TesseractEngine _ocr;
        public static TesseractEngine Instance
        {
            get
            {
                if (_ocr == null)
                {
                    _ocr = new TesseractEngine("./tessdata", "eng", EngineMode.Default);
                }
                return _ocr;
            }
        }
    }
}