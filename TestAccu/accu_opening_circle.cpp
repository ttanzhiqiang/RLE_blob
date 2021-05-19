#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class OpeningCircleTest : public FolderTest
{
public:
	OpeningCircleTest(double radius_)
	{
		low_ = 100;
		high_ = 200;
		radius = radius_;
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	};
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
	double low_;
	double high_;
	double radius;
};

void OpeningCircleTest::run_func()
{
	HRegion Region;
	HRegion OpeningRegion, ErosionRegion;
	int a;
	threshold(GrayImage, Region, low_, high_);

	//erosion_circle(Region, ErosionRegion, radius);
	//dilation_circle(ErosionRegion, OpeningRegion, radius);
	opening_circle(Region, OpeningRegion, radius);
	
	a = OpeningRegion.len;
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_OpeningCircle.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << a << "\n";
	//outfile << "\n";
	//for (int i = 0; i < a; i++){
	//	outfile << "ColumnBegin=" << OpeningRegion.rle_x_start[i] << ",";
	//	outfile << "Row=" << OpeningRegion.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << OpeningRegion.rle_x_end[i]<< "\n";
	//}
	//outfile.close();

	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = OpeningRegion.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = OpeningRegion.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = OpeningRegion.rle_x_end[i];

	}
}


void OpeningCircleTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject Region, OpeningRegion, ErosionRegion, HalconGrayImage;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Mat2HObject(GrayImage, HalconGrayImage);
	threshold(HalconGrayImage, &Region, low_, high_);

	//erosion_circle(Region, &ErosionRegion, radius);
	//dilation_circle(ErosionRegion, &OpeningRegion, radius);
	opening_circle(Region, &OpeningRegion, radius);
	runlength_features(OpeningRegion, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(OpeningRegion, &Row, &ColumnBegin, &ColumnEnd);


	//ofstream outfile;
	//outfile.open("C:/Users/0265  2/Desktop/test/Halcon_OpeningCircle.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << NumRuns << "\n";
	//outfile << "\n";
	//for (int i = 0; i < NumRuns; i++){
	//	outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
	//	outfile << "Row=" << Row[i].I() << ",";
	//	outfile << "ColumnEnd=" << ColumnEnd[i].I()<< "\n";
	//}
	//outfile.close();

	test_mat[REF_OUTPUT][0] = Mat::ones(Size(NumRuns, 3), CV_16SC1);
	for (int i = 0; i < NumRuns; i++){
		test_mat[REF_OUTPUT][0].at<short>(0, i) = ColumnBegin[i].I();
		test_mat[REF_OUTPUT][0].at<short>(1, i) = Row[i].I();
		test_mat[REF_OUTPUT][0].at<short>(2, i) = ColumnEnd[i].I();
	}
}


typedef std::tr1::tuple<double, double> Opening_Circle_c;
typedef ::testing::TestWithParam<Opening_Circle_c> Opening_Circle_opening;
TEST_P(Opening_Circle_opening, accuracy)
{
	Opening_Circle_c t = GetParam();
	double radius = std::tr1::get<0>(t);
	OpeningCircleTest test(radius);
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Erosion_Circle, Opening_Circle_opening, testing::Combine(
	testing::Values(double(35), double(20), double(10), double(12), double(2), double(3), double(25), double(30), double(35),
	double(9.5), double(20.5), double(10.5), double(12.5), double(2.5), double(3.5), double(25.5), double(30.5), double(35.5)),
	testing::Values(double(10))));

