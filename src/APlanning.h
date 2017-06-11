#ifndef APlanning_H
#define APlanning_H

#include "DataStruct.h"


class APlanning 
{
public:
	APlanning();
	~APlanning();

	NavigationNode* m_pCurEmit;			// ��������
	NavigationNode* m_pCurTarget;		// �����Ŀ���

	// ������ͺ���������������
	pRouteList OpenList;       // Open,CLose���� 
	std::set<EarthPoint>CloseList;
	RouteList ShowList;       // ȫ��·��չʾ����
	RouteList TaskList;
	RouteList TempList;
	pNaviNode MinCostNode;           // ����ѭ���ĵ����ڵ��ָ��

	bool run();          //�滮�������
	bool getTask(vector<GroundPoint> &LeaderVector);      //��ȡ����
	bool InitAStar();                     //��ʼ����ر�����������ǰ�����Ƿ�Ϸ�
	bool PlanIsOK(pNaviNode pCurNode);    //�жϵ�ǰ���Ƿ���Ŀ���
    bool IsTempTar(pNaviNode pCurNode);   //�жϵ�ǰ���Ƿ�����ʱĿ���
	bool IsInThreat(pNaviNode pointA, pNaviNode pointB);
	bool IsInForbid(pNaviNode pointA, pNaviNode pointB);
	bool OutPutData(vector<GroundPoint> &ResultVector);   //���A*�ֲ��滮���
	
	//��ȡ���������Ϣ
	void getParaminput(double &minlength, //���������ڵ�֮�����С����
		double &maxlength,               //���������ڵ�֮���������
		double &maxhang,                 //�����ڵ�֮������ת��Ƕ�(���ȱ�ʾ)
		double &maxvang);                //�����ڵ�֮�������¶ȣ����ȱ�ʾ��

	//��ȡ��ͼ��Ϣ
    bool getMapInfo(LPCWSTR szFileName);           // ȫ�ֹ滮��ͼ�ķֱ��� 


	//��ʼ�����ͼ
	bool InitComMap(
		char * szMapFileName ,//���ͼ������
		__int64 i64MapHeightInPixel ,//���ͼ�߶����ص�
		__int64 i64MapWdithInPixel	//���ͼ��� ���ص�
		);

	//�������ͼ
	bool CreateComMap(
		TCHAR * szMapFileName , //���ͼ����
		vector<iPolygon> &forbidAndThreatVector );//��������в����Ϣ�����Ѿ�ת��)

private:

};


#endif