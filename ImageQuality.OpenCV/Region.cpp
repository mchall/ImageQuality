#include "Stdafx.h"
#include "Region.h"

using namespace std;
using namespace cv;

namespace ImageQuality
{
	Region::Region(int x, int y, int width, int height, array<byte>^ tiff)
	{
		_x = x;
		_y = y;
		_width = width;
		_height = height;
		_tiff = tiff;
	}

	int Region::X::get()
	{
		return _x;
	}

	int Region::Y::get()
	{
		return _y;
	}

	int Region::Width::get()
	{
		return _width;
	}

	int Region::Height::get()
	{
		return _height;
	}

	array<byte>^ Region::Tiff::get()
	{
		return _tiff;
	}
}