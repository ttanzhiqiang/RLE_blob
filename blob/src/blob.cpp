#include "../include/blob.h"

namespace blob{
	void threshold(const HMat& Image, HRegion& dst, double MinGray, double MaxGray){
		int iRows = Image.rows;
		int iCols = Image.cols;
		assert(Image.type() == CV_8UC1);

		short* rle_x_start = dst.rle_x_start;
		short* rle_y = dst.rle_y;
		short* rle_x_end = dst.rle_x_end;

		dst.Image_Width = Image.cols;
		dst.Image_Height = Image.rows;
		
		HRegion Region1, Region2, Region3, Region4;

		#pragma omp parallel sections
		{
		#pragma omp section
			threshold2(Image, Region1, 0, iRows / 4, MinGray, MaxGray);
		#pragma omp section
			threshold2(Image, Region2, iRows / 4, iRows / 2, MinGray, MaxGray);
		#pragma omp section
			threshold2(Image, Region3, iRows / 2, iRows* 3 / 4, MinGray, MaxGray);
		#pragma omp section
			threshold2(Image, Region4, iRows * 3 / 4, iRows, MinGray, MaxGray);
		}

		for (int i = 0; i < Region1.len;i++){
			*rle_x_start++ = Region1.rle_x_start[i];
			*rle_y++ = Region1.rle_y[i];
			*rle_x_end++ = Region1.rle_x_end[i];
		}

		for (int i = 0; i < Region2.len; i++){
			*rle_x_start++ = Region2.rle_x_start[i];
			*rle_y++ = Region2.rle_y[i];
			*rle_x_end++ = Region2.rle_x_end[i];
		}

		for (int i = 0; i < Region3.len; i++){
			*rle_x_start++ = Region3.rle_x_start[i];
			*rle_y++ = Region3.rle_y[i];
			*rle_x_end++ = Region3.rle_x_end[i];
		}

		for (int i = 0; i < Region4.len; i++){
			*rle_x_start++ = Region4.rle_x_start[i];
			*rle_y++ = Region4.rle_y[i];
			*rle_x_end++ = Region4.rle_x_end[i];
		}
		dst.len = int(rle_x_start - dst.rle_x_start);
	}

	void gen_image_const(HMat &Image, const char *Type, int Width, int Height)
	{
		if (strcmp(Type, "byte") == 0)
			Image = Mat::zeros(Height, Width, CV_8UC1);
		else
			CV_Error(CV_StsUnsupportedFormat, "gen_image_const only supports byte image!");
	}

	void paint_region(const HRegion &Region, const HMat &Image, HMat &ImageResult, Scalar Grayval, const char *Type)
	{
		ImageResult = Image;
		if (strcmp(Type, "fill") == 0)
		{
			for (int i = 0; i< Region.len;i++){
				for (int j = Region.rle_x_start[i]; j < Region.rle_x_end[i]; ++j){
					ImageResult.at<uchar>(Region.rle_y[i],j) = 255;
				}
			}
			for (int i = 0; i < Region.len; i++){
				ImageResult.row(Region.rle_y[i]).colRange(Region.rle_x_start[i], Region.rle_x_end[i]) = Grayval;
			}
		}
		else if (strcmp(Type, "margin") == 0)
		{
			for (int i = 0; i< Region.len; i++){
				ImageResult.at<unsigned char>(Region.rle_y[i], Region.rle_x_start[i]) = 255;
				ImageResult.at<unsigned char>(Region.rle_y[i], Region.rle_x_end[i]) = 255;
			}
		}
		else
		{
			CV_Error(CV_StsUnsupportedFormat, "paint_region only supports type of fill or margin!");
		}
	}

	void read_image(HMat &Image, const char *FileName, int flag)
	{
		Image = imread(FileName, flag);
	}

	void decompose3(const HMat& Image, HMat& ImageR, HMat& ImageG, HMat& ImageB)
	{
		vector<Mat> channels;
		split(Image, channels);
		ImageB = channels.at(0);
		ImageG = channels.at(1);
		ImageR = channels.at(2);
	}
}

void threshold2(const HMat& Image, HRegion& Region, int start_row, int end_row, double MinGray, double MaxGray){
	int iRows = Image.rows;
	int iCols = Image.cols;
	Region.Image_Width = Image.cols;
	Region.Image_Height = Image.rows;
	short* rle_x_start = Region.rle_x_start;
	short* rle_y = Region.rle_y;
	short* rle_x_end = Region.rle_x_end;
		for (int i = start_row; i < end_row; i++)
		{
			const uchar* first = Image.ptr<uchar>(i);
			int j = 0;
			int blobStart;
			do{
				while (j < iCols && (first[j] < MinGray || first[j] > MaxGray))
				{
					++j;
				}
				if (j == iCols)//≈–∂œ «∑Ò‘ΩΩÁ
					break;
				blobStart = j;
				while (j < iCols && (first[j] >= MinGray && first[j] <= MaxGray))
				{
					++j;
				}
			{
				*rle_x_start++ = blobStart;
				*rle_y++ = i;
				*rle_x_end++ = j - 1;
			}
			} while (iCols != j);
		}
		Region.len = int(rle_x_start - Region.rle_x_start);
}