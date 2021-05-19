#include "perf_precomp.hpp"
#include <fstream>
PERF_TEST(OpenCV1, Dilation)
{
	declare.time(60);
	HMat Image;
	HRegion Region, RegionDilation;
	string path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	threshold(Image, Region, 0, 160);
	
	TEST_CYCLE_N(10)
	{
		dilation_rectangle1(Region, RegionDilation, 10, 10);
	}

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_RegionDilation.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << RegionDilation.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < RegionDilation.len; i++){
	//	outfile << "ColumnBegin=" << RegionDilation.rle_x_start[i] << ",";
	//	outfile << "Row=" << RegionDilation.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << RegionDilation.rle_x_end[i] << "\n";
	//}
	//outfile.close();
	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST(Halcon1, Dilation)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image;
	Hobject Region, RegionDilation;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	read_image(&Image, path);
	threshold(Image, &Region, 0, 160);
	TEST_CYCLE_N(10)
	{
		dilation_rectangle1(Region, &RegionDilation, 10, 10);
	}

	runlength_features(RegionDilation, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionDilation, &Row, &ColumnBegin, &ColumnEnd);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_RegionDilation.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
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