
#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#pragma once
#include "fileHeaders.h"


///////////////////////////////////////////////////////////////////////////////
// 包含C,C++标准库
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

// 包含C,C++标准库
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////   
// 转换宏
#define RADIAN(angle) ((angle)*PI/180.0)  // 角度到弧度的转换宏
#define ANIRAD(angle) ((angle)*180.0/PI)  // 弧度到角度的转换宏
#define Min(a,b) (((a) < (b)) ? (a) : (b))
#define Max(a,b) (((a) >= (b)) ? (a) : (b))
#define ThreatFactor 20 

//////////////////////////////////////////////////////////////////////////
// 常数表
const double PI = 3.14159265358979323846264338327950288419717;
const int SAMPLESTEP = 5;          // 采样步长
const double ZEROANGLE = 0.1;       // 角度的近似零 
const int ANGLEGRIDNUM = 45;
const int RADIUSGRIDNUM = 10;       // 格网划分


// 全局变量
extern double factofthreat;         //威胁调整系数
extern double G_MAPL0;              // 全局地图左上角的经度
extern double G_MAPB0;              // 全局地图左上角的纬度
extern int G_WIDTH;                 // 全局规划地图的宽度
extern int G_HEIGHT;                // 全局规划地图的高度
extern double G_RESOLVE;            // 全局规划地图的分辨率 // 高程图和地形高程图相同
extern DWORD G_dwGran;              // 系统分配粒度

//约束条件  add at 10-30 
extern double minl;         //任意两个节点之间的最小距离
extern double maxl;         //任意两个节点之间的最大距离
extern double maxhangle;    //两个节点之间的最大转弯角度
extern double maxvangle;    //两个节点之间的最大坡度

extern int tt;

//错误类型
enum  ErrorType
{ 
	DefError, GetTaskErr, InitErr, ExpErr,GA_loadTaskErr
};
extern  ErrorType errStr;

//add at 12-19
//自定义点的结构
typedef struct _Point
{
	int x;
	int y;
}iPoint;

//矩形
typedef struct _Rect
{
	int lx;  //左上角x坐标
	int ly;  //左上角y坐标
	int width;
	int height;
}iRect;

//多边形
typedef struct _Polygon 
{
	vector<iPoint> ptPolygon;  //多边形的各个顶点坐标
	iRect rectangle;  //外接矩形
	int level;        //等级
}iPolygon;

//参数结构
typedef struct _Parameter
{
	double minl;         //任意两个节点之间的最小距离
	double maxl;         //任意两个节点之间的最大距离
	double maxhangle;    //两个节点之间的最大转弯角度
	double maxvangle;    //两个节点之间的最大坡度
}Param;

//大地坐标点
typedef struct _GroundPoint
{
	double X;       
	double Y;        
	double Z;  
}GroundPoint;


// 大地坐标点平面结构
typedef struct _EarthPoint
{
	int L;	     // 点的经度坐标
	int B;       // 点的纬度坐标

	bool operator < (const _EarthPoint &a) const 
	{
		if(a.L == L)
		{
			return a.B < B;
		}

		return a.L < L;
	}

}EarthPoint;


// 导航点
typedef struct _NavigationNode
{
	int X;					// 全局图X坐标   
	int Y;					// 全局图Y坐标  
	int Z;					// 空间点海拔高度的高程值
	int PointType;          // 当前节点的类型（0:默认 1:起点  2:导航点 3:目标点）
	double angle;			//方向 
	double g_cost;			// SAS 已知代价   
	double h_cost;			// SAS 预估代价
	double f_cost;			// SAS 平面代价
	double p_cost;			//保留值
	double HP_cost;			// 高度代价值
	double t_cost;			// SAS 总代价
	struct _NavigationNode* pFather;	// 空间点的上一个父节点

}NavigationNode, *pNaviNode;

typedef std::list<NavigationNode> RouteList; //路径链表
typedef std::list<pNaviNode> pRouteList;     //路径指针链表


//搜索匹配区区域位置参数
typedef struct SearchAreaParameter
{
	int OrginX;         //扩展点在全局地图的X坐标
	int OrginY;		    //扩展点在全局地图的Y坐标
	double MinRadius;   //最短距离
	double MaxRadius;	//最长距离
	double MinAngle;    // 最小角度  
	double MaxAngle;    // 最大角度
	
}SearchAreaParam; // 搜索匹配区区域位置参数


//////////////////////////////////////////////////////////////////////////
//各个结构的初始化
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


////////////////////////A*线路规划//////////////////////////////////
////////begin

//求两点连线的角度 
double PLAN_LinkAngle(int L1,int B1,int L2,int B2);

//检测发射点、目标点是否高度合法
bool PLAN_CheckEmitTarPoint(pNaviNode pcurEmit, pNaviNode pcurtarget);

// 计算两点之间的平面距离
double PLAN_Distance(pNaviNode prev, pNaviNode next);

