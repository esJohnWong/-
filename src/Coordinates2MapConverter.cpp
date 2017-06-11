// oordinates2MapConverter.cpp: implementation of the Coordinates2MapConverter class.
//
//////////////////////////////////////////////////////////////////////

#include "Coordinates2MapConverter.h"
#include "CoordinatesFileParse.h"
#include "WriteMapFileOperation.h"
#include "ShortAllocate.h"
#include "iostream"
#include "DataStruct.h"
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Coordinates2MapConverter::Coordinates2MapConverter()
{
	m_isGetHeadInfo = false;
	m_iCurrentRow = 0;
	m_totalRow = 0;
}

bool Coordinates2MapConverter::GetCoordinatesHeadInfo(const char* fileName)
{
	CoordinatesFileParse coordinatesFileParse;
	if (!coordinatesFileParse.OpenFile(fileName))
	{
		return false;
	}
	
	if (!coordinatesFileParse.ParseHeader())
	{
		return false;
	}

    if (!coordinatesFileParse.GetMapFileHeaderFromCoordinatesFile(m_MapFileHeader))
    {
		return false;
    }
	m_isGetHeadInfo = true;
	return true;
}

bool Coordinates2MapConverter::Convert(const char* coordinatesFileName,const char * mapFileName)
{
	//获取coordinatesFile的行列数、起始坐标和精度
	
	if (!m_isGetHeadInfo)
	{
		if (!GetCoordinatesHeadInfo(coordinatesFileName))
		{
			return false;
		}
	}

	m_MapFileHeader.sMin = 0;
	m_MapFileHeader.sMax = 0;
	m_MapFileHeader.NODATA_value = -9999;

	//读取文件头，主要是将文件指针移到数据区
	bool bSuc = false;
	CoordinatesFileParse coordinatesFileParse;
	if (!coordinatesFileParse.OpenFile(coordinatesFileName))
	{
		return false;
	}
	if (!coordinatesFileParse.ParseHeader())
	{
		return false;
	}
	
   	CShortAllocate shortData;
	if (!shortData.Allocate(m_MapFileHeader.nclos))
	{
		return false;
	}
    
	CoordinatesData coordinatesData;
	WriteMapFileOperation wmfo;
	if (!wmfo.OpenFileByWrite(mapFileName))
	{
		return false;
	}

    if (!wmfo.WriteMapFileHeader(m_MapFileHeader))
    {
		return false;
    }
    int row;
	int column;
	int data;
	for (row = 0;row<m_MapFileHeader.nrows;row++)
	{
		//如果是线程结束则不进行计算
		if (isEndThread)
		{
			break;
		}


		for (column = 0;column<m_MapFileHeader.nclos;column++)
		{
			coordinatesFileParse.GetLine(coordinatesData);
			//shortData.m_pData[column] = (short)coordinatesData.height;
			data = coordinatesData.height;
			shortData.m_pData[column] = data;
			//shortData.m_pData[column] = 1;
			if (m_MapFileHeader.sMin>coordinatesData.height)
			{
				m_MapFileHeader.sMin = coordinatesData.height;
			}

			if (m_MapFileHeader.sMax<coordinatesData.height)
			{
				m_MapFileHeader.sMax = coordinatesData.height;
			}
		}
		
		if (!wmfo.WriteMapFile(shortData.m_pData,m_MapFileHeader.nclos))
		{
			return false;
		}
		
    }

	if (!wmfo.WriteMapFileHeader(m_MapFileHeader))
    {
		return false;
    }
	
	return true;
}

Coordinates2MapConverter::~Coordinates2MapConverter()
{

}
