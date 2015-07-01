#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class BlurMeasure
	{
	public:
		double Nayar89(array<byte>^ buffer);
		double Pech2000(array<byte>^ buffer);

	private:
		Mat ReadImage(array<byte>^ buffer);
	};
}