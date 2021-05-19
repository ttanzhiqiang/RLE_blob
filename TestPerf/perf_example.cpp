#include "perf_precomp.hpp"
#include <fstream>
PERF_TEST(OpenCV1, MorphExample2)
{
	declare.time(60);
	HMat Image, GrayImage;
	HRegion Region, RegionDilation, ConnectedRegions, SelectRegion,ClosingRegion;
	int  offsetThres, Width, Height;
	double  Mean, Deviation, Number;
	string path = "D:/tzq/AVI Project/A/test_pic/D5_1.bmp";
	read_image(Image,path.c_str());
	cvtColor(Image, GrayImage, CV_RGB2GRAY);

	offsetThres = 30;
	Width = GrayImage.cols;
	Height = GrayImage.rows;

	Scalar m, std;
	meanStdDev(GrayImage, m, std);
	Mean = m[0];
	Deviation = std[0];
	threshold(GrayImage, Region, (Mean + offsetThres)<245 ? (Mean + offsetThres) : 245, 255);
	connection(Region, ConnectedRegions);

	CondParam m_CondParam;
	m_CondParam.Width.Min = 0;
	m_CondParam.Width.Max = 1000;
	m_CondParam.Height.Min = 0;
	m_CondParam.Height.Max = 1000;
	select_shape(ConnectedRegions, SelectRegion, m_CondParam);
	closing_rectangle1(SelectRegion, ClosingRegion, 1, 20);
	TEST_CYCLE_N(10)
	{
		
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

PERF_TEST(Halcon1, MorphExample2)
{
	using namespace Halcon;

	declare.time(60);
	Hobject Image, GrayImage;
	Hobject Region, RegionDilation;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;

	HTuple  offsetThres, Width, Height;
	double  Mean, Deviation;

	HTuple path = "D:/tzq/AVI Project/A/test_pic/D5_1.bmp";
	read_image(&Image, path);
	rgb1_to_gray(Image, &GrayImage);
	threshold(Image, &Region, 100, 200);
	offsetThres = 30;
	get_image_size(Image, &Width, &Height);
	intensity(GrayImage, GrayImage, &Mean, &Deviation);
	threshold(GrayImage, &Region, (Mean + offsetThres).Min2(245), 255);
	connection(Region, &Region);
	select_shape(Region, &Region, (HTuple("width").Append("height")), "and", (HTuple(300).Append(350)),
		(HTuple(9999).Append(9999)));

	closing_rectangle1(Region, &Region, 1, 20);

	TEST_CYCLE_N(10)
	{

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