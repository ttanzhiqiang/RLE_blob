#include "../include/test_precomp.hpp"

namespace cvt{
	void Mat2HObject8UC1(cv::Mat& image, Halcon::Hobject& hImage)
	{
		CV_Assert(image.type() == CV_8UC1);

		int hgt = image.rows;
		int wid = image.cols;

		uchar* data = new uchar[hgt*wid];
		for (int i = 0; i < hgt; i++)
			memcpy(data + wid*i, image.data + image.step*i, wid);
		gen_image1(&hImage, "byte", wid, hgt, (Hlong)data);
		delete[] data;
		data = NULL;
	}

	void Mat2HObject8UC3(cv::Mat& image, Halcon::Hobject& hImage)
	{
		CV_Assert(image.type() == CV_8UC3);

		int hgt = image.rows;
		int wid = image.cols;

		std::vector<cv::Mat> imgchannel;
		split(image, imgchannel);
		cv::Mat imgB = imgchannel[0];
		cv::Mat imgG = imgchannel[1];
		cv::Mat imgR = imgchannel[2];
		uchar* dataR = new uchar[hgt*wid];
		uchar* dataG = new uchar[hgt*wid];
		uchar* dataB = new uchar[hgt*wid];
		for (int i = 0; i < hgt; i++)
		{
			memcpy(dataR + wid*i, imgR.data + imgR.step*i, wid);
			memcpy(dataG + wid*i, imgG.data + imgG.step*i, wid);
			memcpy(dataB + wid*i, imgB.data + imgB.step*i, wid);
		}
		gen_image3(&hImage, "byte", wid, hgt, (Hlong)dataR, (Hlong)dataG, (Hlong)dataB);

		delete[]dataR;
		delete[]dataG;
		delete[]dataB;
		dataR = NULL;
		dataG = NULL;
		dataB = NULL;
	}

	void Mat2HObject(cv::Mat& image, Halcon::Hobject& hImage)
	{
		//	CV_8UC3
		if (image.type() == CV_8UC3)
		{
			Mat2HObject8UC3(image, hImage);
		}
		//	CV_8UCU1
		else if (image.type() == CV_8UC1)
		{
			Mat2HObject8UC1(image, hImage);
		}
		else
		{
			CV_Error(CV_StsUnsupportedFormat, "Mat2HObject only supports byte image!");
		}
	}

	void HObject2Mat8UC1(const Halcon::Hobject& HImage, cv::Mat& CVImage)
	{
		//char filename[50] = "halcon.bmp";
		//Halcon::write_image(HImage, "bmp", 0, filename);
		//Halcon::Hobject HImage2;
		//Halcon::read_image(&HImage2,filename);
		//CVImage = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

		char type[50];
		Hlong PixelPointer, width, height;

		get_image_type(HImage, type);
		get_image_size(HImage, &width, &height);
		Halcon::get_image_pointer1(HImage, &PixelPointer, type, &width, &height);

		CV_Assert(strcmp(type, "byte") == 0 || strcmp(type, "int1") == 0 || strcmp(type, "int2") == 0);
		if (strcmp(type, "byte") == 0){
			CVImage.create((int)height, (int)width, CV_8UC1);
		}
		else if (strcmp(type, "int1") == 0){
			CVImage.create((int)height, (int)width, CV_8SC1);
		}
		else{
			CVImage.create((int)height, (int)width, CV_16SC1);
			width <<= 1;
		}

		for (int i = 0; i < height; i++)
			memcpy(CVImage.data + CVImage.step*i, (uchar*)PixelPointer + width*i, width);

	}


	void region_to_mat(const Halcon::Hobject &Image, const Halcon::Hobject &Region, Mat& M)
	{
		Hlong Width, Height;

		Halcon::get_image_size(Image, &Width, &Height);
		region_to_mat(Region, M, Width, Height);
	}

	void region_to_mat(const Halcon::Hobject &Region, Mat& M, Hlong Width, Hlong Height)
	{
		Halcon::Hobject Binary;
		Halcon::Hobject ImageResult;

		Halcon::gen_image_const(&Binary, "byte", Width, Height);
		Halcon::paint_region(Region, Binary, &ImageResult, 255, "fill");

		HObject2Mat8UC1(ImageResult, M);
	}

	void hregion_to_mat(const Mat& Image, const HRegion& Region, Mat& M)
	{
		hregion_to_mat(Region, M, Image.cols, Image.rows);
	}

	void hregion_to_mat(const HRegion& Region, Mat& M, int cols, int rows)
	{
		HMat Binary;
		gen_image_const(Binary, "byte", cols, rows);
		paint_region(Region, Binary, Binary, 255, "fill");
		M = Binary;
	}

	void tuple_to_mat_double(const Halcon::HTuple tuple, cv::Mat& mat)
	{
		Hlong Length;
		Halcon::tuple_length(tuple, &Length);
		mat.create(1, (int)Length, CV_64FC1);
		for (int i = 0; i < (int)Length; i++)
		{
			mat.at<double>(i, 0) = tuple[i].D();
		}
	}

	void tuple_to_mat_1_n_double(const Halcon::HTuple tuple, cv::Mat& mat)
	{
		Hlong Length;
		Halcon::tuple_length(tuple, &Length);
		int cnt = MIN(mat.rows, (int)Length);
		int i = 0;
		for (; i < cnt; i++)
		{
			mat.at<double>(i, 0) = tuple[i].D();
		}
		for (; i < mat.rows; i++)
		{
			mat.at<double>(i, 0) = 0;
		}
	}
}