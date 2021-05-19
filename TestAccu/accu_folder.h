#pragma once

namespace cvtest
{
	class FolderTest :public BaseTest
	{
	public:
		FolderTest();
		virtual ~FolderTest();

		virtual void clear();

	protected:
		virtual int prepare_test_case(int test_case_idx);
		virtual int validate_test_results(int test_case_idx);
		virtual void prepare_to_validation(int test_case_idx);
		virtual double get_success_error_level(int test_case_idx, int i, int j);

		bool cvmat_allowed;
		bool iplimage_allowed;
		bool optional_mask;
		bool element_wise_relative_error;

		int min_log_array_size;
		int max_log_array_size;

		enum { INPUT, INPUT_OUTPUT, OUTPUT, REF_INPUT_OUTPUT, REF_OUTPUT, TEMP, MASK, MAX_ARR };

		vector<vector<Mat>> test_mat;
		float buf[4];
		HMat Image, GrayImage;
		string source_folder;
		string fileFullName;
		vector<string> fileNames;
		Directory dir;
	};
}