
#include "DataStruct.h"


// 全局变量
double G_MAPL0;               // 全局地图左上角的经度
double G_MAPB0;               // 全局地图左上角的纬度
int G_WIDTH;                  // 全局规划地图的宽度
int G_HEIGHT;                 // 全局规划地图的高度
DWORD G_dwGran = 65536;       // 系统分配粒度
HANDLE G_hGctDataFilemap;     // 原始高程数据地图的内存文件映射句柄
HANDLE G_hDxppDataFilemap;    // 原始地形匹配数据地图的内存文件映射句柄
__int64 fileSize; //高程图文件的大小

double G_RESOLVE = 20.0;        // 全局规划地图的分辨率 
double factofthreat = 0.1;               //威胁调整系数
int tt  = 0;
ErrorType errStr = DefError;          //错误类型

//约束条件  add at 10-30 
double minl = 200;          //任意两个节点之间的最小距离
double maxl = 400;         //任意两个节点之间的最大距离
double maxhangle = PI/4;      //两个节点之间的最大转弯角度(弧度表示)
double maxvangle = PI/4;      //两个节点之间的最大坡度（弧度表示）


// 求两点连线的角度(弧度)
double PLAN_LinkAngle(int L1,int B1,int L2,int B2)
{
	// L1, B1为起始点,方向为-PI到PI
	double angle = 0.0;    // 用来输出的连线角度
	double dB = B2-B1;
	double dL = L2-L1;

	// 防止除零的情况
	if(fabs(dB) > ZEROANGLE)
	{
		angle = atan(dL/dB);
		if (dL>0 && dB < 0 )
		{
			angle = PI + angle;
		}

		if (dL<0 && dB < 0 )
		{
			angle = PI + angle;
		}

		if (dL<0 && dB > 0 )
		{
			angle = 2*PI + angle;
		}

		if(dL == 0 && dB < 0)
		{
			angle = PI;
		}
	}
	else
	{
		angle = (dL > 0) ? (PI/2) : (3*PI/2);
	}

	return angle;
}


//检测发射点、目标点是否高度合法
bool PLAN_CheckEmitTarPoint(pNaviNode pcurEmit, pNaviNode pcurtarget)
{
	int Z;	// 检测发射点 目标点是否高度合法

	if (!GetZofPoint(pcurEmit->X,pcurEmit->Y,Z,false,G_hGctDataFilemap))
	{
		return false;
	}
	else
	{
		if (abs(pcurEmit->Z - Z) > 1)
		{
			pcurEmit->Z = Z;
		}

		if (Z == -9999)
		{
			//Z = 10;
			//pcurEmit->Z = Z;
			return false;
		}

		//        return true;  // 注意：这里不能返回真 因为目标点还没有检查
	}

	if (!GetZofPoint(pcurtarget->X,pcurtarget->Y,Z,false,G_hGctDataFilemap))
	{
		return false;
	}
	else
	{
		if (abs(pcurtarget->Z - Z) > 1)
		{
			pcurtarget->Z = Z;
		}

		if (Z == -9999)  
		{
			//Z = 10;
			//pcurtarget->Z = Z;
			return false;
		}
		return true;
	}
}


/************************************************************************/
/* 创建高程图文件内存映射 szFileName 为文件路径 ， hFileMap为创建的内存映射句柄		*/
/*add by hj 2013.10.30													*/
/************************************************************************/
bool CreateGctDataMap(LPCWSTR szFileName , HANDLE &hMapFile )
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	G_dwGran = sysInfo.dwAllocationGranularity;//得到系统内存分配粒度

	//打开需要映射的文件
	HANDLE hFile = CreateFile(szFileName , 
		GENERIC_READ | GENERIC_WRITE , 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL ,
		OPEN_EXISTING ,
		FILE_ATTRIBUTE_NORMAL ,
		NULL
		);

	if (INVALID_HANDLE_VALUE == hFile)
	{//出错处理
		return false;
	}

	//进行映射
	//if(hMapFile != NULL)
	//{
	   // CloseHandle(hMapFile);//先关掉以前的
	//}

	hMapFile = CreateFileMapping(hFile , 
		NULL , 
		PAGE_READWRITE ,
		0 , 0 , 
		NULL
		);

	if (hMapFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return false;
	}

	if (hMapFile == NULL)
	{
		CloseHandle(hFile);
		return false;
	}

	DWORD dwFileSizeHigh;
	fileSize = GetFileSize(hFile, &dwFileSizeHigh);
	fileSize |= (((__int64)dwFileSizeHigh) << 32);

	CloseHandle(hFile);
	return true;
}


