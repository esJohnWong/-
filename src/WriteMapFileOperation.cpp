// WriteMapFileOperation.cpp: implementation of the WriteMapFileOperation class.
//
//////////////////////////////////////////////////////////////////////


#include "WriteMapFileOperation.h"
#include "ReadMapFileOperation.h"
#include "ShortAllocate.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WriteMapFileOperation::WriteMapFileOperation()
{
	m_MapFile = NULL;
}

bool WriteMapFileOperation::OpenFileByWrite(const char* fileName)
{
	m_MapFile = fopen(fileName, "wb");
	return (m_MapFile != NULL);
}

bool WriteMapFileOperation::WriteMapFileHeader(const MapFileHeader &mapFileHeader)
{
	if (NULL==m_MapFile)
	{
		return false;
	}
	fseek(m_MapFile,0,SEEK_SET); //转到文件头
	int writeNum;
	writeNum = fwrite(&mapFileHeader,sizeof(MapFileHeader),1,m_MapFile);
	if (1!=writeNum)
	{
		return false;
	}
	return true;
}

bool WriteMapFileOperation::WriteMapFile(const short *data,const int count)
{
	if (NULL==m_MapFile)
	{
		return false;
	}
	int writeNum;
	writeNum = fwrite(data,sizeof(short),count,m_MapFile);
	if (count!=writeNum)
	{
		return false;
	}
	return true;
}

bool WriteMapFileOperation::WriteMapData(const int beginRow,const int beginColumn,const short *data,const int count)
{
	MapFileHeader mapFileHeader;
	ReadMapFileOperation rmfo;
	rmfo.ReadMapFileHeader(mapFileHeader);
	long readBeginPosition;
	
	readBeginPosition = sizeof(MapFileHeader) + beginRow * mapFileHeader.nclos * sizeof(short) + (beginColumn) * sizeof(short);
	fseek(m_MapFile,readBeginPosition,SEEK_SET);
	if(!WriteMapFile(data,count))
	{
		return false;
	}
	return true;
}
//向一个矩形区域写入不同的数据
bool WriteMapFileOperation::WriteMapFileOnRectArea(const int beginRow,const int beginColumn,const int rowCount,const int columnCount,const short **data,const int length)
{
	MapFileHeader mapFileHeader;
	ReadMapFileOperation rmfo;
	for(int row = beginRow,subData = 0;row<=beginRow+rowCount;row++,subData++)
	{
		rmfo.ReadMapFileHeader(mapFileHeader);
		long readBeginPosition;
		
		readBeginPosition = sizeof(MapFileHeader) + beginRow * mapFileHeader.nclos * sizeof(short) + (beginColumn) * sizeof(short);
		fseek(m_MapFile,readBeginPosition,SEEK_SET);
		if(!WriteMapFile(data[subData],columnCount))
		{
			return false;
		}
	}
	
	return true;
}

bool WriteMapFileOperation::WriteMapFileOnCircleArea(const int x_coordinate,const int y_coordiante,const int radius,int data)
{
	return true;
}


WriteMapFileOperation::~WriteMapFileOperation()
{
	if (m_MapFile != NULL)
	{
		fclose(m_MapFile);
		m_MapFile = NULL;
	}
}
