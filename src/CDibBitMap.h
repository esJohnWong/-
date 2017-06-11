/************************************************************************\
* λͼ������
*����ֻ�ܴ���8λ��λͼ                                                      
\************************************************************************/

#pragma once
#include <stdio.h>
#include <Windows.h>

class CDibBitMap
{
public:
	CDibBitMap();
	~CDibBitMap();

	//��λͼ����д�����
	bool WriteMapData(const char * szFileName , int iMapDataSize);

	//���ݸ�������Ϣ����һ��λͼ
	bool CreateBitMap(
		BITMAPFILEHEADER & bitMapFileHader ,
		BITMAPINFO * bitMapInfo ,
		BYTE * mapData ,
		int iMapInfoSize ,
		int iMapDataSize);

	//���ٷ��������
	bool DeleteMap();

	//Ϊ���������ڴ�
	bool AllocateMap(int iMapDataSize);
public:
	BITMAPFILEHEADER * lpBitMapFileHader; //DIB�ļ�ͷ
	BITMAPINFO * lpBitMapInfo;			 //DIB�ļ���Ϣͷ
	BYTE * lpMapData; //ʵ��ͼƬ����
};

// 2��cpp�ļ���

