#include "blob_precomp.hpp"
#include "../include/internal/region.h"
#include "../include/internal/mat.h"

namespace blob{
	BLOB_EXPORTS void erosion_rectangle1(const HRegion&  src, HRegion& dst, int width, int height);
	BLOB_EXPORTS void dilation_rectangle1(const HRegion& src, HRegion& dst, int width, int height);
	BLOB_EXPORTS void opening_rectangle1(const HRegion& src, HRegion& dst, int width, int height);
	BLOB_EXPORTS void closing_rectangle1(const HRegion& src, HRegion& dst, int width, int height);

	BLOB_EXPORTS void erosion_circle(const HRegion& src, HRegion& dst, double Radius);
	BLOB_EXPORTS void dilation_circle(const HRegion& src, HRegion& dst, double Radius);
	BLOB_EXPORTS void opening_circle(const HRegion& src, HRegion& dst, double Radius);
	BLOB_EXPORTS void closing_circle(const HRegion& src, HRegion& dst, double Radius);

	BLOB_EXPORTS void gen_circle(HRegion &Circle, double Row, double Column, double Radius);
	BLOB_EXPORTS void gen_rectangle1(HRegion &Rectangle,double Row1,double Column1,double Row2,double Column2);
	
	BLOB_EXPORTS void intersection1(const ListHRegion& rle, HRegion& dst);
	BLOB_EXPORTS void intersection2(const HRegion& rle, const HRegion &Region, HRegion& dst);
	BLOB_EXPORTS void union1(const ListHRegion& rle, HRegion& dst);
	BLOB_EXPORTS void union2(const HRegion& rle, const HRegion &Region, HRegion& dst);
	BLOB_EXPORTS void difference(const HRegion& rle, const HRegion &Region, HRegion& dst);

	BLOB_EXPORTS void concat_obj(const HRegion&rle, const HRegion &Region, ListHRegion& dst);
	BLOB_EXPORTS void connection(const HRegion& Region, HRegion& ConnectedRegions);
	BLOB_EXPORTS void select_obj(const HRegion& Region, HRegion& SelectedRegions, int Region_ID);
	BLOB_EXPORTS void select_shape(const HRegion& Region, HRegion& SelectedRegions,CondParam& m_CondParam);
	BLOB_EXPORTS void area_center(HRegion& Region, int &Area, float &Row, float &Column);
	BLOB_EXPORTS void select_shape_max_std(const HRegion& Region, HRegion& SelectedRegions);
}
//erosion_rectangle1 start
int erosion_blocks(const HRegion& rle,HRegion& dst,int height, int rank);

void translate(const HRegion& rle, HRegion& dst,int offset_x, int offset_y);

int erosion_rectangel_row(const HRegion& rle, HRegion& dst, int width);
//end


//dilation_rectangle1 start
int dilation_rectangel_row(const HRegion& rle, HRegion& dst, int width);

int dilation_blocks(const HRegion& rle, HRegion& dst, int height, int rank);

void dilation_union(const HRegion& rle, HRegion& dst);

void dilation_translate(const HRegion& rle, HRegion& dst, int offset_x, int offset_y);
//end


//closing_start 
void closing_translate(const HRegion& rle, HRegion& dst, int offset_x, int offset_y);
//end

//erosion_circle start
typedef struct structErosionDilationCircle{
	int col_start;
	int col_end;
	int row;
	int len;
	int count = 0;
}ErosionDilationCircleParam;

int erosion_circle_row(const HRegion& rle, HRegion& dst, int start_x, int len, int dy);

void erosion_intersection(const HRegion& rle, ErosionDilationCircleParam m_ErosionCircleParam, HRegion& dst);

void erosion_intersection2(const HRegion& rle, ErosionDilationCircleParam m_ErosionCircleParam, ErosionDilationCircleParam m_ErosionCircleParam1, HRegion& dst, HRegion& dst1);

void circle_translate(const HRegion& rle, HRegion& dst, int offset_x, int offset_y);
//end

//dilation_circle_start
int dilation_circle_row(const HRegion& rle, HRegion& dst, int start_x, int len, int dy);

void dilation_union1(const HRegion& rle, ErosionDilationCircleParam m_DilationCircleParam, HRegion& dst);

void dilation_union2(const HRegion& rle, ErosionDilationCircleParam m_DilationCircleParam, ErosionDilationCircleParam m_DilationCircleParam1, HRegion& dst, HRegion& dst1);
//end

//union start
void union_two_row(const short* rle_x, const short* rle_end_x, const short* region_x, const short* region_end_x, short** row_rle_x, short** row_rle_y, short** row_rle_end_x,
	short row, int startA, int endA, int startB, int endB);
//end

//intersect start 
void intersect_two_row(const short* rle_x, const short* rle_end_x, const short* region_x, const short* region_end_x, short** row_rle_x, short** row_rle_y, short** row_rle_end_x,
	short row, int startA, int endA, int startB, int endB);

int intersection(int & xe, int const & r_start, int const & r_len);
//end

//difference start
void difference_two_row(const short* rle_x, const short* rle_end_x, const short* region_x, const short* region_end_x, short** row_rle_x, short** row_rle_y, short** row_rle_end_x,
	short row, int startA, int endA, int startB, int endB);
//end


//connection start
int labeling(const short* rle_x, const short* rle_end_x, const short* rle_y, int len, vector<int>& runLabels, int offset);

//unite the two trees containing nodes i and j and return the new root
template<typename LabelT>
inline static
LabelT set_union(LabelT *P, LabelT i, LabelT j);

//Flatten the Union Find tree and relabel the components
template<typename LabelT>
inline static
LabelT flattenL(LabelT *P, LabelT length);

//Find the root of the tree of node i
template<typename LabelT>
inline static
LabelT findRoot(const LabelT *P, LabelT i);

//Make all nodes in the path of node i point to root
template<typename LabelT>
inline static
void setRoot(LabelT *P, LabelT i, LabelT root);

//Find the root of the tree of the node i and compress the path in the process
template<typename LabelT>
inline static
LabelT find(LabelT *P, LabelT i);
//end

//select_shape start 
//πÈ≤¢≈≈–Ú
void Merge(int A[], int left, int mid, int right);
void MergeSortRecursion(int A[], int left, int right);
void MergeSortIteration(int A[], int len);

int IsEqualInt(int number, int m_runLabels_count[], int len);
//end