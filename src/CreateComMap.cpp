#include "CreateComMap.h"
#include "WriteComMapFile.h"
#include "StandardizeConstraint.h"
#include "XmlParse.h"
#include "StandardizeConstraint.h"

CreateComMap::CreateComMap(void)
{
}


CreateComMap::~CreateComMap(void)
{
}

bool CreateComMap::InitComMap(
	char * szMapFileName ,//组合图的名称
	__int64 i64MapHeightInPixel ,//组合图高度像素点
	__int64 i64MapWdithInPixel)	//组合图宽度 像素点
{
	WriteComMapFile wcmf;
	if (!wcmf.WrtieAllData(szMapFileName , i64MapHeightInPixel , i64MapWdithInPixel))
	{
		return false;
	}
	return true;
}

bool CreateComMap::Create(
	TCHAR * szMapFileName , //组合图名称
	HANDLE &hZhtMaping ,//组合图内存映射句柄
	__int64 i64MapHeightInPixel,//组合图高度 像素点
	__int64 i64MapWdithInPixel ,//组合图宽度 像素点
	vector<iPolygon> &forbidAndThreatVector)//禁飞区威胁区信息 坐标已经转换
{
	//初始化组合图
	int needlen = WideCharToMultiByte( CP_ACP, 0, szMapFileName, -1, NULL, 0, NULL, NULL );
	PCHAR pAsciiChar = (PCHAR)HeapAlloc(GetProcessHeap() , 0 , needlen);
	WideCharToMultiByte( CP_ACP, 0, szMapFileName , -1, pAsciiChar , needlen, NULL, NULL );

	if(!InitComMap(pAsciiChar , i64MapHeightInPixel , i64MapWdithInPixel))
	{
		return false;
	}
	HeapFree(GetProcessHeap() , 0 , pAsciiChar);

	//映射内存文件
	if(!CreateGctDataMap(szMapFileName , hZhtMaping))
	{
		return false;
	}

	StandardizeConstraint standcos;//转换坐标用
	//如果没有转换就在此转换
// 	for (int i = 0 ; i < forbidAndThreatVector.size() ; i++)
// 	{
// 		standcos.Standardize(forbidAndThreatVector[i]);
// 	}

	//将禁飞区威胁区写入到组合图
	for(int i = 0 ; i < forbidAndThreatVector.size() ; i++)
	{
		//映射一个外接矩形的大小
		__int64 begin = forbidAndThreatVector[i].rectangle.ly;
		int row = forbidAndThreatVector[i].rectangle.height;
		int errOfStart;
		void * pBuf = ReadComMapData(hZhtMaping , begin , row , errOfStart);

		short * buf = (short *)((PUCHAR)pBuf + errOfStart);

		if (pBuf == NULL)
		{
			return false;
		}

		//边界判断
		int tmp = forbidAndThreatVector[i].rectangle.lx;
		if (tmp < 0)
		{
			tmp = 0;
		}

		int length = tmp + forbidAndThreatVector[i].rectangle.width;
		if (length >= i64MapWdithInPixel)
		{
			length = i64MapWdithInPixel;
		}

		//判断一个点是否在多边形中
		StandardizeConstraint standCons;

		for (int j = 0 ; j < row ; j++)
		{
			int y = j + forbidAndThreatVector[i].rectangle.ly;
			for (int k = tmp ; k < length ; k++)
			{
				iPoint pt;
				pt.x = k;
				pt.y = y;

				//点不在多边形中
				if (!standCons.APointIsInPolygon(pt , forbidAndThreatVector[i]))
				{
					continue;
				}

				short * tmpBuf = buf + j*i64MapWdithInPixel + k;
				if (forbidAndThreatVector[i].level == 1 || forbidAndThreatVector[i].level == 2)
				{//禁飞区
					*tmpBuf = MAKESHORT(forbidAndThreatVector[i].level , FORBID_FLY_AREA);
				}
				else
				{//威胁区
					*tmpBuf = MAKESHORT(forbidAndThreatVector[i].level , THREAT_FLY_AREA);
				}
			}
		}
		FlushViewOfFile(pBuf , 0);//将修改的数据写入磁盘
		UnmapViewOfFile(pBuf);//关闭映射
	}

	return true;
}