// 得到某点的高程值，X,Y为全局图坐标值 isInt判断是要读取int型还是shor型
// hFileMap为响应内存映射的句柄
//add by hj 2013.10.30
bool GetZofPoint(int x, int y, int &Z , bool isInt  , HANDLE hFileMap)
{
	__int64 start1 = sizeof(MapFileHeader) + (y*G_WIDTH + x)*sizeof(short);
	__int64 start2 = (y*G_WIDTH+x)*sizeof(unsigned short);

	__int64 start;
	if (isInt)
	{
		start = start2;
	}
	else
		start = start1;

	__int64 dataMemoryStart ;
	dataMemoryStart = start/G_dwGran*G_dwGran;//内存映射的其实位置
	int errorOfStart =start - dataMemoryStart ;//内存映射的起始点和真实起始点的误差
	DWORD highOffset = dataMemoryStart>>32;
	DWORD lowOffset = dataMemoryStart&0xffffffff;

	//
	LPVOID pVoid = MapViewOfFile(hFileMap,
		FILE_MAP_READ ,
		highOffset , 
		lowOffset , 
		sizeof(short)+errorOfStart
		);

	if (NULL == pVoid)
	{// 出错处理
		return false;
	}

	short *tmps = new short;
	void * tmpBuf = (PUCHAR)pVoid+errorOfStart;

	CopyMemory(tmps , tmpBuf , sizeof(short));
	Z = *tmps;

	delete tmps;

	UnmapViewOfFile(pVoid);  // 必须撤销映射
	return true;
}


bool LBtoIJ(double L,     // 当前点的经度      
	double B,     // 当前点的纬度
	double Level, // 当前使用的地图分辨率 // 这个很重要，很容易弄错
	int& I,       // 在给定分辨率下的全局像素I(横向)位置
	int& J        // 在给定分辨率下的全局像素J(纵向)位置
	)
{
	// 注意I,J与L,B是都是以左上角为起始点的
	// 请注意I,J与L,B的对应关系
	// 由于显示的时候可能会出现越界（需要显示为无效数据），对越界
	// 点同样进行转换
	// 对象素进行四舍五入取值
	if (Level < 0) 
	{
		return  false;
	}

	I = (int)(L - G_MAPL0)/Level + 0.5;
	J = (int)(G_MAPB0 - B)/Level + 0.5;

	return true;
}


// 将当前点的当前分辨率下全局象素坐标转换成经纬度
bool IJtoLB(int I,        // 在给定分辨率下的全局像素I(横向)位置
	int J,        // 在给定分辨率下的全局像素J(纵向)位置
	double Level, // 当前使用的分辨率 				
	double& L,    // 当前点的经度      
	double& B     // 当前点的纬度								
	)
{
	// 注意I,J与L,B是都是以左上角为起始点的
	// 请注意I,J与L,B的对应关系
	// 由于显示的时候可能会出现越界（需要显示为无效数据），对越界
	// 点同样进行转换

	if (Level < 0) 
	{
		return  false;
	}

	L = G_MAPL0 + I*Level;
	B = G_MAPB0 - J*Level;

	return true;
}


// 判断是否在大地图内
bool IsInAreaofPoint(int x, int y)
{
	if (x<0 || x > G_WIDTH || y<0 || y > G_HEIGHT)
	{
		return false;
	}
	return true;
}


// 计算两点之间的平面距离
double PLAN_Distance(pNaviNode prev, pNaviNode next)
{
	// 注意暂时以大地坐标下的欧式距离计算
	return sqrt((double)(prev->X-next->X)*(prev->X-next->X)
		+ (prev->Y-next->Y)*(prev->Y-next->Y));

}


// 计算两空间点之间的距离
double PLAN_KJDistance(pNaviNode prev, pNaviNode next)
{
	return sqrt((double)(prev->X - next->X)*(prev->X - next->X)
		+ (prev->Y - next->Y)*(prev->Y - next->Y)
		+ (prev->Z - next->Z)*(prev->Z - next->Z));
}


