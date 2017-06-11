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
		char * pOrgFileName , //原始文件名
		char * pNewFileName);//转换后的文件名
	bool ReadOrgFileHeader(FILE * fp);//读原始文件头
	bool ReadOrgFileDataPerLine();//读原始文化数据每行
	bool WriteNewFileData(FILE * fpNew);
	bool WriteNewFileHeader(FILE * fp);
	bool WriteNewFileDataPerLine(FILE * fp);
	bool FindMaxAndMin(FILE * fp);//找到最大和最小的高程值
};

