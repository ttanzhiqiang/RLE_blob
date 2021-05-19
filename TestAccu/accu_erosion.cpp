#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class ErosionTest : public FolderTest
{
public:
	ErosionTest(double width, double height)
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

void ErosionTest::run_func()
{
	HRegion Region;
	threshold(GrayImage, Region, low_, high_);
	HRegion Region_erosion;
	int a;
	erosion_rectangle1(Region, Region_erosion, width_, height_);
	a = Region_erosion.len;
	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/opencv_width10_height2.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << a << "\n";
	//outfile << "\n";
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = Region_erosion.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = Region_erosion.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = Region_erosion.rle_x_end[i];
		/*outfile << "ColumnBegin=" << Region_erosion.rle_x_start[i] << ",";
		outfile << "Row=" << Region_erosion.rle_y[i] << ",";
		outfile << "ColumnEnd=" << Region_erosion.rle_x_end[i]<< "\n";*/
	}
	//outfile.close();
}

 
void ErosionTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject Region, RegionErosion, HalconGrayImage;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Mat2HObject(GrayImage, HalconGrayImage);
	threshold(HalconGrayImage, &Region, low_, high_);
	runlength_features(Region, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(Region, &Row, &ColumnBegin, &ColumnEnd);
	erosion_rectangle1(Region, &RegionErosion, width_, height_);
	runlength_features(RegionErosion, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionErosion, &Row, &ColumnBegin, &ColumnEnd);
	test_mat[REF_OUTPUT][0] = Mat::ones(Size(NumRuns, 3), CV_16SC1);

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/halcon_width10_height2.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	for (int i = 0; i < NumRuns; i++){
		test_mat[REF_OUTPUT][0].at<short>(0, i) = ColumnBegin[i].I();
		test_mat[REF_OUTPUT][0].at<short>(1, i) = Row[i].I();
		test_mat[REF_OUTPUT][0].at<short>(2, i) = ColumnEnd[i].I();
		//cout << ColumnEnd[i].I() << endl;
		/*outfile << "ColumnBegin=" << ColumnBegin[i].I() << ",";
		outfile << "Row=" << Row[i].I() << ",";
		outfile << "ColumnEnd=" << ColumnEnd[i].I()<< "\n";*/
	}
	//outfile.close();
}


typedef std::tr1::tuple<double, double> Erosion_Low_High_t;
typedef ::testing::TestWithParam<Erosion_Low_High_t> Erosion_Low_High;
TEST_P(Erosion_Low_High, accuracy)
{
	Erosion_Low_High_t t = GetParam();
	int width = std::tr1::get<0>(t);
	int height = std::tr1::get<1>(t);
	ErosionTest test(width, height);
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Erosion, Erosion_Low_High, testing::Combine(
	testing::Values(int(1),int(10), int(20), int(30), int(40), int(50)),
	testing::Values(int(2), int(3), int(4), int(5), int(6))));

