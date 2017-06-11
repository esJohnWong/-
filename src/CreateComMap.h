/********************************
*生成组合图类
********************************/
#pragma once
#include "DataStruct.h"
class CreateComMap
{
public:
	CreateComMap(void);
	~CreateComMap(void);

	//初始化组合图
	bool InitComMap(
		char * szMapFileName ,//组合图的名称
		__int64 i64MapHeightInPixel ,//组合图高度像素点
		__int64 i64MapWdithInPixel	//组合图宽度 像素点
		);

	//生成组合图
	bool Create(
		TCHAR * szMapFileName , //组合图名称
		HANDLE &hZhtMaping ,//组合图内存映射句柄
		__int64 i64MapHeightInPixel,//组合图高度 像素点
		__int64 i64MapWdithInPixel ,//组合图宽度 像素点
		vector<iPolygon> &forbidAndThreatVector );//禁飞区威胁区信息坐标已经转换)
};