// 判断点是否在禁飞区内
bool PLAN_IsInForbid(int CurNodeX, int CurNodeY)
{
	//-------------------------------------------------------------------------
	//判断节点CurNode是否在禁飞区内

	//判断是否在范围内
	if( !IsInAreaofPoint(CurNodeX, CurNodeY) )
	{
		//如果不在范围外，返回错误
		return false;
	}

	int Z;  //取出该点的信息
	if(!GetZofPoint(CurNodeX, CurNodeY, Z , true, G_hZhtMapping))
	{
		return false;
	}

	if(!IsForbidArea(Z))
	{
		return false;

	}

	return true;
}


// 判断点是否在威胁区内
bool PLAN_IsInThreat(int CurNodeX, int CurNodeY)
{

	//-------------------------------------------------------------------------
	//判断节点CurNode是否在威胁区内

	//判断是否在范围内(TODO：判断是否在禁飞区时已经处理)
	
	int Z;  //取出该点的信息
	if(!GetZofPoint(CurNodeX, CurNodeY, Z , true, G_hZhtMapping))
	{
		return false;
	}

	if(!IsThreatArea(Z))
	{
		return false;
	}

	return true;
}


// 判断目标点是否在搜索匹配区区域
bool PLAN_IsInSearchArea(pNaviNode pCurNode, pNaviNode pTarNode, SearchAreaParam SAP)
{
     double angle = PLAN_LinkAngle(pCurNode->X,pCurNode->Y,pTarNode->X,pTarNode->Y);
	 double dis = PLAN_Distance(pCurNode,pTarNode);
	 if(angle >= SAP.MinAngle && angle <= SAP.MaxAngle && dis <= SAP.MaxRadius)
	 {
		 return true;
	 }
	 return false;
}


//  扩展导航点
bool PLAN_ExpandNaviNode(pNaviNode pCurNode,    // 当前扩展节点 
	pRouteList& OpenList,
	std::set<EarthPoint>& CloseList,
	pNaviNode pEmiNode,	    // 当前发射点
	pNaviNode pTarNode )	// 当前目标点
{
	SearchAreaParameter SAP;  // 扩展的范围参数
	PLAN_CalSAP(pCurNode, SAP);
	
	// 判断目标点是否在此搜索区域
	//TODO: 如果在还要判断能否直接与当前点连接，即判断是否经过禁飞区
	if(PLAN_IsInSearchArea(pCurNode, pTarNode, SAP))
	{
		pTarNode->angle = PLAN_LinkAngle(pCurNode->X,pCurNode->Y,pTarNode->X,pTarNode->Y);
		if(PLAN_BuildTrack(pCurNode,pTarNode))
		{
			pTarNode->t_cost = 0;
			pTarNode->pFather = pCurNode;
			PLAN_NaviNodeInsertToOpenList(pTarNode, OpenList,CloseList);
			return true;
		}
	}
    

	//-------------------------------------------------------------------------
	//划分该区域为子区域，搜索子区域  
	EarthPoint outputpoint;  // 子区域四点坐标
	double UnitAngle ;          // 子区域角度粒度大小
	double UnitDistance ;       // 子区域径向距离粒度大小

	// 按粒度划分扇形区域
	UnitAngle = 40/G_RESOLVE/minl;
	UnitDistance = 40/G_RESOLVE;

	double CurAngle = SAP.MinAngle;
	double CurRadius = SAP.MinRadius;
	
	//-------------------------------------------------------------------------
	// 搜索子区域  
	// 外层角度循环 内层半径循环 TODO
	for(CurAngle=SAP.MinAngle; CurAngle<=SAP.MaxAngle; CurAngle+=UnitAngle)
	{  
		for(CurRadius=SAP.MaxRadius; CurRadius>=SAP.MinRadius; CurRadius-=UnitDistance)
		{
			pNaviNode pTempNode = new NavigationNode;
			*pTempNode= DefNavigationNode;
			if(!PLAN_NextNode(SAP.OrginX,SAP.OrginY,outputpoint,CurRadius,CurAngle))
			{
				continue;
			}
		
			pTempNode->X = outputpoint.L;
			pTempNode->Y = outputpoint.B;
			pTempNode->angle = PLAN_LinkAngle(pCurNode->X,pCurNode->Y,pTempNode->X,pTempNode->Y);

			if(PLAN_IsInForbid(pTempNode->X,pTempNode->Y))
			{
				continue;
			}

			int Z;
			if(!GetZofPoint(pTempNode->X,pTempNode->Y,Z,0,G_hGctDataFilemap) || Z < 0)
			{
				continue;
			}

			if(!PLAN_BuildTrack(pCurNode,pTempNode))
			{
				continue;
			}

			EarthPoint tmp;
			tmp.L = pTempNode->X;
			tmp.B = pTempNode->Y;
			if(CloseList.find(tmp) != CloseList.end())
			{
				continue;
			}

			//-----------------------------------------------------------------
			// 搜索子区域成功则进行代价计算。
			
			//计算平面代价, 高度代价在扩展中判断计算
			double Cost = PLAN_Cost(pEmiNode,pCurNode,pTempNode, pTarNode);


			// 填充当前扩展出来的点
			pTempNode->t_cost = Cost;
			pTempNode->pFather = pCurNode;
			pTempNode->PointType = 0;

			//-----------------------------------------------------------------
			// 将此扩展导航点插入OPEN链表中并排序
			PLAN_NaviNodeInsertToOpenList(pTempNode, OpenList,CloseList);

		}   // end for Radius

	}  // end for Angle

	return true;
}


