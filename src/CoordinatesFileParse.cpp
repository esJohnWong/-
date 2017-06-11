
#include "CoordinatesFileParse.h"
#include "math.h"


CoordinatesFileParse::CoordinatesFileParse(void)
{
	m_CoordinatesFile = NULL;
	m_RowMultColumn = 0;
}
bool CoordinatesFileParse::OpenFile(const char* fileName)
{
	m_CoordinatesFile = fopen(fileName, "r");
	return (m_CoordinatesFile != NULL);
}
bool CoordinatesFileParse::ParseHeader()
{
	
	bool bSuc = false;
	do 
	{
		if (m_CoordinatesFile == NULL)
			break;
		
		//获取文件头参数
		fseek(m_CoordinatesFile,0,SEEK_SET);
		if (fscanf(m_CoordinatesFile, "%d", &m_RowMultColumn) == EOF)
			break;
		bSuc = true;
	} while(0);

	return bSuc;
}

int CoordinatesFileParse::getRowMultColumn()
{
	return m_RowMultColumn;
}

//获取图的行数、列数、起始经纬度、粒度
bool CoordinatesFileParse::GetMapFileHeaderFromCoordinatesFile(MapFileHeader &cMapFileHeader)
{
	if (m_CoordinatesFile != NULL)
	{
		if (0==m_RowMultColumn)
		{
			ParseHeader();
		}
		int column = 0;
		double cellsize = 0;
		CoordinatesData preData;
		CoordinatesData data;
		if(GetLine(preData))
		{
			cMapFileHeader.xllcorner = (int)preData.longitude;
			cMapFileHeader.yllcorner = (int)preData.latitude;
			column = 1;
		}
		else
		{
			return false;
		}

		if(GetLine(data))
		{
			cellsize = fabs(preData.longitude - data.longitude);
			cMapFileHeader.cellsize = (int)cellsize;
			if(preData.latitude!=data.latitude)
			{
				cMapFileHeader.nclos = column;
				cMapFileHeader.nrows = m_RowMultColumn/column;
				return true;
			}
			else
			{
				column = 2;
			}
			preData = data;
		}
		else
		{
			return false;;
		}
		while(GetLine(data))
		{
			if(preData.latitude!=data.latitude)
			{
				break;
			}
			else
			{
				column++;
			}
		}
		cMapFileHeader.nclos = column;
		cMapFileHeader.nrows = m_RowMultColumn/column;
	}
	else
	{
		return false;
	}
	return true;
}

bool CoordinatesFileParse::GetLine(CoordinatesData& data)
{
	int i = 0;
	char skipStr[4][100];
	if (m_CoordinatesFile == NULL)
	{
		return false;
	}
	/*
	if (fscanf(m_CoordinatesFile, "%lf %lf %lf %lf", &data.longitude,&data.latitude,&data.height,&data.deviation) == EOF)
	{
		return false;
	}
	*/
   
	if (fscanf(m_CoordinatesFile, "%s%s%s%s", skipStr[0],skipStr[1],skipStr[2],skipStr[3]) == EOF)
	{
		return false;
	}
	data.longitude = atof(skipStr[0]);
	data.latitude = atof(skipStr[1]);
	data.height = atof(skipStr[2]);
	data.deviation = atof(skipStr[3]);


	return true;
}

CoordinatesFileParse::~CoordinatesFileParse(void)
{
	if (m_CoordinatesFile != NULL)
	{
		fclose(m_CoordinatesFile);
		m_CoordinatesFile = NULL;
	}
}
