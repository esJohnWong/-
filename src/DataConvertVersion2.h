#pragma once
#include "fileHeaders.h"
#include <stdio.h>
#include <string.h>
class CDataConvertVersion2
{
public:
	CDataConvertVersion2(void);
	~CDataConvertVersion2(void);
public:
	MapFileHeader fileHeader;
	short * m_pData;
	bool Allocate(int iLength);
	bool Delete();
	bool Convert(
		char * pOrgFileName , //ԭʼ�ļ���
		char * pNewFileName);//ת������ļ���
	bool ReadOrgFileHeader(FILE * fp);//��ԭʼ�ļ�ͷ
	bool ReadOrgFileDataPerLine();//��ԭʼ�Ļ�����ÿ��
	bool WriteNewFileData(FILE * fpNew);
	bool WriteNewFileHeader(FILE * fp);
	bool WriteNewFileDataPerLine(FILE * fp);
	bool FindMaxAndMin(FILE * fp);//�ҵ�������С�ĸ߳�ֵ
};