// 计算两空间点之间的距离
double PLAN_KJDistance(pNaviNode prev, pNaviNode next);

// 判断点是否在禁飞区内
bool PLAN_IsInForbid(int CurNodeX, int CurNodeY);

// 判断点是否在威胁区内
bool PLAN_IsInThreat(int CurNodeX, int CurNodeY);

// 判断目标点是否在搜索匹配区区域
bool PLAN_IsInSearchArea(pNaviNode pCurNode, pNaviNode pTarNode, SearchAreaParam SAP);

// 威胁区计算
bool PLAN_CrossThreatenIJ(int I,int J, double& TD, int& level);

// 得到某点的高程值，X,Y为全局图坐标值
bool GetZofPoint(int x, int y, int &Z , bool isInt  , HANDLE hFileMap);

bool LBtoIJ(double L,     // 当前点的经度      
	double B,     // 当前点的纬度
	double Level, // 当前使用的地图分辨率 // 这个很重要，很容易弄错
	int& I,       // 在给定分辨率下的全局像素I(横向)位置
	int& J        // 在给定分辨率下的全局像素J(纵向)位置
	);

// 将当前点的当前分辨率下全局象素坐标转换成经纬度
bool IJtoLB(int I,        // 在给定分辨率下的全局像素I(横向)位置
	int J,        // 在给定分辨率下的全局像素J(纵向)位置
	double Level, // 使用的规划地图的分辨率 				
	double& L,    // 当前点的经度      
	double& B     // 当前点的纬度								
	);

// 某点是否在规划区域内
bool IsInAreaofPoint(int x, int y);

//  扩展导航点
bool PLAN_ExpandNaviNode(pNaviNode pCurNode,    // 当前扩展节点 
	pRouteList& OpenList,
	std::set<EarthPoint>& CloseList,
	pNaviNode pEmiNode,	// 当前发射点
	pNaviNode pTarNode	// 当前目标点
	);

// 计算当前扩展点的区域参数(SAP = SearchAreaParameter)
void PLAN_CalSAP(pNaviNode pCurNode, SearchAreaParameter &SAP);

// 将扩展的节点加入OPEN链表并同时在链表中进行代价排序，将代价最小的点放在最前面
void PLAN_NaviNodeInsertToOpenList(pNaviNode pTempNode,pRouteList &OpenList,std::set<EarthPoint>&CloseList);

// 计算平面上一条直线上的下一点
bool  PLAN_NextNode(int prevL, int prevB, EarthPoint &next, double distance,double angle);

//计算代价
double PLAN_Cost(pNaviNode pEmiNode,pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode);
double PLAN_Cost_LinkAngle(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode);
double PLAN_Cost_Threat_Forbid(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode);
double PLAN_Cost_Dis(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode);

//判断两点之间能否连接，能连接的话就建立航迹
bool  PLAN_BuildTrack(pNaviNode pCurNode,pNaviNode NextNode);

//返回规划的最后错误
string GetPlanError();

////////////////////////A*线路规划//////////////////////////////////
////////end

//内存映射
bool CreateGctDataMap(LPCWSTR szFileName , HANDLE &hMapFile);

//约束条件  add at 10-30 
extern double minl;         //任意两个节点之间的最小距离
extern double maxl;         //任意两个节点之间的最大距离
extern double maxhangle;    //两个节点之间的最大转弯角度
extern double maxvangle;    //两个节点之间的最大坡度
extern __int64 fileSize;    //高程图文件的大小


//add by hj 2013.11.3 begin------------------------------
extern HANDLE G_hZhtMapping;	//组合图文件映射句柄
extern HANDLE G_hGctDataFilemap;    // 原始高程数据地图的内存文件映射句柄
///change by hj 12.20
#define RAND_FLY_AREA		0x0001		//任意飞行区标志
#define FORBID_FLY_AREA		0x0002		//禁飞区
#define THREAT_FLY_AREA		0x0004		//威胁区

//判断给定的一个点是不是禁飞区，威胁区，任意飞行区
#define  IsRandFlyArea(param) (LOBYTE((param)) & RAND_FLY_AREA) 
#define  IsForbidArea(param) (LOBYTE((param)) & FORBID_FLY_AREA) 
#define  IsThreatArea(param) (LOBYTE((param)) & THREAT_FLY_AREA)

//获得威胁区的等级
#define  GetThreatType(param) HIBYTE((param)) 

//填写威胁区禁飞区信息
#define  MAKESHORT(hibyteno , lobytetype) ((hibyteno)<<8|(lobytetype))

///change by hj 12.20

//得到内存映射中的数据
void * ReadComMapData(HANDLE hFileMap , __int64 begin , int &row , int &errorOfStart);
//add by hj 2013.11.3 end------------------------------

extern bool isEndThread;//控制线程的结束

#endif

