#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class DifferenceTest : public FolderTest
{
public:
	DifferenceTest(){
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	}
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
};

void DifferenceTest::run_func()
{
	HRegion Region;
	int width = 10;
	int height = 2;
	HRegion RegionDifference;
	HRegion Region1, Region2;

	threshold(GrayImage, Region1, 100, 200);
	/*ofstream outfile;
	outfile.open("C:/Users/02652/Desktop/test/Opencv_Region1.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	outfile << "count=" << Region1.len << "\n";
	outfile << "\n";
	for (int i = 0; i < Region1.len; i++){
	outfile << "ColumnBegin=" << Region1.rle_x_start[i] << ",";
	outfile << "Row=" << Region1.rle_y[i] << ",";
	outfile << "ColumnEnd=" << Region1.rle_x_end[i] << "\n";
	}
	outfile.close();*/

	threshold(GrayImage, Region2, 150, 250);
	/*outfile.open("C:/Users/02652/Desktop/test/Opencv_Region2.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	outfile << "count=" << Region2.len << "\n";
	outfile << "\n";
	for (int i = 0; i < Region2.len; i++){
	outfile << "ColumnBegin=" << Region2.rle_x_start[i] << ",";
	outfile << "Row=" << Region2.rle_y[i] << ",";
	outfile << "ColumnEnd=" << Region2.rle_x_end[i] << "\n";
	}
	outfile.close();*/

	difference(Region1, Region2, RegionDifference);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_Difference.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << RegionDifference.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < RegionDifference.len; i++){
	//	outfile << "ColumnBegin=" << RegionDifference.rle_x_start[i] << ",";
	//	outfile << "Row=" << RegionDifference.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << RegionDifference.rle_x_end[i] << "\n";
	//}
	//outfile.close();

	int a = RegionDifference.len;
	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = RegionDifference.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = RegionDifference.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = RegionDifference.rle_x_end[i];
	}
}


void DifferenceTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject HalconGrayImage;
	Hobject RegionDifference;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Mat2HObject(GrayImage, HalconGrayImage);
	Hobject Region1, Region2;

	threshold(HalconGrayImage, &Region1, 100, 200);
	//runlength_features(Region1, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	//get_region_runs(Region1, &Row, &ColumnBegin, &ColumnEnd);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_Region1.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << NumRuns << "\n";
	//outfile << "\n";
	//for (int i = 0; i < NumRuns; i++){
	//outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
	//outfile << "Row=" << Row[i].I() << ",";
	//outfile << "ColumnEnd=" << ColumnEnd[i].I() << "\n";
	//}
	//outfile.close();

	threshold(HalconGrayImage, &Region2, 150, 250);
	//runlength_features(Region2, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	//get_region_runs(Region2, &Row, &ColumnBegin, &ColumnEnd);
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_Region2.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << NumRuns << "\n";
	//outfile << "\n";
	//for (int i = 0; i < NumRuns; i++){
	//outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
	//outfile << "Row=" << Row[i].I() << ",";
	//outfile << "ColumnEnd=" << ColumnEnd[i].I() << "\n";
	//}
	//outfile.close();

	difference(Region1, Region2, &RegionDifference);
	runlength_features(RegionDifference, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionDifference, &Row, &ColumnBegin, &ColumnEnd);
	/*ofstream outfile;
	outfile.open("C:/Users/02652/Desktop/test/Halcon_Difference.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	outfile << "count=" << NumRuns << "\n";
	outfile << "\n";
	for (int i = 0; i < NumRuns; i++){
	outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
	outfile << "Row=" << Row[i].I() << ",";
	outfile << "ColumnEnd=" << ColumnEnd[i].I() << "\n";
	}
	outfile.close();*/

	test_mat[REF_OUTPUT][0] = Mat::ones(Size(NumRuns, 3), CV_16SC1);
	for (int i = 0; i < NumRuns; i++){
		test_mat[REF_OUTPUT][0].at<short>(0, i) = ColumnBegin[i].I();
		test_mat[REF_OUTPUT][0].at<short>(1, i) = Row[i].I();
		test_mat[REF_OUTPUT][0].at<short>(2, i) = ColumnEnd[i].I();
	}
}


typedef std::tr1::tuple<double, double>Difference_d;
typedef ::testing::TestWithParam<Difference_d> Difference_diff;
TEST_P(Difference_diff, accuracy)
{
	Difference_d t = GetParam();
	DifferenceTest test;
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Diff, Difference_diff, testing::Combine(
	testing::Values(0, 0),
	testing::Values(0, 0)));

