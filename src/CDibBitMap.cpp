
#include "CDibBitMap.h"//����

CDibBitMap::CDibBitMap()
{
	lpBitMapFileHader = NULL;
	lpBitMapInfo = NULL;
	lpMapData = NULL;
}

bool CDibBitMap::WriteMapData(const char * szFileName , int iMapDataSize)
{
	//�ļ������Ϸ�
	if(szFileName == NULL)
		return false ;

	//��û�з����ڴ�
	if (lpMapData == NULL || 
		lpBitMapInfo == NULL ||
		lpBitMapFileHader == NULL)
	{
		return false;
	}

	//д�ļ�ͷ
	FILE *fp = fopen(szFileName , "wb");
	if (NULL == fp)
	{
		return false;
	}
	fwrite(lpBitMapFileHader , sizeof(BITMAPFILEHEADER) , 1 , fp);

	//д�ļ���Ϣͷ
	fwrite(&(lpBitMapInfo->bmiHeader) , sizeof(BITMAPINFOHEADER) , 1 , fp);
	

	//дȡ��ɫ����Ϣ
	fwrite(lpBitMapInfo->bmiColors , 256*sizeof(RGBQUAD) , 1 , fp);
	

	//дȡͼƬʵ������
	fwrite(lpMapData , iMapDataSize , 1 , fp);
	
	fclose(fp);

	return true;
}


bool CDibBitMap::CreateBitMap(
	BITMAPFILEHEADER & bitMapFileHader ,  //�ļ�ͷ
	BITMAPINFO * bitMapInfo ,			//�ļ���Ϣͷ
	BYTE * mapData,						//ʵ������ָ��
	int iMapInfoSize ,					//�ļ���Ϣͷ���ݽṹ�Ĵ�С
	int iMapDataSize)					//ʵ�����ݵĴ�С
{
	//�ļ���Ϣͷ���ݽṹ�Ĵ�С����
	if (iMapInfoSize != sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD))
	{
		return false;
	}

	//��û�з����ڴ�
	if (lpMapData == NULL || 
		lpBitMapInfo == NULL ||
		lpBitMapFileHader == NULL)
	{
		if(!AllocateMap(iMapDataSize))
			return false;
	}

	//��������
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
	//�ͷ���ǰ������
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
