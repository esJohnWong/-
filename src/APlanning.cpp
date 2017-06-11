
#include "APlanning.h"
#include "ReadMapFileOperation.h"
#include "WriteComMapFile.h"
#include "StandardizeConstraint.h"

APlanning::APlanning()
{
	//成员变量的初始化
	m_pCurEmit = new NavigationNode;
	*m_pCurEmit = DefNavigationNode;				// 当前任务的起点

	m_pCurTarget = new NavigationNode;   
	*m_pCurTarget = DefNavigationNode;              // 当前任务的目标点

	MinCostNode = new NavigationNode;
	*MinCostNode = DefNavigationNode;

	minl = 200/G_RESOLVE;          //任意两个节点之间的最小距离
	maxl = 400/G_RESOLVE;         //任意两个节点之间的最大距离
	factofthreat = 0.1;    //威胁调整系数
	ShowList.clear();      //路径展示链表初始化
}


APlanning::~APlanning()
{
// 	delete m_pCurEmit;
// 	delete m_pCurTarget;
// 	delete MinCostNode;
}


//run()调用函数:
//1. 【扩展模块】PLAN_ExpandNaviNode
//2. 【代价计算】
//3. 【代价排序】
// 问题:
// 1. 目前是一对一的航路规划 ,采用简化方法 没有引入多任务的向量容器  
// 这会降低这部分的程序扩展性,做多任务的航路规划 复用的代码会比较少
// 2. 高度规划应该考虑
bool APlanning::run()
{
	//-----------------------------------------------------------------------
	// 初始化open，closed链表，open表加入发射点，close表为空
	if(!InitAStar())
	{
		errStr = InitErr;
		return false;
	}

// 	fstream ioFile;
// 	ioFile.open("MinCost.dat",ios::out);
// 	ioFile<<"MinCost"<<endl;

	//-----------------------------------------------------------------------
	// A*算法规划循环
	// OPEN表不为空就继续执行规划线程 
	while(!OpenList.empty())
	{

		//如果是线程结束则不进行计算
		if (isEndThread)
		{
			break;
		}

		// 当前任务open表中代价最小的点(open表中代价最小的点排在最前面，已经排好序)
		MinCostNode = OpenList.front();

 //ioFile<<20*MinCostNode->X<<" "<<20*MinCostNode->Y<<" "<<MinCostNode->Z<<endl;		

		// 从Open表中弹出此点
		OpenList.pop_front(); 
	
		//判断此点是否为目标点（如果是则规划完成，成功退出）
		if(PlanIsOK(MinCostNode))
		{
			return true;
		}

		//当前点是导航点，继续以该点作为起点搜索
		if(IsTempTar(MinCostNode))
		{
			continue;
		}


		//-----------------------------------------------------------------------
		// 【扩展模块】开始扩展此条航迹的此个最小代价节点
		if(!PLAN_ExpandNaviNode(MinCostNode,OpenList,CloseList,m_pCurEmit,m_pCurTarget))
		{
			continue;
		}
	}//end of while

//ioFile.close();

	errStr = ExpErr;
 	return false;  //TODO:返回错误包括OpenList已空，规划超时等
}


