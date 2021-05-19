#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class Gen_CircleTest : public FolderTest
{
public:
	Gen_CircleTest(double radius_)
	{
		//low_ = 100;
		//high_ = 200;
		radius = radius_;
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	};
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
	//double low_;
	//double high_;
	double radius;
};

void Gen_CircleTest::run_func()
{
	HRegion Region;
	HRegion RegionCircle;
	int a;
	gen_circle(RegionCircle, 100, 100, radius);
	a = RegionCircle.len;

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_RegionCicle.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << a << "\n";
	//outfile << "\n";
	//for (int i = 0; i < a; i++){
	//	outfile << "ColumnBegin=" << RegionCircle.rle_x_start[i] << ",";
	//	outfile << "Row=" << RegionCircle.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << RegionCircle.rle_x_end[i] << "\n";
	//}
	//outfile.close();

	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = RegionCircle.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = RegionCircle.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = RegionCircle.rle_x_end[i];

	}

}


void Gen_CircleTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject Region, RegionCircle, HalconGrayImage;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Mat2HObject(GrayImage, HalconGrayImage);

	gen_circle(&RegionCircle, 100, 100, radius);
	runlength_features(RegionCircle, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionCircle, &Row, &ColumnBegin, &ColumnEnd);

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_RegionCicle.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
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


typedef std::tr1::tuple<double, double> Gen_circle_c;
typedef ::testing::TestWithParam<Gen_circle_c> Gen_circle_gen;
TEST_P(Gen_circle_gen, accuracy)
{
	Gen_circle_c t = GetParam();
	double radius = std::tr1::get<0>(t);
	Gen_CircleTest test(radius);
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Circle, Gen_circle_gen, testing::Combine(
	testing::Values(double(1.5), double(10.5), double(3.5), double(20), double(25.5), double(30.5), double(35.5)),
	testing::Values(double(10))));



