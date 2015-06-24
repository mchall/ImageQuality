#pragma once

#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class SceneTextRegionExtractor
	{
	public:
		IList<double>^ GetRegions(array<unsigned char>^ buffer);
	};
}