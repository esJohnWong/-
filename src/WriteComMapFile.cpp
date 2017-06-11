#include "WriteComMapFile.h"
#include <cstdio>
#include <cstring>
#include "DataStruct.h"


WriteComMapFile::WriteComMapFile(void)
{
	m_pDataLine = NULL;
}


WriteComMapFile::~WriteComMapFile(void)
{
	if (m_pDataLine != NULL)
	{
		delete [] m_pDataLine;
	}
}

bool WriteComMapFile::Allocate(__int64 length)
{
	if (m_pDataLine == NULL)
	{
		m_pDataLine = new unsigned short[length];
	}

	if (m_pDataLine == NULL)
	{
		return false;
	}

	return true;
}

bool WriteComMapFile::WrtieAllData(const char * szFileName , __int64 height , __int64 width)
{
	//..
	CloseHandle(G_hZhtMapping);
	FILE * fp = fopen(szFileName , "wb");

	if (fp == NULL)
	{
		return false;
	}

	if (Allocate(width))
	{
		for (int i = 0 ; i < width ; i++)
		{
			m_pDataLine[i] = RAND_FLY_AREA;
		}

		for (int i = 0 ; i < height ; i++)
		{
			//每次写一行以免数据过大
			int cnt = fwrite(m_pDataLine  , width*sizeof(unsigned short) , 1 , fp);
			if (cnt != 1)
			{
				fclose(fp);
				return false;
			}
			fflush(fp);
		}
		
		fclose(fp);
	}
	else 
	{
		fclose(fp);
		return false;
	}
	return true;
}
