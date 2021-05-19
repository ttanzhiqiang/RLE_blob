#include "blob_precomp.hpp"
#include "../include/internal/region.h"
#include "../include/internal/mat.h"

namespace blob{
	BLOB_EXPORTS void threshold(const HMat& Image, HRegion& dst, double MinGray, double MaxGray);
	BLOB_EXPORTS void gen_image_const(HMat& Image, const char *Type, int Width, int Height);
	BLOB_EXPORTS void paint_region(const HRegion& Region, const HMat& Image, HMat& ImageResult, Scalar Grayval, const char *Type);
	BLOB_EXPORTS void read_image(HMat& Image, const char *FileName, int flag = CV_LOAD_IMAGE_UNCHANGED);
	//将一张彩色图转为R G B三个分量
	BLOB_EXPORTS void decompose3(const HMat& Image, HMat& ImageR, HMat& ImageG, HMat& ImageB);
}

void threshold2(const HMat& Image, HRegion& Region, int start_row, int end_row, double MinGray, double MaxGray);