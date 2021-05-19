/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "accu_precomp.hpp"

namespace cvtest
{

	static const int default_test_case_count = 203;//318
	//static const int default_test_case_count = 318;
	static const int default_max_log_array_size = 9;
	FolderTest::FolderTest()
	{
		source_folder = "D:/tzq/AVI Project/A/pic_test/";
		//source_folder = "D:/tzq/AVI Project/A/pic/";
	    fileNames = dir.GetListFiles(source_folder, "*.bmp", false);
		test_case_count = default_test_case_count;
		iplimage_allowed = true;
		cvmat_allowed = true;
		optional_mask = false;
		min_log_array_size = 0;
		max_log_array_size = default_max_log_array_size;
		element_wise_relative_error = true;
		test_mat.resize(MAX_ARR);
	}

	FolderTest::~FolderTest()
	{
		clear();
	}


	void FolderTest::clear()
	{
		for (size_t i = 0; i < test_mat.size(); i++)
		{
			for (size_t j = 0; j < test_mat[i].size(); j++)
				test_mat[i][j].release();
		}
		BaseTest::clear();
	}




	static const unsigned int icvTsTypeToDepth[] =
	{
		IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16U, IPL_DEPTH_16S,
		IPL_DEPTH_32S, IPL_DEPTH_32F, IPL_DEPTH_64F
	};


	int FolderTest::prepare_test_case(int test_case_idx)
	{
		fileFullName = source_folder + fileNames[test_case_idx];
		//cout << "test_case_idx=" << test_case_idx << endl;
		Image = imread(fileFullName.c_str());
		cvtColor(Image, GrayImage, CV_RGB2GRAY);
		int code = 1;
		return code;
	}


	double FolderTest::get_success_error_level(int /*test_case_idx*/, int i, int j)
	{
		int elem_depth = test_mat[i][j].depth();
		assert(i == OUTPUT || i == INPUT_OUTPUT);
		return elem_depth < CV_32F ? 0 : elem_depth == CV_32F ? FLT_EPSILON * 100 : DBL_EPSILON * 5000;
	}


	void FolderTest::prepare_to_validation(int /*test_case_idx*/)
	{
		assert(0);
	}


	int FolderTest::validate_test_results(int test_case_idx)
	{
		static const char* arr_names[] = { "input", "input/output", "output",
			"ref input/output", "ref output",
			"temporary", "mask" };
		size_t i, j;
		prepare_to_validation(test_case_idx);

		for (i = 0; i < 2; i++)
		{
			int i0 = i == 0 ? OUTPUT : INPUT_OUTPUT;
			int i1 = i == 0 ? REF_OUTPUT : REF_INPUT_OUTPUT;
			size_t sizei = test_mat[i0].size();

			assert(sizei == test_mat[i1].size());
			for (j = 0; j < sizei; j++)
			{
				double err_level;
				int code;

				if (test_mat[i1][j].empty())
					continue;

				err_level = get_success_error_level(test_case_idx, i0, (int)j);
				code = cmpEps2(ts, test_mat[i0][j], test_mat[i1][j], err_level, element_wise_relative_error, arr_names[i0]);

				if (code == 0)
					continue;

				for (i0 = 0; i0 < (int)test_mat.size(); i0++)
				{
					size_t sizei0 = test_mat[i0].size();
					if (i0 == REF_INPUT_OUTPUT || i0 == OUTPUT || i0 == TEMP)
						continue;
					for (i1 = 0; i1 < (int)sizei0; i1++)
					{
						const Mat& arr = test_mat[i0][i1];
						if (!arr.empty())
						{
							string sizestr = vec2str(", ", &arr.size[0], arr.dims);
							ts->printf(TS::LOG, "%s array %d type=%sC%d, size=(%s)\n",
								arr_names[i0], i1, getTypeName(arr.depth()),
								arr.channels(), sizestr.c_str());
						}
					}
				}
				ts->set_failed_test_info(code);
				return code;
			}
		}

		return 0;
	}

}

/* End of file. */
