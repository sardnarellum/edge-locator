// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define _USE_MATH_DEFINES
#define KERNEL_N 3
#define KERNEL_N_POW (KERNEL_N * KERNEL_N)

#include <stdio.h>
#include <tchar.h>

#include <cmath>
#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2\imgcodecs.hpp>

const auto sigma = 0.4f;





// TODO: reference additional headers your program requires here
