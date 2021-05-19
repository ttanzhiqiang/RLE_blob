#include "../include/blob_precomp.hpp"
#include "../include/morph.h"
#include <fstream>

namespace blob{
	void erosion_rectangle1(const HRegion& src, HRegion& dst, int width, int height){
		//当width=20时，需构建1，1&&2，1&&2&&3&&4，1&&2&&3&&4&&5&&6&&7&&8,...
		dst.Image_Width = src.Image_Width;
		dst.Image_Height = src.Image_Height;
		int step = 1;
		int rank = 1;

		//获得需要做交集的次数,时间复杂度为(log2^(n)),n为高度
		while (step < height)
		{
			step <<= 1;
			++rank;
		}

		HRegion *erosion_temp = new HRegion[rank];
		for (int i = 0; i < rank; ++i)
		{
			erosion_temp[i].Image_Width = src.Image_Width;
			erosion_temp[i].Image_Height = src.Image_Height;
		}

		//step1:erosion_temp[0].len表示为去掉不满足宽度要求的行程时候的行程总长度
		erosion_temp[0].len = erosion_rectangel_row(src, erosion_temp[0], width);

		//step2:当前级数取交集获得下一个级数的Region
		for (int i = 1; i < rank; ++i)
		{
			erosion_temp[i].len = erosion_blocks(erosion_temp[i - 1], erosion_temp[i], height, i);
		}

		--rank;

		//step3:将行程的值移动到原点的值,x的偏移为log2^(width -1),y的偏移为log2^(height -1)
		int offset_x = ((width - 1) >> 1);
		int offset_y = ((height - 1) >> 1);
		translate(erosion_temp[rank], dst, offset_x, offset_y);
		
		//释放内存
		delete[] erosion_temp;
	}

	void dilation_rectangle1(const HRegion& src, HRegion& dst, int width, int height){
		dst.Image_Width = src.Image_Width;
		dst.Image_Height = src.Image_Height;

		//获得需要做并集的次数,时间复杂度为(log2^(n)),n为高度
		int step = 1;
		int rank = 1;
		while (step < height)
		{
			step <<= 1;
			++rank;
		}
		HRegion dilation_width;
		HRegion *dilation_temp = new HRegion[rank];
		for (int i = 0; i < rank; ++i)
		{
			dilation_temp[i].Image_Width = src.Image_Width;
			dilation_temp[i].Image_Height = src.Image_Height;
		}

		//step1:给src的每个行程编码添加width
		dilation_width.len = dilation_rectangel_row(src, dilation_width, width);
		
		//step2:给当前Region合并行程编码,输出dilation_temp[0]
		dilation_union(dilation_width, dilation_temp[0]);

		//step3:当前级数取并集获得下一个级数的Region
		for (int i = 1; i < rank; ++i)
		{
			dilation_temp[i].len = dilation_blocks(dilation_temp[i - 1], dilation_temp[i], height, i);
		}
		--rank;

		//step4:将行程的值移动到原点的值,x的偏移为log2^(width),y的偏移为log2^(height)
		int offset_x = ((width) >> 1);
		int offset_y = ((height) >> 1);
		dilation_translate(dilation_temp[rank], dst, offset_x, offset_y);
		delete[] dilation_temp;
	}

	void opening_rectangle1(const HRegion& src, HRegion& dst, int width, int height){
		//开运算先腐蚀后膨胀
		HRegion Region;
		Region.Image_Width = src.Image_Width;
		Region.Image_Height = src.Image_Height;
		erosion_rectangle1(src, Region,width,height);
		dilation_rectangle1(Region, dst, width, height);
	}

	void closing_rectangle1(const HRegion& src, HRegion& dst, int width, int height){
		//闭运算先膨胀后运算
		HRegion Region;
		Region.Image_Width = src.Image_Width;
		Region.Image_Height = src.Image_Height;

		dst.Image_Width = src.Image_Width;
		dst.Image_Height = src.Image_Height;

		//获得需要做并集的次数,时间复杂度为(log2^(n)),n为高度
		int step = 1;
		int rank = 1;
		while (step < height)
		{
			step <<= 1;
			++rank;
		}
		HRegion dilation_width;
		HRegion *dilation_temp = new HRegion[rank];
		for (int i = 0; i < rank; ++i)
		{
			dilation_temp[i].Image_Width = src.Image_Width;
			dilation_temp[i].Image_Height = src.Image_Height;
		}

		//step1:给src的每个行程编码添加width
		dilation_width.len = dilation_rectangel_row(src, dilation_width, width);

		//step2:给当前Region合并行程编码,输出dilation_temp[0]
		dilation_union(dilation_width, dilation_temp[0]);

		//step3:当前级数取并集获得下一个级数的Region
		for (int i = 1; i < rank; ++i)
		{
			dilation_temp[i].len = dilation_blocks(dilation_temp[i - 1], dilation_temp[i], height, i);
		}
		--rank;

		//step4:将行程的值移动到原点的值,x的偏移为log2^(width),y的偏移为log2^(height)
		int offset_x = ((width) >> 1);
		int offset_y = ((height) >> 1);
		//闭运算先膨胀后腐蚀,预留膨胀后的所有信息
		closing_translate(dilation_temp[rank], Region, offset_x, offset_y);
		delete[] dilation_temp;
	
		//step5:腐蚀
		erosion_rectangle1(Region, dst, width, height);
	}

	void erosion_circle(const HRegion& src, HRegion& dst, double Radius){
		dst.Image_Width = src.Image_Width;
		dst.Image_Height = src.Image_Height;

		//n.0<Radius<n.5之间Radius=n.0;  n.5<=Radius<n.0+1之间Radius=n.5; n为整数;n>0
		int Radius_Number;
		double Radius_value;
		Radius_Number = max(saturate_cast<int>(Radius), 0);
		if (Radius_Number > Radius){
			Radius_value = Radius_Number - 0.5;
		}
		else if (Radius_Number + 0.5 == Radius){
			Radius_value = Radius;
		}
		else{
			Radius_value = Radius_Number;
		}

		//获得圆形结构元素
		HRegion CircleRegion;
		gen_circle(CircleRegion, 0, 0, Radius_value);

		//step1:将圆形结构元素中相同长度且相邻的行程编码放在一起(记录当前行程编码的长度/列开始/列结束/行值)
		int numberParam;
		vector<ErosionDilationCircleParam> m_ErosionCircleParam;
		ErosionDilationCircleParam l_ErosionCircleParam;
		l_ErosionCircleParam.len = CircleRegion.rle_x_end[0] - CircleRegion.rle_x_start[0] + 1;
		l_ErosionCircleParam.col_start = CircleRegion.rle_x_start[0];
		l_ErosionCircleParam.col_end = CircleRegion.rle_x_end[0];
		l_ErosionCircleParam.row = CircleRegion.rle_y[0];
		l_ErosionCircleParam.count++;
		numberParam = 0;
		m_ErosionCircleParam.push_back(l_ErosionCircleParam);
		for (int i = 1; i < Radius_value * 2; ++i){
			if ((CircleRegion.rle_x_end[i] - CircleRegion.rle_x_start[i] + 1) == (CircleRegion.rle_x_end[i-1] - CircleRegion.rle_x_start[i-1] + 1)){
				m_ErosionCircleParam[numberParam].count++;
			}
			else{
				ErosionDilationCircleParam temp_ErosionCircleParam;
				temp_ErosionCircleParam.len = CircleRegion.rle_x_end[i] - CircleRegion.rle_x_start[i] + 1;
				temp_ErosionCircleParam.col_start = CircleRegion.rle_x_start[i];
				temp_ErosionCircleParam.col_end = CircleRegion.rle_x_end[i];
				temp_ErosionCircleParam.row = CircleRegion.rle_y[i];
				temp_ErosionCircleParam.count++;
				numberParam++;
				m_ErosionCircleParam.push_back(temp_ErosionCircleParam);
			}
		}

		//参数初始化
		HRegion *erosion_width_temp = new HRegion[m_ErosionCircleParam.size() / 2 + 1];
		for (int i = 0; i < m_ErosionCircleParam.size()/2 +1; ++i)
		{
			erosion_width_temp[i].Image_Width = src.Image_Width;
			erosion_width_temp[i].Image_Height = src.Image_Height;
		}


		HRegion *erosion_intersection_temp = new HRegion[m_ErosionCircleParam.size()];
		for (int i = 0; i < m_ErosionCircleParam.size(); ++i)
		{
			erosion_intersection_temp[i].Image_Width = src.Image_Width;
			erosion_intersection_temp[i].Image_Height = src.Image_Height;
		}


		HRegion *erosion_temp = new HRegion[m_ErosionCircleParam.size()];
		for (int i = 0; i < m_ErosionCircleParam.size(); ++i)
		{
			erosion_temp[i].Image_Width = src.Image_Width;
			erosion_temp[i].Image_Height = src.Image_Height;
		}

		//step2：相同长度的矩形块算出减去宽度后对应的行程编码的数据(这里的个数为step1的一半加1,因为圆形结构元素是对称的,算一半即可推出另一半)
		for (int j = 0; j <m_ErosionCircleParam.size() / 2 + 1; ++j){
			erosion_width_temp[j].len = erosion_circle_row(src, erosion_width_temp[j], m_ErosionCircleParam[j].col_start, m_ErosionCircleParam[j].len, m_ErosionCircleParam[j].row + m_ErosionCircleParam[j].count - 1);
		}

		//step3:计算圆形结构元素最中间长度相等且相邻的行程编码相互取交集后的结果(方法参考矩形元素)
		erosion_intersection(erosion_width_temp[m_ErosionCircleParam.size() / 2], m_ErosionCircleParam[m_ErosionCircleParam.size() / 2], erosion_intersection_temp[m_ErosionCircleParam.size() / 2]);

		//step4:计算圆形结构元素长度相等且相邻的行程编码相互取交集后的结果(方法参考矩形元素)(圆形结构元素上下对称,移动y值可以获得另一半)
		for (int j = 0; j <m_ErosionCircleParam.size()/2; ++j){
			erosion_intersection2(erosion_width_temp[j], m_ErosionCircleParam[j], m_ErosionCircleParam[m_ErosionCircleParam.size()-j-1], erosion_intersection_temp[j], erosion_intersection_temp[m_ErosionCircleParam.size() - j -1]);
		}

		//参数赋值,优化内存处理
		erosion_temp[0] = erosion_intersection_temp[0];

		//step5:计算剩下所有Region的交集
		for (int k = 1; k < m_ErosionCircleParam.size(); ++k)
		{
			intersection2(erosion_temp[k - 1], erosion_intersection_temp[k], erosion_temp[k]);
		}

		//step6:将行程的值移动到原点的值
		//n.0<Radius<n.5之间x向左移动一,y无偏移;n.5<=Radius<n.0+1之间x,y无偏移; n为整数;n>0
		//在圆形结构元素中做腐蚀,半径为整数时,腐蚀宽高为偶数,中心应稍偏左一点;其他不变
		int offset_x;
		int offset_y;
		if (Radius_Number > Radius){
			offset_x = 0;
			offset_y = 0;
		}
		else if (Radius_Number + 0.5 == Radius){
			offset_x = 0;
			offset_y = 0;
		}
		else{
			offset_x = -1;
			offset_y = 0;
		}
		translate(erosion_temp[m_ErosionCircleParam.size() - 1], dst, offset_x, offset_y);

		//step7:内存释放
		m_ErosionCircleParam.clear();
		delete[] erosion_width_temp;
		delete[] erosion_intersection_temp;
		delete[] erosion_temp;
	}

