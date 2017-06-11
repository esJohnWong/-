#ifndef FILE_HEADERS_H
#define FILE_HEADERS_H

struct CoordinatesData
{
	double longitude;//经度
	double latitude;//纬度
	double height;//高度
	double deviation;//误差
};

struct MapFileHeader 
{
	int nclos;			//列数
	int nrows;			//行数
	double xllcorner;		//左上角经度
	double yllcorner;		//左上角经度
	double sMin;		//最小海拔
	double sMax;		//最大海拔
	int cellsize;		//精度
	short NODATA_value; //无效数据
};

#endif