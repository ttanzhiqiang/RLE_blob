#pragma once
#define max_run_length 300000
class BLOB_EXPORTS HRegion
{
public:
	HRegion();
	HRegion(const HRegion& Region);
	HRegion & operator=(HRegion const & Region);
	virtual ~HRegion();
public:
	//成员函数
	int const Width() const;
	int const Height() const;
	int const Area() const;
	double const Circularity() const;
	double const Rectangularity() const;

	//成员变量
	int Image_Width;//当前图片的宽度
	int Image_Height;//当前图片的高度
	short* rle_x_start;//行程编码的列起始值
	short* rle_y;//行程编码的列结束值
	short* rle_x_end;//行程编码的行值
	int len;//行程编码长度
	vector<int> runLabels;//当前Region的标签值
	int runLabels_Count;//当前Region在Connection下包含多少Region
private:
	int count;//复制构造函数叠加值
};

typedef struct structMinAndMax{
	double Min = 0;
	double Max = 0;
	double Value = 0;
	string Name;
	bool flag = false;//判断当前属性是否存在
}MinMaxParam;


typedef struct structCond{
	MinMaxParam Width;
	MinMaxParam Height;
	MinMaxParam Area;
}CondParam;

typedef struct structListHRegion{
	HRegion* Region;
	int count = 0;
}ListHRegion;