	void dilation_circle(const HRegion& src, HRegion& dst, double Radius){
		dst.Image_Width = src.Image_Width;
		dst.Image_Height = src.Image_Height;

		//n.0<Radius<n.5之间Radius=n.0;  n.5<=Radius<n.0+1之间Radius=n.5; n为整数;n>0
		int Radius_Number;
		double Radius_value;
		Radius_Number = max(saturate_cast<int>(Radius), 0);
		if (Radius_Number > Radius){
			Radius_value = Radius_Number - 0.5;
		}
		else if (Radius_Number + 0.5 == Radius){
			Radius_value = Radius;
		}
		else{
			Radius_value = Radius_Number;
		}

		//获得圆形结构元素
		HRegion CircleRegion;
		gen_circle(CircleRegion, 0, 0, Radius_value);

		//step1:将圆形结构元素中相同长度且相邻的行程编码放在一起(记录当前行程编码的长度/列开始/列结束/行值)
		int numberParam;
		vector<ErosionDilationCircleParam> m_DilationCircleParam;
		ErosionDilationCircleParam l_DilationCircleParam;
		l_DilationCircleParam.len = CircleRegion.rle_x_end[0] - CircleRegion.rle_x_start[0] + 1;
		l_DilationCircleParam.col_start = CircleRegion.rle_x_start[0];
		l_DilationCircleParam.col_end = CircleRegion.rle_x_end[0];
		l_DilationCircleParam.row = CircleRegion.rle_y[0];
		l_DilationCircleParam.count++;
		numberParam = 0;
		m_DilationCircleParam.push_back(l_DilationCircleParam);
		for (int i = 1; i < Radius_value * 2; ++i){
			if ((CircleRegion.rle_x_end[i] - CircleRegion.rle_x_start[i] + 1) == (CircleRegion.rle_x_end[i - 1] - CircleRegion.rle_x_start[i - 1] + 1)){
				m_DilationCircleParam[numberParam].count++;
			}
			else{
				ErosionDilationCircleParam temp_DilationCircleParam;
				temp_DilationCircleParam.len = CircleRegion.rle_x_end[i] - CircleRegion.rle_x_start[i] + 1;
				temp_DilationCircleParam.col_start = CircleRegion.rle_x_start[i];
				temp_DilationCircleParam.col_end = CircleRegion.rle_x_end[i];
				temp_DilationCircleParam.row = CircleRegion.rle_y[i];
				temp_DilationCircleParam.count++;
				numberParam++;
				m_DilationCircleParam.push_back(temp_DilationCircleParam);
			}
		}

		//参数初始化
		HRegion *Dilation_width_temp = new HRegion[m_DilationCircleParam.size() / 2 + 1];
		for (int i = 0; i < m_DilationCircleParam.size() / 2 + 1; ++i)
		{
			Dilation_width_temp[i].Image_Width = src.Image_Width;
			Dilation_width_temp[i].Image_Height = src.Image_Height;
		}

		HRegion *Dilation_Union_temp = new HRegion[m_DilationCircleParam.size() / 2 + 1];
		for (int i = 0; i < m_DilationCircleParam.size() / 2 + 1; ++i)
		{
			Dilation_Union_temp[i].Image_Width = src.Image_Width;
			Dilation_Union_temp[i].Image_Height = src.Image_Height;
		}

		HRegion *Dilation_Union = new HRegion[m_DilationCircleParam.size()];
		for (int i = 0; i < m_DilationCircleParam.size(); ++i)
		{
			Dilation_Union[i].Image_Width = src.Image_Width;
			Dilation_Union[i].Image_Height = src.Image_Height;
		}

		HRegion *Dilation_temp = new HRegion[m_DilationCircleParam.size()];
		for (int i = 0; i < m_DilationCircleParam.size(); ++i)
		{
			Dilation_temp[i].Image_Width = src.Image_Width;
			Dilation_temp[i].Image_Height = src.Image_Height;
		}

		//step2：相同长度的矩形块算出加上宽度后对应的行程编码的数据(这里的个数为step1的一半加1,因为圆形结构元素是对称的,算一半即可推出另一半)
		for (int j = 0; j <m_DilationCircleParam.size() / 2 + 1; ++j){
			Dilation_width_temp[j].len = dilation_circle_row(src, Dilation_width_temp[j], m_DilationCircleParam[j].col_start, m_DilationCircleParam[j].len, m_DilationCircleParam[j].row + m_DilationCircleParam[j].count - 1);
		}

		//step3：计算step2的行程编码取并集之后的信息
		for (int j = 0; j <m_DilationCircleParam.size() / 2 + 1; ++j){
			dilation_union(Dilation_width_temp[j], Dilation_Union_temp[j]);
		}

		//step4:计算圆形结构元素最中间长度相等且相邻的行程编码相互取并集后的结果(方法参考矩形元素)
		dilation_union1(Dilation_Union_temp[m_DilationCircleParam.size() / 2], m_DilationCircleParam[m_DilationCircleParam.size() / 2], Dilation_Union[m_DilationCircleParam.size() / 2]);
		
		//step5:计算圆形结构元素长度相等且相邻的行程编码相互取并集后的结果(方法参考矩形元素)(圆形结构元素上下对称,移动y值可以获得另一半)
		for (int j = 0; j <m_DilationCircleParam.size() / 2; ++j){
			dilation_union2(Dilation_Union_temp[j], m_DilationCircleParam[j], m_DilationCircleParam[m_DilationCircleParam.size() - j - 1], Dilation_Union[j], Dilation_Union[m_DilationCircleParam.size() - j - 1]);
		}

		//参数赋值,优化内存处理
		Dilation_temp[0] = Dilation_Union[0];

		//step6:计算行程编码的并集
		for (int k = 1; k < m_DilationCircleParam.size(); ++k)
		{
			union2(Dilation_temp[k - 1], Dilation_Union[k], Dilation_temp[k]);
		}

		//step7:将行程的值移动到原点的值
		//n.0<Radius<n.5之间x,y无偏移;n.5<=Radius<n.0+1之间x,y无偏移; n为整数;n>0
		//在圆形结构元素中做膨胀,中心均不变
		int offset_x;
		int offset_y;
		offset_x = 0;
		offset_y = 0;
		dilation_translate(Dilation_temp[m_DilationCircleParam.size() - 1], dst, offset_x, offset_y);

		//step8:内存释放
		m_DilationCircleParam.clear();
		delete[] Dilation_width_temp;
		delete[] Dilation_Union_temp;
		delete[] Dilation_Union;
		delete[] Dilation_temp;
	}

	void opening_circle(const HRegion& src, HRegion& dst, double Radius){
		//先腐蚀后膨胀
		HRegion Region;
		Region.Image_Width = src.Image_Width;
		Region.Image_Height = src.Image_Height;

		//step1:腐蚀
		erosion_circle(src, Region, Radius);

		//dilation_circle
		dst.Image_Width = Region.Image_Width;
		dst.Image_Height = Region.Image_Height;

		//n.0<Radius<n.5之间Radius=n.0;  n.5<=Radius<n.0+1之间Radius=n.5; n为整数;n>0
		int Radius_Number;
		double Radius_value;
		Radius_Number = max(saturate_cast<int>(Radius), 0);
		if (Radius_Number > Radius){
			Radius_value = Radius_Number - 0.5;
		}
		else if (Radius_Number + 0.5 == Radius){
			Radius_value = Radius;
		}
		else{
			Radius_value = Radius_Number;
		}

		//获得圆形结构元素(开运算的膨胀需考虑原点的移动)
		//n.0<Radius<n.5之间x右偏1,y右偏1;  n.5<=Radius<n.0+1之间不变; n为整数;n>0
		HRegion CircleRegion;
		if (Radius_Number > Radius){
			gen_circle(CircleRegion, 0, 0, Radius_value);
		}
		else if (Radius_Number + 0.5 == Radius){
			gen_circle(CircleRegion, 0, 0, Radius_value);
		}
		else{
			gen_circle(CircleRegion, 1, 1, Radius_value);
		}

		//step2:将圆形结构元素中相同长度且相邻的行程编码放在一起(记录当前行程编码的长度/列开始/列结束/行值)
		int numberParam;
		vector<ErosionDilationCircleParam> m_DilationCircleParam;
		ErosionDilationCircleParam l_DilationCircleParam;
		l_DilationCircleParam.len = CircleRegion.rle_x_end[0] - CircleRegion.rle_x_start[0] + 1;
		l_DilationCircleParam.col_start = CircleRegion.rle_x_start[0];
		l_DilationCircleParam.col_end = CircleRegion.rle_x_end[0];
		l_DilationCircleParam.row = CircleRegion.rle_y[0];
		l_DilationCircleParam.count++;
		numberParam = 0;
		m_DilationCircleParam.push_back(l_DilationCircleParam);
		for (int i = 1; i < Radius_value * 2; ++i){
			if ((CircleRegion.rle_x_end[i] - CircleRegion.rle_x_start[i] + 1) == (CircleRegion.rle_x_end[i - 1] - CircleRegion.rle_x_start[i - 1] + 1)){
				m_DilationCircleParam[numberParam].count++;
			}
			else{
				ErosionDilationCircleParam temp_DilationCircleParam;
				temp_DilationCircleParam.len = CircleRegion.rle_x_end[i] - CircleRegion.rle_x_start[i] + 1;
				temp_DilationCircleParam.col_start = CircleRegion.rle_x_start[i];
				temp_DilationCircleParam.col_end = CircleRegion.rle_x_end[i];
				temp_DilationCircleParam.row = CircleRegion.rle_y[i];
				temp_DilationCircleParam.count++;
				numberParam++;
				m_DilationCircleParam.push_back(temp_DilationCircleParam);
			}
		}

		//参数初始化
		HRegion *Dilation_width_temp = new HRegion[m_DilationCircleParam.size() / 2 + 1];
		for (int i = 0; i < m_DilationCircleParam.size() / 2 + 1; ++i)
		{
			Dilation_width_temp[i].Image_Width = Region.Image_Width;
			Dilation_width_temp[i].Image_Height = Region.Image_Height;
		}

		HRegion *Dilation_Union_temp = new HRegion[m_DilationCircleParam.size() / 2 + 1];
		for (int i = 0; i < m_DilationCircleParam.size() / 2 + 1; ++i)
		{
			Dilation_Union_temp[i].Image_Width = Region.Image_Width;
			Dilation_Union_temp[i].Image_Height = Region.Image_Height;
		}

		HRegion *Dilation_Union = new HRegion[m_DilationCircleParam.size()];
		for (int i = 0; i < m_DilationCircleParam.size(); ++i)
		{
			Dilation_Union[i].Image_Width = Region.Image_Width;
			Dilation_Union[i].Image_Height = Region.Image_Height;
		}

		HRegion *Dilation_temp = new HRegion[m_DilationCircleParam.size()];
		for (int i = 0; i < m_DilationCircleParam.size(); ++i)
		{
			Dilation_temp[i].Image_Width = Region.Image_Width;
			Dilation_temp[i].Image_Height = Region.Image_Height;
		}

		//step3：相同长度的矩形块算出加上宽度后对应的行程编码的数据(这里的个数为step1的一半加1,因为圆形结构元素是对称的,算一半即可推出另一半)
		for (int j = 0; j <m_DilationCircleParam.size() / 2 + 1; ++j){
			Dilation_width_temp[j].len = dilation_circle_row(Region, Dilation_width_temp[j], m_DilationCircleParam[j].col_start, m_DilationCircleParam[j].len, m_DilationCircleParam[j].row + m_DilationCircleParam[j].count - 1);
		}

		//step4：计算step2的行程编码取并集之后的信息
		for (int j = 0; j <m_DilationCircleParam.size() / 2 + 1; ++j){
			dilation_union(Dilation_width_temp[j], Dilation_Union_temp[j]);
		}

		//step5:计算圆形结构元素最中间长度相等且相邻的行程编码相互取并集后的结果(方法参考矩形元素)
		dilation_union1(Dilation_Union_temp[m_DilationCircleParam.size() / 2], m_DilationCircleParam[m_DilationCircleParam.size() / 2], Dilation_Union[m_DilationCircleParam.size() / 2]);
		
		//step6:计算圆形结构元素长度相等且相邻的行程编码相互取并集后的结果(方法参考矩形元素)(圆形结构元素上下对称,移动y值可以获得另一半)
		for (int j = 0; j <m_DilationCircleParam.size() / 2; ++j){
			dilation_union2(Dilation_Union_temp[j], m_DilationCircleParam[j], m_DilationCircleParam[m_DilationCircleParam.size() - j - 1], Dilation_Union[j], Dilation_Union[m_DilationCircleParam.size() - j - 1]);
		}

		//参数赋值,优化内存处理
		Dilation_temp[0] = Dilation_Union[0];

		//step7:计算行程编码的并集
		for (int k = 1; k < m_DilationCircleParam.size(); ++k)
		{
			union2(Dilation_temp[k - 1], Dilation_Union[k], Dilation_temp[k]);
		}

		//step8:将行程的值移动到原点的值
		int offset_x;
		int offset_y;
		offset_x = 0;
		offset_y = 0;
		dilation_translate(Dilation_temp[m_DilationCircleParam.size() - 1], dst, offset_x, offset_y);

		//step9:内存释放
		m_DilationCircleParam.clear();
		delete[] Dilation_width_temp;
		delete[] Dilation_Union_temp;
		delete[] Dilation_Union;
		delete[] Dilation_temp;
	}

