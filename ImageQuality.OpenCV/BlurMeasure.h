#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class BlurMeasure
	{
	public:
		double BlurTest(cli::array<byte>^ buffer);

	private:
		Mat ReadImage(cli::array<byte>^ buffer);
	};
}