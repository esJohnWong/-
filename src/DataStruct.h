
#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#pragma once
#include "fileHeaders.h"


///////////////////////////////////////////////////////////////////////////////
// ����C,C++��׼��
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <windows.h>
#include <math.h>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
using namespace std;

// ����C,C++��׼��
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////   
// ת����
#define RADIAN(angle) ((angle)*PI/180.0)  // �Ƕȵ����ȵ�ת����
#define ANIRAD(angle) ((angle)*180.0/PI)  // ���ȵ��Ƕȵ�ת����
#define Min(a,b) (((a) < (b)) ? (a) : (b))
#define Max(a,b) (((a) >= (b)) ? (a) : (b))
#define ThreatFactor 20 

//////////////////////////////////////////////////////////////////////////
// ������
const double PI = 3.14159265358979323846264338327950288419717;
const int SAMPLESTEP = 5;          // ��������
const double ZEROANGLE = 0.1;       // �ǶȵĽ����� 
const int ANGLEGRIDNUM = 45;
const int RADIUSGRIDNUM = 10;       // ��������


// ȫ�ֱ���
extern double factofthreat;         //��в����ϵ��
extern double G_MAPL0;              // ȫ�ֵ�ͼ���Ͻǵľ���
extern double G_MAPB0;              // ȫ�ֵ�ͼ���Ͻǵ�γ��
extern int G_WIDTH;                 // ȫ�ֹ滮��ͼ�Ŀ��
extern int G_HEIGHT;                // ȫ�ֹ滮��ͼ�ĸ߶�
extern double G_RESOLVE;            // ȫ�ֹ滮��ͼ�ķֱ��� // �߳�ͼ�͵��θ߳�ͼ��ͬ
extern DWORD G_dwGran;              // ϵͳ��������

//Լ������  add at 10-30 
extern double minl;         //���������ڵ�֮�����С����
extern double maxl;         //���������ڵ�֮���������
extern double maxhangle;    //�����ڵ�֮������ת��Ƕ�
extern double maxvangle;    //�����ڵ�֮�������¶�

extern int tt;

//��������
enum  ErrorType
{ 
	DefError, GetTaskErr, InitErr, ExpErr,GA_loadTaskErr
};
extern  ErrorType errStr;

//add at 12-19
//�Զ����Ľṹ
typedef struct _Point
{
	int x;
	int y;
}iPoint;

//����
typedef struct _Rect
{
	int lx;  //���Ͻ�x����
	int ly;  //���Ͻ�y����
	int width;
	int height;
}iRect;

//�����
typedef struct _Polygon 
{
	vector<iPoint> ptPolygon;  //����εĸ�����������
	iRect rectangle;  //��Ӿ���
	int level;        //�ȼ�
}iPolygon;

//�����ṹ
typedef struct _Parameter
{
	double minl;         //���������ڵ�֮�����С����
	double maxl;         //���������ڵ�֮���������
	double maxhangle;    //�����ڵ�֮������ת��Ƕ�
	double maxvangle;    //�����ڵ�֮�������¶�
}Param;

//��������
typedef struct _GroundPoint
{
	double X;       
	double Y;        
	double Z;  
}GroundPoint;


// ��������ƽ��ṹ
typedef struct _EarthPoint
{
	int L;	     // ��ľ�������
	int B;       // ���γ������

	bool operator < (const _EarthPoint &a) const 
	{
		if(a.L == L)
		{
			return a.B < B;
		}

		return a.L < L;
	}

}EarthPoint;


// ������
typedef struct _NavigationNode
{
	int X;					// ȫ��ͼX����   
	int Y;					// ȫ��ͼY����  
	int Z;					// �ռ�㺣�θ߶ȵĸ߳�ֵ
	int PointType;          // ��ǰ�ڵ�����ͣ�0:Ĭ�� 1:���  2:������ 3:Ŀ��㣩
	double angle;			//���� 
	double g_cost;			// SAS ��֪����   
	double h_cost;			// SAS Ԥ������
	double f_cost;			// SAS ƽ�����
	double p_cost;			//����ֵ
	double HP_cost;			// �߶ȴ���ֵ
	double t_cost;			// SAS �ܴ���
	struct _NavigationNode* pFather;	// �ռ�����һ�����ڵ�

}NavigationNode, *pNaviNode;

typedef std::list<NavigationNode> RouteList; //·������
typedef std::list<pNaviNode> pRouteList;     //·��ָ������


//����ƥ��������λ�ò���
typedef struct SearchAreaParameter
{
	int OrginX;         //��չ����ȫ�ֵ�ͼ��X����
	int OrginY;		    //��չ����ȫ�ֵ�ͼ��Y����
	double MinRadius;   //��̾���
	double MaxRadius;	//�����
	double MinAngle;    // ��С�Ƕ�  
	double MaxAngle;    // ���Ƕ�
	
}SearchAreaParam; // ����ƥ��������λ�ò���


//////////////////////////////////////////////////////////////////////////
//�����ṹ�ĳ�ʼ��
const EarthPoint DefEarthPoint ={
	0,			//int L
	0			//int B
};

const NavigationNode DefNavigationNode = {
	0,					//int X
	0,					//int Y
	0,					//int Z
	0,					//int PointType;
	0.0,				//double angle
	0.0,				//double g_cost
	0.0,				//double h_cost
	0.0,				//double f_cost
	0.0,				//double p_cost
	0.0,				//double HP_cost
	0.0,				//double t_cost
	NULL				//struct _NavigationNode* pFather;
};


////////////////////////A*��·�滮//////////////////////////////////
////////begin

