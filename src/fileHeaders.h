#ifndef FILE_HEADERS_H
#define FILE_HEADERS_H

struct CoordinatesData
{
	double longitude;//����
	double latitude;//γ��
	double height;//�߶�
	double deviation;//���
};

struct MapFileHeader 
{
	int nclos;			//����
	int nrows;			//����
	double xllcorner;		//���ϽǾ���
	double yllcorner;		//���ϽǾ���
	double sMin;		//��С����
	double sMax;		//��󺣰�
	int cellsize;		//����
	short NODATA_value; //��Ч����
};

#endif