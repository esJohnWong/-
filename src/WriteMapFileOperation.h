// WriteMapFileOperation.h: interface for the WriteMapFileOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRITEMAPFILEOPERATION_H__31DBAD3F_1921_42CB_8F5B_02B955CFD988__INCLUDED_)
#define AFX_WRITEMAPFILEOPERATION_H__31DBAD3F_1921_42CB_8F5B_02B955CFD988__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "cstdio"
#include "fileHeaders.h"

class WriteMapFileOperation  
{
public:
	WriteMapFileOperation();
	~WriteMapFileOperation();
public:
	bool OpenFileByWrite(const char* fileName);
	bool WriteMapFileHeader(const MapFileHeader &mapFileHeader);
	bool WriteMapFile(const short *data,const int count);
	bool WriteMapData(const int beginRow,const int beginColumn,const short *data,const int count);
	bool WriteMapFileOnRectArea(const int beginRow,const int beginColumn,const int rowCount,const int columnCount,const short **data,const int length);
	bool WriteMapFileOnCircleArea(const int x_coordinate,const int y_coordiante,const int radius,int data);
	
private:
	FILE *m_MapFile;
};

#endif // !defined(AFX_MAPFILEOPERATION_H__31DBAD3F_1921_42CB_8F5B_02B955CFD988__INCLUDED_)
