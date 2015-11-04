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
		double _red;
		double _green;
		double _blue;
		double _yellow;

	internal:
		ColorInfo(double red, double green, double blue, double yellow);

	public:
		property double Red { double get(); }
		property double Green { double get(); }
		property double Blue { double get(); }
		property double Yellow { double get(); }
	};
}