	void closing_circle(const HRegion& src, HRegion& dst, double Radius){
		//先膨胀后腐蚀
		HRegion Region;
		Region.Image_Width = src.Image_Width;
		Region.Image_Height = src.Image_Height;

		//n.0<Radius<n.5之间Radius=n.0;  n.5<=Radius<n.0+1之间Radius=n.5; n为整数;n>0
		int Radius_Number;
		double Radius_value;
		Radius_Number = max(saturate_cast<int>(Radius), 0);
		if (Radius_Number > Radius){
			Radius_value = Radius_Number - 0.5;
		}
		else if (Radius_Number + 0.5 == Radius){
			Radius_value = Radius;
		}
		else{
			Radius_value = Radius_Number;
		}

		//获得圆形结构元素(开运算的膨胀需考虑原点的移动)
		//n.0<Radius<n.5之间x右偏1,y右偏1;  n.5<=Radius<n.0+1之间不变; n为整数;n>0
		HRegion CircleRegion;
		if (Radius_Number > Radius){
			gen_circle(CircleRegion, 0, 0, Radius_value);
		}
		else if (Radius_Number + 0.5 == Radius){
			gen_circle(CircleRegion, 0, 0, Radius_value);
		}
		else{
			gen_circle(CircleRegion, 1, 1, Radius_value);
		}

		//step1:将圆形结构元素中相同长度且相邻的行程编码放在一起(记录当前行程编码的长度/列开始/列结束/行值)
		int numberParam;
		vector<ErosionDilationCircleParam> m_DilationCircleParam;
		ErosionDilationCircleParam l_DilationCircleParam;
		l_DilationCircleParam.len = CircleRegion.rle_x_end[0] - CircleRegion.rle_x_start[0] + 1;
		l_DilationCircleParam.col_start = CircleRegion.rle_x_start[0];
		l_DilationCircleParam.col_end = CircleRegion.rle_x_end[0];
		l_DilationCircleParam.row = CircleRegion.rle_y[0];
		l_DilationCircleParam.count++;
		numberParam = 0;
		m_DilationCircleParam.push_back(l_DilationCircleParam);
		for (int i = 1; i < Radius_value * 2; ++i){
			if ((CircleRegion.rle_x_end[i] - CircleRegion.rle_x_start[i] + 1) == (CircleRegion.rle_x_end[i - 1] - CircleRegion.rle_x_start[i - 1] + 1)){
				m_DilationCircleParam[numberParam].count++;
			}
			else{
				ErosionDilationCircleParam temp_DilationCircleParam;
				temp_DilationCircleParam.len = CircleRegion.rle_x_end[i] - CircleRegion.rle_x_start[i] + 1;
				temp_DilationCircleParam.col_start = CircleRegion.rle_x_start[i];
				temp_DilationCircleParam.col_end = CircleRegion.rle_x_end[i];
				temp_DilationCircleParam.row = CircleRegion.rle_y[i];
				temp_DilationCircleParam.count++;
				numberParam++;
				m_DilationCircleParam.push_back(temp_DilationCircleParam);
			}
		}

		//参数初始化
		HRegion *Dilation_width_temp = new HRegion[m_DilationCircleParam.size() / 2 + 1];
		for (int i = 0; i < m_DilationCircleParam.size() / 2 + 1; ++i)
		{
			Dilation_width_temp[i].Image_Width = src.Image_Width;
			Dilation_width_temp[i].Image_Height = src.Image_Height;
		}

		HRegion *Dilation_Union_temp = new HRegion[m_DilationCircleParam.size() / 2 + 1];
		for (int i = 0; i < m_DilationCircleParam.size() / 2 + 1; ++i)
		{
			Dilation_Union_temp[i].Image_Width = src.Image_Width;
			Dilation_Union_temp[i].Image_Height = src.Image_Height;
		}

		HRegion *Dilation_Union = new HRegion[m_DilationCircleParam.size()];
		for (int i = 0; i < m_DilationCircleParam.size(); ++i)
		{
			Dilation_Union[i].Image_Width = src.Image_Width;
			Dilation_Union[i].Image_Height = src.Image_Height;
		}

		HRegion *Dilation_temp = new HRegion[m_DilationCircleParam.size()];
		for (int i = 0; i < m_DilationCircleParam.size(); ++i)
		{
			Dilation_temp[i].Image_Width = src.Image_Width;
			Dilation_temp[i].Image_Height = src.Image_Height;
		}

		//step2：相同长度的矩形块算出加上宽度后对应的行程编码的数据(这里的个数为step1的一半加1,因为圆形结构元素是对称的,算一半即可推出另一半)
		for (int j = 0; j <m_DilationCircleParam.size() / 2 + 1; ++j){
			Dilation_width_temp[j].len = dilation_circle_row(src, Dilation_width_temp[j], m_DilationCircleParam[j].col_start, m_DilationCircleParam[j].len, m_DilationCircleParam[j].row + m_DilationCircleParam[j].count - 1);
		}

		//step3：计算step2的行程编码取并集之后的信息
		for (int j = 0; j <m_DilationCircleParam.size() / 2 + 1; ++j){
			dilation_union(Dilation_width_temp[j], Dilation_Union_temp[j]);
		}

		//step4:计算圆形结构元素最中间长度相等且相邻的行程编码相互取并集后的结果(方法参考矩形元素)
		dilation_union1(Dilation_Union_temp[m_DilationCircleParam.size() / 2], m_DilationCircleParam[m_DilationCircleParam.size() / 2], Dilation_Union[m_DilationCircleParam.size() / 2]);
		
		//step5:计算圆形结构元素长度相等且相邻的行程编码相互取并集后的结果(方法参考矩形元素)(圆形结构元素上下对称,移动y值可以获得另一半)
		for (int j = 0; j <m_DilationCircleParam.size() / 2; ++j){
			dilation_union2(Dilation_Union_temp[j], m_DilationCircleParam[j], m_DilationCircleParam[m_DilationCircleParam.size() - j - 1], Dilation_Union[j], Dilation_Union[m_DilationCircleParam.size() - j - 1]);
		}

		//参数赋值,优化内存处理
		Dilation_temp[0] = Dilation_Union[0];

		//step6:计算行程编码的并集
		for (int k = 1; k < m_DilationCircleParam.size(); ++k)
		{
			union2(Dilation_temp[k - 1], Dilation_Union[k], Dilation_temp[k]);
		}

		//step7:将行程的值移动到原点的值
		int offset_x;
		int offset_y;
		offset_x = 0;
		offset_y = 0;
		closing_translate(Dilation_temp[m_DilationCircleParam.size() - 1], Region, offset_x, offset_y);

		//step8:腐蚀
		erosion_circle(Region, dst, Radius);

		//step9:内存释放
		m_DilationCircleParam.clear();
		delete[] Dilation_width_temp;
		delete[] Dilation_Union_temp;
		delete[] Dilation_Union;
		delete[] Dilation_temp;
	}

	void gen_circle(HRegion &Circle, double Row, double Column, double Radius){
		short* rle_x = Circle.rle_x_start;
		short* rle_y = Circle.rle_y;
		short* rle_x_end = Circle.rle_x_end;

		//n.0<Radius<n.5之间Radius=n.0;  n.5<=Radius<n.0+1之间Radius=n.5; n为整数;n>0
		int Radius_Number;
		double Radius_value;
		Radius_Number = max(saturate_cast<int>(Radius), 0);
		if (Radius_Number > Radius){
			Radius_value = Radius_Number - 0.5;
		}
		else if (Radius_Number + 0.5 == Radius){
			Radius_value = Radius;
		}
		else{
			Radius_value = Radius_Number;
		}

		//当圆形半径为1.5
		if (Radius_value*2 == 3){
			*rle_x++ = Column ;
			*rle_y++ = Row  - 1;
			*rle_x_end++ = Column;

			*rle_x++ = Column -1;
			*rle_y++ = Row;
			*rle_x_end++ = Column + 1;

			*rle_x++ = Column;
			*rle_y++ = Row +1;
			*rle_x_end++ = Column;
		}
		else{//根据当前点的dy,计算当前点的dx,从而计算当前圆形的行程编码
			int offset = (int)((Radius_value*2.0 - 1.0) / 2.0);//偏移量,(j1+(j2-1))/2,中心偏左
			double r = Radius_value;
			double c = Radius_value;
			double inv_r2 = r ? 1. / ((double)r*r) : 0;
			for (int i = 0; i < Radius_value * 2; i++){
				int j1 = 0, j2 = 0;
				double dy = i + 0.5 - r;
				if (abs(dy) <= r){
					double dx = c*sqrt((r*r - dy*dy)*inv_r2);
					j1 = max(saturate_cast<int>(c - dx), 0);
					j2 = min(saturate_cast<int>(c + dx), Radius_value * 2);//这个值取不到
				}
				if (Radius_Number > Radius){
					*rle_x++ = Column + (j1 - offset);
					*rle_y++ = Row + (i - offset);
					*rle_x_end++ = Column + (j1 - offset) + (j2 - j1) - 1;
				}
				else if (Radius_Number + 0.5 == Radius){
					*rle_x++ = Column + (j1 - offset);
					*rle_y++ = Row + (i - offset);
					*rle_x_end++ = Column + (j1 - offset) + (j2 - j1) - 1;
				}
				else{
					*rle_x++ = Column + (j1 - offset) - 1;
					*rle_y++ = Row + (i - offset) - 1;
					*rle_x_end++ = Column + (j1 - offset) + (j2 - j1) - 1 - 1;
				}
			}
		}
		Circle.len = rle_y - Circle.rle_y;
	}

	void gen_rectangle1(HRegion &Rectangle,double Row1, double Column1, double Row2,double Column2){
		assert(Row1 < Row2);
		assert(Column1 < Column2);
		short* p_rle_x = Rectangle.rle_x_start;
		short* p_rle_end_x = Rectangle.rle_x_end;
		short* p_rle_y = Rectangle.rle_y;
		for (double row = Row1; row <= Row2; row++){
			*p_rle_x++ = short(Column1);
			*p_rle_end_x++ = short(Column2);
			*p_rle_y++ = row;
		}
		Rectangle.len = int(Row2 - Row1 + 1);
	}

	void intersection1(const ListHRegion& rle, HRegion& dst){//region合并
		ListHRegion temp;
		temp.Region = new HRegion[rle.count];//分配内存
		if (rle.count> 0){
			if (rle.count == 1){
				dst = rle.Region[0];
			}
			else{
				temp.Region[0] = rle.Region[0];
				for (int i = 1; i < rle.count; i++){
					intersection2(temp.Region[i - 1], rle.Region[i], temp.Region[i]);
				}
				dst = temp.Region[rle.count - 1];
			}
		}
		delete[] temp.Region;
	}

