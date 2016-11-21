#include "stdafx.h"

#include "ClApp.h"
#include "Timer.hpp"

void convolution(const uchar* in, uchar* out, const double* kernel, const int& mat_m, const int& mat_n, const int& kernel_n)
{
	auto kernel_half = kernel_n / 2;
	for (auto mat_i = kernel_half; mat_i < mat_m - kernel_half; ++mat_i) {
		for (auto mat_j = kernel_half; mat_j < mat_n - kernel_half; ++mat_j) {
			auto pixel = 0.0;
			auto c = 0;

			for (auto delta_i = -kernel_half; delta_i <= kernel_half; ++delta_i) {
				for (auto delta_j = -kernel_half; delta_j <= kernel_half; ++delta_j) {
					pixel += in[(mat_j - delta_j) * mat_m + mat_i - delta_i] * kernel[c++];
				}
			}

			out[mat_j*mat_m + mat_i] = (char)pixel;
		}
	}
}

void gaussian_filter(const uchar* in, uchar* out, const int& mat_m, const int& mat_n, const float& sigma)
{
	const auto kernel_n = static_cast<int>(std::ceil(4 * sigma) + 3);
	const auto mean = std::floor(kernel_n / 2.0);
	auto kernel = new double[kernel_n * kernel_n];

	auto c = 0;
	for (auto i = 0; i < kernel_n; i++)
		for (auto j = 0; j < kernel_n; j++) {
			kernel[c++] = std::exp(-0.5 * (std::pow((i - mean) / sigma, 2.0) +
				std::pow((j - mean) / sigma, 2.0f)))
				/ (2 * M_PI * sigma * sigma);
		}

	convolution(in, out, kernel, mat_m, mat_n, kernel_n);

	delete kernel;
}

void edges(const uchar* in, uchar* out, const int& mat_m, const int& mat_n, const int& tmin, const int& tmax, const float& sigma)
{
	gaussian_filter(in, out, mat_m, mat_n, sigma);

	const double Gx[] = {
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
	};

	auto gx_out = new uchar[mat_m*mat_n];
	convolution(out, gx_out, Gx, mat_m, mat_n, KERNEL_N);

	const double Gy[] = {
		1, 2, 1,
		0, 0, 0,
	   -1,-2,-1
	};

	auto gy_out = new uchar[mat_m*mat_n];
	convolution(out, gy_out, Gy, mat_m, mat_n, KERNEL_N);

	auto G = new uchar[mat_m*mat_n];

	for (auto i = 1; i < mat_m - 1; i++)
	{
		for (auto j = 1; j < mat_n - 1; j++)
		{
			const auto c = i + mat_m * j;
			G[c] = static_cast<char>(std::hypot(gx_out[c], gy_out[c]));
		}
	}

	auto nms = new uchar[mat_m*mat_n];

	for (auto i = 1; i < mat_m - 1; i++)
		for (auto j = 1; j < mat_n - 1; j++) {
			const auto c = i + mat_m * j;
			const auto nn = c - mat_m;
			const auto ss = c + mat_m;
			const auto ww = c + 1;
			const auto ee = c - 1;
			const auto nw = nn + 1;
			const auto ne = nn - 1;
			const auto sw = ss + 1;
			const auto se = ss - 1;

			const auto dir = (float)(std::fmod(std::atan2(gy_out[c],
				gx_out[c]) + M_PI,
				M_PI) / M_PI) * 8;

			if (((dir <= 1 || dir > 7) && G[c] > G[ee] &&
				G[c] > G[ww]) || // 0 deg
				((dir > 1 && dir <= 3) && G[c] > G[nw] &&
					G[c] > G[se]) || // 45 deg
					((dir > 3 && dir <= 5) && G[c] > G[nn] &&
						G[c] > G[ss]) || // 90 deg
						((dir > 5 && dir <= 7) && G[c] > G[ne] &&
							G[c] > G[sw]))   // 135 deg
				nms[c] = G[c];
			else
				nms[c] = 0;
		}

	// Reuse array
	// used as a stack. mat_m*mat_n/2 elements should be enough.
	auto edges = (int*)gy_out;
	memset(out, 0, sizeof(uchar) * mat_m * mat_n);
	memset(edges, 0, sizeof(uchar) * mat_m * mat_n);

	// Tracing edges with hysteresis . Non-recursive implementation.
	auto c = 1;
	for (auto j = 1; j < mat_n - 1; j++)
		for (auto i = 1; i < mat_m - 1; i++) {
			if (nms[c] >= tmax && out[c] == 0) { // trace edges
				out[c] = MAX_BRIGHTNESS;
				auto nedges = 1;
				edges[0] = c;

				do {
					nedges--;
					const auto t = edges[nedges];

					int nbs[8]; // neighbours
					nbs[0] = t - mat_m;     // nn
					nbs[1] = t + mat_m;     // ss
					nbs[2] = t + 1;      // ww
					nbs[3] = t - 1;      // ee
					nbs[4] = nbs[0] + 1; // nw
					nbs[5] = nbs[0] - 1; // ne
					nbs[6] = nbs[1] + 1; // sw
					nbs[7] = nbs[1] - 1; // se

					for (auto k = 0; k < 8; k++)
						if (nms[nbs[k]] >= tmin && out[nbs[k]] == 0) {
							out[nbs[k]] = MAX_BRIGHTNESS;
							edges[nedges] = nbs[k];
							nedges++;
						}
				} while (nedges > 0);
			}
			c++;
		}

	delete gx_out;
	delete gy_out;
	delete G;
	delete nms;
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
	auto in = new uchar[pixel_num];
	for (auto i = 0; i < width; ++i) {
		for (auto j = 0; j < height; ++j) {
			auto pixel = src.at<cv::Vec3b>(j, i);
			auto color = (unsigned char)(((int)pixel.val[0] + (int)pixel.val[1] + (int)pixel.val[2]) / 3);
			in[width*j + i] = color;
		}
	}

	auto out = new uchar[pixel_num];
#pragma endregion


#pragma region CPU
#ifndef USE_GPU
	{
		Timer<> t;
		edges(in, out, width, height, tmin, tmax, sigma);
		std::cout << "CPU: " << t << std::endl;
	}
#endif
#pragma endregion

#pragma region OpenCL
#ifdef USE_GPU
	{
		ClApp ocl_app;
		auto ok = ocl_app.Init(in, out, width, height, sigma);
		if (ok)
		{
			Timer<> t;
			ocl_app.Run();
			std::cout << "GPU: " << t << std::endl;
		}
	}
#endif
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
	delete in;
	delete out;

	return 0;
}

