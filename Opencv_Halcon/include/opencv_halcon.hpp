#pragma once
#include "config.h"
namespace cvt{
	OPENCV_HALCON_EXPORTS void Mat2HObject8UC1(cv::Mat& image, Halcon::Hobject& hImage);
	OPENCV_HALCON_EXPORTS void Mat2HObject8UC3(cv::Mat& image, Halcon::Hobject& hImage);
	OPENCV_HALCON_EXPORTS void Mat2HObject(cv::Mat& image, Halcon::Hobject& hImage);
	OPENCV_HALCON_EXPORTS void HObject2Mat8UC1(const Halcon::Hobject& HImage, cv::Mat& CVImage);
	OPENCV_HALCON_EXPORTS void tuple_to_mat_double(const Halcon::HTuple tuple, cv::Mat& mat);
	OPENCV_HALCON_EXPORTS void tuple_to_mat_1_n_double(const Halcon::HTuple tuple, cv::Mat& mat);
	OPENCV_HALCON_EXPORTS void region_to_mat(const Halcon::Hobject &Region, Mat& M, Hlong Width, Hlong Height);
	OPENCV_HALCON_EXPORTS void region_to_mat(const Halcon::Hobject &Image, const Halcon::Hobject &Region, Mat& mat);
	OPENCV_HALCON_EXPORTS void hregion_to_mat(const HRegion& Region, Mat& M, int cols, int rows);
	OPENCV_HALCON_EXPORTS void hregion_to_mat(const Mat& Image, const HRegion& Region, Mat& Binary);
}


///////////////////////////////////////// halcon to opencv tests ////////////////////////////////////////////

class Halcon_OpenCVTest : public cvtest::ArrayTest
{
public:
	typedef cvtest::ArrayTest Base;
	Halcon_OpenCVTest(int in_count, int out_count);
protected:
	void trans_input_mat();
	void trans_output_to_mat();

	//函数的输入和输出都是Hmat数据类型，输入Mat->输入Hmat->输出Hmat->输出Mat
	vector<vector<HMat> > test_hmat;
	//Halcon的输入和输出都是Hobject类型，输入Mat->输入Hobject->输出Hobject->输出Mat
	vector<vector<Halcon::Hobject>> test_hobject;

private:
	template<typename T1, typename T2>
	void resize_vector(vector<vector<T1>>& test_vector, const vector<vector<T2>>& test_mat);

};