#pragma once
//#include "test_precomp.hpp"
class Halcon_Blob_Test : public cvtest::FolderTest
{
public:
	typedef cvtest::FolderTest Base;
	Halcon_Blob_Test(string filename,int in_count, int out_count);
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