//获取任务
bool APlanning::getTask(vector<GroundPoint> &LeaderVector)
{
	int sz = LeaderVector.size();
	if(sz < 2)
	{
		errStr = GetTaskErr;
		return false;
	}

	NavigationNode temp;
	for(int i=0;i<sz;i++)
	{
		LBtoIJ(LeaderVector[i].X,LeaderVector[i].Y, G_RESOLVE, temp.X, temp.Y);
		temp.Z =(int)(LeaderVector[i].Z + 0.5);
		if(i == 0)
		{
			temp.PointType =1;
		}
		else if(i == sz-1)
		{
			temp.PointType =3;
		}
		else
		{
			//判断高程值是否合法
			int Z;
			if(!GetZofPoint(temp.X, temp.Y, Z, 0, G_hGctDataFilemap) || Z < 0)
			{
				continue;
			}

			//判断引导点是否在禁飞区
			if(PLAN_IsInForbid(temp.X, temp.Y))
			{
				continue;
			}

			double dis = PLAN_Distance(&temp,&TaskList.back());
			if(dis <= maxl)
			{
				continue;
			}

			temp.PointType =2;
		}
		TaskList.push_back(temp);
	}  //end for
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// 函数功能：解析当前的任务并判断该任务是否合法
//           初始化open，closed链表; open表加入发射点，close表为空; 
// 输入参数：当前的任务(包含该任务的起点和目标点)
// 输出参数：当前任务的Open表、Close表
bool APlanning::InitAStar()
{
// 	*m_pCurEmit = DefNavigationNode;
// 	*m_pCurTarget = DefNavigationNode;
// 	*MinCostNode = DefNavigationNode;
	factofthreat = 0.1;

	//取出当前任务的起点和终点
	if(!TaskList.empty())
	{
		*m_pCurEmit = TaskList.front();
		m_pCurEmit->PointType = 1;   //重置该点为起点
		m_pCurEmit->pFather = NULL;  //置空便于路径回朔
		TaskList.pop_front();

		*m_pCurTarget = TaskList.front();

		//已找到最终目标，后面不用再搜了
		if(3 == m_pCurTarget->PointType)   
		{
			TaskList.pop_front(); 
		}
	}
	

	//计算起点与目标点连线的角度
	m_pCurEmit->angle = PLAN_LinkAngle(m_pCurEmit->X, m_pCurEmit->Y,m_pCurTarget->X,m_pCurTarget->Y);
	m_pCurTarget->angle = m_pCurEmit->angle;


	//-----------------------------------------------------------------------
	// 可能出现的情况：坐标越界、高度违法、点在禁飞区or威胁区
	// TODO：这里仍然沿用了读取地形高程图的Z值,没有采用读取组合地形图
	// TODO: 判断当前任务的发射点和目标点平面坐标是否越界

	// 判断当前任务的发射点和目标点是否合法
	if (!PLAN_CheckEmitTarPoint(m_pCurEmit, m_pCurTarget) || 
		!IsInAreaofPoint(m_pCurEmit->X, m_pCurEmit->Y) ||
		!IsInAreaofPoint(m_pCurTarget->X, m_pCurTarget->Y)) 
	{
		return false;
	}


	if(IsInForbid(m_pCurEmit,m_pCurTarget))
	{
		return false;
	}

	// TODO: 判断当前任务的发射点和目标点是否在合法的禁架区or威胁区内
	// 如果起点和目标点都在威胁区内,则调整威胁代价系数
	if(IsInThreat(m_pCurEmit,m_pCurTarget))
	{
		factofthreat = 0.001;
	}

	//-------------------------------------------------------------------------
	// 算法开始前的数据清理, 清空A*算法中要用到的open、close表
	OpenList.clear();
	CloseList.clear();
	TempList.clear();

	//-------------------------------------------------------------------------
	// 将当前任务的起点加入到当前任务的OPEN链表。注意：此链表是指针路径链表!
	OpenList.push_front(m_pCurEmit);

	return true;
}


bool APlanning::IsInThreat(pNaviNode pointA, pNaviNode pointB)
{
	if (PLAN_IsInThreat(pointA->X,pointA->Y) || PLAN_IsInThreat(pointB->X,pointB->Y))
	{                
		return true;
	}

	return false;
}


bool APlanning::IsInForbid(pNaviNode pointA, pNaviNode pointB)
{
	if ( PLAN_IsInForbid(pointA->X,pointA->Y)|| PLAN_IsInForbid(pointB->X,pointB->Y))    
	{
		return true;
	}
	
	return false;
}


// 函数描述:PLAN_PLANEDOK 判断当前点是否是目标点
//TODO:如果当前点是目标点则回朔找出规划路径
bool APlanning::PlanIsOK(pNaviNode pCurNode)
{
	//判断此点是否为目标点
	//节点类型（0:默认 1:起点  2:导航点 3:目标点）
	if(3 == pCurNode->PointType)  
	{	
		pNaviNode p;      //回朔找出规划路径
		p = pCurNode;
		while(p != NULL)
		{
			TempList.push_front(*p);  //将当前节点压入路劲展示链表
			p = p->pFather;  
		}

		list<NavigationNode>::iterator last,beg,end;  // 设置迭代器 
		last = ShowList.end();
		beg = TempList.begin();
		end = TempList.end();
		ShowList.insert(last,beg,end);

		//销毁OpenList、CloseList再返回
		OpenList.clear();
		CloseList.clear();

		return true;
	}
	return false;
}


//判断当前点是否是导航点
//TODO:如果当前点是导航点则回朔找出规划路径
bool APlanning::IsTempTar(pNaviNode pCurNode)  
{
	//节点类型（0:默认 1:起点  2:导航点 3:目标点）
	if(2 == pCurNode->PointType)  
	{								 
		pNaviNode p;      //回朔找出规划路径
		p = pCurNode;
		while(p != NULL)
		{
			TempList.push_front(*p);  //将当前节点压入路劲展示链表
			p = p->pFather;  
		}

		list<NavigationNode>::iterator last,beg,end;  // 设置迭代器 
		last = ShowList.end();
		beg = TempList.begin();
		end = TempList.end();
		ShowList.insert(last,beg,end);
	
		InitAStar();   //从新初始化，为下一段规划做准备
		return true;
	}
	return false;
}


//输出A*局部规划结果
bool APlanning::OutPutData(vector<GroundPoint> &ResultVector)
{
	NavigationNode temp;
	GroundPoint gPoint;
	while(!ShowList.empty())
	{
		temp = ShowList.front();
		ShowList.pop_front();
		IJtoLB(temp.X,temp.Y,G_RESOLVE,gPoint.X, gPoint.Y);
		gPoint.Z = (double)temp.Z;
		ResultVector.push_back(gPoint);
	}

	return true;
}

//获取输入参数信息
void APlanning::getParaminput(double &minlength, //任意两个节点之间的最小距离
	double &maxlength,               //任意两个节点之间的最大距离
	double &maxhang,                 //两个节点之间的最大转弯角度(弧度表示)
	double &maxvang)                 //两个节点之间的最大坡度（弧度表示）
{
	minl = minlength/G_RESOLVE;
	maxl = maxlength/G_RESOLVE;
	maxhangle = RADIAN(maxhang);
	maxvangle = RADIAN(maxvang);
}

//获取地图信息
bool APlanning::getMapInfo(LPCWSTR szFileName)            // 全局规划地图的分辨率 
{
	ReadMapFileOperation rmf;
	//打开原始文件

	int iLengthFileName = WideCharToMultiByte(CP_ACP , 0 , szFileName , -1 , 
		NULL , 0 , NULL , NULL);
	char * szFileaNameA = new char[iLengthFileName];
	WideCharToMultiByte(CP_ACP , 0 , szFileName , iLengthFileName , 
		szFileaNameA , iLengthFileName , NULL , NULL);

	if (!rmf.OpenFileByRead(szFileaNameA))
		return false;

	delete []szFileaNameA;

	MapFileHeader mfh;
	//读文件头
	if (!rmf.ReadMapFileHeader(mfh))
	{
		return false;
	}

	// add by mj at 10-28
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);
	G_dwGran = sinf.dwAllocationGranularity;    //初始化系统分配粒度
	G_MAPL0 = mfh.xllcorner;               // 全局地图左上角的经度
	G_MAPB0 = mfh.yllcorner;               // 全局地图左上角的纬度
	G_WIDTH = mfh.nclos;                   // 全局规划地图的宽度
	G_HEIGHT = mfh.nrows; // 全局规划地图的高度
	G_RESOLVE = mfh.cellsize; //add by hj 2013.10.31


	if(!CreateGctDataMap(szFileName , G_hGctDataFilemap))
	{
		return false;
	}

	return true;
}