	void intersection2(const HRegion& rle, const HRegion &Region, HRegion& dst){
		//step1：判断当前两个Region是否均存在数
		dst.Image_Width = rle.Image_Width;
		dst.Image_Height = rle.Image_Height;
		if (rle.len == 0 && Region.len == 0){
			return;
		}
		else if (rle.len == 0){
			for (int i = 0; i < Region.len; i++){
				dst.rle_x_start[i] = Region.rle_x_start[i];
				dst.rle_x_end[i] = Region.rle_x_end[i];
				dst.rle_y[i] = Region.rle_y[i];
			}
			dst.len = Region.len;
			return;
		}
		else if (Region.len == 0){
			for (int i = 0; i < rle.len; i++){
				dst.rle_x_start[i] = rle.rle_x_start[i];
				dst.rle_x_end[i] = rle.rle_x_end[i];
				dst.rle_y[i] = rle.rle_y[i];
			}
			dst.len = rle.len;
			return;
		}

		//step2：获得rle的同一行的所有行程编码
		short* last_union_rle_y = dst.rle_y;
		int last_id = 0;
		int row_start_idx = 0;
		int* row_starts = new int[5000];//当前行开始数 如第一行开始的值~第一行结束的值
		int* row_ends = new int[5000];//当前行结束数
		int nrows = 0;
		short* rle_x_start = dst.rle_x_start;
		short* rle_y = dst.rle_y;
		short* rle_x_end = dst.rle_x_end;
		for (int id = 1; id < rle.len; ++id)
		{
			if (rle.rle_y[id] - rle.rle_y[last_id]>0)
			{
				row_starts[nrows] = row_start_idx;
				row_ends[nrows] = last_id;
				nrows++;
				row_start_idx = id;
			}
			last_id = id;
		}
		row_starts[nrows] = row_start_idx;
		row_ends[nrows] = last_id;
		nrows++;

		//step3:获得Region的同一行的所有行程编码
		int region_last_id = 0;
		int region_row_start_idx = 0;
		int* region_row_starts = new int[5000];//当前行开始数 如第一行开始的值~第一行结束的值
		int* region_row_ends = new int[5000];//当前行结束数
		int region_nrows = 0;
		for (int Region_id = 1; Region_id < Region.len; ++Region_id)
		{
			if (Region.rle_y[Region_id] - Region.rle_y[region_last_id]>0)
			{
				region_row_starts[region_nrows] = region_row_start_idx;
				region_row_ends[region_nrows] = region_last_id;
				region_nrows++;
				region_row_start_idx = Region_id;
			}
			region_last_id = Region_id;
		}
		region_row_starts[region_nrows] = region_row_start_idx;
		region_row_ends[region_nrows] = region_last_id;
		region_nrows++;

		//step4:在两个Region的行信息相同时,计算当前行的交集
		int rle_count = 0;
		int region_count = 0;
		BOOL end_flag = false;
		while (nrows != rle_count && region_nrows != region_count)
		{
			/*if (nrows <= rle_count || region_nrows <= region_count){
			break;
			}
			else */if (rle.rle_y[row_ends[rle_count]] == Region.rle_y[region_row_ends[region_count]]){//当两个Region的行程编码在同一行
				intersect_two_row(rle.rle_x_start, rle.rle_x_end, Region.rle_x_start, Region.rle_x_end,
					&rle_x_start, &rle_y, &rle_x_end, rle.rle_y[row_ends[rle_count]],
					row_starts[rle_count], row_ends[rle_count] + 1, region_row_starts[region_count], region_row_ends[region_count] + 1);
				rle_count++;
				region_count++;
			}
			else if (rle.rle_y[row_ends[rle_count]] > Region.rle_y[region_row_ends[region_count]]){//Region的行值大于Region1的行值,加Region1的数
				region_count++;
			}
			else if (rle.rle_y[row_ends[rle_count]] < Region.rle_y[region_row_ends[region_count]]){//Region的行值小于Region1的行值,加Region的数
				rle_count++;
			}
		}

		//内存释放
		delete[] row_starts;
		delete[] row_ends;
		delete[] region_row_starts;
		delete[] region_row_ends;
		dst.len = int(rle_y - last_union_rle_y);
	}

	void union1(const ListHRegion& rle, HRegion& dst){
		ListHRegion temp;
		temp.Region = new HRegion[rle.count];//分配内存
		if (rle.count> 0){
			if (rle.count == 1){
				dst = rle.Region[0];
			}
			else{
				temp.Region[0] = rle.Region[0];
				for (int i = 1; i < rle.count; i++){
					union2(temp.Region[i - 1], rle.Region[i], temp.Region[i]);
				}
				dst = temp.Region[rle.count - 1];
			}
		}
		delete[] temp.Region;
	}

	void union2(const HRegion& rle, const HRegion &Region, HRegion& dst){
		//step1:判断当前两个Region是否均存在数据
		dst.Image_Width = rle.Image_Width;
		dst.Image_Height = rle.Image_Height;
		if (rle.len == 0 && Region.len == 0){
			return;
		}
		else if (rle.len == 0){
			dst = Region;
			return;
		}
		else if (Region.len == 0){
			dst = rle;
			return;
		}

		//step2:获得rle的同一行的所有行程编码
		short* last_union_rle_y = dst.rle_y;
		int last_id = 0;
		int row_start_idx = 0;
		int* row_starts = new int[5000];//当前行开始数 如第一行开始的值~第一行结束的值
		int* row_ends = new int[5000];//当前行结束数
		int nrows = 0;
		short* rle_x_start = dst.rle_x_start;
		short* rle_y = dst.rle_y;
		short* rle_x_end = dst.rle_x_end;
		for (int id = 1; id < rle.len; ++id)
		{
			if (rle.rle_y[id] - rle.rle_y[last_id]>0)
			{
				row_starts[nrows] = row_start_idx;
				row_ends[nrows] = last_id;
				nrows++;
				row_start_idx = id;
			}
			last_id = id;
		}
		row_starts[nrows] = row_start_idx;
		row_ends[nrows] = last_id;
		nrows++;

		//step3:获得Region的同一行的所有行程编码
		int region_last_id = 0;
		int region_row_start_idx = 0;
		int* region_row_starts = new int[5000];//当前行开始数 如第一行开始的值~第一行结束的值
		int* region_row_ends = new int[5000];//当前行结束数
		int region_nrows = 0;
		for (int Region_id = 1; Region_id < Region.len; ++Region_id)
		{
			if (Region.rle_y[Region_id] - Region.rle_y[region_last_id]>0)
			{
				region_row_starts[region_nrows] = region_row_start_idx;
				region_row_ends[region_nrows] = region_last_id;
				region_nrows++;
				region_row_start_idx = Region_id;
			}
			region_last_id = Region_id;
		}
		region_row_starts[region_nrows] = region_row_start_idx;
		region_row_ends[region_nrows] = region_last_id;
		region_nrows++;

		//step4:在两个Region的行信息相同时,计算当前行的并集;
		//行不同时,取行较小的数
		//有行超过Region结束值,取另一个Region信息
		int rle_count = 0;
		int region_count = 0;
		BOOL end_flag = false;
		while (1)
		{
			if (nrows <= rle_count && region_nrows <= region_count){//两个Region均遍历完
				break;
			}
			else if (nrows == rle_count){//rle遍历完,获得剩下的Region值
				for (int index = region_row_starts[region_count]; index <region_row_ends[region_count] + 1; index++){
					*rle_x_start++ = Region.rle_x_start[index];
					*rle_y++ = Region.rle_y[index];
					*rle_x_end++ = Region.rle_x_end[index];
				}
				region_count++;
			}
			else if (region_nrows == region_count){//Region遍历完,获得剩下rle值
				for (int index = row_starts[rle_count]; index <row_ends[rle_count] + 1; index++){
					*rle_x_start++ = rle.rle_x_start[index];
					*rle_y++ = rle.rle_y[index];
					*rle_x_end++ = rle.rle_x_end[index];
				}
				rle_count++;
			}
			else if (rle.rle_y[row_ends[rle_count]] == Region.rle_y[region_row_ends[region_count]]){//rle行信息等于Region行信息
				union_two_row(rle.rle_x_start, rle.rle_x_end, Region.rle_x_start, Region.rle_x_end,
					&rle_x_start, &rle_y, &rle_x_end, rle.rle_y[row_ends[rle_count]],
					row_starts[rle_count], row_ends[rle_count] + 1, region_row_starts[region_count], region_row_ends[region_count] + 1);
				rle_count++;
				region_count++;
			}
			else if (rle.rle_y[row_ends[rle_count]] > Region.rle_y[region_row_ends[region_count]]){//rle当前行信息大于Region当前行信息,获得当前的Region值
				for (int index = region_row_starts[region_count]; index <region_row_ends[region_count] + 1; index++){
					*rle_x_start++ = Region.rle_x_start[index];
					*rle_y++ = Region.rle_y[index];
					*rle_x_end++ = Region.rle_x_end[index];
				}
				region_count++;
			}
			else if (rle.rle_y[row_ends[rle_count]] < Region.rle_y[region_row_ends[region_count]]){//rle当前行信息小于Region当前行信息,获得当前的rle值
				for (int index = row_starts[rle_count]; index <row_ends[rle_count] + 1; index++){
					*rle_x_start++ = rle.rle_x_start[index];
					*rle_y++ = rle.rle_y[index];
					*rle_x_end++ = rle.rle_x_end[index];
				}
				rle_count++;
			}
		}

		//内存释放
		delete[] row_starts;
		delete[] row_ends;
		delete[] region_row_starts;
		delete[] region_row_ends;
		dst.len = int(rle_y - last_union_rle_y);
	}

	void difference(const HRegion& rle, const HRegion &Region, HRegion& dst){
		//step1:判断当前两个Region是否均存在数
		dst.Image_Width = rle.Image_Width;
		dst.Image_Height = rle.Image_Height;
		if (rle.len == 0 && Region.len == 0){
			return;
		}
		else if (rle.len == 0){
			for (int i = 0; i < Region.len; i++){
				dst.rle_x_start[i] = Region.rle_x_start[i];
				dst.rle_x_end[i] = Region.rle_x_end[i];
				dst.rle_y[i] = Region.rle_y[i];
			}
			dst.len = Region.len;
			return;
		}
		else if (Region.len == 0){
			for (int i = 0; i < rle.len; i++){
				dst.rle_x_start[i] = rle.rle_x_start[i];
				dst.rle_x_end[i] = rle.rle_x_end[i];
				dst.rle_y[i] = rle.rle_y[i];
			}
			dst.len = rle.len;
			return;
		}

		//step2:获得rle的同一行的所有行程编码
		short* last_union_rle_y = dst.rle_y;
		int last_id = 0;
		int row_start_idx = 0;
		int* row_starts = new int[5000];
		int* row_ends = new int[5000];
		int nrows = 0;
		short* rle_x_start = dst.rle_x_start;
		short* rle_y = dst.rle_y;
		short* rle_x_end = dst.rle_x_end;
		for (int id = 1; id < rle.len; ++id)
		{
			if (rle.rle_y[id] - rle.rle_y[last_id]>0)
			{
				row_starts[nrows] = row_start_idx;
				row_ends[nrows] = last_id;
				nrows++;
				row_start_idx = id;
			}
			last_id = id;
		}
		row_starts[nrows] = row_start_idx;
		row_ends[nrows] = last_id;
		nrows++;

		//step3:获得Region的同一行的所有行程编码
		int region_last_id = 0;
		int region_row_start_idx = 0;
		int* region_row_starts = new int[5000];
		int* region_row_ends = new int[5000];
		int region_nrows = 0;
		for (int Region_id = 1; Region_id < Region.len; ++Region_id)
		{
			if (Region.rle_y[Region_id] - Region.rle_y[region_last_id]>0)
			{
				region_row_starts[region_nrows] = region_row_start_idx;
				region_row_ends[region_nrows] = region_last_id;
				region_nrows++;
				region_row_start_idx = Region_id;
			}
			region_last_id = Region_id;
		}
		region_row_starts[region_nrows] = region_row_start_idx;
		region_row_ends[region_nrows] = region_last_id;
		region_nrows++;

		//step4:在两个Region的行信息相同时,计算当前行的差集;
		//取rle存在而Region不存在的行程编码
		int rle_count = 0;
		int region_count = 0;
		BOOL end_flag = false;
		while (1)
		{
			if (nrows <= rle_count && region_nrows <= region_count){
				break;
			}
			else if (nrows == rle_count){
				region_count++;
			}
			else if (region_nrows == region_count){
				for (int index = row_starts[rle_count]; index <row_ends[rle_count] + 1; index++){
					*rle_x_start++ = rle.rle_x_start[index];
					*rle_y++ = rle.rle_y[index];
					*rle_x_end++ = rle.rle_x_end[index];
				}
				rle_count++;
			}
			else if (rle.rle_y[row_ends[rle_count]] == Region.rle_y[region_row_ends[region_count]]){
				difference_two_row(rle.rle_x_start, rle.rle_x_end, Region.rle_x_start, Region.rle_x_end,
					&rle_x_start, &rle_y, &rle_x_end, rle.rle_y[row_ends[rle_count]],
					row_starts[rle_count], row_ends[rle_count] + 1, region_row_starts[region_count], region_row_ends[region_count] + 1);
				rle_count++;
				region_count++;
			}
			else if (rle.rle_y[row_ends[rle_count]] > Region.rle_y[region_row_ends[region_count]]){
				region_count++;
			}
			else if (rle.rle_y[row_ends[rle_count]] < Region.rle_y[region_row_ends[region_count]]){
				for (int index = row_starts[rle_count]; index <row_ends[rle_count] + 1; index++){
					*rle_x_start++ = rle.rle_x_start[index];
					*rle_y++ = rle.rle_y[index];
					*rle_x_end++ = rle.rle_x_end[index];
				}
				rle_count++;
			}
		}

		//step5:释放内存
		delete[] row_starts;
		delete[] row_ends;
		delete[] region_row_starts;
		delete[] region_row_ends;
		dst.len = int(rle_y - last_union_rle_y);

	}

