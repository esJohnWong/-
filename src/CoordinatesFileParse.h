#ifndef _COORDINATESFILEPARSE_H_
#define _COORDINATESFILEPARSE_H_
#include <cstdio>
#include "fileHeaders.h"

class CoordinatesFileParse
{
public:
	CoordinatesFileParse(void);
	~CoordinatesFileParse(void);
public:
	bool OpenFile(const char* fileName);
	bool ParseHeader();
	int getRowMultColumn();
	bool GetMapFileHeaderFromCoordinatesFile(MapFileHeader &cAscFileHeader);
	bool GetLine(CoordinatesData& data);
private:
	FILE* m_CoordinatesFile;
	int m_RowMultColumn;
};
#endif
