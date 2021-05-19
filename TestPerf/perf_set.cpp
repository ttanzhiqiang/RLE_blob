#include "perf_precomp.hpp"
#include "HalconCpp.h"

typedef std::tr1::tuple<cv::Size, int, int> Size_Thresh_Int_t;
typedef TestBaseWithParam<Size_Thresh_Int_t> Size_Thresh_Int;

#define TYPICAL_IMAGE_SIZE_MORPH  cv::Size(5472,3648)
#define TYPICAL_IMAGE_THRESH_MORPH int(0)
#define TYPICAL_KERNEL_SIZE_MORPH int(10),int(20),int(30),int(40),int(50),int(60),int(70),int(80)
#define TYPICAL_MATS_MORPH       testing::Combine(testing::Values(TYPICAL_IMAGE_SIZE_MORPH),\
	testing::Values(TYPICAL_IMAGE_THRESH_MORPH), testing::Values(TYPICAL_KERNEL_SIZE_MORPH))

PERF_TEST_P(Size_Thresh_Int, cv_set, TYPICAL_MATS_MORPH)
{
	Size sz = get<0>(GetParam());
	int type = CV_8UC1;
	HMat Image, Image1;
	string path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	HRegion Region1, Region2, RegionUnion;
	threshold(Image, Region1, get<2>(GetParam()), 150);
	erosion_rectangle1(Region1, Region2, 10, 10);
	declare.time(600);
	TEST_CYCLE_N(10)
	{
		//union2(Region1, Region2, RegionUnion);
		//intersection2(Region1, Region2, RegionUnion);
		difference(Region1, Region2, RegionUnion);
	}

	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST_P(Size_Thresh_Int, halcon_set, TYPICAL_MATS_MORPH)
{
	using namespace Halcon;

	Hobject Image;
	Hobject  Region1, Region2, RegionUnion;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	read_image(&Image, path);
	int dilation_size = get<2>(GetParam());
	int Width = dilation_size;
	int Height = dilation_size;
	threshold(Image, &Region1, get<1>(GetParam()), 150);
	erosion_rectangle1(Region1, &Region2, 10, 10);
	TEST_CYCLE_N(100)
	{
		//union2(Region1, Region2, &RegionUnion);
		//intersection(Region1, Region2, &RegionUnion);
		difference(Region1, Region2, &RegionUnion);
	}

	SANITY_CHECK(0);
	reportMetrics(false);
}
