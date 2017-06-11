// GolbalPlan.h: interface for the GolbalPlan class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GL_GLOBAL_PLAN_
#define _GL_GLOBAL_PLAN_

#include "Datastruct.h"


#define NOOFROUTES 40    //初始种群大小
#define MAXGEN   10000  //加大代数提高收敛性
#define MAXNUM  INT_MAX


//规划区间的矩形范围
typedef struct _TransInfo
{
	int leftupX;    //规划矩形左上角X坐标
	int leftupY;    //规划矩形左上角Y坐标
	int width;      //规划矩形的宽（横向）
	int height;     //规划矩形的高（纵向）
}transInfo; 


typedef struct _genoType
{
	EarthPoint*  genes;  //对应一条引导点序列
	double totalcost;
}genoType;
 

class GL_GolbalPlan 
{
public:
	GL_GolbalPlan();
	~GL_GolbalPlan();


	int SEED;            //每条序列引导点个数
	int num;             //记录输出路径的条数
	transInfo transinfo;   //规划区域
	genoType *planningRoutes[NOOFROUTES]; //引导点序列数组
	NavigationNode emitPoint;       //起点
	NavigationNode targetPoint;		//目标点
	

	//全局规划相关函数声明...........................
	//从任务向量中读入发射点目标点信息
	bool run();   //入口函数

	//读入发射点目标点信息并判断任务是否合法
	bool loadTasks_GL(std::list<NavigationNode> &ETList);

	//从发射目标点计算出矩形的信息
	void GetTransInfo_GL();

	//完成对初始的几条导引序列的初始化  
	void initialize_GL();

	double calAngle_GL(EarthPoint * frontNode,EarthPoint* centreNode,EarthPoint* lastNode);//直接求得当前点夹角的弧度

	//航迹个体的评价
	void evaluateCostOfARuote_GL(genoType *planningRoute);

	//按代价对每条航迹计算总代价，按照代价值对航迹种群按从低到高排序
	void evaluate_GL();

	//坐标按X方向排序
	void orderByX_GL(genoType *Track);
	
	//交叉算子
	void crossover_GL();
	void simpleCrossover_GL(genoType *TrackOne,genoType *TrackTwo,genoType *LastOne,genoType *LastTwo);
	void segmentCrossover_GL(genoType *TrackOne,genoType *TrackTwo,genoType *LastOne,genoType *LastTwo);
	
	//变异算子
	void mutate_GL();
	void distrub_GL(genoType *Track,int i);
	void disturbWithinACircle_GL(EarthPoint *centrePoint,int r);
	//变异:平滑算子
	void smooth_GL(genoType *Track);
	//拉伸算子：
	void stretch_GL(genoType *Track);
	bool applyProjection_GL(int beginPoint,int endPoint,genoType *Track); //连成直线
	
	//求两点距离
	double distanc_GL(EarthPoint point1,EarthPoint point2);

	//对种群重新排序
	void reOrder_GL();

	//输出数据
	void OutPutData_GL();

};

#endif // !defined(AFX_GOLBALPLAN_H__AB013ECA_3CE7_43B4_9665_8BA8CE266E98__INCLUDED_)
