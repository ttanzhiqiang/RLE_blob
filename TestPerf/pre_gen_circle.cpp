#include "perf_precomp.hpp"

PERF_TEST(OpenCV, GenCircle)
{
	declare.time(60);
	HMat Image;
	HRegion Region, RegionCircle;
	string path = "D:/tzq/AVI Project/A/test_pic/Gray.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	TEST_CYCLE_N(1000)
	{
		gen_circle(RegionCircle,100,100,40.5);
	}
	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST(Halcon, Gen)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image;
	Hobject Region, RegionCircle;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/Gray.bmp";
	read_image(&Image, path);
	TEST_CYCLE_N(1000)
	{
		gen_circle(&RegionCircle, 100, 100, 40.5);
	}
	SANITY_CHECK(0);
	reportMetrics(false);
}