/******************************************************************************
描  述: 代价计算的核心函数
计算平面代价并判断此二维航迹段是否经过禁飞区
备  注: 分别求各类代价；然后求和累加；
******************************************************************************/
double PLAN_Cost(pNaviNode pEmiNode,pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode)
{
	// 最后的总代价的初始化
	double cost = 0.0;  

	// 对一个导航点 代价及其含义如下：
	// g_cost : SAS已知代价
	// h_cost : SAS预估代价
	// f_cost : SAS平面代价
	// t_cost : SAS总代价
	// p_cost : 预留
	// HP_cost: 高度代价
	// 其中 t_cost = f_cost + HP_cost = (g_cost+h_cost) + HP_cost
	// 代价由距离 是否威胁禁飞区等来决定 



	// ------------------------------------------------------------------------
	// 声明 每一个子项代价 和 代价系数
	double cost_dis, factor_dis; //distance
	double cost_thr, factor_thr; //threat and forbid
	double cost_lia, factor_lia; //link-angle
	double cost_for, factor_for; //forbid in future route


	// ------------------------------------------------------------------------
	// 初始化 每一项代价 和 代价系数
	cost_dis = 0.0;
	cost_thr = 0.0;
	cost_lia = 0.0;
	cost_for = 0.0;

	factor_dis = 2.0;   //factor_dis = 2.0;
	factor_thr = 0.5;   //factor_thr = 0.5;
	factor_lia = 1.0;
	factor_for = 1.0;


	// ------------------------------------------------------------------------
	// 求每一项代价

	//-------------------------------------------------------------------------
	// 1 距离代价
	cost_dis = PLAN_Cost_Dis( pEmiNode,  pCurNode, pTempNode,  pTarNode);

	//-------------------------------------------------------------------------
	// 2 计算禁飞区、威胁区代价, 对于通过禁飞区的要严格排除,用矢量形式代表禁飞区、威胁区    
	cost_thr = PLAN_Cost_Threat_Forbid( pEmiNode,  pCurNode, pTempNode,  pTarNode);  
	

	// ------------------------------------------------------------------------
	// 3扩展点的方向与该点和目标连线方向的角度代价
	cost_lia = PLAN_Cost_LinkAngle( pEmiNode,  pCurNode, pTempNode,  pTarNode);
	if (cost_lia <= -0.5) // cost_lia = -1.000000 注意精度 表示角度大于120
	{
		cost = -1;
		return cost;
	}
	
	//-------------------------------------------------------------------------    
	// 4 估计的禁飞区代价
	double dis_f = PLAN_Distance(pCurNode, pTarNode);	//Distance in Future
	double Angle_CT = PLAN_LinkAngle(pCurNode->X,pCurNode->Y, pTarNode->X,pTarNode->Y); 

	//向下取整
	int STEP = 60/G_RESOLVE;
	int nn = (int) (dis_f/STEP);
	int n_counter = 0;
	EarthPoint outputPoint = DefEarthPoint;        // 连线采样点
	EarthPoint outputPoint1 = DefEarthPoint;        // 延线采样点

	//当前点到目标点连线采样
	for (int ii = 1; ii <= nn; ii++)
	{
		// 求出当前采样点的坐标
		if (!PLAN_NextNode(pCurNode->X, pCurNode->Y, outputPoint, ii*STEP, Angle_CT))
		{
			break;
		}

		//判断采样点是否在禁飞区
		if (PLAN_IsInForbid(outputPoint.L, outputPoint.B))
		{
			n_counter++;
		}
	}

	
	//当前点延长线采样
	int j=1;
	if (dis_f > 1000/G_RESOLVE)
	{	
		PLAN_NextNode(pCurNode->X, pCurNode->Y, outputPoint, 5000/G_RESOLVE, pCurNode->angle);
		PLAN_NextNode(pCurNode->X, pCurNode->Y, outputPoint1,10000/G_RESOLVE, pCurNode->angle);
		if (PLAN_IsInForbid(outputPoint1.L, outputPoint1.B))
		{
			while (PLAN_IsInForbid(outputPoint1.L, outputPoint1.B))
			{
				PLAN_NextNode(outputPoint.L, outputPoint.B, outputPoint1, j*STEP, pCurNode->angle);
				j++;
				n_counter++;
			} //end of while 
		} // end of first outputpoint is in Forbid
	} //end of if 'dis'
	

	//punish factor
	double pub = n_counter*SAMPLESTEP/PLAN_Distance(pEmiNode, pTarNode) + 10.0;
 	cost_for = n_counter*pub;

	// ------------------------------------------------------------------------
	// 最后总代价的求和计算

	cost = factor_dis*cost_dis 
		+ factor_thr*cost_thr 
		+ factor_lia*cost_lia 
		+ factor_for*cost_for;

	return cost;
}


