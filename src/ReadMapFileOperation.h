// ReadMapFileOperation.h: interface for the MapFileOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READMAPFILEOPERATION_H__31DBAD3F_1921_42CB_8F5B_02B955CFD988__INCLUDED_)
#define AFX_READMAPFILEOPERATION_H__31DBAD3F_1921_42CB_8F5B_02B955CFD988__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "cstdio"
#include "fileHeaders.h"
class ReadMapFileOperation
{
public:
	ReadMapFileOperation();
	~ReadMapFileOperation();
public:
	bool OpenFileByRead(const char* fileName);
	bool ReadMapFileHeader(MapFileHeader &mapFileHeader);
	bool ReadMapFileData(short *data,const int count);
	bool ReadMapFileData(const int beginRow,const int beginColumn,short *data,const int count);
	bool ReadMapFileOnRectArea(const int beginRow,const int beginColumn,const int width,const int Hight,short **data,const int count);
	bool ReadMapFileOnCircleArea(const int x_coordinate,const int y_coordiante,const int radius,int data);
	bool Closefp();
private:
	FILE *m_MapFile;
};

#endif // !defined(AFX_MAPFILEOPERATION_H__31DBAD3F_1921_42CB_8F5B_02B955CFD988__INCLUDED_)
