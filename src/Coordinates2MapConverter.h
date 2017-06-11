// oordinates2MapConverter.h: interface for the Coordinates2MapConverter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OORDINATES2MAPCONVERTER_H__4BC36D0D_5D02_48C1_8961_BA8364880CA3__INCLUDED_)
#define AFX_OORDINATES2MAPCONVERTER_H__4BC36D0D_5D02_48C1_8961_BA8364880CA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "cstdio"
#include "fileHeaders.h"
#include "CoordinatesFileParse.h"
#include "ReadMapFileOperation.h"
#include "WriteMapFileOperation.h"

class Coordinates2MapConverter  
{
public:
	Coordinates2MapConverter();
	virtual ~Coordinates2MapConverter();
public:
	bool GetCoordinatesHeadInfo(const char* fileName);
	bool Convert(const char* coordinatesFileName,const char * MapFileName);
private:
	
	MapFileHeader m_MapFileHeader;
	bool m_isGetHeadInfo;
	int m_iCurrentRow;
	int m_totalRow;
    ReadMapFileOperation r;
};

#endif // !defined(AFX_OORDINATES2MAPCONVERTER_H__4BC36D0D_5D02_48C1_8961_BA8364880CA3__INCLUDED_)
