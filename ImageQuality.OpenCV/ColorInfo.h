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

	public ref class ColorInfo
	{
	private:
		double _r;
		double _g;
		double _b;

	internal:
		ColorInfo(double r, double g, double b);

	public:
		property double R { double get(); }
		property double G { double get(); }
		property double B { double get(); }
	};
}