//���������ߵĽǶ� 
double PLAN_LinkAngle(int L1,int B1,int L2,int B2);

//��ⷢ��㡢Ŀ����Ƿ�߶ȺϷ�
bool PLAN_CheckEmitTarPoint(pNaviNode pcurEmit, pNaviNode pcurtarget);

// ��������֮���ƽ�����
double PLAN_Distance(pNaviNode prev, pNaviNode next);

// �������ռ��֮��ľ���
double PLAN_KJDistance(pNaviNode prev, pNaviNode next);

// �жϵ��Ƿ��ڽ�������
bool PLAN_IsInForbid(int CurNodeX, int CurNodeY);

// �жϵ��Ƿ�����в����
bool PLAN_IsInThreat(int CurNodeX, int CurNodeY);

// �ж�Ŀ����Ƿ�������ƥ��������
bool PLAN_IsInSearchArea(pNaviNode pCurNode, pNaviNode pTarNode, SearchAreaParam SAP);

// ��в������
bool PLAN_CrossThreatenIJ(int I,int J, double& TD, int& level);

// �õ�ĳ��ĸ߳�ֵ��X,YΪȫ��ͼ����ֵ
bool GetZofPoint(int x, int y, int &Z , bool isInt  , HANDLE hFileMap);

bool LBtoIJ(double L,     // ��ǰ��ľ���      
	double B,     // ��ǰ���γ��
	double Level, // ��ǰʹ�õĵ�ͼ�ֱ��� // �������Ҫ��������Ū��
	int& I,       // �ڸ����ֱ����µ�ȫ������I(����)λ��
	int& J        // �ڸ����ֱ����µ�ȫ������J(����)λ��
	);

// ����ǰ��ĵ�ǰ�ֱ�����ȫ����������ת���ɾ�γ��
bool IJtoLB(int I,        // �ڸ����ֱ����µ�ȫ������I(����)λ��
	int J,        // �ڸ����ֱ����µ�ȫ������J(����)λ��
	double Level, // ʹ�õĹ滮��ͼ�ķֱ��� 				
	double& L,    // ��ǰ��ľ���      
	double& B     // ��ǰ���γ��								
	);

// ĳ���Ƿ��ڹ滮������
bool IsInAreaofPoint(int x, int y);

//  ��չ������
bool PLAN_ExpandNaviNode(pNaviNode pCurNode,    // ��ǰ��չ�ڵ� 
	pRouteList& OpenList,
	std::set<EarthPoint>& CloseList,
	pNaviNode pEmiNode,	// ��ǰ�����
	pNaviNode pTarNode	// ��ǰĿ���
	);

// ���㵱ǰ��չ����������(SAP = SearchAreaParameter)
void PLAN_CalSAP(pNaviNode pCurNode, SearchAreaParameter &SAP);

// ����չ�Ľڵ����OPEN����ͬʱ�������н��д������򣬽�������С�ĵ������ǰ��
void PLAN_NaviNodeInsertToOpenList(pNaviNode pTempNode,pRouteList &OpenList,std::set<EarthPoint>&CloseList);

// ����ƽ����һ��ֱ���ϵ���һ��
bool  PLAN_NextNode(int prevL, int prevB, EarthPoint &next, double distance,double angle);

//�������
double PLAN_Cost(pNaviNode pEmiNode,pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode);
double PLAN_Cost_LinkAngle(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode);
double PLAN_Cost_Threat_Forbid(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode);
double PLAN_Cost_Dis(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode);

//�ж�����֮���ܷ����ӣ������ӵĻ��ͽ�������
bool  PLAN_BuildTrack(pNaviNode pCurNode,pNaviNode NextNode);

//���ع滮��������
string GetPlanError();

////////////////////////A*��·�滮//////////////////////////////////
////////end

//�ڴ�ӳ��
bool CreateGctDataMap(LPCWSTR szFileName , HANDLE &hMapFile);

//Լ������  add at 10-30 
extern double minl;         //���������ڵ�֮�����С����
extern double maxl;         //���������ڵ�֮���������
extern double maxhangle;    //�����ڵ�֮������ת��Ƕ�
extern double maxvangle;    //�����ڵ�֮�������¶�
extern __int64 fileSize;    //�߳�ͼ�ļ��Ĵ�С


//add by hj 2013.11.3 begin------------------------------
extern HANDLE G_hZhtMapping;	//���ͼ�ļ�ӳ����
extern HANDLE G_hGctDataFilemap;    // ԭʼ�߳����ݵ�ͼ���ڴ��ļ�ӳ����
///change by hj 12.20
#define RAND_FLY_AREA		0x0001		//�����������־
#define FORBID_FLY_AREA		0x0002		//������
#define THREAT_FLY_AREA		0x0004		//��в��

//�жϸ�����һ�����ǲ��ǽ���������в�������������
#define  IsRandFlyArea(param) (LOBYTE((param)) & RAND_FLY_AREA) 
#define  IsForbidArea(param) (LOBYTE((param)) & FORBID_FLY_AREA) 
#define  IsThreatArea(param) (LOBYTE((param)) & THREAT_FLY_AREA)

//�����в���ĵȼ�
#define  GetThreatType(param) HIBYTE((param)) 

//��д��в����������Ϣ
#define  MAKESHORT(hibyteno , lobytetype) ((hibyteno)<<8|(lobytetype))

///change by hj 12.20

//�õ��ڴ�ӳ���е�����
void * ReadComMapData(HANDLE hFileMap , __int64 begin , int &row , int &errorOfStart);
//add by hj 2013.11.3 end------------------------------

extern bool isEndThread;//�����̵߳Ľ���

#endif

