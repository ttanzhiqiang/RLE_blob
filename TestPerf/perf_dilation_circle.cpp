#include "perf_precomp.hpp"
#include "HalconCpp.h"

typedef std::tr1::tuple<cv::Size, int, int> Size_Thresh_Int_t;
typedef TestBaseWithParam<Size_Thresh_Int_t> Size_Thresh_Int;

#define TYPICAL_IMAGE_SIZE_MORPH  cv::Size(5472,3648)
#define TYPICAL_IMAGE_THRESH_MORPH int(0)
#define TYPICAL_KERNEL_SIZE_MORPH int(10),int(20),int(30),int(40),int(50),int(60),int(70),int(80)
#define TYPICAL_MATS_MORPH       testing::Combine(testing::Values(TYPICAL_IMAGE_SIZE_MORPH),\
	testing::Values(TYPICAL_IMAGE_THRESH_MORPH), testing::Values(TYPICAL_KERNEL_SIZE_MORPH))

PERF_TEST_P(Size_Thresh_Int, cv_erosion, TYPICAL_MATS_MORPH)
{
	Size sz = get<0>(GetParam());
	int type = CV_8UC1;
	HMat Image;
	string path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	HRegion Region;
	HRegion RegionErosion;
	int dilation_size = get<2>(GetParam());
	int Width = dilation_size;
	int Height = dilation_size;
	threshold(Image, Region, get<1>(GetParam()), 150);

	declare.time(600);
	TEST_CYCLE_N(10)
	{
		//erosion_circle(Region, RegionErosion, dilation_size>>1);
		dilation_circle(Region, RegionErosion, dilation_size >> 1);
		//erosion_rectangle1(Region, RegionErosion, Width, Height);
		//dilation_rectangle1(Region, RegionErosion, Width, Height);
	}

	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST_P(Size_Thresh_Int, halcon_erosion, TYPICAL_MATS_MORPH)
{
	using namespace Halcon;

	Hobject Image;
	Hobject  Region, RegionErosion;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	read_image(&Image, path);
	int dilation_size = get<2>(GetParam());
	int Width = dilation_size;
	int Height = dilation_size;
	
	threshold(Image, &Region, get<1>(GetParam()), 150);
	TEST_CYCLE_N(100)
	{

		//erosion_circle(Region, &RegionErosion, dilation_size >> 1);
		dilation_circle(Region, &RegionErosion, dilation_size >> 1);
		//erosion_rectangle1(Region, &RegionErosion, Width, Height);
		//dilation_rectangle1(Region, &RegionErosion, Width, Height);
	}

	SANITY_CHECK(0);
	reportMetrics(false);
}
