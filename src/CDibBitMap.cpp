
#include "CDibBitMap.h"//改名

CDibBitMap::CDibBitMap()
{
	lpBitMapFileHader = NULL;
	lpBitMapInfo = NULL;
	lpMapData = NULL;
}

bool CDibBitMap::WriteMapData(const char * szFileName , int iMapDataSize)
{
	//文件名不合法
	if(szFileName == NULL)
		return false ;

	//还没有分配内存
	if (lpMapData == NULL || 
		lpBitMapInfo == NULL ||
		lpBitMapFileHader == NULL)
	{
		return false;
	}

	//写文件头
	FILE *fp = fopen(szFileName , "wb");
	if (NULL == fp)
	{
		return false;
	}
	fwrite(lpBitMapFileHader , sizeof(BITMAPFILEHEADER) , 1 , fp);

	//写文件信息头
	fwrite(&(lpBitMapInfo->bmiHeader) , sizeof(BITMAPINFOHEADER) , 1 , fp);
	

	//写取调色板信息
	fwrite(lpBitMapInfo->bmiColors , 256*sizeof(RGBQUAD) , 1 , fp);
	

	//写取图片实际数据
	fwrite(lpMapData , iMapDataSize , 1 , fp);
	
	fclose(fp);

	return true;
}


bool CDibBitMap::CreateBitMap(
	BITMAPFILEHEADER & bitMapFileHader ,  //文件头
	BITMAPINFO * bitMapInfo ,			//文件信息头
	BYTE * mapData,						//实际数据指针
	int iMapInfoSize ,					//文件信息头数据结构的大小
	int iMapDataSize)					//实际数据的大小
{
	//文件信息头数据结构的大小不对
	if (iMapInfoSize != sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD))
	{
		return false;
	}

	//还没有分配内存
	if (lpMapData == NULL || 
		lpBitMapInfo == NULL ||
		lpBitMapFileHader == NULL)
	{
		if(!AllocateMap(iMapDataSize))
			return false;
	}

	//复制数据
	CopyMemory(
		lpBitMapFileHader , 
		&bitMapFileHader , 
		sizeof(BITMAPFILEHEADER));
	
	lpBitMapInfo->bmiHeader = bitMapInfo->bmiHeader;

	CopyMemory(
		lpBitMapInfo->bmiColors ,
		bitMapInfo->bmiColors , 
		256*sizeof(RGBQUAD));

	CopyMemory(lpMapData , mapData , iMapDataSize);
	return true;
}

bool CDibBitMap::AllocateMap(int iMapDataSize) 
{
	//释放先前的数据
	DeleteMap();

	lpBitMapFileHader = new BITMAPFILEHEADER;
	if (lpBitMapFileHader == NULL)
	{
		return false;
	}

	lpBitMapInfo =(BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)];
	if (lpBitMapInfo == NULL)
	{
		return false;
	}

	lpMapData = new BYTE[iMapDataSize];
	if (lpMapData == NULL)
	{
		return false ;
	}

	return true;
}

bool CDibBitMap::DeleteMap()
{
	if (lpBitMapFileHader != NULL)
	{
		delete lpBitMapFileHader;
		lpBitMapFileHader = NULL;
	}

	if (lpBitMapInfo != NULL)
	{
		delete lpBitMapInfo;
		lpBitMapInfo = NULL;
	}

	if (lpMapData != NULL)
	{
		delete lpMapData;
		lpMapData = NULL;
	}

	return true;
}

CDibBitMap::~CDibBitMap()
{
	DeleteMap();
}