bool APlanning::InitComMap(
	char * szMapFileName ,//组合图的名称
	__int64 i64MapHeightInPixel ,//组合图高度像素点
	__int64 i64MapWdithInPixel)	//组合图宽度 像素点
{
	WriteComMapFile wcmf;
	if (!wcmf.WrtieAllData(szMapFileName , i64MapHeightInPixel , i64MapWdithInPixel))
	{
		return false;
	}
	return true;
}

bool APlanning::CreateComMap(
	TCHAR * szMapFileName , //组合图名称
	vector<iPolygon> &forbidAndThreatVector)//禁飞区威胁区信息 坐标已经转换
{
	//初始化组合图
	int needlen = WideCharToMultiByte( CP_ACP, 0, szMapFileName, -1, NULL, 0, NULL, NULL );
	PCHAR pAsciiChar = (PCHAR)HeapAlloc(GetProcessHeap() , 0 , needlen);
	WideCharToMultiByte( CP_ACP, 0, szMapFileName , -1, pAsciiChar , needlen, NULL, NULL );

	if(!InitComMap(pAsciiChar , G_HEIGHT , G_WIDTH))
	{
		return false;
	}
	HeapFree(GetProcessHeap() , 0 , pAsciiChar);

	//映射内存文件
	if(!CreateGctDataMap(szMapFileName , G_hZhtMapping))
	{
		return false;
	}

	StandardizeConstraint standcos;//转换坐标用
	//如果没有转换就在此转换
	// 	for (int i = 0 ; i < forbidAndThreatVector.size() ; i++)
	// 	{
	// 		standcos.Standardize(forbidAndThreatVector[i]);
	// 	}

	//将禁飞区威胁区写入到组合图
	for(int i = 0 ; i < forbidAndThreatVector.size() ; i++)
	{
		//映射一个外接矩形的大小
		__int64 begin = forbidAndThreatVector[i].rectangle.ly;
		int row = forbidAndThreatVector[i].rectangle.height;
		int errOfStart;
		void * pBuf = ReadComMapData(G_hZhtMapping , begin , row , errOfStart);

		short * buf = (short *)((PUCHAR)pBuf + errOfStart);

		if (pBuf == NULL)
		{
			return false;
		}

		//边界判断
		int tmp = forbidAndThreatVector[i].rectangle.lx;
		if (tmp < 0)
		{
			tmp = 0;
		}

		int length = tmp + forbidAndThreatVector[i].rectangle.width;
		if (length >= G_WIDTH)
		{
			length = G_WIDTH;
		}

		//判断一个点是否在多边形中
		StandardizeConstraint standCons;

		for (int j = 0 ; j < row ; j++)
		{
			int y = j + forbidAndThreatVector[i].rectangle.ly;
			for (int k = tmp ; k < length ; k++)
			{
				iPoint pt;
				pt.x = k;
				pt.y = y;

				//点不在多边形中
				if (!standCons.APointIsInPolygon(pt , forbidAndThreatVector[i]))
				{
					continue;
				}

				short * tmpBuf = buf + j*G_WIDTH + k;
				if (forbidAndThreatVector[i].level == 1 || forbidAndThreatVector[i].level == 2)
				{//禁飞区
					*tmpBuf = MAKESHORT(forbidAndThreatVector[i].level , FORBID_FLY_AREA);
				}
				else
				{//威胁区
					*tmpBuf = MAKESHORT(forbidAndThreatVector[i].level , THREAT_FLY_AREA);
				}
			}
		}
		FlushViewOfFile(pBuf , 0);//将修改的数据写入磁盘
		UnmapViewOfFile(pBuf);//关闭映射
	}

	return true;
}