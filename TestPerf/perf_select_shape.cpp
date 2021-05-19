#include "perf_precomp.hpp"
#include <fstream>

typedef std::tr1::tuple<cv::Size, int, int> Size_Thresh_Int_t;
typedef TestBaseWithParam<Size_Thresh_Int_t> Size_Thresh_Int;

#define TYPICAL_IMAGE_SIZE_MORPH  cv::Size(5472,3648)
#define TYPICAL_IMAGE_THRESH_MORPH int(10)
//#define TYPICAL_KERNEL_SIZE_MORPH int(150)
#define TYPICAL_KERNEL_SIZE_MORPH int(150),int(160),int(170),int(180),int(190),int(200),int(210),int(220)
#define TYPICAL_MATS_MORPH       testing::Combine(testing::Values(TYPICAL_IMAGE_SIZE_MORPH),\
	testing::Values(TYPICAL_IMAGE_THRESH_MORPH), testing::Values(TYPICAL_KERNEL_SIZE_MORPH))

PERF_TEST_P(Size_Thresh_Int, cv_select, TYPICAL_MATS_MORPH)
{
	HMat Image;
	HMat ImageR, ImageG, ImageB;
	string path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	HRegion Region;
	HRegion RegionConnection, SelectRegion;
	threshold(Image, Region, get<1>(GetParam()), get<2>(GetParam()));
	connection(Region, RegionConnection);
	CondParam m_CondParam;
	m_CondParam.Width.Min = 0;
	m_CondParam.Width.Max = 1000;
	m_CondParam.Height.Min = 0;
	m_CondParam.Height.Max = 1000;

	declare.time(600);
	TEST_CYCLE_N(100)
	{
		select_shape(RegionConnection, SelectRegion, m_CondParam);
	}
	//select_shape_max_std(RegionConnection, SelectRegion);
	//select_shape(RegionConnection, SelectRegion, m_CondParam);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_select_shape.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << SelectRegion.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < SelectRegion.len; i++){
	//	outfile << "ColumnBegin=" << SelectRegion.rle_x_start[i] << ",";
	//	outfile << "Row=" << SelectRegion.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << SelectRegion.rle_x_end[i] << "\n";
	//}
	//outfile.close();
	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST_P(Size_Thresh_Int, halcon_select, TYPICAL_MATS_MORPH)
{
	using namespace Halcon;

	Hobject Image, ImageR, ImageG, ImageB;
	Hobject  Region, RegionConnection, SelectRegion;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	read_image(&Image, path);
	threshold(Image, &Region, get<1>(GetParam()), get<2>(GetParam()));
	connection(Region, &RegionConnection);
	TEST_CYCLE_N(100)
	{
		select_shape(RegionConnection, &SelectRegion, (HTuple("width").Append("height")),
			"and", (HTuple(0).Append(0)), (HTuple(1000).Append(1000)));
	}
	//select_shape_std(RegionConnection, &SelectRegion, "max_area", 70);

	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	union1(SelectRegion, &SelectRegion);
	runlength_features(SelectRegion, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(SelectRegion, &Row, &ColumnBegin, &ColumnEnd);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_select_shape.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << NumRuns << "\n";
	//outfile << "\n";
	//for (int i = 0; i < NumRuns; i++){
	//	outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
	//	outfile << "Row=" << Row[i].I() << ",";
	//	outfile << "ColumnEnd=" << ColumnEnd[i].I() << "\n";
	//}
	//outfile.close();


	SANITY_CHECK(0);
	reportMetrics(false);
}
