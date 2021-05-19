#include "perf_precomp.hpp"
#include <fstream>
PERF_TEST(OpenCV1, Connection)
{
	declare.time(60);
	HMat Image;
	HRegion Region, RegionRectangle;
	string path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	HRegion Region1, RegionConnection, SelectRegion;
	threshold(Image, Region1, 10, 220);
	
	TEST_CYCLE_N(10)
	{
		connection(Region1, RegionConnection);
	}

	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST(Halcon1, Connection)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image;
	Hobject Region1, RegionConnection, SelectRegion;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	read_image(&Image, path);
	threshold(Image, &Region1, 10, 220);

	TEST_CYCLE_N(10)
	{
		connection(Region1, &RegionConnection);
	}
	
	SANITY_CHECK(0);
	reportMetrics(false);
}