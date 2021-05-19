#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class Union1Test : public FolderTest
{
public:
	Union1Test(){
		test_mat[OUTPUT].push_back(Mat());
		test_mat[REF_OUTPUT].push_back(Mat());
	}
protected:
	void run_func();
	void prepare_to_validation(int test_case_idx);
private:
};

void Union1Test::run_func()
{
	int width = 10;
	int height = 2;;
	HRegion Region1, Region2, RegionHRegion;
	ListHRegion RegionUnion;
	RegionUnion.Region = new HRegion[10];//分配内存
	threshold(GrayImage, Region1, 100, 200);

	ofstream outfile;
	threshold(GrayImage, Region2, 150, 250);
	concat_obj(Region1, Region2, RegionUnion);
	union1(RegionUnion, RegionHRegion);

	//outfile.open("C:/Users/02652/Desktop/test/Opencv_Union1.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
	//outfile << "count=" << RegionHRegion.len << "\n";
	//outfile << "\n";
	//for (int i = 0; i < RegionHRegion.len; i++){
	//	outfile << "ColumnBegin=" << RegionHRegion.rle_x_start[i] << ",";
	//	outfile << "Row=" << RegionHRegion.rle_y[i] << ",";
	//	outfile << "ColumnEnd=" << RegionHRegion.rle_x_end[i] << "\n";
	//}
	//outfile.close();

	test_mat[OUTPUT][0] = Mat::ones(Size(RegionHRegion.len, 3), CV_16SC1);
	for (int i = 0; i < RegionHRegion.len; i++){
		test_mat[OUTPUT][0].at<short>(0, i) = RegionHRegion.rle_x_start[i];
		test_mat[OUTPUT][0].at<short>(1, i) = RegionHRegion.rle_y[i];
		test_mat[OUTPUT][0].at<short>(2, i) = RegionHRegion.rle_x_end[i];
	}
	delete[] RegionUnion.Region;
}


void Union1Test::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject Region, HalconGrayImage;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Mat2HObject(GrayImage, HalconGrayImage);
	Hobject Region1, Region2, RegionUnion, RegionHRegion;

	threshold(HalconGrayImage, &Region1, 100, 200);
	threshold(HalconGrayImage, &Region2, 150, 250);
	concat_obj(Region1, Region2, &RegionUnion);
	union1(RegionUnion, &RegionHRegion);
	runlength_features(RegionHRegion, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
	get_region_runs(RegionHRegion, &Row, &ColumnBegin, &ColumnEnd);

	//ofstream outfile;
	//outfile.open("C:/Users/02652/Desktop/test/Halcon_Union1.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
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


typedef std::tr1::tuple<double, double> Union1_u;
typedef ::testing::TestWithParam<Union1_u> Union1_union;
TEST_P(Union1_union, accuracy)
{
	Union1_u t = GetParam();
	Union1Test test;
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Union1, Union1_union, testing::Combine(
	testing::Values(0,0),
	testing::Values(0,0)));

