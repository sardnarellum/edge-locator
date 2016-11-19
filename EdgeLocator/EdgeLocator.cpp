// EdgeLocator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream> 
#include <cmath>
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

void convolution(const char* in, char* out, const double* kernel, const int& mat_m, const int& mat_n, const int& kernel_n) {
	auto kernel_half = kernel_n / 2;
	for (int mat_i = kernel_half; mat_i < mat_m - kernel_half; ++mat_i) {
		for (int mat_j = kernel_half; mat_j < mat_n - kernel_half; ++mat_j) {
			auto pixel = 0.0;
			size_t c = 0;

			for (int delta_i = -kernel_half; delta_i <= kernel_half; ++delta_i) {
				for (int delta_j = -kernel_half; delta_j <= kernel_half; ++delta_j) {
					pixel += in[(mat_j - delta_j) * mat_m + mat_i - delta_i] * kernel[c++];
				}
			}

			out[mat_j*mat_m + mat_i] = (char)pixel;
		}
	}
}

void gaussian_filter(const char* in, char* out, const int& mat_m, const int& mat_n, const float& sigma) {

	const auto kernel_n = static_cast<int>(ceil(4 * sigma) + 3);
	const auto mean = floor(kernel_n / 2.0);
	auto kernel = new double[kernel_n * kernel_n]; // variable length array

	size_t c = 0;
	for (int i = 0; i < kernel_n; i++)
		for (int j = 0; j < kernel_n; j++) {
			kernel[c++] = exp(-0.5 * (pow((i - mean) / sigma, 2.0) +
				pow((j - mean) / sigma, 2.0f)))
				/ (2 * M_PI * sigma * sigma);
		}

	convolution(in, out, kernel, mat_m, mat_n, kernel_n);

	delete kernel;
}

void edges(const char* in, char* out, const int& mat_m, const int& mat_n, const float& sigma) {
	gaussian_filter(in, out, mat_m, mat_n, sigma);
}

void CannyThreshold(int, void*)
{
	Timer<> t;

#if !defined(MYCPU)
	/// Reduce noise with a kernel 3x3
	cv::blur(src, detected_edges, cv::Size(KERNEL_N, KERNEL_N));
	cv::Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
#else

	auto width = src.cols;
	auto height = src.rows;
	auto in = new char[width*height];
	for (auto i = 0; i < width; ++i) {
		for (auto j = 0; j < height; ++j) {
			auto pixel = src.at<cv::Vec3b>(j, i);
			auto color = (unsigned char)(((int)pixel.val[0] + (int)pixel.val[1] + (int)pixel.val[2]) / 3);
			in[width*j + i] = color;
		}
	}

	auto out = new char[width*height];

	edges(in, out, width, height, 0.4f);

	cv::Mat resImg(height, width, CV_8UC3);

	for (auto i = 0; i < width; ++i) {
		for (auto j = 0; j < height; ++j) {
			cv::Vec3b pixel;
			pixel.val[0] = pixel.val[1] = pixel.val[2] = out[width*j + i];
			resImg.at<decltype(pixel)>(j, i) = pixel;
		}
	}


#endif
	std::cout << t << std::endl;

	/// Using Canny's output as a mask, we display our result
	//dst = cv::Scalar::all(0);

	//src.copyTo(dst, detected_edges);
	cv::imshow(window_name, resImg);

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
	src = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
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

