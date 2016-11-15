#include "stdafx.h"
#include "OpenCLAdapter.h"


OpenCLAdapter::OpenCLAdapter()
{
}


OpenCLAdapter::~OpenCLAdapter()
{
}

bool OpenCLAdapter::Init()
{
	//try {
	//	// Get available platforms
	//	cl:vector<cl::Platform> platforms;
	//	Platform::get(&platforms);

	//	vector<Device> devices; // !
	//	Context context; // !

	//	for (auto p : platforms) {

	//		try {
	//			std::cout << p.getInfo<CL_PLATFORM_NAME>() << std::endl;
	//			std::cout << p.getInfo<CL_PLATFORM_VERSION>() << std::endl;

	//			// Select the default platform and create a context using this platform and the GPU
	//			cl_context_properties cps[3] = {
	//				CL_CONTEXT_PLATFORM,
	//				(cl_context_properties)(p)(),
	//				0
	//			};

	//			context = Context(CL_DEVICE_TYPE_GPU, cps);

	//			// Get a list of devices on this platform
	//			devices = context.getInfo<CL_CONTEXT_DEVICES>();

	//		}
	//		catch (Error error) {
	//			oclPrintError(error);
	//			continue;
	//		}

	//		if (devices.size() > 0)
	//			break;
	//	}

	//	if (devices.size() == 0) {
	//		throw Error(CL_INVALID_CONTEXT, "Failed to create a valid context!");
	//	}

	return false;
}
