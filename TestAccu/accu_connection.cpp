#include "accu_precomp.hpp"
#include <fstream>
///////// threshold //////////

class ConnectionTest : public FolderTest
{
public:
	ConnectionTest(int low, int height){
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

void ConnectionTest::run_func()
{
	int width = 10;
	int height = 2;
	HRegion Region1;
	HRegion ConnectionRegion;
	threshold(GrayImage, Region1, low_, high_);
	connection(Region1, ConnectionRegion);
	
	CondParam m_CondParam;
	m_CondParam.Width.Min = 0;
	m_CondParam.Width.Max = 1000;
	m_CondParam.Height.Min = 0;
	m_CondParam.Height.Max = 1000;
	//for (int i = 1; i <= ConnectionRegion.runLabels_Count;i++){
		HRegion SelectRegion;
		select_obj(ConnectionRegion, SelectRegion, 1);
		//select_shape(ConnectionRegion, SelectRegion, m_CondParam);
		//ofstream outfile;
		//outfile.open("C:/Users/02652/Desktop/test/Opencv_Connection.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
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
	//}
}


void ConnectionTest::prepare_to_validation(int test_case_idx)
{
	/*参数初始化*/
	using namespace Halcon;
	Hobject HalconGrayImage;
	Hobject RegionConnection,RegionSelect;
	Hlong NumRuns, Bytes;
	double KFactor, LFactor, MeanLength;
	HTuple Row, ColumnBegin, ColumnEnd, number;
	Hobject Region1, Region2;
	HTuple Number;

	gen_empty_obj(&RegionSelect);
	Mat2HObject(GrayImage, HalconGrayImage);
	threshold(HalconGrayImage, &Region1, low_, high_);
	connection(Region1,&RegionConnection);
	HTuple NumberCount;
	count_obj(RegionConnection, &NumberCount);
	//select_shape(RegionConnection, &RegionSelect, (HTuple("width").Append("height")),
	//	"and", (HTuple(0).Append(0)), (HTuple(1000).Append(1000)));
	//union1(RegionSelect, &RegionSelect);

	//for (int i = 1; i <= NumberCount; i++){
		select_obj(RegionConnection, &RegionSelect, 1);
		//ofstream outfile;
		//outfile.open("C:/Users/02652/Desktop/test/Halcon_Connection.xml", ios::binary | ios::app | ios::in | ios::out, ios::trunc);
		runlength_features(RegionSelect, &NumRuns, &KFactor, &LFactor, &MeanLength, &Bytes);
		get_region_runs(RegionSelect, &Row, &ColumnBegin, &ColumnEnd);
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
	//}
}


typedef std::tr1::tuple<double, double>Connection_c;
typedef ::testing::TestWithParam<Connection_c> Connection_con;
TEST_P(Connection_con, accuracy)
{
	Connection_c t = GetParam();
	int low = std::tr1::get<0>(t);
	int height = std::tr1::get<1>(t);
	ConnectionTest test(low, height);
	test.safe_run();
}

INSTANTIATE_TEST_CASE_P(Core_Connection, Connection_con, testing::Combine(
	testing::Values(0, 10),
	testing::Values(150, 160, 170, 180, 190, 200, 210, 220)));
