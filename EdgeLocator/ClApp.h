#pragma once

#define __NO_STD_VECTOR
#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <utility>

using namespace cl;

class ClApp
{
public:
	ClApp() {}
	~ClApp() {}

	void Init(const char* in, char* out, const int& m, const int& n, const float& sigma);
	void Run();

private:
	const char* oclErrorString(cl_int error);
	void oclPrintError(const cl::Error &error);
	int pixel_num;
	char* result;

	Context context;
	CommandQueue queue;
	Program program;

	Kernel kernel_conv;
	Buffer buffer1;
	Buffer buffer2;
};

