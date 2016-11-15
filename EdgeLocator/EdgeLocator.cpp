// EdgeLocator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
//#include <opencv2\imgcodecs.hpp>

#include "Timer.hpp"

const auto window_name = "Edge Locator GUI";

cv::Mat src, dst, detected_edges;

auto const max_lowThreshold = 100;
auto lowThreshold = 0;
auto ratio = 3;
auto kernel_size = 3;

void CannyThreshold(int, void*)
{
	Timer<> t;
	/// Reduce noise with a kernel 3x3
	cv::blur(src, detected_edges, cv::Size(3, 3));

	/// Canny detector
	cv::Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
	std::cout << t << std::endl;

	/// Using Canny's output as a mask, we display our result
	dst = cv::Scalar::all(0);

	src.copyTo(dst, detected_edges);
	cv::imshow(window_name, dst);

}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		for (int i = 0; i < argc; ++i) {
			std::cout << argv[i] << std::endl;
		}
		std::cerr << "Usage: edge_locator image_to_process " << argc << " " << argv[1] << std::endl;
		return -1;
	}
	src = cv::imread(argv[1], cv::ImreadModes::IMREAD_GRAYSCALE);
	dst.create(src.size(), src.type());

	if (!src.data)
	{
		std::cerr << "Could not open or find the image" << std::endl;
		return -1;
	}

	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	cv::createTrackbar("Min Treshold", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

	CannyThreshold(0, 0);

	cvWaitKey(0);

    return 0;
}