	void concat_obj(const HRegion&rle, const HRegion &Region, ListHRegion& dst){
		dst.Region[dst.count] = rle;
		dst.count++;
		dst.Region[dst.count] = Region;
		dst.count++;
	}

	void connection(const HRegion& Region, HRegion& ConnectedRegions){
		/*算法流程:
		1.给第一行所有行程编码标号
		2.对于除了第一行外的所有行里的团，如果它与前一行中的所有团都没有重合区域，则给它一个新的标号;
		如果它仅与上一行中一个团有重合区域，则将上一行的那个团的标号赋给它;
		如果它与上一行的2个以上的团有重叠区域，则给当前团赋一个相连团的最小标号，并将上一行的这几个团的标记写入等价对，说明它们属于一类
		3.将等价对转换为等价序列，每一个序列需要给一相同的标号，因为它们都是等价的
		4.遍历开始团的标记,查找等价序列,给予新的标记
		*/
		
		//参数初始化
		ConnectedRegions.Image_Width = Region.Image_Width;
		ConnectedRegions.Image_Height = Region.Image_Height;
		vector<int> runLabels;
		runLabels.assign(Region.len, 0);

		//计算行程编码属于哪个Region
		int n = labeling(Region.rle_x_start, Region.rle_x_end, Region.rle_y, Region.len, runLabels, 1);

		//赋值
		ConnectedRegions = Region;
		ConnectedRegions.len = Region.len;
		ConnectedRegions.runLabels = runLabels;
		ConnectedRegions.runLabels_Count = n;
		runLabels.clear();
	}  

	void select_obj(const HRegion& Region, HRegion& SelectedRegions,int Region_ID){
		SelectedRegions.Image_Width = Region.Image_Width;
		SelectedRegions.Image_Height = Region.Image_Height;
		short* Selected_x_start = SelectedRegions.rle_x_start;
		short* Selected_y = SelectedRegions.rle_y;
		short* Selected_x_end = SelectedRegions.rle_x_end;

		assert(0<Region_ID&&Region_ID<=Region.runLabels_Count);
		for (int i = 0; i < Region.runLabels.size();i++){
			if (Region.runLabels[i] == Region_ID){
				*Selected_x_start++ = Region.rle_x_start[i];
				*Selected_y++ = Region.rle_y[i];
				*Selected_x_end++ = Region.rle_x_end[i];
				SelectedRegions.len++;
			}
		}
	}

	void select_shape(const HRegion& Region, HRegion& SelectedRegions,CondParam& m_CondParam){

		//new
		SelectedRegions.Image_Width = Region.Image_Width;
		SelectedRegions.Image_Height = Region.Image_Height;
		//参数初始化
		CondParam m_Select_CondParam;
		
		short* Selected_x_start = SelectedRegions.rle_x_start;
		short* Selected_y = SelectedRegions.rle_y;
		short* Selected_x_end = SelectedRegions.rle_x_end;
		
		CondParam *ve_CondParam = new CondParam[Region.runLabels_Count];
		for (int i = 0; i < Region.runLabels_Count;i++){
			ve_CondParam[i].Width.Min = 9999;
			ve_CondParam[i].Height.Min = 9999;
			ve_CondParam[i].Area.Value = 9999;
		}

		int Rle_Number = (int)Region.runLabels.size();

		//宽度计算,获得当前Region的最大宽度值和最小宽度值,两者做差
		//高度计算,获得当前Region的最大高度值和最小高度值,两者做差
		//面积计算,将当前Region所有行程编码长度加起来

		//获得Region最大宽度值/最小宽度值,最大高度值/最小高度值/面积
		for (int Index = 0; Index < Rle_Number; Index++){
			if (ve_CondParam[Region.runLabels[Index] - 1].Width.Min>Region.rle_x_start[Index]){
				ve_CondParam[Region.runLabels[Index] - 1].Width.Min = Region.rle_x_start[Index];
			}
			if (ve_CondParam[Region.runLabels[Index] - 1].Width.Max<Region.rle_x_end[Index]){
				ve_CondParam[Region.runLabels[Index] - 1].Width.Max = Region.rle_x_end[Index];
			}
			//ve_CondParam[Region.runLabels[Index] - 1].Width.Name = "width";
			if (ve_CondParam[Region.runLabels[Index] - 1].Height.Min>Region.rle_y[Index]){
				ve_CondParam[Region.runLabels[Index] - 1].Height.Min = Region.rle_y[Index];
			}
			if (ve_CondParam[Region.runLabels[Index] - 1].Height.Max<Region.rle_y[Index]){
				ve_CondParam[Region.runLabels[Index] - 1].Height.Max = Region.rle_y[Index];
			}
			//ve_CondParam[Region.runLabels[Index] - 1].Height.Name = "height";
			ve_CondParam[Region.runLabels[Index] - 1].Area.Value = 
				ve_CondParam[Region.runLabels[Index] - 1].Area.Value + 
				Region.rle_x_end[Index] - Region.rle_x_start[Index] + 1;
			//ve_CondParam[Region.runLabels[Index] - 1].Area.Name = "area";
		}
	
		//获得Region的宽度值和高度值
		int Regions_Number = Region.runLabels_Count;
		for (int j = 0; j < Regions_Number; j++){
			ve_CondParam[j].Width.Value = ve_CondParam[j].Width.Max - ve_CondParam[j].Width.Min + 1;
			ve_CondParam[j].Height.Value = ve_CondParam[j].Height.Max - ve_CondParam[j].Height.Min + 1;
		}

		//判断m_CondParam width/height/area是否存在(是否被调用)
		if (m_CondParam.Width.Max > 0){
			//m_Select_CondParam.Width.Name = "width";
			m_Select_CondParam.Width.flag = true;
			m_Select_CondParam.Width.Min = m_CondParam.Width.Min;
			m_Select_CondParam.Width.Max = m_CondParam.Width.Max;
		}

		if (m_CondParam.Height.Max > 0){
			//m_Select_CondParam.Height.Name = "height";
			m_Select_CondParam.Height.flag = true;
			m_Select_CondParam.Height.Min = m_CondParam.Height.Min;
			m_Select_CondParam.Height.Max = m_CondParam.Height.Max;
		}

		if (m_CondParam.Area.Max > 0){
			//m_Select_CondParam.Area.Name = "area";
			m_Select_CondParam.Area.flag = true;
			m_Select_CondParam.Area.Min = m_CondParam.Area.Min;
			m_Select_CondParam.Area.Max = m_CondParam.Area.Max;
		}

		int *m_select_runLabels = new int[Rle_Number];
		int m_select_runLabels_count = 0;
		for (int k = 0; k < Rle_Number; k++){
			if (m_Select_CondParam.Width.flag == true){//判断宽度属性是否存在
				if (m_Select_CondParam.Width.Min<ve_CondParam[Region.runLabels[k] - 1].Width.Value &&
					m_Select_CondParam.Width.Max>ve_CondParam[Region.runLabels[k] - 1].Width.Value){
					ve_CondParam[Region.runLabels[k] - 1].Width.flag = true;
				}
			}

			if (m_Select_CondParam.Height.flag == true){//判断高度属性是否存在
				if (m_Select_CondParam.Height.Min<ve_CondParam[Region.runLabels[k] - 1].Height.Value &&
					m_Select_CondParam.Height.Max>ve_CondParam[Region.runLabels[k] - 1].Height.Value){
					ve_CondParam[Region.runLabels[k] - 1].Height.flag = true;
				}
			}

			if (m_Select_CondParam.Area.flag == true){//判断面积属性是否存在
				if (m_Select_CondParam.Area.Min<ve_CondParam[Region.runLabels[k] - 1].Area.Value &&
					m_Select_CondParam.Area.Max>ve_CondParam[Region.runLabels[k] - 1].Area.Value){
					ve_CondParam[Region.runLabels[k] - 1].Area.flag = true;
				}
			}

			//三个条件同时满足,将参数写入Region
			if (m_Select_CondParam.Width.flag == ve_CondParam[Region.runLabels[k] - 1].Width.flag &&
				m_Select_CondParam.Height.flag == ve_CondParam[Region.runLabels[k] - 1].Height.flag &&
				m_Select_CondParam.Area.flag == ve_CondParam[Region.runLabels[k] - 1].Area.flag)
			{
				*Selected_x_start++ = Region.rle_x_start[k];
				*Selected_x_end++ = Region.rle_x_end[k];
				*Selected_y++ = Region.rle_y[k];
				//将筛选过后的Region重新编码
				if (m_select_runLabels_count == 0){
					m_select_runLabels[0] = Region.runLabels[k];
					m_select_runLabels_count++;
				}else{
					if (IsEqualInt(Region.runLabels[k], m_select_runLabels, m_select_runLabels_count) == -1){
						m_select_runLabels[m_select_runLabels_count] = Region.runLabels[k];
						m_select_runLabels_count++;
					}
				}
				int  matchID = IsEqualInt(Region.runLabels[k], m_select_runLabels, m_select_runLabels_count);
				if (matchID == -1){
				} else{
					SelectedRegions.runLabels.push_back(matchID + 1);
				}
			}
		}

		SelectedRegions.runLabels_Count = m_select_runLabels_count;
		SelectedRegions.len = int(Selected_y - SelectedRegions.rle_y);

		delete[] ve_CondParam;
		delete[] m_select_runLabels;
	}

	void area_center(HRegion& Region, int &Area, float &Row, float &Column){
		Area = Region.Area();

		int Sum_Row = 0;
		int Sum_Column = 0;
		int Sum_count = 0;
		for (int i = 0; i < Region.len;i++){
			Sum_Row += Sum_Row + Region.rle_y[i] * (Region.rle_x_end[i] - Region.rle_x_start[i] + 1);
			Sum_Column += Sum_Column + (Region.rle_x_end[i] + Region.rle_x_start[i])/2 
				*(Region.rle_x_end[i] - Region.rle_x_start[i] + 1);
			Sum_count = Sum_count + 1 * (Region.rle_x_end[i] - Region.rle_x_start[i] + 1);
		}
		Row = (float)Sum_Row / (float)Sum_count;
		Column = (float)Sum_Column / (float)Sum_count;
	}

	void select_shape_max_std(const HRegion& Region, HRegion& SelectedRegions){
		//参数初始化
		SelectedRegions.Image_Width = Region.Image_Width;
		SelectedRegions.Image_Height = Region.Image_Height;

		CondParam l_CondParam;
		CondParam m_Select_CondParam;
		vector<CondParam> ve_CondParam;

		short* Selected_x_start = SelectedRegions.rle_x_start;
		short* Selected_y = SelectedRegions.rle_y;
		short* Selected_x_end = SelectedRegions.rle_x_end;

		l_CondParam.Area.Value = 0;
		ve_CondParam.assign(Region.runLabels_Count, l_CondParam);

		//获得Region面积
		int Rle_Number = (int)Region.runLabels.size();
		for (int Index = 0; Index < Rle_Number; Index++){
			ve_CondParam[Region.runLabels[Index] - 1].Area.Value =
				ve_CondParam[Region.runLabels[Index] - 1].Area.Value +
				Region.rle_x_end[Index] - Region.rle_x_start[Index] + 1;
			//ve_CondParam[Region.runLabels[Index] - 1].Area.Name = "area";
		}

		//计算最大面积的ID
		int SelectedID;
		int Regions_Number = Region.runLabels_Count;
		for (int i = 0; i < Regions_Number;i++){
			if (m_Select_CondParam.Area.Value <ve_CondParam[i].Area.Value){
				m_Select_CondParam.Area.Value = ve_CondParam[i].Area.Value;
				SelectedID = i + 1;
			}
		}

		//获得最大面积的所有行程编码
		for (int k = 0; k < Rle_Number; k++){
			if (SelectedID == Region.runLabels[k]){
				*Selected_x_start++ = Region.rle_x_start[k];
				*Selected_x_end++ = Region.rle_x_end[k];
				*Selected_y++ = Region.rle_y[k];
			}
		}
		SelectedRegions.len = int(Selected_y - SelectedRegions.rle_y);
		ve_CondParam.clear();
	}
}

