/**********************************
*将从文件读进来的约束标准化（转换坐标）
***********************************/
#pragma once
#include "DataStruct.h"
class StandardizeConstraint
{
public:
	StandardizeConstraint(void);
	~StandardizeConstraint(void);
	//将多边形的坐标转换为屏幕坐标 多边形
	bool PolygonConvertToScreenCoordinate(iPolygon & polygon);
	//求多边形的外接矩形
	bool MakeRectForPolygon(iPolygon & polygon);
	//转换主函数 多边形
	bool Standardize(iPolygon & polygon);
	//判断一个点是否在多边形内
	bool APointIsInPolygon(iPoint point , const iPolygon polygon);
	//将一个点的坐标转换成屏幕坐标
	bool PointLBToSC(GroundPoint ptOrg , iPoint &ptNew);
	//将一个点从屏幕坐标转换成经纬度
	bool PointSCToLB(iPoint ptOrg , GroundPoint & ptNew);
	//将引导点经纬度转换为屏幕坐标 引导点或者 规划结果
	bool GuidOrResConvertToScreenCoordinate(vector<GroundPoint> &vctOrg , vector<iPoint> &vctNew);
	//将引导点屏幕坐标转换为经纬度
	bool GuidOrResConvertToLBCoordinate(vector<iPoint> &vctOrg , vector<GroundPoint> & vctNew);
};