//返回值： -1：经过了禁飞区；0.5：未经过禁飞区且未经过威胁区；其他正值：通过威胁区的代价值
double PLAN_Cost_Threat_Forbid(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode)
{

	//-------------------------------------------------------------------------
	// 计算禁飞区、威胁区代价, 对于通过禁飞区的要严格排除,用矢量形式代表禁飞区、威胁区
	double cost_t_f = 0.0;

	if (PLAN_IsInThreat(pCurNode->X, pCurNode->Y) ||
		PLAN_IsInThreat(pTempNode->X, pTempNode->Y))
	{
		double TotalLength = PLAN_Distance(pCurNode, pTempNode);
		int STEP = 60/G_RESOLVE;
		int nn = (int)(TotalLength/STEP);   // 分成nn段,强制成int型
		EarthPoint outputPoint  = DefEarthPoint;  // 采样点输出点平面坐标

		// 采样是为了计算威胁区域代价
		for (int ii = 1; ii <= nn; ii++)
		{
			// 给出当前采样点的坐标
			if (PLAN_NextNode(pCurNode->X,pCurNode->Y,outputPoint,ii*STEP,pTempNode->angle))
			{
				// 判断当前航迹段上采样点经过威胁区的代价
				double TD = 0.0;  // 当前采样点与当前威胁区中心点距离(像素距离)
				int level = 1;    // 当前威胁区的威胁等级

				if (PLAN_CrossThreatenIJ(outputPoint.L, outputPoint.B, TD,level))  
				{                
					cost_t_f += level*ThreatFactor*factofthreat;  // TODO: here level = threatArea[circleInt].level; 
				}  
			}   // end of if PLAN_NextNode

		} // end of for (采样)
	}  // end of if PLAN_IsInThreat
	else
	{
		cost_t_f = 0.5;	
	}

	return cost_t_f;
}


