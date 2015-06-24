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

	public ref class Region
	{
	private:
		int _x;
		int _y;
		int _width;
		int _height;

	internal:
		Region(int x, int y, int width, int height);

	public:
		property int X { int get(); }
		property int Y { int get(); }

		property int Width { int get(); }
		property int Height { int get(); }
	};
}