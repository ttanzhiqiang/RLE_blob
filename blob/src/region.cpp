#include "../include/blob_precomp.hpp"
#include "../include/internal/region.h"
HRegion::HRegion()
{
	rle_x_start = (short*)malloc(max_run_length*sizeof(short));
	rle_y = (short*)malloc(max_run_length*sizeof(short));
	rle_x_end = (short*)malloc(max_run_length*sizeof(short));
	len = 0;
	count = 0;
}

HRegion::HRegion(const HRegion& Region){
	rle_x_start = Region.rle_x_start;
	rle_y = Region.rle_y;
	rle_x_end = Region.rle_x_end;
	len = Region.len;
	count++;
}

HRegion::~HRegion(){
	if (count == 0){
		free(rle_x_start);
		free(rle_y);
		free(rle_x_end);
	}
	else{
		count--;
	}
}

HRegion &HRegion::operator=(const HRegion& Region){
	for (int i = 0; i < Region.len;i++){
		rle_x_start[i] = Region.rle_x_start[i];
		rle_y[i] = Region.rle_y[i];
		rle_x_end[i] = Region.rle_x_end[i];
	}
	Image_Width = Region.Image_Width;
	Image_Height = Region.Image_Height;
	len = Region.len;
	runLabels = Region.runLabels;
	runLabels_Count = Region.runLabels_Count;
	return *this;
}

int const HRegion::Width() const{
	int Width = 0;
	int min = 9999;
	int max = 0;
	for (int i = 0; i < this->len;i++){
		if (min>rle_x_start[i]){
			min = rle_x_start[i];
		}
		if (max<rle_x_end[i]){
			max = rle_x_end[i];
		}
	}
	Width = max - min + 1;
	return Width;
}

int const HRegion::Height() const{
	int Height = rle_y[len - 1] - *rle_y +1;
	return Height;
}

int const HRegion::Area() const{
	int Area = 0;
	for (int i = 0; i < len; i++){
		Area += rle_x_end[i] - rle_x_start[i] + 1;
	}
	return Area;
}

double const HRegion::Circularity() const{
	double Circularity = 0.0;
	return Circularity;
}

double const HRegion::Rectangularity() const{
	double Rectangularity =0.0;
	/*int minx;
	int maxx;
	int miny = *rle_y;
	int maxy = rle_y[len - 1];

	short* rle_x = rle_x_start;
	for (int i = 0; i < len; i++){
		int sx = rle_x_start[i];
		int ex = rle_x_end[i];
		minx = min(minx, sx);
		maxx = max(maxx, ex);
	}

	int Area_Rectangle;
	for (int i = 0; i < maxx - minx + 1;i++){
		Area_Rectangle = (maxx - minx + 1)*(maxy - miny + 1);
	}
	Rectangularity = (double)Area_Rectangle / Area();*/
	return Rectangularity;
}