double PLAN_Cost_LinkAngle(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode)
{
	//-------------------------------------------------------------------------   
	// 扩展点与目标点之间的角度代价

	// 当前扩展点 与 目标的连线角度
	double Angle_Link = PLAN_LinkAngle(pTempNode->X,pTempNode->Y, pTarNode->X,pTarNode->Y);

	// 当前扩展点角度 与 当前点同目标连线角度的偏角
	double crossAngle = Angle_Link - pTempNode->angle;

     return fabs(crossAngle)*180/PI;

	/*if ( crossAngle >= 0.0)
	{
		if ( crossAngle >= PI)
		{
			crossAngle =  crossAngle - 2*PI;
		}
	}
	else
	{
		if ( crossAngle <= -PI)
		{
			crossAngle =  crossAngle + 2*PI;
		}
	}

	// 转为正角度
	if (crossAngle<=0.1)
	{
		crossAngle += 2*fabs(crossAngle);
	}

	// 排除0度的情况
	if(crossAngle<=0.001)
		crossAngle = 0.00101;

	if (crossAngle >= PI*11/12)  //TODO: could be changed as 45
	{
		return  -1;
	}
	else
	{
		return 0.1*crossAngle*180/PI;	
	}*/
}


// 代价函数的每一项
double PLAN_Cost_Dis(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode)
{
	double targetdistance = PLAN_Distance(pTempNode, pTarNode);
	return fabs(targetdistance);
}


// 计算平面上一条直线上的下一点,由于距离较短
// 没有考虑大地坐标的影响,坐标轴原点为左上角
bool  PLAN_NextNode(int prevL, int prevB, EarthPoint &next, double distance,double angle)
{
	// 注意这里的方向是飞行方向(正北方向为基准，顺时针为正，逆时针负)
	
	if(angle >= 2*PI)
	{
		int ang = angle*180/PI;
		ang %= 360;
		angle = ang*PI/180;
	}

	if(angle < 0)
	{
		angle = angle + 2*PI;
	}

	if(angle>=0 && angle < PI/2)
	{
		next.L = prevL + distance * sin(angle);
		next.B = prevB + distance * cos(angle);	
	}

	if(angle >= PI/2 && angle < PI)
	{
		next.L = prevL + distance * sin(PI-angle);
		next.B = prevB - distance * cos(PI-angle);	
	}

	if(angle>=PI && angle < PI*3/2)
	{
		next.L = prevL - distance * sin(angle - PI);
		next.B = prevB - distance * cos(angle - PI);	
	}

	if(angle>=PI*3/2 && angle < 2*PI)
	{
		next.L = prevL - distance * sin(2*PI - angle);
		next.B = prevB + distance * cos(2*PI - angle);	
	}

	return true;
}


// 将扩展的节点加入OPEN链表并同时在链表中进行代价排序，将代价最小的点放在最前面
void PLAN_NaviNodeInsertToOpenList(pNaviNode pTempNode,pRouteList &OpenList,std::set<EarthPoint>&CloseList)
{

	// 将导航点插入openlist
	pRouteList::iterator first,last;  // 设置迭代器 
	first = OpenList.begin();
	last = OpenList.end();

	// 在扩展时产生这个节点是应该已经更新改变量。
	while (first != last) 
	{
		double t=(*first)->t_cost;
		if( (pTempNode->t_cost) < t)
		{
			OpenList.insert(first, pTempNode);  // 当找到比自己大的节点就加入OPENLIST
			break;
		}
		++first;
	}

	// 已经循环完OPEN表完毕,自己的代价最大，那么就放在最后面
	if(first == last)
	{
		OpenList.push_back(pTempNode);
	}

	EarthPoint tmp;
	tmp.L = pTempNode->X;
	tmp.B = pTempNode->Y;
	CloseList.insert(tmp);
}


// 计算当前扩展点的区域参数
void PLAN_CalSAP(pNaviNode pCurNode, SearchAreaParameter &SAP)
{
	if(1 == pCurNode->PointType)
	{
		SAP.OrginX = pCurNode->X;
		SAP.OrginY = pCurNode->Y;
		SAP.MinRadius = 60/G_RESOLVE;
		SAP.MaxRadius = maxl;
		SAP.MinAngle = pCurNode->angle - PI;
		SAP.MaxAngle = pCurNode->angle + PI;
	}
	else
	{
		SAP.OrginX = pCurNode->X;
		SAP.OrginY = pCurNode->Y;
		SAP.MinRadius = minl;
		SAP.MaxRadius = maxl;
		SAP.MinAngle = pCurNode->angle - maxhangle;
		SAP.MaxAngle = pCurNode->angle + maxhangle;
	}
}