//erosion_rectangle start
int erosion_blocks(const HRegion& rle, HRegion& dst, int height, int rank)
{
	//region
	int region_nrows = 0;
	HRegion Region;
	Region.Image_Width = rle.Image_Width;
	Region.Image_Height = rle.Image_Height;
	short* Region_x_start = Region.rle_x_start;
	short* Region_y = Region.rle_y;
	short* Region_x_end = Region.rle_x_end;

	//获得需要做交集的次数,由高度决定时间复杂度为(log2^(n))
	int offset = 1;
	int step = 1;
	while (step < height)
	{
		step <<= 1;
		++offset;
	}

	for (int id = 0; id < rle.len; ++id)
	{
		//使用矩形元素做腐蚀,同一级的列开始和列结束均相同,但行信息不同(行信息相差2^(n-1),n表示当前级数)
		if (rank == 1){//第一级，差2^0 = 1
			*Region_y++ = rle.rle_y[id] - 1;
		}
		else if (offset - 1 > rank){//第2~(n-1)级,差2^(n-1)
			*Region_y++ = rle.rle_y[id] - (1 << (rank - 1));
		}
		else{
			if (1 << rank == height){//第n级,当前树为完全树,差2^(n-1)
				*Region_y++ = rle.rle_y[id] - (1 << (rank - 1));
			}
			else{//第n级,当前树为不完全树,差height-2^(n-1)
				*Region_y++ = rle.rle_y[id] - (height - (1 << (rank - 1)));
			}
		}
		*Region_x_start++ = rle.rle_x_start[id];
		*Region_x_end++ = rle.rle_x_end[id];
	}
	Region.len = rle.len;
	//两个Region的交集
	blob::intersection2(rle, Region, dst);
	return dst.len;
}

//将原点还原到原处
void translate(const HRegion& rle, HRegion& dst, int offset_x, int offset_y)
{
	short* rle_x_start = dst.rle_x_start;
	short* rle_x_end = dst.rle_x_end;
	short* rle_y = dst.rle_y;
	for (int i = 0; i < rle.len; ++i)
	{
		//去除不满足图像的高度
		if (rle.rle_y[i] + offset_y >= 0 && rle.rle_y[i] + offset_y<rle.Image_Height){
			*rle_y++ = rle.rle_y[i] + offset_y;
			*rle_x_end++ = rle.rle_x_end[i] + offset_x;
			*rle_x_start++ = rle.rle_x_start[i] + offset_x;
		}
	}
	dst.len = rle_y - dst.rle_y;
}

//去掉不满足宽度要求的行程
int erosion_rectangel_row(const HRegion& rle, HRegion& dst, int width){
	int rle_cnt = rle.len;
	const short* p_rle_start_x = rle.rle_x_start;
	const short* p_rle_y = rle.rle_y;
	const short* p_rle_end_x = rle.rle_x_end;

	short* rle_x_start = dst.rle_x_start;
	short* rle_x_end = dst.rle_x_end;
	short* rle_y = dst.rle_y;

	for (int id = 0; id < rle_cnt; ++id)
	{
		//满足宽度为width的行程编码
		if (p_rle_end_x[id] - p_rle_start_x[id] + 1 >= width)
		{
			//原点为左边
			*rle_x_start++ = rle.rle_x_start[id];
			*rle_x_end++ = rle.rle_x_end[id] - width + 1;//宽度为10,行程为10,腐蚀过后,余1
			*rle_y++ = rle.rle_y[id];
		}
	}

	return int(rle_y - dst.rle_y);
}
//end

//dilation_rectangle1 start

//给当前的行程编码扩充行
int dilation_rectangel_row(const HRegion& rle, HRegion& dst, int width){
	int rle_cnt = rle.len;

	short* rle_x_start = dst.rle_x_start;
	short* rle_x_end = dst.rle_x_end;
	short* rle_y = dst.rle_y;

	for (int id = 0; id < rle_cnt; ++id)
	{
		//原点为左边
		*rle_x_start++ = rle.rle_x_start[id] - width + 1;//宽度为10,行程为10,膨胀过后,列开始往前移动width-1
		*rle_x_end++ = rle.rle_x_end[id];
		*rle_y++ = rle.rle_y[id];
	}
	return int(rle_y - dst.rle_y);
}

int dilation_blocks(const HRegion& rle, HRegion& dst, int height, int rank)
{
	//region
	int region_nrows = 0;
	HRegion Region;
	Region.Image_Width = rle.Image_Width;
	Region.Image_Height = rle.Image_Height;
	short* Region_x_start = Region.rle_x_start;
	short* Region_y = Region.rle_y;
	short* Region_x_end = Region.rle_x_end;

	//获得需要做并集的次数,由高度决定时间复杂度为(log2^(n))
	int offset=1;
	int step = 1;
	while (step < height)
	{
		step <<= 1;
		++offset;
	}

	for (int id = 0; id < rle.len; ++id)
	{
		//使用矩形元素做膨胀,同一级的列开始和列结束均相同,但行信息不同(行信息相差2^(n-1),n表示当前级数)
		if (rank ==1){//第一级，差2^0 = 1
			*Region_y++ = rle.rle_y[id] - 1;
		}
		else if (offset-1 > rank){//第2~(n-1)级,差2^(n-1)
			*Region_y++ = rle.rle_y[id] - (1 << (rank - 1));
		}
		else{
			if (1 << rank == height){//第n级,当前树为完全树,差2^(n-1)
				*Region_y++ = rle.rle_y[id] - (1 << (rank - 1));
			}else{//第n级,当前树为不完全树,差height-2^(n-1)
				*Region_y++ = rle.rle_y[id] - (height - (1 << (rank - 1)));
			}
		}
		*Region_x_start++ = rle.rle_x_start[id];
		*Region_x_end++ = rle.rle_x_end[id];
	}
	Region.len = rle.len;
	//两个Region的并集
	blob::union2(rle,Region,dst);
	return dst.len;
}

void dilation_union(const HRegion& rle, HRegion& dst){
	//获得rle的同一行的所有行程编码
	short* last_union_rle_y = dst.rle_y;
	int last_id = 0;
	int row_start_idx = 0;
	int* row_starts = new int[5000];
	int* row_ends = new int[5000];
	int nrows = 0;
	short* rle_x_start = dst.rle_x_start;
	short* rle_y = dst.rle_y;
	short* rle_x_end = dst.rle_x_end;
	for (int id = 1; id < rle.len; ++id)
	{
		if (rle.rle_y[id] - rle.rle_y[last_id] > 0)
		{
			row_starts[nrows] = row_start_idx;
			row_ends[nrows] = last_id;
			nrows++;
			row_start_idx = id;
		}
		last_id = id;
	}
	row_starts[nrows] = row_start_idx;
	row_ends[nrows] = last_id;
	nrows++;
	//将Region的当前行取交集
	for (int rle_count = 0; rle_count < nrows; rle_count++){
		union_two_row(rle.rle_x_start, rle.rle_x_end, rle.rle_x_start, rle.rle_x_end, &rle_x_start, &rle_y, &rle_x_end,
			rle.rle_y[row_ends[rle_count]], row_starts[rle_count], row_ends[rle_count] + 1, row_starts[rle_count], row_ends[rle_count] + 1);
	}
	delete[] row_starts;
	delete[] row_ends;
	dst.len = int(rle_y - last_union_rle_y);
}

void dilation_translate(const HRegion& rle, HRegion& dst, int offset_x, int offset_y){
	short* rle_x_start = dst.rle_x_start;
	short* rle_x_end = dst.rle_x_end;
	short* rle_y = dst.rle_y;
	for (int i = 0; i < rle.len; ++i)
	{
		//去除不满足图像的高度
		if (rle.rle_y[i] + offset_y >= 0 && rle.rle_y[i] + offset_y<rle.Image_Height){
			//若行程编码宽度超出图像宽度
			if (rle.rle_x_start[i] + offset_x <= 0 && rle.rle_x_end[i] + offset_x >= rle.Image_Width){
				*rle_x_start++ = 0;
				*rle_x_end++ = rle.Image_Width - 1;
			}
			else if (rle.rle_x_start[i] + offset_x <= 0){
				*rle_x_start++ = 0;
				*rle_x_end++ = rle.rle_x_end[i] + offset_x;
			}
			else if (rle.rle_x_end[i] + offset_x >= rle.Image_Width){
				*rle_x_start++ = rle.rle_x_start[i] + offset_x;
				*rle_x_end++ = rle.Image_Width - 1;
			}
			else{//没超过图像宽度
				*rle_x_end++ = rle.rle_x_end[i] + offset_x;
				*rle_x_start++ = rle.rle_x_start[i] + offset_x;
			}
			*rle_y++ = rle.rle_y[i] + offset_y;
		}
	}
	dst.len = rle_y - dst.rle_y;
}
//end

//closing_rectangle start 
void closing_translate(const HRegion& rle, HRegion& dst, int offset_x, int offset_y)
{
	dst.len = rle.len;
	short* rle_x_start = dst.rle_x_start;
	short* rle_x_end = dst.rle_x_end;
	short* rle_y = dst.rle_y;
	for (int i = 0; i < rle.len; ++i)
	{
		//先膨胀后腐蚀,获得所有行程编码的信息
		*rle_y++ = rle.rle_y[i] + offset_y;
		*rle_x_end++ = rle.rle_x_end[i] + offset_x;
		*rle_x_start++ = rle.rle_x_start[i] + offset_x;
	}
	dst.len = rle_y - dst.rle_y;
}
//end


//erosion_circle start
int erosion_circle_row(const HRegion& rle, HRegion& dst, int start_x, int len, int dy){
	short* last_erosion_row_y = dst.rle_y;
	//丢掉过短的行程
	int rle_cnt = rle.len;
	const short* p_rle_start_x = rle.rle_x_start;
	const short* p_rle_y = rle.rle_y;
	const short* p_rle_end_x = rle.rle_x_end;

	short* rle_x_start = dst.rle_x_start;
	short* rle_x_end = dst.rle_x_end;
	short* rle_y = dst.rle_y;

	for (int id = 0; id < rle_cnt; ++id)
	{
		//满足宽度为width的行程编码
		if (p_rle_end_x[id] - p_rle_start_x[id] + 1 >= len)
		{
			//腐蚀过后的列开始和列结束,原点为中心点
			//rle_x_start =a.start()-b.start()
			//rle_x_len =a.len() - b.len() + 1
			//rle_x_end = rle_x_start+rle_x_len-1
			//a.len() = rle.rle_x_end[id] - rle.rle_x_start[id] + 1
			//b.len() = len
			*rle_x_start++ = rle.rle_x_start[id] - start_x;
			*rle_x_end++ = (rle.rle_x_start[id] - start_x) +(rle.rle_x_end[id] - rle.rle_x_start[id] + 1) - len;
			*rle_y++ = rle.rle_y[id] + dy;
		}
	}

	return int(rle_y - last_erosion_row_y);
}

void erosion_intersection(const HRegion& rle, ErosionDilationCircleParam m_ErosionCircleParam, HRegion& dst){
	dst.Image_Width = rle.Image_Width;
	dst.Image_Height = rle.Image_Height;

	//获得需要做交集的次数,时间复杂度为(log2^(n)),n为高度
	int step = 1;
	int rank = 1;
	while (step < m_ErosionCircleParam.count)
	{
		step <<= 1;
		++rank;
	}

	//参数初始化
	HRegion *erosion_temp = new HRegion[rank];
	for (int i = 0; i < rank; ++i)
	{
		erosion_temp[i].Image_Width = rle.Image_Width;
		erosion_temp[i].Image_Height = rle.Image_Height;
	}

	//参数赋值,优化内存
	erosion_temp[0] = rle;

	//step1:当前级数取交集获得下一个级数的Region
	for (int i = 1; i < rank; ++i)
	{
		erosion_temp[i].len = erosion_blocks(erosion_temp[i - 1], erosion_temp[i], m_ErosionCircleParam.count, i);
	}

	--rank;
	//step2:将行程的值移动到原点的值,偏移不在这处理
	int offset_x = 0;
	int offset_y = 0;
	circle_translate(erosion_temp[rank], dst, offset_x, offset_y);
	delete[] erosion_temp;
}

