#pragma once

#include "Region.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class SceneTextRegionExtractor
	{
	public:
		IList<Region^>^ GetRegions(array<unsigned char>^ buffer, Stream^ ocrImgStream, Stream^ regionStream);
		IList<MemoryStream^>^ SimpleWatermark(array<unsigned char>^ buffer);

	private:
		Mat DetectAndRotate(Mat img, Scalar lower, Scalar upper);
		void WriteToStream(const std::string& extension, Mat image, Stream^ outStream);
		Mat ReadImage(array<unsigned char>^ buffer);
	};
}