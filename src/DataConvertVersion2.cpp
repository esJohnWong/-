#include "DataConvertVersion2.h"

CDataConvertVersion2::CDataConvertVersion2(void)
{
	m_pData = NULL;
}


CDataConvertVersion2::~CDataConvertVersion2(void)
{
	Delete();
}

bool CDataConvertVersion2::Allocate(int iLength)
{
	if (NULL != m_pData)
	{
		Delete();
	}

	m_pData = new short[iLength];
	if (NULL == m_pData)
	{
		return false;
	}

	return true;
}

bool CDataConvertVersion2::Delete()
{
	if (NULL != m_pData)
	{
		delete []m_pData;
		m_pData = NULL;
	}

	return true;
}

bool CDataConvertVersion2::ReadOrgFileHeader(FILE * fp)
{
	char ncols[6];
	char nrows[6];
	char xllcorner[10];
	char yllcorner[10];
	char cellsize[9];
	char NODATA_value[13];

	rewind(fp);//返回文件头

	scanf("%s" , ncols);
	if (0 != strcmp(ncols , "ncols"))
	{
		return false;
	}
	scanf("%d" , &fileHeader.nclos);

	scanf("%s" , nrows);
	if (0 != strcmp(nrows , "nrows"))
	{
		return false;
	}
	scanf("%d" , &fileHeader.nrows);

	scanf("%s" , xllcorner);
	if (0 != strcmp("xllcorner" , xllcorner))
	{
		return false;
	}
	scanf("%lf" , &fileHeader.xllcorner);

	scanf("%s" , yllcorner);
	if (0 != strcmp("yllcorner" , yllcorner))
	{
		return false;
	}
	scanf("%lf" , &fileHeader.yllcorner);

	scanf("%s" , cellsize);
	if (0 != strcmp(cellsize , "cellsize"))
	{
		return false;
	}
	scanf("%d" , &fileHeader.cellsize);

	scanf("%s" , NODATA_value);
	if (0 != strcmp(NODATA_value , "NODATA_value"))
	{
		return false;
	}
	scanf("%d" , &fileHeader.NODATA_value);

	return true;
}

bool CDataConvertVersion2::ReadOrgFileDataPerLine()
{
	if (NULL == m_pData)
	{
		return false;
	}

	for (int i = 0 ; i <fileHeader.nclos ; i++)
	{	
		double dbTmp;
		scanf("%lf" , &dbTmp);
		if (dbTmp == fileHeader.NODATA_value)
		{
			m_pData[i]=fileHeader.NODATA_value;
			continue;
		}
		dbTmp += 0.5;
		m_pData[i] = (short)(dbTmp);
	}

	return true;
}

bool CDataConvertVersion2::WriteNewFileHeader(FILE * fp)
{
	rewind(fp);//重新返回文件头写文件头信息
	if(1 != fwrite(&fileHeader , sizeof(MapFileHeader) , 1 , fp))
		return false;
	return true;
}

bool CDataConvertVersion2::WriteNewFileDataPerLine(FILE * fp)
{
	if (1 != fwrite(m_pData , fileHeader.nclos*sizeof(short) , 1 , fp))
	{
		return false;
	}
	return true;
}

bool CDataConvertVersion2::FindMaxAndMin(FILE * fp)
{
	ReadOrgFileHeader(fp);

	double dbTmp;
	int i = 0 , iCnt = fileHeader.nrows * fileHeader.nclos;

	fileHeader.sMax = -10000;
	fileHeader.sMin = 8849;
	
	while (i < iCnt)
	{
		scanf("%lf" , &dbTmp);
		if (fileHeader.NODATA_value == dbTmp)
		{
			i++;
			continue;
		}
	
		if (fileHeader.sMax < dbTmp)
		{
			fileHeader.sMax = dbTmp;
		}

		if (fileHeader.sMin > dbTmp)
		{
			fileHeader.sMin = dbTmp;
		}
		i++;
	}

	return true;
}

bool CDataConvertVersion2::WriteNewFileData(FILE * fpNew)
{
	fseek(fpNew , sizeof(MapFileHeader) , SEEK_SET);
	for (int i = 0 ; i < fileHeader.nrows ; i++)
	{
		if(!ReadOrgFileDataPerLine())
			return false;

		if(!WriteNewFileDataPerLine(fpNew))
			return false;
	}

	return true;
}

bool CDataConvertVersion2::Convert(char * pOrgFileName , char * pNewFileName)
{
	
	//重定向输入输出流
	FILE * fpOrg = freopen(pOrgFileName , "r" , stdin);
	if (NULL == fpOrg)
	{
		return false;
	}

	//
	FILE * fpNew = fopen(pNewFileName , "wb");
	if (NULL == fpNew)
	{
		return false;
	}

	//先找出最大值和最小值
	if (!FindMaxAndMin(fpOrg))
	{
		return false;
	}

	if(!ReadOrgFileHeader(fpOrg))
		return false;

	if(!Allocate(fileHeader.nclos))
		return false;

	if(!WriteNewFileHeader(fpNew))
		return false;

	if (!WriteNewFileData(fpNew))
	{
		return false;
	}
	
	fclose(fpNew);
	fclose(fpOrg);
	return true;
}