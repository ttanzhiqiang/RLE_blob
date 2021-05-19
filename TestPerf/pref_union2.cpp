#include "perf_precomp.hpp"
#include <fstream>
PERF_TEST(OpenCV1, Union2)
{
	declare.time(60);
	HMat Image;
	HRegion Region, RegionRectangle;
	string path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	Image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	HRegion Region1, Region2, RegionUnion;
	threshold(Image, Region1, 100, 200);
	threshold(Image, Region2, 220, 250);
	TEST_CYCLE_N(10)
	{		
		//intersection2(Region1, Region2, RegionUnion);
		//difference(Region1, Region2, RegionUnion);
		union2(Region1, Region2, RegionUnion);
	}
	


	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_RegionUnion.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << RegionUnion.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < RegionUnion.len; i++){
	//	outfile << "ColumnBegin=" << RegionUnion.rle_x_start[i] << ",";
	//	outfile << "Row=" << RegionUnion.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << RegionUnion.rle_x_end[i] << "\n";
	//}
	//outfile.close();
	SANITY_CHECK(0);
	reportMetrics(false);
}

PERF_TEST(Halcon1, Union2)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image;
	Hobject Region1, Region2, RegionUnion;
	HTuple path = "D:/tzq/AVI Project/A/test_pic/A82.bmp";
	read_image(&Image, path);
	threshold(Image, &Region1, 100, 200);
	threshold(Image, &Region2, 220, 250);
	TEST_CYCLE_N(1000)
	{
		//intersection(Region1, Region2, &RegionUnion);
		//difference(Region1, Region2, &RegionUnion);
		union2(Region1, Region2, &RegionUnion);
	}

	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	runlength_features(RegionUnion, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionUnion, &Row, &ColumnBegin, &ColumnEnd);

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_RegionUnion.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
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