#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class Union2Test : public FolderTest
{
public:
	Union2Test(){
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	}
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
};

void Union2Test::run_func()
{
	HRegion Region;
	int width = 10;
	int height = 2;
	HRegion RegionUnion;
	HRegion Region1, Region2;

	threshold(GrayImage, Region1, 100, 200);
	threshold(GrayImage, Region2, 150, 250);

	union2(Region1, Region2, RegionUnion);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_Union2.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << RegionUnion.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < RegionUnion.len; i++){
	//	outfile << "ColumnBegin=" << RegionUnion.rle_x_start[i] << ",";
	//	outfile << "Row=" << RegionUnion.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << RegionUnion.rle_x_end[i] << "\n";
	//}
	//outfile.close();

	int a = RegionUnion.len;
	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = RegionUnion.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = RegionUnion.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = RegionUnion.rle_x_end[i];
	}
}


void Union2Test::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject HalconGrayImage;
	Hobject RegionRectangle1, RegionRectangle2,RegionUnion;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Mat2HObject(GrayImage, HalconGrayImage);
	Hobject Region1, Region2;

	threshold(HalconGrayImage, &Region1, 100, 200);
	threshold(HalconGrayImage, &Region2, 150, 250);

	union2(Region1, Region2, &RegionUnion);
	runlength_features(RegionUnion, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionUnion, &Row, &ColumnBegin, &ColumnEnd);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_Union2.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << NumRuns << "\n";
	//outfile << "\n";
	//for (int i = 0; i < NumRuns; i++){
	//	outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
	//	outfile << "Row=" << Row[i].I() << ",";
	//	outfile << "ColumnEnd=" << ColumnEnd[i].I() << "\n";
	//}
	//outfile.close();

	test_mat[REF_OUTPUT][0] = Mat::ones(Size(NumRuns, 3), CV_16SC1);
	for (int i = 0; i < NumRuns; i++){
		test_mat[REF_OUTPUT][0].at<short>(0, i) = ColumnBegin[i].I();
		test_mat[REF_OUTPUT][0].at<short>(1, i) = Row[i].I();
		test_mat[REF_OUTPUT][0].at<short>(2, i) = ColumnEnd[i].I();
	}
}


typedef std::tr1::tuple<double, double> Union2_u;
typedef ::testing::TestWithParam<Union2_u> Union2_union;
TEST_P(Union2_union, accuracy)
{
	Union2_u t = GetParam();
	Union2Test test;
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Union2, Union2_union, testing::Combine(
	testing::Values(0, 0),
	testing::Values(0, 0)));

