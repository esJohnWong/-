/********************************
*�������ͼ��
********************************/
#pragma once
#include "DataStruct.h"
class CreateComMap
{
public:
	CreateComMap(void);
	~CreateComMap(void);

	//��ʼ�����ͼ
	bool InitComMap(
		char * szMapFileName ,//���ͼ������
		__int64 i64MapHeightInPixel ,//���ͼ�߶����ص�
		__int64 i64MapWdithInPixel	//���ͼ��� ���ص�
		);

	//�������ͼ
	bool Create(
		TCHAR * szMapFileName , //���ͼ����
		HANDLE &hZhtMaping ,//���ͼ�ڴ�ӳ����
		__int64 i64MapHeightInPixel,//���ͼ�߶� ���ص�
		__int64 i64MapWdithInPixel ,//���ͼ��� ���ص�
		vector<iPolygon> &forbidAndThreatVector );//��������в����Ϣ�����Ѿ�ת��)
};

