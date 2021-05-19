#include "perf_precomp.hpp"

PERF_TEST(OpenCV, Gen)
{
	declare.time(60);
	HMat Image;
	HRegion Region, RegionRectangle;
	string path = "D:/tzq/AVI Project/A/test_pic/Gray.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	TEST_CYCLE_N(10)
	{
		//threshold(Image, Region, 0, 160);
		gen_rectangle1(RegionRectangle, 100, 100, 1000, 1000);
	}
	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST(Halcon, Gen)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image;
	Hobject Region, RegionErosion;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/Gray.bmp";
	read_image(&Image, path);
	TEST_CYCLE_N(10)
	{
		threshold(Image, &Region, 0, 160);
		//gen_rectangle1(&RegionRectangle, 100, 100, 1000, 1000);
	}
	SANITY_CHECK(0);
	reportMetrics(false);
}