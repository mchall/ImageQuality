// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <cmath>

#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>

// OpenCV Library
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/highgui/highgui.hpp>

// SFMT: Simple Fast Mersenne Twister
#define HAVE_SSE2
#define SFMT_MEXP 19937
#define DSFMT_MEXP 19937
