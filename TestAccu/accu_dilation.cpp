#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class DilationTest : public FolderTest
{
public:
	DilationTest(double width, double height)
	{
		low_ = 100;
		high_ = 200;
		width_ = width;
		height_ = height;
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	};
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
	double low_;
	double high_;
	int width_;
	int height_;
};

void DilationTest::run_func()
{
	HRegion Region;
	HRegion Region_dilation;
	int a;
	threshold(GrayImage, Region, low_, high_);

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_Region.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << Region.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < Region.len; i++){
	//	outfile << "ColumnBegin=" << Region.rle_x_start[i] << ",";
	//	outfile << "Row=" << Region.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << Region.rle_x_end[i] << "\n";
	//}
	//outfile.close();
	
	
	dilation_rectangle1(Region, Region_dilation, width_, height_);
	a = Region_dilation.len;

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_RegionDilation.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << a << "\n";
	//outfile << "\n";
	//for (int i = 0; i < a; i++){
	//	outfile << "ColumnBegin=" << Region_dilation.rle_x_start[i] << ",";
	//	outfile << "Row=" << Region_dilation.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << Region_dilation.rle_x_end[i] << "\n";
	//}
	//outfile.close();

	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = Region_dilation.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = Region_dilation.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = Region_dilation.rle_x_end[i];

	}

}


void DilationTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject Region, RegionDilation, HalconGrayImage;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Mat2HObject(GrayImage, HalconGrayImage);
	threshold(HalconGrayImage, &Region, low_, high_);
	runlength_features(Region, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(Region, &Row, &ColumnBegin, &ColumnEnd);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_Region1.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << NumRuns << "\n";
	//outfile << "\n";
	//for (int i = 0; i < NumRuns; i++){
	//	outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
	//	outfile << "Row=" << Row[i].I() << ",";
	//	outfile << "ColumnEnd=" << ColumnEnd[i].I() << "\n";
	//}
	//outfile.close();



	dilation_rectangle1(Region, &RegionDilation, width_, height_);
	runlength_features(RegionDilation, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionDilation, &Row, &ColumnBegin, &ColumnEnd);
	

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_RegionDilation.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
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


typedef std::tr1::tuple<double, double> Dilation_Low_High_t;
typedef ::testing::TestWithParam<Dilation_Low_High_t> Dilation_Low_High;
TEST_P(Dilation_Low_High, accuracy)
{
	Dilation_Low_High_t t = GetParam();
	int width = std::tr1::get<0>(t);
	int height = std::tr1::get<1>(t);
	DilationTest test(width, height);
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Dilation, Dilation_Low_High, testing::Combine(
	testing::Values(int(10), int(20),int(50)),
	testing::Values(int(10), int(11),int(12), int(13), int(14), int(15), int(16), int(17), int(18),int(20))));



