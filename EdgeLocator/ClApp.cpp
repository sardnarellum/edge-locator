#include "stdafx.h"
#include "ClApp.h"
#include <vector>
#include <fstream>


void ClApp::Init(const char* in, char* out, const int& m, const int& n, const float& sigma)
{
	pixel_num = m*n;
	result = out;

	try {
		// Get available platforms
		vector<Platform> platforms;
		Platform::get(&platforms);

		vector<Device> devices;

		for (auto p : platforms) {

			try {
				std::cout << p.getInfo<CL_PLATFORM_NAME>() << std::endl;
				std::cout << p.getInfo<CL_PLATFORM_VERSION>() << std::endl;

				// Select the default platform and create a context using this platform and the GPU
				// These are key-value pairs.
				cl_context_properties cps[3] = {
					CL_CONTEXT_PLATFORM,
					(cl_context_properties)(p)(),
					0
				};

				context = Context(CL_DEVICE_TYPE_GPU, cps);

				// Get a list of devices on this platform
				devices = context.getInfo<CL_CONTEXT_DEVICES>();

			}
			catch (Error error) {
				oclPrintError(error);
				continue;
			}

		}

		if (devices.size() == 0) {
			throw Error(CL_INVALID_CONTEXT, "Failed to create a valid context!");
		}

		for (const auto& device : devices) {
			std::cout << device.getInfo<CL_DEVICE_NAME>() << std::endl;
			std::cout << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
		}

		// Create a command queue and use the first device
		queue = CommandQueue(context, devices[0]);

		// Read source file
		std::ifstream sourceFile("convolution.cl");
		std::string sourceCode(
			std::istreambuf_iterator<char>(sourceFile),
			(std::istreambuf_iterator<char>()));
		Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));

		// Make program of the source code in the context
		program = Program(context, source);

		// Build program for these specific devices
		program.build(devices);

		// Make kernel
		kernel_conv = Kernel(program, "convolution");

		buffer1 = Buffer(context, CL_MEM_READ_ONLY, (pixel_num) * sizeof(char));
		buffer2 = Buffer(context, CL_MEM_WRITE_ONLY, (pixel_num) * sizeof(char));
		queue.enqueueWriteBuffer(buffer1, CL_TRUE, 0, pixel_num * sizeof(char), in);
		queue.finish();

	}
	catch (Error error) {
		oclPrintError(error);
	}
}

void ClApp::Run()
{
	NDRange _global_(pixel_num);
	queue.enqueueNDRangeKernel(kernel_conv, cl::NullRange, _global_, cl::NullRange);
	queue.enqueueReadBuffer(buffer2, CL_TRUE, 0, pixel_num * sizeof(char), result);
}


#pragma region Error Utils

const char* ClApp::oclErrorString(cl_int error)
{
	static const char* errorString[] = {
		"CL_SUCCESS",
		"CL_DEVICE_NOT_FOUND",
		"CL_DEVICE_NOT_AVAILABLE",
		"CL_COMPILER_NOT_AVAILABLE",
		"CL_MEM_OBJECT_ALLOCATION_FAILURE",
		"CL_OUT_OF_RESOURCES",
		"CL_OUT_OF_HOST_MEMORY",
		"CL_PROFILING_INFO_NOT_AVAILABLE",
		"CL_MEM_COPY_OVERLAP",
		"CL_IMAGE_FORMAT_MISMATCH",
		"CL_IMAGE_FORMAT_NOT_SUPPORTED",
		"CL_BUILD_PROGRAM_FAILURE",
		"CL_MAP_FAILURE",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"CL_INVALID_VALUE",
		"CL_INVALID_DEVICE_TYPE",
		"CL_INVALID_PLATFORM",
		"CL_INVALID_DEVICE",
		"CL_INVALID_CONTEXT",
		"CL_INVALID_QUEUE_PROPERTIES",
		"CL_INVALID_COMMAND_QUEUE",
		"CL_INVALID_HOST_PTR",
		"CL_INVALID_MEM_OBJECT",
		"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
		"CL_INVALID_IMAGE_SIZE",
		"CL_INVALID_SAMPLER",
		"CL_INVALID_BINARY",
		"CL_INVALID_BUILD_OPTIONS",
		"CL_INVALID_PROGRAM",
		"CL_INVALID_PROGRAM_EXECUTABLE",
		"CL_INVALID_KERNEL_NAME",
		"CL_INVALID_KERNEL_DEFINITION",
		"CL_INVALID_KERNEL",
		"CL_INVALID_ARG_INDEX",
		"CL_INVALID_ARG_VALUE",
		"CL_INVALID_ARG_SIZE",
		"CL_INVALID_KERNEL_ARGS",
		"CL_INVALID_WORK_DIMENSION",
		"CL_INVALID_WORK_GROUP_SIZE",
		"CL_INVALID_WORK_ITEM_SIZE",
		"CL_INVALID_GLOBAL_OFFSET",
		"CL_INVALID_EVENT_WAIT_LIST",
		"CL_INVALID_EVENT",
		"CL_INVALID_OPERATION",
		"CL_INVALID_GL_OBJECT",
		"CL_INVALID_BUFFER_SIZE",
		"CL_INVALID_MIP_LEVEL",
		"CL_INVALID_GLOBAL_WORK_SIZE",
	};

	const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

	const int index = -error;

	return (index >= 0 && index < errorCount) ? errorString[index] : "Unspecified Error";
}

void ClApp::oclPrintError(const cl::Error &error)
{
	std::cout << error.what() << "(" << error.err() << " == " << oclErrorString(error.err()) << ")" << std::endl;
}

#pragma endregion
