#include "Stdafx.h"
#include "ColorInfo.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	ColorInfo::ColorInfo(double red, double green, double blue, double yellow)
	{
		_red = red;
		_green = green;
		_blue = blue;
		_yellow = yellow;
	}

	double ColorInfo::Red::get()
	{
		return _red;
	}

	double ColorInfo::Green::get()
	{
		return _green;
	}

	double ColorInfo::Blue::get()
	{
		return _blue;
	}

	double ColorInfo::Yellow::get()
	{
		return _yellow;
	}
}