// ReadMapFileOperation.cpp: implementation of the ReadMapFileOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "ReadMapFileOperation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ReadMapFileOperation::ReadMapFileOperation()
{
	m_MapFile = NULL;
}

bool ReadMapFileOperation::OpenFileByRead(const char* fileName)
{
	m_MapFile = fopen(fileName, "rb");
	return (m_MapFile != NULL);
}

bool ReadMapFileOperation::ReadMapFileHeader(MapFileHeader &mapFileHeader)
{
	if (NULL==m_MapFile)
	{
		return false;
	}
	int readNum;
	fseek(m_MapFile,0,SEEK_SET);
	readNum = fread(&mapFileHeader,sizeof(MapFileHeader),1,m_MapFile);
	if (1>readNum)
	{
		return false;
	}
	return true;
}

bool ReadMapFileOperation::ReadMapFileData(short *data,const int count)
{
	if (NULL==m_MapFile)
	{
		return false;
	}

	int readNum;
	readNum = fread(data,sizeof(short),count,m_MapFile);
	if (count!=readNum)
	{
		return false;
	}
	return true;
}
bool ReadMapFileOperation::ReadMapFileData(const int beginRow,const int beginColumn,short *data,const int count)
{
	MapFileHeader mapFileHeader;
	ReadMapFileHeader(mapFileHeader);
	long readBeginPosition;
	
	readBeginPosition = sizeof(MapFileHeader) + beginRow * mapFileHeader.nclos * sizeof(short) + (beginColumn) * sizeof(short);
	fseek(m_MapFile,readBeginPosition,SEEK_SET);
	if(!ReadMapFileData(data,count))
	{
		return false;
	}
	return true;
}

bool ReadMapFileOperation::ReadMapFileOnRectArea(const int beginRow,const int beginColumn,const int width,const int Hight,short **data,const int count)
{
	return true;
}

bool ReadMapFileOperation::ReadMapFileOnCircleArea(const int x_coordinate,const int y_coordiante,const int radius,int data)
{
	return true;
}

ReadMapFileOperation::~ReadMapFileOperation()
{
	Closefp();
}

bool ReadMapFileOperation::Closefp()
{
	if (m_MapFile != NULL)
	{
		fclose(m_MapFile);
		m_MapFile = NULL;
	}
	return true;
}