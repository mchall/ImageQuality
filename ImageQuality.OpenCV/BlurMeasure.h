#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class BlurMeasure
	{
	public:
		double Nayar89(array<uchar>^ buffer);
		double Pech2000(array<uchar>^ buffer);

	private:
		Mat ReadImage(array<uchar>^ buffer);
	};
}