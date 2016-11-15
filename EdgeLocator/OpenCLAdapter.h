#pragma once
//#define __NO_STD_VECTOR
//#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl2.hpp>


class OpenCLAdapter
{
public:
	OpenCLAdapter();
	~OpenCLAdapter();

	bool Init();
};

