#include "StandardizeConstraint.h"


StandardizeConstraint::StandardizeConstraint(void)
{
}


StandardizeConstraint::~StandardizeConstraint(void)
{
}

bool StandardizeConstraint::PointLBToSC(GroundPoint ptOrg , iPoint &ptNew)
{
	if (!LBtoIJ(ptOrg.X , ptOrg.Y , G_RESOLVE , ptNew.x , ptNew.y))
	{
		return false;
	}
	return true;
}

bool StandardizeConstraint::PointSCToLB(iPoint ptOrg , GroundPoint & ptNew)
{
	if (!IJtoLB(ptOrg.x , ptOrg.y , G_RESOLVE , ptNew.X , ptNew.Y))
	{
		return false;
	}
	return true;
}

bool StandardizeConstraint::GuidOrResConvertToLBCoordinate(
	vector<iPoint> &vctOrg ,
	vector<GroundPoint> & vctNew)
{
	for (int i = 0 ; i < vctOrg.size() ; i++)
	{
		GroundPoint ptTmp;
		if(!PointSCToLB(vctOrg[i] , ptTmp))
		{
			return false;
		}
		vctNew.push_back(ptTmp);
	}
	return true;
}

bool StandardizeConstraint::GuidOrResConvertToScreenCoordinate(
	vector<GroundPoint> &vctOrg , 
	vector<iPoint> &vctNew)
{
	for (int i = 0 ; i < vctOrg.size() ; i++)
	{
		iPoint ptTmp;
		if(!PointLBToSC(vctOrg[i] , ptTmp))
		{
			return false;
		}
		vctNew.push_back(ptTmp);
	}
	return true;
}

bool StandardizeConstraint::PolygonConvertToScreenCoordinate(iPolygon & polygon)
{
	for (int i = 0 ; i < polygon.ptPolygon.size() ; i++)
	{
		int I , J;//屏幕坐标
		if(!LBtoIJ(polygon.ptPolygon[i].x , polygon.ptPolygon[i].y , G_RESOLVE , I , J))
			return false;
		polygon.ptPolygon[i].x = I;
		polygon.ptPolygon[i].y = J;
	}
	return true;
}

bool StandardizeConstraint::MakeRectForPolygon(iPolygon & polygon)
{
	//左上角坐标
	int iXLeftUpCorner = INT_MAX , iYLeftUpCorner = INT_MAX;
	//右下角坐标
	int iXRightDownCorner = INT_MIN , iYRightDownCorner = INT_MIN;

	for (int i = 0 ; i < polygon.ptPolygon.size() ; i++)
	{
		if (polygon.ptPolygon[i].x < iXLeftUpCorner)
		{
			iXLeftUpCorner = polygon.ptPolygon[i].x;
		}

		if (polygon.ptPolygon[i].x > iXRightDownCorner)
		{
			iXRightDownCorner = polygon.ptPolygon[i].x;
		}

		if (polygon.ptPolygon[i].y < iYLeftUpCorner)
		{
			iYLeftUpCorner = polygon.ptPolygon[i].y;
		}

		if (polygon.ptPolygon[i].y > iYRightDownCorner)
		{
			iYRightDownCorner = polygon.ptPolygon[i].y;
		}
	}

	polygon.rectangle.lx = iXLeftUpCorner;
	polygon.rectangle.ly = iYLeftUpCorner;
	polygon.rectangle.height = iYRightDownCorner - iYLeftUpCorner + 1;
	polygon.rectangle.width = iXRightDownCorner - iXLeftUpCorner + 1;

	return true;
}

bool StandardizeConstraint::Standardize(iPolygon & polygon)
{
	//转换坐标
	if (!PolygonConvertToScreenCoordinate(polygon))
		return false;
	//求外接矩形
	MakeRectForPolygon(polygon);
	return true;
}

bool StandardizeConstraint::APointIsInPolygon(
	iPoint point , 
	const iPolygon polygon)
{
	int nCross = 0;

	for (int i = 0; i < polygon.ptPolygon.size(); i++) 
	{
		iPoint p1 = polygon.ptPolygon[i];  
		iPoint p2 = polygon.ptPolygon[(i + 1) % polygon.ptPolygon.size()];

		// 求解 y=p.y 与 p1 p2 的交点  
		if ( p1.y == p2.y )      // p1,p2 与 y=p0.y平行
			continue;

		if ( point.y <  Min(p1.y, p2.y) )   // 交点在p1p2延长线上
			continue;
		if ( point.y >= Max(p1.y, p2.y) )   // 交点在p1p2延长线上
			continue;

		// 求交点的 X 坐标 --------------------------------------------------------------
		double x = (double)(point.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;

		if ( x > point.x ) 
			nCross++;       // 只统计单边交点
	}

	// 单边交点为偶数，点在多边形之外 ---
	return (nCross % 2 == 1);
}