void erosion_intersection2(const HRegion& rle, ErosionDilationCircleParam m_ErosionCircleParam, ErosionDilationCircleParam m_ErosionCircleParam1, HRegion& dst, HRegion& dst1){
	dst.Image_Width = rle.Image_Width;
	dst.Image_Height = rle.Image_Height;
	dst1.Image_Width = rle.Image_Width;
	dst1.Image_Height = rle.Image_Height;

	//获得需要做交集的次数,时间复杂度为(log2^(n)),n为高度
	int step = 1;
	int rank = 1;
	while (step < m_ErosionCircleParam.count)
	{
		step <<= 1;
		++rank;
	}

	//参数初始化
	HRegion *erosion_temp = new HRegion[rank];
	for (int i = 0; i < rank; ++i)
	{
		erosion_temp[i].Image_Width = rle.Image_Width;
		erosion_temp[i].Image_Height = rle.Image_Height;
	}

	//参数赋值,优化内存
	erosion_temp[0] = rle;

	//step1:当前级数取交集获得下一个级数的Region
	for (int i = 1; i < rank; ++i)
	{
		erosion_temp[i].len = erosion_blocks(erosion_temp[i - 1], erosion_temp[i], m_ErosionCircleParam.count, i);
	}

	--rank;

	//step2:将行程的值移动到原点的值,偏移不在这处理
	int offset_x = 0;
	int offset_y = 0;
	circle_translate(erosion_temp[rank], dst, offset_x, offset_y);
	offset_x = 0;
	offset_y = m_ErosionCircleParam1.row - m_ErosionCircleParam.row;
	circle_translate(erosion_temp[rank], dst1, offset_x, offset_y);
	delete[] erosion_temp;
}

void circle_translate(const HRegion& rle, HRegion& dst, int offset_x, int offset_y)
{
	dst.len = rle.len;
	short* rle_x_start = dst.rle_x_start;
	short* rle_x_end = dst.rle_x_end;
	short* rle_y = dst.rle_y;
	for (int i = 0; i < rle.len; ++i)
	{
		//获取当前行程编码所有信息
		*rle_y++ = rle.rle_y[i] + offset_y;
		*rle_x_end++ = rle.rle_x_end[i] + offset_x;
		*rle_x_start++ = rle.rle_x_start[i] + offset_x;
	}
	dst.len = rle_y - dst.rle_y;
}
//end

//dilation_circle_start
int dilation_circle_row(const HRegion& rle, HRegion& dst, int start_x, int len, int dy){
	short* last_erosion_row_y = dst.rle_y;
	//丢掉过短的行程
	int rle_cnt = rle.len;
	const short* p_rle_start_x = rle.rle_x_start;
	const short* p_rle_y = rle.rle_y;
	const short* p_rle_end_x = rle.rle_x_end;

	short* rle_x_start = dst.rle_x_start;
	short* rle_x_end = dst.rle_x_end;
	short* rle_y = dst.rle_y;

	for (int id = 0; id < rle_cnt; ++id)
	{
		//膨胀过后的列开始和列结束,原点为中心点
		//rle_x_start =a.start() + b.start()
		//rle_x_len =a.len() + b.len() - 1
		//rle_x_end = rle_x_start+rle_x_len-1
		//a.len() = rle.rle_x_end[id] - rle.rle_x_start[id] + 1
		//b.len() = len
		*rle_x_start++ = rle.rle_x_start[id] + start_x;
		*rle_x_end++ = (rle.rle_x_start[id] + start_x) + (rle.rle_x_end[id] - rle.rle_x_start[id] + 1) + len - 1 - 1;
		*rle_y++ = rle.rle_y[id] + dy;
	}

	return int(rle_y - last_erosion_row_y);
}

void dilation_union1(const HRegion& rle, ErosionDilationCircleParam m_DilationCircleParam, HRegion& dst){
	dst.Image_Width = rle.Image_Width;
	dst.Image_Height = rle.Image_Height;

	//获得需要做并集的次数,时间复杂度为(log2^(n)),n为高度
	int step = 1;
	int rank = 1;
	while (step < m_DilationCircleParam.count)
	{
		step <<= 1;
		++rank;
	}

	//参数初始化
	HRegion *dilation_temp = new HRegion[rank];
	for (int i = 0; i < rank; ++i)
	{
		dilation_temp[i].Image_Width = rle.Image_Width;
		dilation_temp[i].Image_Height = rle.Image_Height;
	}

	//参数赋值,优化内存
	dilation_temp[0] = rle;

	//step1:当前级数取并集获得下一个级数的Region
	for (int i = 1; i < rank; ++i)
	{
		dilation_temp[i].len = dilation_blocks(dilation_temp[i - 1], dilation_temp[i], m_DilationCircleParam.count, i);
	}

	--rank;
	//step2:将行程的值移动到原点的值,偏移不在这处理
	int offset_x = 0;
	int offset_y = 0;
	circle_translate(dilation_temp[rank], dst, offset_x, offset_y);
	delete[] dilation_temp;
}

void dilation_union2(const HRegion& rle, ErosionDilationCircleParam m_DilationCircleParam, ErosionDilationCircleParam m_DilationCircleParam1, HRegion& dst, HRegion& dst1){
	dst.Image_Width = rle.Image_Width;
	dst.Image_Height = rle.Image_Height;
	dst1.Image_Width = rle.Image_Width;
	dst1.Image_Height = rle.Image_Height;

	//获得需要做交集的次数,时间复杂度为(log2^(n)),n为高度
	int step = 1;
	int rank = 1;
	while (step < m_DilationCircleParam.count)
	{
		step <<= 1;
		++rank;
	}

	//参数初始化
	HRegion *dilation_temp = new HRegion[rank];
	for (int i = 0; i < rank; ++i)
	{
		dilation_temp[i].Image_Width = rle.Image_Width;
		dilation_temp[i].Image_Height = rle.Image_Height;
	}

	//参数赋值,优化内存
	dilation_temp[0] = rle;

	//step1:当前级数取并集获得下一个级数的Region
	for (int i = 1; i < rank; ++i)
	{
		dilation_temp[i].len = dilation_blocks(dilation_temp[i - 1], dilation_temp[i], m_DilationCircleParam.count, i);
	}

	--rank;
	//step2:将行程的值移动到原点的值,偏移不在这处理
	int offset_x = 0;
	int offset_y = 0;
	circle_translate(dilation_temp[rank], dst, offset_x, offset_y);
	offset_x = 0;
	offset_y = m_DilationCircleParam1.row - m_DilationCircleParam.row;
	circle_translate(dilation_temp[rank], dst1, offset_x, offset_y);
	delete[] dilation_temp;
}

//end

//union start
void union_two_row(const short* rle_x, const short* rle_end_x, const short* region_x, const short* region_end_x, short** row_rle_x, short** row_rle_y, short** row_rle_end_x,
	short row, int startA  , int endA, int startB, int endB){
	//获取第一个两个Region均为空的值为开始值,第二个两个Region均为空的值为结束值
	int i = startA;
	int j = startB;
	/*指针*/
	short* p_row_rle_x = *row_rle_x;
	short* p_row_rle_y = *row_rle_y;
	short* p_row_rle_end_x = *row_rle_end_x;

	//step1:参数初始化
	//对比当前两个Region的列开始值,获得较小Region的当前列开始值和列结束值
	int last_rle_x;
	int last_rle_end_x;
	if (rle_x[i] < region_x[j]){
		last_rle_x = rle_x[i];
		last_rle_end_x = rle_end_x[i] + 1;//加1,124~130和131~140可以取并集
		i++;
	}
	else{
		last_rle_x = region_x[j];
		last_rle_end_x = region_end_x[j] + 1;//加1,124~130和131~140可以取并集
		j++;
	}

	//step2:遍历两个Region的当前行所有Union的信息
	//rle和Region的当前行的结束值
	while (i != endA && j != endB){
		//对比当前两个Region的列开始值,获得较小Region的当前列开始值和列结束值
		int cur_rle_x;
		int cur_rle_end_x;
		if (rle_x[i]<region_x[j]){
			cur_rle_x = rle_x[i];
			cur_rle_end_x = rle_end_x[i] + 1;
			i++;
		}
		else{
			cur_rle_x = region_x[j];
			cur_rle_end_x = region_end_x[j] + 1;
			j++;
		}

		int col_start = cur_rle_x > last_rle_x ? cur_rle_x : last_rle_x;//对比两个行程编码的列开始值,获得两个行程编码的列开始值的最大值
		int col_end = cur_rle_end_x > last_rle_end_x ? last_rle_end_x : cur_rle_end_x;//对比两个行程编码的列结束值,获得两个行程编码的列结束值的最小值
		if (col_end >= col_start){//当前列结束值最小值大于等于当前列开始值最大值,不为空
			int start_ = cur_rle_x > last_rle_x ? last_rle_x : cur_rle_x;//对比两个行程编码的列开始值,获得两个行程编码的列开始值的最小值
			int end_ = cur_rle_end_x > last_rle_end_x ? cur_rle_end_x : last_rle_end_x;//对比两个行程编码的列结束值,获得两个行程编码的列结束值的最大值
			last_rle_x = start_;
			last_rle_end_x = end_;
		}
		else{//当前列结束值最小值小于当前列开始值最大值,为空,可写交集运算的值
			*p_row_rle_x++ = last_rle_x;
			*p_row_rle_y++ = row;
			*p_row_rle_end_x++ = last_rle_end_x - 1;
			last_rle_x = cur_rle_x;
			last_rle_end_x = cur_rle_end_x;
		}
	}

	int cur_rle_x;
	int cur_rle_end_x;
	while (i != endA){
		//获得rle的当前列开始值和列结束值
		cur_rle_x = rle_x[i];
		cur_rle_end_x = rle_end_x[i] + 1;
		int col_start = cur_rle_x > last_rle_x ? cur_rle_x : last_rle_x;//对比两个行程编码的列开始值,获得两个行程编码的列开始值的最大值
		int col_end = cur_rle_end_x > last_rle_end_x ? last_rle_end_x : cur_rle_end_x;//对比两个行程编码的列结束值,获得两个行程编码的列结束值的最小值
		if (col_end >= col_start){//当前列结束值最小值大于等于当前列开始值最大值,不为空
			int start_ = cur_rle_x > last_rle_x ? last_rle_x : cur_rle_x;//对比两个行程编码的列开始值,获得两个行程编码的列开始值的最小值
			int end_ = cur_rle_end_x > last_rle_end_x ? cur_rle_end_x : last_rle_end_x;//对比两个行程编码的列结束值,获得两个行程编码的列结束值的最大值
			last_rle_x = start_;
			last_rle_end_x = end_;
			i++;
		}
		else{
			break;
		}
	}
	while (j != endB){
		//获得Region的当前列开始值和列结束值
		cur_rle_x = region_x[j];
		cur_rle_end_x = region_end_x[j] + 1;
		int col_start = cur_rle_x > last_rle_x ? cur_rle_x : last_rle_x;//对比两个行程编码的列开始值,获得两个行程编码的列开始值的最大值
		int col_end = cur_rle_end_x > last_rle_end_x ? last_rle_end_x : cur_rle_end_x;//对比两个行程编码的列结束值,获得两个行程编码的列结束值的最小值
		if (col_end >= col_start){//当前列结束值最小值大于等于当前列开始值最大值,不为空
			int start_ = cur_rle_x > last_rle_x ? last_rle_x : cur_rle_x;//对比两个行程编码的列开始值,获得两个行程编码的列开始值的最小值
			int end_ = cur_rle_end_x > last_rle_end_x ? cur_rle_end_x : last_rle_end_x;//对比两个行程编码的列结束值,获得两个行程编码的列结束值的最大值
			last_rle_x = start_;
			last_rle_end_x = end_;
			j++;
		}
		else{
			break;
		}
	}
	*p_row_rle_x++ = last_rle_x;
	*p_row_rle_y++ = row;
	*p_row_rle_end_x++ = last_rle_end_x - 1;

	//没有遍历到rle_x当前行的结束值
	while (i != endA){
		last_rle_x = rle_x[i];
		last_rle_end_x = rle_end_x[i];
		*p_row_rle_x++ = last_rle_x;
		*p_row_rle_y++ = row;
		*p_row_rle_end_x++ = last_rle_end_x;
		i++;
	}

	//没有遍历到Region当前行的结束值
	while (j != endB){
		last_rle_x = region_x[j];
		last_rle_end_x = region_end_x[j];
		*p_row_rle_x++ = last_rle_x;
		*p_row_rle_y++ = row;
		*p_row_rle_end_x++ = last_rle_end_x;
		j++;
	}

	*row_rle_x = p_row_rle_x;
	*row_rle_y = p_row_rle_y;
	*row_rle_end_x = p_row_rle_end_x;
}
//end

