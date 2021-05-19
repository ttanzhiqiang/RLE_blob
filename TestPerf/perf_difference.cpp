#include "perf_precomp.hpp"
#include <fstream>
PERF_TEST(OpenCV1, Difference)
{
	declare.time(60);
	HMat Image;
	HRegion Region, RegionRectangle;
	string path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	HRegion Region1, Region2, RegionUnion;
	threshold(Image, Region1, 100, 200);
	threshold(Image, Region2, 150, 250);
	TEST_CYCLE_N(10)
	{
		difference(Region1, Region2, RegionUnion);
	}
	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST(Halcon1, Difference)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image;
	Hobject Region1, Region2, RegionUnion;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	read_image(&Image, path);
	threshold(Image, &Region1, 100, 200);
	threshold(Image, &Region2, 150, 250);
	TEST_CYCLE_N(10)
	{
		difference(Region1, Region2, &RegionUnion);
	}
	SANITY_CHECK(0);
	reportMetrics(false);
}