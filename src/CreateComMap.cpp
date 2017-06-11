#include "CreateComMap.h"
#include "WriteComMapFile.h"
#include "StandardizeConstraint.h"
#include "XmlParse.h"
#include "StandardizeConstraint.h"

CreateComMap::CreateComMap(void)
{
}


CreateComMap::~CreateComMap(void)
{
}

bool CreateComMap::InitComMap(
	char * szMapFileName ,//���ͼ������
	__int64 i64MapHeightInPixel ,//���ͼ�߶����ص�
	__int64 i64MapWdithInPixel)	//���ͼ��� ���ص�
{
	WriteComMapFile wcmf;
	if (!wcmf.WrtieAllData(szMapFileName , i64MapHeightInPixel , i64MapWdithInPixel))
	{
		return false;
	}
	return true;
}

bool CreateComMap::Create(
	TCHAR * szMapFileName , //���ͼ����
	HANDLE &hZhtMaping ,//���ͼ�ڴ�ӳ����
	__int64 i64MapHeightInPixel,//���ͼ�߶� ���ص�
	__int64 i64MapWdithInPixel ,//���ͼ��� ���ص�
	vector<iPolygon> &forbidAndThreatVector)//��������в����Ϣ �����Ѿ�ת��
{
	//��ʼ�����ͼ
	int needlen = WideCharToMultiByte( CP_ACP, 0, szMapFileName, -1, NULL, 0, NULL, NULL );
	PCHAR pAsciiChar = (PCHAR)HeapAlloc(GetProcessHeap() , 0 , needlen);
	WideCharToMultiByte( CP_ACP, 0, szMapFileName , -1, pAsciiChar , needlen, NULL, NULL );

	if(!InitComMap(pAsciiChar , i64MapHeightInPixel , i64MapWdithInPixel))
	{
		return false;
	}
	HeapFree(GetProcessHeap() , 0 , pAsciiChar);

	//ӳ���ڴ��ļ�
	if(!CreateGctDataMap(szMapFileName , hZhtMaping))
	{
		return false;
	}

	StandardizeConstraint standcos;//ת��������
	//���û��ת�����ڴ�ת��
// 	for (int i = 0 ; i < forbidAndThreatVector.size() ; i++)
// 	{
// 		standcos.Standardize(forbidAndThreatVector[i]);
// 	}

	//����������в��д�뵽���ͼ
	for(int i = 0 ; i < forbidAndThreatVector.size() ; i++)
	{
		//ӳ��һ����Ӿ��εĴ�С
		__int64 begin = forbidAndThreatVector[i].rectangle.ly;
		int row = forbidAndThreatVector[i].rectangle.height;
		int errOfStart;
		void * pBuf = ReadComMapData(hZhtMaping , begin , row , errOfStart);

		short * buf = (short *)((PUCHAR)pBuf + errOfStart);

		if (pBuf == NULL)
		{
			return false;
		}

		//�߽��ж�
		int tmp = forbidAndThreatVector[i].rectangle.lx;
		if (tmp < 0)
		{
			tmp = 0;
		}

		int length = tmp + forbidAndThreatVector[i].rectangle.width;
		if (length >= i64MapWdithInPixel)
		{
			length = i64MapWdithInPixel;
		}

		//�ж�һ�����Ƿ��ڶ������
		StandardizeConstraint standCons;

		for (int j = 0 ; j < row ; j++)
		{
			int y = j + forbidAndThreatVector[i].rectangle.ly;
			for (int k = tmp ; k < length ; k++)
			{
				iPoint pt;
				pt.x = k;
				pt.y = y;

				//�㲻�ڶ������
				if (!standCons.APointIsInPolygon(pt , forbidAndThreatVector[i]))
				{
					continue;
				}

				short * tmpBuf = buf + j*i64MapWdithInPixel + k;
				if (forbidAndThreatVector[i].level == 1 || forbidAndThreatVector[i].level == 2)
				{//������
					*tmpBuf = MAKESHORT(forbidAndThreatVector[i].level , FORBID_FLY_AREA);
				}
				else
				{//��в��
					*tmpBuf = MAKESHORT(forbidAndThreatVector[i].level , THREAT_FLY_AREA);
				}
			}
		}
		FlushViewOfFile(pBuf , 0);//���޸ĵ�����д�����
		UnmapViewOfFile(pBuf);//�ر�ӳ��
	}

	return true;
}