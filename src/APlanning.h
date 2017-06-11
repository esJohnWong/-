#ifndef APlanning_H
#define APlanning_H

#include "DataStruct.h"


class APlanning 
{
public:
	APlanning();
	~APlanning();

	NavigationNode* m_pCurEmit;			// 任务的起点
	NavigationNode* m_pCurTarget;		// 任务的目标点

	// 导航点和航迹向量和链表集合
	pRouteList OpenList;       // Open,CLose链表 
	std::set<EarthPoint>CloseList;
	RouteList ShowList;       // 全局路径展示链表
	RouteList TaskList;
	RouteList TempList;
	pNaviNode MinCostNode;           // 用来循环的弹出节点的指针

	bool run();          //规划函数入口
	bool getTask(vector<GroundPoint> &LeaderVector);      //获取任务
	bool InitAStar();                     //初始化相关变量并解析当前任务是否合法
	bool PlanIsOK(pNaviNode pCurNode);    //判断当前点是否是目标点
    bool IsTempTar(pNaviNode pCurNode);   //判断当前点是否是临时目标点
	bool IsInThreat(pNaviNode pointA, pNaviNode pointB);
	bool IsInForbid(pNaviNode pointA, pNaviNode pointB);
	bool OutPutData(vector<GroundPoint> &ResultVector);   //输出A*局部规划结果
	
	//获取输入参数信息
	void getParaminput(double &minlength, //任意两个节点之间的最小距离
		double &maxlength,               //任意两个节点之间的最大距离
		double &maxhang,                 //两个节点之间的最大转弯角度(弧度表示)
		double &maxvang);                //两个节点之间的最大坡度（弧度表示）

	//获取地图信息
    bool getMapInfo(LPCWSTR szFileName);           // 全局规划地图的分辨率 


	//初始化组合图
	bool InitComMap(
		char * szMapFileName ,//组合图的名称
		__int64 i64MapHeightInPixel ,//组合图高度像素点
		__int64 i64MapWdithInPixel	//组合图宽度 像素点
		);

	//生成组合图
	bool CreateComMap(
		TCHAR * szMapFileName , //组合图名称
		vector<iPolygon> &forbidAndThreatVector );//禁飞区威胁区信息坐标已经转换)

private:

};


#endif