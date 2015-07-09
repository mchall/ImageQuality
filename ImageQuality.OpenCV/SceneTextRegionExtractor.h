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
		IList<Region^>^ GetRegions(array<byte>^ buffer, Stream^ regionStream);
		IList<array<byte>^>^ SimpleWatermark(array<byte>^ buffer);

	private:
		Mat DetectAndRotate(Mat img, Scalar lower, Scalar upper);
		array<byte>^ ToByteArray(Mat image, const string& extension);
		void WriteToStream(const std::string& extension, Mat image, Stream^ stream);
		Mat ReadImage(array<byte>^ buffer);
	};
}