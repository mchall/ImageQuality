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

	private:
		void WriteToStream(const std::string& extension, Mat image, Stream^ outStream);
	};
}