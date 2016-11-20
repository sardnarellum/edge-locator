#include "stdafx.h"

#include "ClApp.h"
#include "Timer.hpp"

void convolution(const char* in, char* out, const double* kernel, const int& mat_m, const int& mat_n, const int& kernel_n)
{
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

void gaussian_filter(const char* in, char* out, const int& mat_m, const int& mat_n, const float& sigma)
{
	const auto kernel_n = static_cast<int>(std::ceil(4 * sigma) + 3);
	const auto mean = std::floor(kernel_n / 2.0);
	auto kernel = new double[kernel_n * kernel_n];

	size_t c = 0;
	for (int i = 0; i < kernel_n; i++)
		for (int j = 0; j < kernel_n; j++) {
			kernel[c++] = std::exp(-0.5 * (std::pow((i - mean) / sigma, 2.0) +
				std::pow((j - mean) / sigma, 2.0f)))
				/ (2 * M_PI * sigma * sigma);
		}

	convolution(in, out, kernel, mat_m, mat_n, kernel_n);

	delete kernel;
}

void edges(const char* in, char* out, const int& mat_m, const int& mat_n, const float& sigma)
{
	gaussian_filter(in, out, mat_m, mat_n, sigma);

	const double Gx[] = {
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
	};

	auto gx_out = new char[mat_m*mat_n];
	convolution(out, gx_out, Gx, mat_m, mat_n, KERNEL_N);

	const double Gy[] = {
		1, 2, 1,
		0, 0, 0,
	   -1,-2,-1
	};

	auto gy_out = new char[mat_m*mat_n];
	convolution(out, gy_out, Gy, mat_m, mat_n, KERNEL_N);

	auto G = new char[mat_m*mat_n];

	for (int i = 1; i < mat_m - 1; i++)
	{
		for (int j = 1; j < mat_n - 1; j++)
		{
			const int c = i + mat_m * j;
			G[c] = static_cast<char>(std::hypot(gx_out[c], gy_out[c]));
		}
	}
	out = G;
}

int main(int argc, char** argv)
{

#pragma region Check params
	if (argc != 2)
	{
		for (int i = 0; i < argc; ++i) {
			std::cout << argv[i] << std::endl;
		}
		std::cerr << "Usage: edge_locator image_to_process " << argc << " " << argv[1] << std::endl;
		return -1;
	}
#pragma endregion

#pragma region Variables and Init

	const auto window_name = "Edge Locator GUI";
	cv::Mat src = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
	cv::Mat dst;
	dst.create(src.size(), src.type());

	if (!src.data)
	{
		std::cerr << "Could not open or find the image" << std::endl;
		return -1;
	}

	auto width = src.cols;
	auto height = src.rows;
	auto pixel_num = width * height;
	auto in = new char[pixel_num];
	for (auto i = 0; i < width; ++i) {
		for (auto j = 0; j < height; ++j) {
			auto pixel = src.at<cv::Vec3b>(j, i);
			auto color = (unsigned char)(((int)pixel.val[0] + (int)pixel.val[1] + (int)pixel.val[2]) / 3);
			in[width*j + i] = color;
		}
	}

	auto out = new char[pixel_num];
#pragma endregion

#pragma region CPU

	Timer<> t;
	edges(in, out, width, height, sigma);
	std::cout << "CPU: " << t << std::endl;

#pragma endregion

#pragma region OpenCL

	ClApp ocl_app;
	ocl_app.Init(in, out, width, height, sigma);
	ocl_app.Run();

#pragma endregion

#pragma region display results

	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	cv::Mat resImg(height, width, CV_8UC3);

	for (auto i = 0; i < width; ++i) {
		for (auto j = 0; j < height; ++j) {
			cv::Vec3b pixel;
			pixel.val[0] = pixel.val[1] = pixel.val[2] = out[width*j + i];
			resImg.at<decltype(pixel)>(j, i) = pixel;
		}
	}

	cv::imshow(window_name, resImg);
	cvWaitKey(0);

#pragma endregion

	return 0;
}

