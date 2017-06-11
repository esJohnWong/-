/************************************************************************\
* 位图处理类
*此类只能处理8位的位图                                                      
\************************************************************************/

#pragma once
#include <stdio.h>
#include <Windows.h>

class CDibBitMap
{
public:
	CDibBitMap();
	~CDibBitMap();

	//将位图数据写入磁盘
	bool WriteMapData(const char * szFileName , int iMapDataSize);

	//根据给定的信息创建一个位图
	bool CreateBitMap(
		BITMAPFILEHEADER & bitMapFileHader ,
		BITMAPINFO * bitMapInfo ,
		BYTE * mapData ,
		int iMapInfoSize ,
		int iMapDataSize);

	//销毁分配的数据
	bool DeleteMap();

	//为变量分配内存
	bool AllocateMap(int iMapDataSize);
public:
	BITMAPFILEHEADER * lpBitMapFileHader; //DIB文件头
	BITMAPINFO * lpBitMapInfo;			 //DIB文件信息头
	BYTE * lpMapData; //实际图片数据
};

// 2、cpp文件：

