#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class SelectTest : public FolderTest
{
public:
	SelectTest(int low,int height){
		low_ = low;
		high_ = height;
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	}
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
	int low_;
	int high_;
};

void SelectTest::run_func()
{
	HRegion Region;
	int width = 10;
	int height = 2;
	HRegion Region1;
	HRegion ConnectionRegion;
	HRegion SelectRegion, SelectMaxRegion;
	threshold(GrayImage, Region1, low_, high_);
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
	CondParam m_SelectCondParam;
	m_SelectCondParam.Width.Min = 0;
	m_SelectCondParam.Width.Max = 1000;
	m_SelectCondParam.Height.Min = 0;
	m_SelectCondParam.Height.Max = 1000;
	select_shape(ConnectionRegion, SelectRegion, m_SelectCondParam);
	select_shape_max_std(SelectRegion, SelectMaxRegion);
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

	int a = SelectMaxRegion.len;
	test_mat[OUTPUT][0] = Mat::ones(Size(a, 3), CV_16SC1);
	for (int i = 0; i < a; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = SelectMaxRegion.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = SelectMaxRegion.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = SelectMaxRegion.rle_x_end[i];
	}
}


void SelectTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject HalconGrayImage;
	Hobject RegionConnection, RegionSelect, RegionMaxSelect;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd;
	Hobject Region1, Region2;
	Hlong Number;

	gen_empty_obj(&RegionSelect);
	Mat2HObject(GrayImage, HalconGrayImage);

	//write_image(HalconGrayImage, HTuple("bmp"), HTuple(0), HTuple("C:/Users/02652/Desktop/test/Gary.bmp"));
	threshold(HalconGrayImage, &Region1, low_, high_);


	connection(Region1, &RegionConnection);
	select_shape(RegionConnection, &RegionSelect, (HTuple("width").Append("height")),
		"and", (HTuple(0).Append(0)), (HTuple(1000).Append(1000)));
	select_shape_std(RegionSelect, &RegionMaxSelect,"max_area",70);
	union1(RegionMaxSelect, &RegionMaxSelect);
	//union1(RegionSelect, &RegionSelect);
	count_obj(RegionMaxSelect, &Number);
	if (Number == 0){
		int a = 0;
		NumRuns = 0;
	}
	else{
		runlength_features(RegionMaxSelect, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
		get_region_runs(RegionMaxSelect, &Row, &ColumnBegin, &ColumnEnd);
	}


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


typedef std::tr1::tuple<double, double>Select_s;
typedef ::testing::TestWithParam<Select_s> Select_select;
TEST_P(Select_select, accuracy)
{
	Select_s t = GetParam();
	int low = std::tr1::get<0>(t);
	int height = std::tr1::get<1>(t);
	SelectTest test(low, height);
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Select, Select_select, testing::Combine(
	testing::Values(0,10),
	testing::Values(150, 160,170,180,190,200,210,220)));