//intersect start 
void intersect_two_row(const short* rle_x, const short* rle_end_x, const short* region_x, const short* region_end_x, short** row_rle_x, short** row_rle_y, short** row_rle_end_x,
	short row, int startA, int endA, int startB, int endB)
{
	//对比两个Region的同时存在的值,即为交集
	int i = startA;
	int j = startB;
	short* p_row_rle_x = *row_rle_x;
	short* p_row_rle_y = *row_rle_y;
	short* p_row_rle_end_x = *row_rle_end_x;

	//step1:参数初始化
	//将两个Region合并成一个,升序排序,获得第一个最小的列结束的值
	int xe = rle_x[i] <= region_x[j] ? (rle_end_x[i++] + 1) : (region_end_x[j++] + 1);
	int last_rle_x;
	int last_rle_end_x;
	//获得下一个最小的列开始值和列结束值
	if (i == endA){
		last_rle_x = region_x[j];
		last_rle_end_x = region_end_x[j];
	}
	else if (j == endB){
		last_rle_x = rle_x[i];
		last_rle_end_x = rle_end_x[i];
	}
	else{
		last_rle_x = rle_x[i] <= region_x[j] ? rle_x[i] : region_x[j];
		last_rle_end_x = rle_x[i] <= region_x[j] ? rle_end_x[i] : region_end_x[j];
	}

	//step2:遍历两个Region所存在的交集
	while (i != endA && j != endB){
		//获得当前交集的长度
		int len = intersection(xe, last_rle_x, last_rle_end_x - last_rle_x + 1);
		if (len > 0){
			*p_row_rle_x++ = last_rle_x;//当前的列开始值
			*p_row_rle_y++ = row;
			*p_row_rle_end_x++ = last_rle_x + len - 1;//当前的列开始值+当前交集的长度
		}
		//获得下一个最小的列开始值和列结束值
		int a = rle_x[i] <= region_x[j] ? (i++) : (j++);
		if (i == endA){
			last_rle_x = region_x[j];
			last_rle_end_x = region_end_x[j];
			break;
		}
		else if (j == endB){
			last_rle_x = rle_x[i];
			last_rle_end_x = rle_end_x[i];
			break;
		}
		else{
			last_rle_x = rle_x[i] <= region_x[j] ? rle_x[i] : region_x[j];
			last_rle_end_x = rle_x[i] <= region_x[j] ? rle_end_x[i] : region_end_x[j];
		}
	}

	//没有遍历到rle_x当前行的结束值
	while (i != endA){
		//获得当前交集的长度
		int len = intersection(xe, last_rle_x, last_rle_end_x - last_rle_x + 1);
		if (len > 0){
			*p_row_rle_x++ = last_rle_x;
			*p_row_rle_y++ = row;
			*p_row_rle_end_x++ = last_rle_x + len - 1;
		}
		//获得下一个最小的列开始值和列结束值
		i++;
		last_rle_x = rle_x[i];
		last_rle_end_x = rle_end_x[i];
	}

	//没有遍历到region_x当前行的结束值
	while (j != endB){
		//获得当前交集的长度
		int len = intersection(xe, last_rle_x, last_rle_end_x - last_rle_x + 1);
		if (len > 0){
			*p_row_rle_x++ = last_rle_x;
			*p_row_rle_y++ = row;
			*p_row_rle_end_x++ = last_rle_x + len - 1;
		}
		//获得下一个最小的列开始值和列结束值
		j++;
		last_rle_x = region_x[j];
		last_rle_end_x = region_end_x[j];
	}

	*row_rle_x = p_row_rle_x;
	*row_rle_y = p_row_rle_y;
	*row_rle_end_x = p_row_rle_end_x;
}

int intersection(int & xe, int const & r_start, int const & r_len){
	int xs = r_start;
	int len = r_len;
	int xt = r_start + r_len;

	if (xe <= xs) {//当前列开始值大于最小的列结束值
		xe = r_start + r_len;//使用当前列结束值修改最小列结束值
		return -1;
	}
	if (xe < xt) {//当前列开始值小于最小的列结束值并且当前列结束值大于最小结束值
		len = xe - r_start;//获得当前交集的长度
		xe = r_start + r_len;//使用当前列结束值修改最小列结束值
	}
	return len;
}
//end

//difference start
void difference_two_row(const short* rle_x, const short* rle_end_x, const short* region_x, const short* region_end_x, short** row_rle_x, short** row_rle_y, short** row_rle_end_x,
	short row, int startA, int endA, int startB, int endB)
{
	//寻找rle存在的行程编码而Region不存在的行程编码
	int i = startA;
	int j = startB;
	/*指针*/
	short* p_row_rle_x = *row_rle_x;
	short* p_row_rle_y = *row_rle_y;
	short* p_row_rle_end_x = *row_rle_end_x;

	while (i != endA && j != endB){
		//赋值
		int d_rle_x = rle_x[i];
		int d_rle_end_x = rle_end_x[i];
		i++;
		do{//共四种可能
			if (d_rle_end_x <= region_end_x[j])//d_rle_x为region_end_x[j]有一种可能
			{
				if (region_x[j] > d_rle_x){//Region的列开始值大于当前参数,d_rle_x为rle_x[i]两种可能
					*p_row_rle_x++ = d_rle_x;
					*p_row_rle_y++ = row;
					int len = min(d_rle_end_x - d_rle_x, region_x[j] - d_rle_x - 1);//判断最小长度
					*p_row_rle_end_x++ = d_rle_x + len;
				}

				//获得下一个参数的值
				if (i != endA){
					d_rle_x = rle_x[i];
					d_rle_end_x = rle_end_x[i];
					i++;
				}
				else{
					d_rle_x = region_end_x[j] + 1;
					break;
				}
			}
			else
			{
				if (d_rle_x < region_x[j]){//Region的列开始值大于当前参数,d_rle_x为rle_x[i]一种可能
					*p_row_rle_x++ = d_rle_x;
					*p_row_rle_y++ = row;
					int len = region_x[j] - d_rle_x - 1;
					*p_row_rle_end_x++ = d_rle_x + len;
				}

				//获得下一个参数的值
				if (region_end_x[j] >= d_rle_x){
					d_rle_x = region_end_x[j] + 1;
				}
				++j;
			}
		} while (j != endB);

		//获取一组参数并赋值
		if (d_rle_end_x - d_rle_x >= 0){
			*p_row_rle_x++ = d_rle_x;
			*p_row_rle_y++ = row;
			*p_row_rle_end_x++ = d_rle_end_x;
			d_rle_x = rle_x[i];
			d_rle_end_x = rle_end_x[i];
		}
		else{
			break;
		}

		//rle没遍历到最后的值,取剩下的所有值
		while (1){
			if (i == endA){
				break;
			}
			*p_row_rle_x++ = d_rle_x;
			*p_row_rle_y++ = row;
			*p_row_rle_end_x++ = d_rle_end_x;
			i++;
			d_rle_x = rle_x[i];
			d_rle_end_x = rle_end_x[i];
		}
	}

	*row_rle_x = p_row_rle_x;
	*row_rle_y = p_row_rle_y;
	*row_rle_end_x = p_row_rle_end_x;
}
//end



//connection start 
//offset = 0 -> 4联通
//offset = 1 -> 8联通
int labeling(const short* rle_x, const short* rle_end_x, const short* rle_y, int len,vector<int>& runLabels, int offset){
	int idxLabel = 1;
	int curRowIdx = 0;
	int firstRunOnCur = 0;
	int firstRunOnPre = 0;
	int lastRunOnPre = -1;
	int NumberOfRuns = len;

	//完成团的标记和等价对列表的生成,将P改成等价对中最小的值
	int *P = new int[NumberOfRuns];
	//first label is for background pixels
	P[0] = 0;

	for (int i = 0; i < NumberOfRuns;i++){

		if (rle_y[i] != curRowIdx){
			curRowIdx = rle_y[i];
			firstRunOnPre = firstRunOnCur;
			lastRunOnPre = i - 1;
			firstRunOnCur = i;
		}

		for (int j = firstRunOnPre; j <= lastRunOnPre; j++){
			if (rle_x[i] <= rle_end_x[j] + offset && rle_end_x[i] >= rle_x[j] - offset && rle_y[i] == rle_y[j] + 1)
			{
				if (runLabels[i] == 0) // 没有被标号过
					runLabels[i] = runLabels[j];
				else if (runLabels[i] != runLabels[j])// 已经被标号      
					runLabels[i] = set_union(P, runLabels[i], runLabels[j]);
			}
		}
		if (runLabels[i] == 0) // 没有与前一列的任何run重合
		{
			P[idxLabel] = idxLabel;
			runLabels[i] = idxLabel++;
		}
	}

	//analysis 重新排序
	int nLabels = flattenL(P, idxLabel);

	for (int i = 0; i < runLabels.size(); ++i)
	{
		//int & l = runLabels.at(i);
		int l = runLabels.at(i);
		runLabels[i] = P[l];
		//l = P[l];
	}

	delete[]  P;
	return nLabels;
}

//Find the root of the tree of node i
template<typename LabelT>
inline static
LabelT findRoot(const LabelT *P, LabelT i){
	LabelT root = i;
	while (P[root] < root){
		root = P[root];
	}
	return root;
}

//Make all nodes in the path of node i point to root
template<typename LabelT>
inline static
void setRoot(LabelT *P, LabelT i, LabelT root){
	while (P[i] < i){
		LabelT j = P[i];
		P[i] = root;
		i = j;
	}
	P[i] = root;
}

//Find the root of the tree of the node i and compress the path in the process
template<typename LabelT>
inline static
LabelT find(LabelT *P, LabelT i){
	LabelT root = findRoot(P, i);
	setRoot(P, i, root);
	return root;
}

//unite the two trees containing nodes i and j and return the new root
template<typename LabelT>
inline static
LabelT set_union(LabelT *P, LabelT i, LabelT j){
	LabelT root = findRoot(P, i);
	if (i != j){
		LabelT rootj = findRoot(P, j);
		if (root > rootj){
			root = rootj;
		}
		setRoot(P, j, root);
	}
	setRoot(P, i, root);
	return root;
}

//Flatten the Union Find tree and relabel the components
template<typename LabelT>
inline static
LabelT flattenL(LabelT *P, LabelT length){
	LabelT k = 1;
	for (LabelT i = 1; i < length; ++i){
		if (P[i] < i){
			P[i] = P[P[i]];
		}
		else{
			P[i] = k; k = k + 1;
		}
	}
	return k - 1;
}
//end

//select_shape start 

//归并排序
void Merge(int A[], int left, int mid, int right)// 合并两个已排好序的数组A[left...mid]和A[mid+1...right]
{
	int len = right - left + 1;
	int *temp = new int[len];       // 辅助空间O(n)
	int index = 0;
	int i = left;                   // 前一数组的起始元素
	int j = mid + 1;                // 后一数组的起始元素
	while (i <= mid && j <= right)
	{
		temp[index++] = A[i] <= A[j] ? A[i++] : A[j++];  // 带等号保证归并排序的稳定性
	}
	while (i <= mid)
	{
		temp[index++] = A[i++];
	}
	while (j <= right)
	{
		temp[index++] = A[j++];
	}
	for (int k = 0; k < len; k++)
	{
		A[left++] = temp[k];
	}
}

void MergeSortRecursion(int A[], int left, int right)    // 递归实现的归并排序(自顶向下)
{
	if (left == right)    // 当待排序的序列长度为1时，递归开始回溯，进行merge操作
		return;
	int mid = (left + right) / 2;
	MergeSortRecursion(A, left, mid);
	MergeSortRecursion(A, mid + 1, right);
	Merge(A, left, mid, right);
}

void MergeSortIteration(int A[], int len)    // 非递归(迭代)实现的归并排序(自底向上)
{
	int left, mid, right;// 子数组索引,前一个为A[left...mid]，后一个子数组为A[mid+1...right]
	for (int i = 1; i < len; i *= 2)        // 子数组的大小i初始为1，每轮翻倍
	{
		left = 0;
		while (left + i < len)              // 后一个子数组存在(需要归并)
		{
			mid = left + i - 1;
			right = mid + i < len ? mid + i : len - 1;// 后一个子数组大小可能不够
			Merge(A, left, mid, right);
			left = right + 1;               // 前一个子数组索引向后移动
		}
	}
}

int IsEqualInt(int number, int m_runLabels_count[], int len){
	for (int i = 0; i < len; i++){
		if (m_runLabels_count[i] == number){
			return i;
		}
	}
	return -1;
}
//end

