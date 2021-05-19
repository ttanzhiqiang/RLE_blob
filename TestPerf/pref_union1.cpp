#include "perf_precomp.hpp"

PERF_TEST(OpenCV1, Union1)
{
	declare.time(60);
	HMat Image;
	HRegion Region, RegionRectangle;
	string path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	HRegion Region1, Region2, RegionHRegion;
	threshold(Image, Region1, 100, 200);
	threshold(Image, Region2, 150, 250);
	vector<Ptr<HRegion>> RegionUnion;
	concat_obj(Region1, Region2, RegionUnion);
	TEST_CYCLE_N(10)
	{
		union1(RegionUnion, RegionHRegion);
	}
	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST(Halcon1, Union1)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image;
	Hobject Region1, Region2, RegionUnion, RegionHRegion;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	read_image(&Image, path);
	threshold(Image, &Region1, 100, 200);
	threshold(Image, &Region2, 150, 250);
	concat_obj(Region1, Region2, &RegionUnion);
	TEST_CYCLE_N(10)
	{
		union1(RegionUnion, &RegionHRegion);
	}
	SANITY_CHECK(0);
	reportMetrics(false);
}