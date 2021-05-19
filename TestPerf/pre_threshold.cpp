#include "perf_precomp.hpp"

typedef std::tr1::tuple<cv::Size, int, int> Size_Thresh_Int_t;
typedef TestBaseWithParam<Size_Thresh_Int_t> Size_Thresh_Int;

#define TYPICAL_IMAGE_SIZE_MORPH  cv::Size(5472,3648)
#define TYPICAL_IMAGE_THRESH_MORPH int(10)
#define TYPICAL_KERNEL_SIZE_MORPH int(150),int(160),int(170),int(180),int(190),int(200),int(210),int(220)
#define TYPICAL_MATS_MORPH       testing::Combine(testing::Values(TYPICAL_IMAGE_SIZE_MORPH),\
	testing::Values(TYPICAL_IMAGE_THRESH_MORPH), testing::Values(TYPICAL_KERNEL_SIZE_MORPH))


PERF_TEST_P(Size_Thresh_Int, cv_threshold, TYPICAL_MATS_MORPH)
{
	HMat Image;
	string path = "D:/tzq/AVI Project/A/test_pic/Gray.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	HRegion Region;
	
	TEST_CYCLE_N(100)
	{
		threshold(Image, Region, get<1>(GetParam()), get<2>(GetParam()));
	}

	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST_P(Size_Thresh_Int, halcon_threshold, TYPICAL_MATS_MORPH)
{
	using namespace Halcon;

	Hobject Image;
	Hobject  Region, RegionErosion;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/Gray.bmp";
	read_image(&Image, path);
	

	TEST_CYCLE_N(100)
	{
		threshold(Image, &Region, get<1>(GetParam()), get<2>(GetParam()));
	}

	SANITY_CHECK(0);
	reportMetrics(false);
}
