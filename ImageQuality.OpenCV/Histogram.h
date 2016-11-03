#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace cv;

namespace ImageQuality {

	public ref class Histogram
	{
	public:
		double Compare(cli::array<byte>^ left, cli::array<byte>^ right);

	private:
		Mat ReadImage(cli::array<byte>^ buffer);
	};
}