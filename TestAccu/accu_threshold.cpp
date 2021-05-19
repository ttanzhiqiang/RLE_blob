#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class ThresholdTest : public FolderTest
{
public:
	ThresholdTest(double low, double high)
	{
		low_ = low;
		high_ = high;
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	};
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
	double low_;
	double high_;
};

void ThresholdTest::run_func()
{
	HRegion Region;
	int width = 10;
	int height = 2;
	threshold(GrayImage, Region, low_, high_);

	int a = Region.len;
	/*ofstream outfile;
	outfile.open("C:/Users/02652/Desktop/test/Opencv_threshold.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	outfile << "count=" << a << "\n";
	outfile << "\n";
	for (int i = 0; i < a; i++){
		outfile << "ColumnBegin=" << Region.rle_x_start[i] << ",";
		outfile << "Row=" << Region.rle_y[i] << ",";
		outfile << "ColumnEnd=" << Region.rle_x_end[i] << "\n";
	}
	outfile.close();*/

	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = Region.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = Region.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = Region.rle_x_end[i];
	}
}


void ThresholdTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject Region, HalconGrayImage;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	int width = 10;
	int height = 2;
	Mat2HObject(GrayImage, HalconGrayImage);
	threshold(HalconGrayImage, &Region, low_, high_);
	runlength_features(Region, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(Region, &Row, &ColumnBegin, &ColumnEnd);

	/*ofstream outfile;
	outfile.open("C:/Users/02652/Desktop/test/Halcon_threshold.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
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


typedef std::tr1::tuple<double, double> Threshold_Low_High_t;
typedef ::testing::TestWithParam<Threshold_Low_High_t> Threshold_Low_High;
TEST_P(Threshold_Low_High, accuracy)
{
	Threshold_Low_High_t t = GetParam();
	int low = std::tr1::get<0>(t);
	int high = std::tr1::get<1>(t);
	ThresholdTest test(low, high);
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Threshold, Threshold_Low_High, testing::Combine(
	testing::Values(double(100), double(125), double(120), double(130), double(140)),
	testing::Values(double(200), double(215), double(220), double(230), double(240))));
 
