#include "perf_precomp.hpp"
#include <fstream>
PERF_TEST(OpenCV1, Select)
{
	declare.time(60);
	HMat Image;
	HRegion Region, RegionRectangle;
	string path = "D:/tzq/AVI Project/A/test_pic/Gray.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	HRegion Region1, RegionConnection;
	threshold(Image, Region1, 100, 255);
	connection(Region1, RegionConnection);

	HRegion SelectRegion;
	
	select_shape_max_std(RegionConnection, SelectRegion);
	TEST_CYCLE_N(100)
	{
		
	}
	ofstream outfile;
	outfile.open("C:/Users/02652/Desktop/test/Opencv_select_shape_max.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	outfile << "count=" << SelectRegion.len << "\n";
	outfile << "\n";
	for (int i = 0; i < SelectRegion.len; i++){
		outfile << "ColumnBegin=" << SelectRegion.rle_x_start[i] << ",";
		outfile << "Row=" << SelectRegion.rle_y[i] << ",";
		outfile << "ColumnEnd=" << SelectRegion.rle_x_end[i] << "\n";
	}
	outfile.close();

	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST(Halcon1, Select)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image;
	Hobject Region1, RegionConnection, SelectRegion;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/Gray.bmp";
	read_image(&Image, path);
	threshold(Image, &Region1, 100, 255);
	connection(Region1, &RegionConnection);
	

	TEST_CYCLE_N(100)
	{
		select_shape_std(RegionConnection, &SelectRegion, "max_area", 70);
	}
	union1(SelectRegion, &SelectRegion);
	runlength_features(SelectRegion, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(SelectRegion, &Row, &ColumnBegin, &ColumnEnd);
	ofstream outfile;
	outfile.open("C:/Users/02652/Desktop/test/Halcon_select_shape_max.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	outfile << "count=" << NumRuns << "\n";
	outfile << "\n";
	for (int i = 0; i < NumRuns; i++){
		outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
		outfile << "Row=" << Row[i].I() << ",";
		outfile << "ColumnEnd=" << ColumnEnd[i].I() << "\n";
	}
	outfile.close();

	SANITY_CHECK(0);
	reportMetrics(false);
}