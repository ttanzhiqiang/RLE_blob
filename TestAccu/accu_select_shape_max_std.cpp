#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class SelectMaxTest : public FolderTest
{
public:
	SelectMaxTest(){
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	}
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
};

void SelectMaxTest::run_func()
{
	HRegion Region;
	int width = 10;
	int height = 2;
	HRegion Region1;
	HRegion ConnectionRegion;
	HRegion SelectRegion;
	threshold(GrayImage, Region1, 100, 255);
	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_Region1.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << Region1.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < Region1.len; i++){
	//	outfile << "ColumnBegin=" << Region1.rle_x_start[i] << ",";
	//	outfile << "Row=" << Region1.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << Region1.rle_x_end[i] << "\n";
	//}
	//outfile.close();

	connection(Region1, ConnectionRegion);
	select_shape_max_std(ConnectionRegion, SelectRegion);

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Opencv_select_shape.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << SelectRegion.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < SelectRegion.len; i++){
	//	outfile << "ColumnBegin=" << SelectRegion.rle_x_start[i] << ",";
	//	outfile << "Row=" << SelectRegion.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << SelectRegion.rle_x_end[i] << "\n";
	//}
	//outfile.close();

	int a = SelectRegion.len;
	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = SelectRegion.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = SelectRegion.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = SelectRegion.rle_x_end[i];
	}
}


void SelectMaxTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject HalconGrayImage;
	Hobject RegionConnection, RegionSelect;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Hobject Region1, Region2;
	HTuple Number;

	gen_empty_obj(&RegionSelect);
	Mat2HObject(GrayImage, HalconGrayImage);

	//write_image(HalconGrayImage, HTuple("bmp"), HTuple(0), HTuple("C:/Users/02652/Desktop/test/Gary.bmp"));
	threshold(HalconGrayImage, &Region1, 100, 255);


	connection(Region1, &RegionConnection);
	select_shape_std(RegionConnection, &RegionSelect,"max_area",70);
	union1(RegionSelect, &RegionSelect);

	runlength_features(RegionSelect, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionSelect, &Row, &ColumnBegin, &ColumnEnd);

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_select_shape.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
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


typedef std::tr1::tuple<double, double>SelectMax_sm;
typedef ::testing::TestWithParam<SelectMax_sm> SelectMax_selectmax;
TEST_P(SelectMax_selectmax, accuracy)
{
	SelectMax_sm t = GetParam();
	SelectMaxTest test;
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_SelectMax, SelectMax_selectmax, testing::Combine(
	testing::Values(0, 0),
	testing::Values(0, 0)));

