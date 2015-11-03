#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class Histogram
	{
	public:
		double Compare(array<byte>^ left, array<byte>^ right);

	private:
		Mat ReadImage(array<byte>^ buffer);
	};
}