//   判断两点之间能否连接，能连接的话就建立航迹
bool PLAN_BuildTrack(pNaviNode pCurNode,pNaviNode NextNode)
{
	int Z1, Z2;   //取出点的信息

	//判断坡度是否小于maxvangle
	if(!GetZofPoint(pCurNode->X,pCurNode->Y,Z1,0,G_hGctDataFilemap) ||
	   !GetZofPoint(NextNode->X,NextNode->Y,Z2,0,G_hGctDataFilemap))
	{
		return false;
	}
	if(Z1<0 || Z2<0)
	{
		return false;
	}

	double dh = Z2-Z1;
	double dis = PLAN_KJDistance(pCurNode, NextNode);
	double angle = asin(dh/dis);
	if((fabs)(angle) > maxvangle)
	{
		if(angle>0)
		{
			if((fabs)(asin(dh-20)/dis) > maxvangle)
			{
				return false;
			}
			else
			{
				pCurNode->Z = Z1+20;
			}		
		}

	    if(angle<0)
		{
			if((fabs)(asin(dh+20)/dis) > maxvangle)
			{
				return false;
			}
			else
			{
				NextNode->Z = Z2+20;
			}
		}
	}
	else
	{
		NextNode->Z = Z2;
	}

	//-----------------------------------------------------------
	//判断是否经过禁飞区
	double distance = PLAN_Distance(pCurNode, NextNode);	
	int nn = (int)distance + 0.5;  //向上取整
	EarthPoint outputPoint = DefEarthPoint;   // 连线采样点

	for (int ii = 1; ii <= nn; ii+=3)
	{
		// 求出当前采样点的坐标
		if (!PLAN_NextNode(pCurNode->X, pCurNode->Y, outputPoint, ii,NextNode->angle))
		{
			return false;
		}

		//判断高程值是否合法
		int Z3;
		if(!GetZofPoint(outputPoint.L, outputPoint.B,Z3,0,G_hGctDataFilemap) || Z3 < 0)
		{
			return false;
		}

		//判断采样点是否在禁飞区
		if(PLAN_IsInForbid(outputPoint.L, outputPoint.B))
		{
			return false;
		}
	}

	//NextNode->Z = Z2;
	NextNode->HP_cost = dh;  //以高度差作为高度代价

	return true;
}


bool PLAN_CrossThreatenIJ(int I,int J, double& TD, int& level)
{
	int Z;     //取出改点的信息值
	if(!GetZofPoint(I,J,Z,1,G_hZhtMapping))
	{
		return false;
	}
	level = GetThreatType(Z);
	return true;	
}


//返回规划的最后错误
string GetPlanError()
{
	string str;
	switch(errStr)
	{
	case 0: 
		str = "系统错误";
		break;
	case 1: 
		str = "局部规划获取任务失败";
		break;
	case 2: 
		str = "局部规划初始化失败";
		break;
	case 3: 
		str = "局部规划扩展点失败";
		break;
	case 4: 
		str = "整体规划失败";
		break;
	}
	return str;
}


//add by hj 2013.11.3 begin----------------------------------
HANDLE G_hZhtMapping = INVALID_HANDLE_VALUE;			//组合图文件映射句柄

//add by hj 2013.12.20

//得到组合图内存映射begin为开始的行号 row为要映射多少行 , errorOfStart 为想要映射的内存起始地址
//和实际内存映射起始地址的偏移量
void * ReadComMapData(HANDLE hFileMap , __int64 begin , int &row , int &errorOfStart)
{
	//得到每行数据的宽度 以字节记
	int width = G_WIDTH * sizeof(unsigned short);

	//边界判断
	if (begin < 0)
	{
		row += begin;
		begin = 0;
	}

	if (begin + row >= G_HEIGHT)
	{
		row = G_HEIGHT - begin;
	}

	__int64 start = (begin * width) / G_dwGran * G_dwGran;
	errorOfStart =begin*width -  start;//内存映射的起始点和真实起始点的误差
	DWORD highOffset = start>>32;
	DWORD lowOffset = start&0xffffffff;

	//
	LPVOID pVoid = MapViewOfFile(hFileMap,
		FILE_MAP_ALL_ACCESS ,
		highOffset , 
		lowOffset , 
		row*width+errorOfStart);

	if (NULL == pVoid)
	{// 出错处理
		return NULL;
	}
	return pVoid;
}
bool isEndThread = false;

//add by hj 2013.11.3 end----------------------------------
