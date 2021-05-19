#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class Gen_Rectangle1Test : public FolderTest
{
public:
	Gen_Rectangle1Test(double row1,double col1,double row2,double col2){
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
		row1_ = row1;
		col1_ = col1;
		row2_ = row2;
		col2_ = col2;
	}
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
	double row1_;
	double col1_;
	double row2_;
	double col2_;
};

void Gen_Rectangle1Test::run_func()
{
	HRegion Region;
	gen_rectangle1(Region, row1_, col1_, row2_, col2_);
	int a = Region.len;
	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);

	ofstream outfile;
	outfile.open("C:/Users/02652/Desktop/test/opencv_gen_rectangle1.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	outfile << "count=" << a << "\n";
	outfile << "\n";
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = Region.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = Region.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = Region.rle_x_end[i];
		outfile << "ColumnBegin=" << Region.rle_x_start[i] << ",";
		outfile << "Row=" << Region.rle_y[i] << ",";
		outfile << "ColumnEnd=" << Region.rle_x_end[i] << "\n";
	}
	outfile.close();
}


void Gen_Rectangle1Test::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject Region, HalconGrayImage;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Hobject RegionRectangle;
	Mat2HObject(GrayImage, HalconGrayImage);
	gen_rectangle1(&RegionRectangle, row1_, col1_, row2_, col2_);
	union1(RegionRectangle, &RegionRectangle);
	runlength_features(RegionRectangle, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionRectangle, &Row, &ColumnBegin, &ColumnEnd);
	test_mat[REF_OUTPUT][0] = Mat::ones(Size(NumRuns, 3), CV_16SC1);
	ofstream outfile;
	outfile.open("C:/Users/02652/Desktop/test/halcon_gen_rectangle1.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	outfile << "count=" << NumRuns << "\n";
	outfile << "\n";
	for (int i = 0; i < NumRuns; i++){
		test_mat[REF_OUTPUT][0].at<short>(0, i) = ColumnBegin[i].I();
		test_mat[REF_OUTPUT][0].at<short>(1, i) = Row[i].I();
		test_mat[REF_OUTPUT][0].at<short>(2, i) = ColumnEnd[i].I();
		outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
		outfile << "Row=" << Row[i].I() << ",";
		outfile << "ColumnEnd=" << ColumnEnd[i].I()<< "\n";
	}
	outfile.close();
}


typedef std::tr1::tuple<double, double,double,double> Gen_Rectangle1Test_g;
typedef ::testing::TestWithParam<Gen_Rectangle1Test_g> Gen_Rectangle1Test_gen;
TEST_P(Gen_Rectangle1Test_gen, accuracy)
{
	Gen_Rectangle1Test_g t = GetParam();
	double row1 = std::tr1::get<0>(t);
	double col1 = std::tr1::get<1>(t);
	double row2 = std::tr1::get<2>(t);
	double col2 = std::tr1::get<3>(t);
	Gen_Rectangle1Test test(row1, col1,row2,col2);
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Gen_Rectangle1, Gen_Rectangle1Test_gen, testing::Combine(
	testing::Values(double(100)),
	testing::Values(double(100)),
	testing::Values(double(1000)), 
	testing::Values(double(1000))
	));

