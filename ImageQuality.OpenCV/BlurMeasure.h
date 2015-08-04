#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class BlurMeasure
	{
	public:
		double BlurTest(array<byte>^ buffer);

	private:
		Mat ReadImage(array<byte>^ buffer);
	};
}