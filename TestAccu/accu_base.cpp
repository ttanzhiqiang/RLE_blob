#include "test_precomp.hpp"

Halcon_Blob_Test::Halcon_Blob_Test(string Filename,int in_count, int out_count)
{
	int i;
	for (i = 0; i < in_count; i++)
		test_array[INPUT].push_back(NULL);

	for (i = 0; i < out_count; i++)
	{
		test_array[OUTPUT].push_back(NULL);
		test_array[REF_OUTPUT].push_back(NULL);
	}

	resize_vector(test_hmat, test_array);
	resize_vector(test_hobject, test_array);

	element_wise_relative_error = false;
}

template<typename T1, typename T2>
void Halcon_Blob_Test::resize_vector(vector<vector<T1>>& test_vector, const vector<vector<T2>>& test_mat)
{
	test_vector.resize(test_mat.size());
	for (int i = 0; i < test_mat.size(); i++)
	{
		test_vector[i].resize(test_mat[i].size());
	}
}

void Halcon_Blob_Test::trans_input_mat()
{
	for (int i = 0; i < test_mat[INPUT].size(); i++)
	{
		test_hmat[INPUT][i] = test_mat[INPUT][i];
		cvt::Mat2HObject(test_mat[INPUT][i], test_hobject[INPUT][i]);
	}
}

void Halcon_Blob_Test::trans_output_to_mat()
{
	int i;
	for (i = 0; i < test_mat[OUTPUT].size(); i++)
	{
		test_mat[OUTPUT][i] = test_hmat[OUTPUT][i];
		cvt::HObject2Mat8UC1(test_hobject[OUTPUT][i], test_mat[REF_OUTPUT][i]);
	}
}