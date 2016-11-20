__kernel void convolution(
	__global const char* in,
	__global char* out,
	__global const int& m,
	__global const int& n,
	__global const char* kernel_conv,
	__global const int& kernel_n)
{
	int kernel_half = kernel_n / 2;
	int i = get_global_id(0);
	int act_row = i/m;
	int act_col = i - (act_row*m);

	if (act_row - kernel_half >= 0
	 && act_row + kernel_half < n
	 && )
}