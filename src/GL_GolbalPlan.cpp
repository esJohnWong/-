
#include "GL_GolbalPlan.h"


GL_GolbalPlan::GL_GolbalPlan()
{
	srand((int)time(0));   //随即种子，避免重复相同随即数
	this->SEED=5;          //每条序列引导点个数
	this->num=0;             //记录输出路径的条数
}


GL_GolbalPlan::~GL_GolbalPlan()
{

}


//全局规划的实现过程入口函数
bool GL_GolbalPlan::run()
{
	GetTransInfo_GL();  //计算规划区域
	initialize_GL();    //初始化
	if(2 == SEED)   //这种情况由于起点和目标点隔得较近，直接交由局部规划处理 
	{
		OutPutData_GL();
		return true; 
	}

	evaluate_GL();      //评价所有航迹，按优劣排序
	
	//采取末位淘汰的方法代替选择操作,直接淘汰最差的末两位个体，其他全部进入下一代
	int generationc=0;     //遗传算法的代数
	int Maxiter=MAXGEN;    //迭代次数
	int breakCount=0;      //加入循环终止条件判断
	double lastCost=MAXNUM;
	num = 0;
	
	while(generationc<=Maxiter)
	{
		//如果是线程结束则不进行计算
		if (isEndThread)
		{
			break;
		}

		//交叉算子，在被选入下一代的个体中随即挑选其中两条航迹进行交叉
		//交叉后结果直接替换被淘汰的位置，并立即做评价，交叉可以采取多种方式
		crossover_GL();

		//变异算子，以一定概率挑选其中两条进行变异，变异后替代原来个体，盲变异不在最优个体上施行（第0个）
		//变异后立即对个体重新做出评价，变异方式可以根据先验知识多样化
		mutate_GL(); 

		//按代价重新对群体按升序排序
		reOrder_GL();

		//加入终止条件的判断:如果代价为正;且持续Maxiter/10;代则终止;
		if(fabs(lastCost-planningRoutes[0]->totalcost)<10)
		{
			breakCount++;
		}
		else
		{
			breakCount=0;
		}
		
		if(breakCount>=Maxiter/10)	
		{
			OutPutData_GL();
			break;
		}


        lastCost=planningRoutes[0]->totalcost;
		generationc++;

	}//end while

	//////////////////////////////////////////////////////////////////////////////
	//.................................遗传算法结束...............................
	
	
	OutPutData_GL();  //输出数据

	//释放申请的动态内存
	for(int i=0;i<NOOFROUTES;i++)
	{
		if(planningRoutes[i]!=NULL)
		{
			if(planningRoutes[i]->genes != NULL)
			{
				free(planningRoutes[i]->genes);
			}
			free(planningRoutes[i]);
		}
	}

	return true;
}


//根据给定的发射点，目标点计算出规划相关的区域信息
void GL_GolbalPlan::GetTransInfo_GL()
{
	//算出规划框的描述
	int leftup_x,leftup_y,rightdown_x,rightdown_y;
	leftup_x= min(emitPoint.X,targetPoint.X) - 20;   //增加一个余量，防止规划溢出
	leftup_y= min(emitPoint.Y,targetPoint.Y) - 20;
	rightdown_x = max(emitPoint.X, targetPoint.X) + 20;
	rightdown_y = max(emitPoint.Y, targetPoint.Y) + 20;

	transinfo.leftupX = leftup_x;
	transinfo.leftupY = leftup_y;
	transinfo.width = rightdown_x -leftup_x;
	transinfo.height = rightdown_y- leftup_y;
}


//完成对初始的几条引导序列的初始化：
//初始化为随即生成N个引导点（按照在规划区域的X方向增长）
//个数N为:发射点到目标点距离/L（其中L为一经验长度）
void GL_GolbalPlan::initialize_GL()
{
	//确定编码序列的长度
	double ETDitance=PLAN_Distance(&emitPoint,&targetPoint);
	if(ETDitance<=400) //规划距离小于400像素
	{
		//直接交给局部规划发射点和目标点
		SEED = 2;
		return ;
	}
	else if(ETDitance>400 && ETDitance<=800)
	{
		SEED=4;   //reduce a node
	}
	else if(ETDitance>2000)
	{
		SEED=6;   //add a node
	}
	else
	{
		SEED=5;
	}

	//初始化种群
	for(int i=0;i<NOOFROUTES;i++)
	{   
		//为各条航迹申请存储空间
		planningRoutes[i]=(genoType *)malloc(sizeof(genoType));//每条航迹包含SEED个引导点,设置成指向一条航迹结构指针，便于排序后的交换
		planningRoutes[i]->genes=(EarthPoint *)malloc((SEED)*sizeof(EarthPoint));
		for(int j=0;j<SEED;j++)
		{
			planningRoutes[i]->genes[j].L=0;      //赋予初值0
			planningRoutes[i]->genes[j].B=0;      //赋予初值0
		}
	}


	int interval = transinfo.width/(SEED-1);  //初始种子的间距
	int Width = transinfo.width;              //规划矩形框的宽度
	int Height = transinfo.height;            //规划矩形框的高度
    int rx = transinfo.leftupX;
	int ry = transinfo.leftupY;

	//产生第一代种群
	for(int i=0;i<NOOFROUTES;i++)
	{
		//对每条航迹发射点赋同样值
		planningRoutes[i]->genes[0].L = emitPoint.X;
		planningRoutes[i]->genes[0].B = emitPoint.Y;

		for(int j=1;j<SEED-1;j++)
		{
			//按照一定interval生成随即点
			if(emitPoint.X < targetPoint.X)
			{
				planningRoutes[i]->genes[j].L=rx+interval*(j-1)+rand()%interval;
				planningRoutes[i]->genes[j].B=ry+rand()%Height;
			
				//如果在禁飞区就重新随机生成
				while(PLAN_IsInForbid(planningRoutes[i]->genes[j].L,planningRoutes[i]->genes[j].B))
				{
					planningRoutes[i]->genes[j].L=rx+interval*(j-1)+rand()%interval;
					planningRoutes[i]->genes[j].B=ry+rand()%Height;
				}
			}
			else
			{
				planningRoutes[i]->genes[j].L=emitPoint.X-interval*(j-1)-rand()%interval;
				planningRoutes[i]->genes[j].B=ry+rand()%Height;

				//如果在禁飞区就重新随机生成
				while(PLAN_IsInForbid(planningRoutes[i]->genes[j].L,planningRoutes[i]->genes[j].B))
				{
					planningRoutes[i]->genes[j].L=emitPoint.X-interval*(j-1)-rand()%interval;
					planningRoutes[i]->genes[j].B=ry+rand()%Height;
				}
			}
		}

		//对每条航迹目标点赋同样值
		planningRoutes[i]->genes[SEED-1].L=targetPoint.X;
		planningRoutes[i]->genes[SEED-1].B=targetPoint.Y;
		planningRoutes[i]->totalcost=0.0;
		orderByX_GL(planningRoutes[i]);   //按X方向排序

	} //end for
}


// 计算角度
double GL_GolbalPlan::calAngle_GL(EarthPoint * frontNode,EarthPoint* centreNode,EarthPoint* lastNode)//直接求得当前点夹角的弧度
{
	double angle=0;
	double cosa=0;
	double a,b,c;
	a=(frontNode->L-lastNode->L)*(frontNode->L-lastNode->L)+(frontNode->B-lastNode->B)*(frontNode->B-lastNode->B);
	b=(centreNode->L-lastNode->L)*(centreNode->L-lastNode->L)+(centreNode->B-lastNode->B)*(centreNode->B-lastNode->B);
	c=(frontNode->L-centreNode->L)*(frontNode->L-centreNode->L)+(frontNode->B-centreNode->B)*(frontNode->B-centreNode->B);

	cosa=(double)(c+b-a)/(2.0*sqrtf((float)(b*c)));
	if(abs(cosa-1.0)<0.00001)
		cosa=1.0;
	if(cosa<=-0.9995) 
		angle=PI;
	else angle=acosl(cosa);
	return angle;
}

 
//航迹个体的评价
void GL_GolbalPlan::evaluateCostOfARuote_GL(genoType *planningRoute)
{
	double cost= 0.0;
	double dis=0.0;
	NavigationNode prev,next,temp;
	
	//从头到尾计算一遍各个引导段的代价值,只对算子操作后的航迹进行
	for(int i=0;i<SEED-1;i++)
	{
		prev.X = (planningRoute->genes[i]).L;
		prev.Y = (planningRoute->genes[i]).B;
		next.X = (planningRoute->genes[i+1]).L;
		next.Y = (planningRoute->genes[i+1]).B;

		if(PLAN_IsInForbid(prev.X,prev.Y))
		{
			cost+=100;
		}

		if(PLAN_IsInForbid(next.X,next.Y))
		{
			cost+=100;
		}
		cost += PLAN_Cost(&emitPoint,&prev,&next,&targetPoint);
	}

	//拐点角度代价
// 	for(int i=1;i<SEED-1;i++)
// 	{
// 		prev.X = (planningRoute->genes[i]).L;
// 		prev.Y = (planningRoute->genes[i]).B;
// 		next.X = (planningRoute->genes[i+1]).L;
// 		next.Y = (planningRoute->genes[i+1]).B;
// 
// 		angle = calAngle_GL(&(planningRoute->genes[i-1]),&(planningRoute->genes[i]),&(planningRoute->genes[i+1]));
// 		cost -= angle*180/PI;
// 	}

	//add at 11-20
	temp.X = (planningRoute->genes[1]).L;
	temp.Y = (planningRoute->genes[1]).B;
	dis = PLAN_Distance(&emitPoint,&temp);
	cost -= dis;

	temp.X = (planningRoute->genes[SEED-2]).L;
	temp.Y = (planningRoute->genes[SEED-2]).B;
	dis = PLAN_Distance(&targetPoint,&temp);
	cost -= dis;
	
    planningRoute->totalcost=cost;
}

       
//按代价对每条航迹计算总代价，按照代价值对航迹种群按从低到高排序
void GL_GolbalPlan::evaluate_GL()
{
	for(int i=0;i<NOOFROUTES;i++)
	{
		evaluateCostOfARuote_GL(planningRoutes[i]);
	}

	reOrder_GL();  //对航迹按代价冒泡排序
}

      
//对航迹按X方向冒泡排个序
void GL_GolbalPlan::orderByX_GL(genoType *Track)
{
	//下面对航迹按X方向冒泡排个序，序号再这里没有恢复，如果有必要，则必须考虑序号的有序性
	for(int j=0;j<SEED-2;j++)//这里只对0~SEED-2的节点排序.既前SEED-1个按X轴来排
	{
		for(int k=0;k<SEED-2-j;k++)//每轮进行n－1－j 次比较，最多n－1－j 次交换
		{
			if(Track->genes[k].L>Track->genes[k+1].L)
			{
				EarthPoint temlandMark;
				temlandMark = Track->genes[k];
				Track->genes[k] = Track->genes[k+1];
				Track->genes[k+1] = temlandMark;
			}

		}
	}
}


//简单的从中间一点交叉
void GL_GolbalPlan::simpleCrossover_GL(genoType *TrackOne,genoType *TrackTwo,genoType *LastOne,genoType *LastTwo)
{
	int crossPoint=0;
	crossPoint=rand()%SEED;
	for(int i=0;i<SEED;i++)
	{
		//实现从中间某点的交叉
		if(i<=crossPoint)
		{
			LastOne->genes[i]=TrackOne->genes[i];
			LastTwo->genes[i]=TrackTwo->genes[i];
		}
		else
		{
			LastOne->genes[i]=TrackTwo->genes[i];
			LastTwo->genes[i]=TrackOne->genes[i];
		}
	}

	//对有可能改变引导点按X方向排列顺序的操作，切记要对操作后的航迹进行一次这样的排序
	orderByX_GL(LastOne);
	orderByX_GL(LastTwo);
}

    
//执行对中间某一段涉及两点的交叉操作
void GL_GolbalPlan::segmentCrossover_GL(genoType *TrackOne,genoType *TrackTwo,genoType *LastOne,genoType *LastTwo)
{ 
	int crossPoint=0;
	crossPoint=rand()%(SEED-1);

	for(int i=0;i<SEED;i++)
	{
		//实现从中间某段的交叉
		if(i!=crossPoint||i!=(crossPoint+1))
		{
			LastOne->genes[i]=TrackOne->genes[i];
			LastTwo->genes[i]=TrackTwo->genes[i];
		}
		else
		{
			LastOne->genes[i]=TrackTwo->genes[i];
			LastTwo->genes[i]=TrackOne->genes[i];
		}
	}

	//对有可能改变引导点按X方向排列顺序的操作，切记要对操作后的航迹进行一次这样的排序
	orderByX_GL(LastOne);
	orderByX_GL(LastTwo);
}

   
//交叉算子
void GL_GolbalPlan::crossover_GL()
{
	double ran_P = 0.0;  //0~1间概率P
	ran_P = rand()/(double)(RAND_MAX);

	//下面决定挑选在哪两条航迹间进行交叉操作,策略是奇数里随即选一条，偶数里随即选一条
	//但记住是在前NOOFROUTES-2个个体里挑选
	int Num1=rand()%((NOOFROUTES-2)/2);
	Num1=2*Num1;  //偶位里挑一个

	int Num2=rand()%((NOOFROUTES-2)/2);
	Num2=2*Num2+1;  //奇位里挑一个

	if(ran_P>0&&ran_P<=0.12)
	{
		simpleCrossover_GL(planningRoutes[Num1],planningRoutes[Num2],planningRoutes[NOOFROUTES-1],planningRoutes[NOOFROUTES-2]);
	}
	else if (ran_P>0.12&&ran_P<=0.15)
	{	
		segmentCrossover_GL(planningRoutes[Num1],planningRoutes[Num2],planningRoutes[NOOFROUTES-1],planningRoutes[NOOFROUTES-2]);
	}
	
	if(ran_P>0&&ran_P<=0.45) //加大收敛性
	{
		//重新评价交叉后的个体
		evaluateCostOfARuote_GL(planningRoutes[NOOFROUTES-2]);
		evaluateCostOfARuote_GL(planningRoutes[NOOFROUTES-1]);
	}
}


//在一个圆区域内实现随即的扰动
void GL_GolbalPlan::disturbWithinACircle_GL(EarthPoint *centrePoint,int r)
{
	double ran_P=0.0;//0~1间概率P
	ran_P = rand() / (double)(RAND_MAX);
	double angle = ran_P*360;   //随机旋转角
	ran_P = rand() / (double)(RAND_MAX);
	double Radius= ran_P*r*0.3;	//随机半径
	EarthPoint beforeP;
	beforeP.L=centrePoint->L;
	beforeP.B=centrePoint->B;
	centrePoint->L=(int)(centrePoint->L+Radius*cos(RADIAN(angle)));
	centrePoint->B=(int)(centrePoint->B+Radius*sin(RADIAN(angle)));

	if(!(centrePoint->L <= transinfo.leftupX + transinfo.width &&
		 centrePoint->L >= transinfo.leftupX &&
		 centrePoint->B <= transinfo.leftupY + transinfo.height &&
		 centrePoint->B >= transinfo.leftupY))
	{
		centrePoint->L=beforeP.L;
		centrePoint->B=beforeP.B;
	}	
}

//变异算子
void GL_GolbalPlan::distrub_GL(genoType *Track,int i)
{
	int width=transinfo.width;    //宽度
	int length=transinfo.height;  //高度
	int distrubPoint=0;                //施行扰动的位置
	while(distrubPoint==0||distrubPoint==SEED-1)//要注意，发射点与目标点是不允许有扰动的，如果是则再随即一次
	{
		distrubPoint=rand()%(SEED-1);
	}

	//就在一个范围圆内扰动
	//下面求一下前后相关两段引导段的距离，取较小的一个作为扰动半径
	int disOne=(int)sqrt((double)(Track->genes[distrubPoint].L-Track->genes[distrubPoint-1].L)*(Track->genes[distrubPoint].L-Track->genes[distrubPoint-1].L)+(Track->genes[distrubPoint].B-Track->genes[distrubPoint-1].B)*(Track->genes[distrubPoint].B-Track->genes[distrubPoint-1].B));
	int disTwo=(int)sqrt((double)(Track->genes[distrubPoint].L-Track->genes[distrubPoint+1].L)*(Track->genes[distrubPoint].L-Track->genes[distrubPoint+1].L)+(Track->genes[distrubPoint].B-Track->genes[distrubPoint+1].B)*(Track->genes[distrubPoint].B-Track->genes[distrubPoint+1].B));
	int r=0;
	if(disOne<disTwo) 
	{
		r=disOne;
	}
	else 
	{
		r=disTwo;
	}

	//在一个圆区域内实现随即的扰动
	disturbWithinACircle_GL(&Track->genes[distrubPoint],r);

	//对有可能改变引导点按X方向排列顺序的操作，切记要对操作后的航迹进行一次这样的排序
	orderByX_GL(Track);
}

 
//平滑算子
void GL_GolbalPlan::smooth_GL(genoType *Track)
{
	//随即找一点进行平滑处理
	int distrubPoint=0;

	while(distrubPoint==0)//要注意，发射点与目标点是不允许有扰动的，如果是则再随即一次
	{
		distrubPoint=rand()%(SEED-1);
	}

	//求最陡角所在三角形的内切圆圆心来作平滑处理
	double x1,y1,x2,y2,x3,y3;
	double a,b,c;
	x1=Track->genes[distrubPoint-1].L;y1=Track->genes[distrubPoint-1].B;
	x2=Track->genes[distrubPoint].L;y2=Track->genes[distrubPoint].B;
	x3=Track->genes[distrubPoint+1].L;y3=Track->genes[distrubPoint+1].B;

	a=distanc_GL(Track->genes[distrubPoint+1],Track->genes[distrubPoint]);
	b=distanc_GL(Track->genes[distrubPoint-1],Track->genes[distrubPoint+1]);
	c=distanc_GL(Track->genes[distrubPoint-1],Track->genes[distrubPoint]);

	//平滑到所构成三角形的形心
	Track->genes[distrubPoint].L=(int)((a*x1+b*x2+c*x3)/(a+b+c));
	Track->genes[distrubPoint].B=(int)((a*y1+b*y2+c*y3)/(a+b+c));

	//对有可能改变引导点按X方向排列顺序的操作，切记要对操作后的航迹进行一次这样的排序
	orderByX_GL(Track);
}

     
//求两点距离
double GL_GolbalPlan::distanc_GL(EarthPoint point1,EarthPoint point2)
{
	double distance=0;
	distance=(point1.L-point2.L)*(point1.L-point2.L)+(point1.B-point2.B)*(point1.B-point2.B);
	distance=sqrt(distance);
	return distance;
}

  
//将近似为一条直线上的各点连成直线
bool GL_GolbalPlan::applyProjection_GL(int beginPoint,int endPoint,genoType *Track)//连成直线
{
	
	if(beginPoint<=0||endPoint>=SEED-1) 
		return 0;
	
	for(int i=beginPoint;i<=endPoint;i++)
	{
		double scale=fabs((double)(Track->genes[beginPoint-1].L-Track->genes[i].L)/(Track->genes[beginPoint-1].L-Track->genes[endPoint+1].L));
		if(Track->genes[beginPoint-1].B>Track->genes[endPoint-1].B)
		{
			Track->genes[i].B=Track->genes[beginPoint-1].B-(int)(abs(Track->genes[beginPoint-1].B-Track->genes[endPoint+1].B)*scale);
		}
		else
		{
			Track->genes[i].B=Track->genes[beginPoint-1].B+(int)(abs(Track->genes[beginPoint-1].B-Track->genes[endPoint+1].B)*scale);
		}
	}
	return 1;
}


//拉伸算子：
void GL_GolbalPlan::stretch_GL(genoType *Track)
{
	int beginPoint=0;//标记拉伸处理的开始
	int endPoint=0;//标记拉伸处理的最末
	int counter=0;
	int tag=0;//是否存在拉伸的必要，只找第一个最长的拉伸段拉伸,已经拉伸过的将检测发现是平角，会被算子跳过

	for(int i=1;i<SEED-1;i++)//发射点与目标点是没有角度的，对中间点检测并决定拉伸范围
	{	
		double angleOfI=fabs(calAngle_GL(&Track->genes[i],&Track->genes[i],&Track->genes[i])-PI);
		if(0.0001<angleOfI&&angleOfI<=0.05)//接近平角的三度范围内
		{

			if(tag==0)//第一次碰到“平角”
			{tag=1;
			beginPoint=i;
			endPoint=i;
			}
			else if(tag==1)//已经不是第一个了
			{
				endPoint=i;
			}
		}
		else
		{//碰到不是“平角”情况有两种:
			if(tag==0) continue;//还没有找到拉伸段就直接往后找
			else//否者是碰到该段结束位
			{
				tag=2;
				break;//结束拉伸段的查找
			}

		}
	}//end for

	if(tag!=0)//存在拉伸的必要
	{
		applyProjection_GL(beginPoint,endPoint,Track);//连成直线
	}

	orderByX_GL(Track);
}


//变异操作的总调度方法
void GL_GolbalPlan::mutate_GL()
{
	double ran_P=0.0;//0~1间概率P
	int count=rand()%3;
	while(--count!=-1)
	{
		ran_P = rand()/(double)(RAND_MAX);
	}

	//下面决定挑选在哪条航迹间进行变异操作,策略是奇数里随即选一条，偶数里随即选一条
	//但记住是在前NOOFROUTES-2个个体里挑选
	count=rand()%3;
	int Num1;
	Num1=rand()%((NOOFROUTES-2)/2);
	if(Num1==0)
	{
		Num1++;
	}
	Num1=2*Num1; //偶位里挑一个

	count=rand()%3;
	int Num2=rand()%((NOOFROUTES-2)/2);
	Num2=2*Num2+1; //奇位里挑一个

	if(ran_P>0&&ran_P<=0.78)
	{
		distrub_GL(planningRoutes[Num1],Num1);
		distrub_GL(planningRoutes[Num2],Num2);
	}
	else if(ran_P>0.78&&ran_P<=0.83)
	{
		smooth_GL(planningRoutes[Num1]);
		smooth_GL(planningRoutes[Num2]);
	}
	else if(ran_P>0.83&&ran_P<=0.84)
	{
		stretch_GL(planningRoutes[Num1]);
		stretch_GL(planningRoutes[Num2]);
	}
	
	//重新评价变异后的个体
	evaluateCostOfARuote_GL(planningRoutes[Num1]);
	evaluateCostOfARuote_GL(planningRoutes[Num2]);
}

      
//对种群重新排序：冒泡排序
void GL_GolbalPlan::reOrder_GL()
{
	
	for(int j=0;j<NOOFROUTES-1;j++)
	{
		for(int k=0;k<NOOFROUTES-1-j;k++)//每轮进行n－1－j 次比较，最多n－1－j 次交换
		{
			if(planningRoutes[k+1]->totalcost < planningRoutes[k]->totalcost)	
			{
				genoType *temRoute;
				temRoute=planningRoutes[k];
				planningRoutes[k]=planningRoutes[k+1];
				planningRoutes[k+1]=temRoute;
			}

		}
	}//end for
}


//读入发射点目标点信息并判断任务是否合法
bool GL_GolbalPlan::loadTasks_GL(std::list<NavigationNode> &ETList)
{
	int sz = ETList.size();
	if(sz < 2)
	{
		errStr = GA_loadTaskErr;
		return false;
	}
	emitPoint = ETList.front();
	targetPoint = ETList.back();
	
	if(emitPoint.X > targetPoint.X)
	{
		NavigationNode tmp;
		tmp = emitPoint;
		emitPoint = targetPoint;
		targetPoint = tmp;
	}

	emitPoint.PointType=1;     //表示此点类型为起点
	targetPoint.PointType = 3;  //表示此点类型为目标点
	
	emitPoint.angle = PLAN_LinkAngle(emitPoint.X,emitPoint.Y,targetPoint.X,targetPoint.Y);
	targetPoint.angle = emitPoint.angle;

	// 判断当前任务的发射点和目标点是否合法
	if (!PLAN_CheckEmitTarPoint(&emitPoint, &targetPoint) || 
		!IsInAreaofPoint(emitPoint.X, emitPoint.Y) ||
		!IsInAreaofPoint(targetPoint.X, targetPoint.Y)) 
	{
		errStr = GA_loadTaskErr;
		return false;
	}

	return true;
}

       
//规划结果的输出过程(可根据需要保留几条较好的引导点序列）
void GL_GolbalPlan::OutPutData_GL()
{
	NavigationNode temp = DefNavigationNode;  //引导点
	RouteList Markers;   //一条路径链表

	//取其中的10条作为参考
	for(int n = 0; n*4<NOOFROUTES; n++)
	{
		int j = n*4;

		//发射点一定是引导点
		Markers.push_back(emitPoint);

		//引导点去重复
		for(int i=1;i<SEED-1;i++)
		{
			//在禁飞区的去除
			if(PLAN_IsInForbid(planningRoutes[j]->genes[i].L,planningRoutes[j]->genes[i].B))
			{
				continue;
			}

			//如果不是角点,就可以去处
			NavigationNode prev,next;
			double dis = 0.0;
			prev.X = planningRoutes[j]->genes[i-1].L;
			prev.Y = planningRoutes[j]->genes[i-1].B;
			next.X = planningRoutes[j]->genes[i].L;
			next.Y = planningRoutes[j]->genes[i].B;
			dis = PLAN_Distance(&prev,&next);
			if(dis < maxl)
			{
				continue; //去掉
			}

			prev.X = planningRoutes[j]->genes[i].L;
			prev.Y = planningRoutes[j]->genes[i].B;
			next.X = planningRoutes[j]->genes[i+1].L;
			next.Y = planningRoutes[j]->genes[i+1].B;
			dis = PLAN_Distance(&prev,&next);
			if(dis < maxl)
			{
				continue; //去掉
			}

			//直接求得当前点夹角的弧度
			double angle = calAngle_GL(&planningRoutes[j]->genes[i-1],&planningRoutes[j]->genes[i],&planningRoutes[j]->genes[i+1]);
			if(angle == PI)
			{
				continue;//去掉
			}

			//平滑处理，便于局部规划
			while(angle < PI*2/3)
			{
				//求最陡角所在三角形的内切圆圆心来作平滑处理
				double x1,y1,x2,y2,x3,y3;
				double a,b,c;
				x1=planningRoutes[j]->genes[i-1].L;
				y1=planningRoutes[j]->genes[i-1].B;
				x2=planningRoutes[j]->genes[i].L;
				y2=planningRoutes[j]->genes[i].B;
				x3=planningRoutes[j]->genes[i+1].L;
				y3=planningRoutes[j]->genes[i+1].B;

				a=distanc_GL(planningRoutes[j]->genes[i+1],planningRoutes[j]->genes[i]);
				b=distanc_GL(planningRoutes[j]->genes[i-1],planningRoutes[j]->genes[i+1]);
				c=distanc_GL(planningRoutes[j]->genes[i-1],planningRoutes[j]->genes[i]);

				//平滑到所构成三角形的形心
				planningRoutes[j]->genes[i].L=(int)((a*x1+b*x2+c*x3)/(a+b+c));
				planningRoutes[j]->genes[i].B=(int)((a*y1+b*y2+c*y3)/(a+b+c));

				if(PLAN_IsInForbid(planningRoutes[j]->genes[i].L,planningRoutes[j]->genes[i].B))
				{
					continue;
				}

				angle = calAngle_GL(&planningRoutes[j]->genes[i-1],&planningRoutes[j]->genes[i],&planningRoutes[j]->genes[i+1]);
			}

			//对有可能改变引导点按X方向排列顺序的操作，切记要对操作后的航迹进行一次这样的排序
			orderByX_GL(planningRoutes[j]);

			//有用引导点加入序列
			if(!GetZofPoint(temp.X,temp.Y,temp.Z, 0, G_hGctDataFilemap))
			{
				continue;
			}
			temp.X = (planningRoutes[j]->genes[i]).L;
			temp.Y = (planningRoutes[j]->genes[i]).B;
			temp.PointType = 2;   //表示此点为导航点
			Markers.push_back(temp);
		}
		Markers.push_back(targetPoint);   //目标点一定是引导点


		//将规划结果写入文件
		double L,B,Z;
		char buf[64];
		sprintf(buf,"WholePlan\\路径_%d.dat",n+1);   
		fstream ioFile;
		ioFile.open(buf,ios::out);
		ioFile<<"整体规划结果"<<endl;
		while(!Markers.empty())
		{
			temp = Markers.front();
			Markers.pop_front();
			IJtoLB(temp.X,temp.Y,G_RESOLVE,L,B);
			Z = (double)temp.Z;
			ioFile<<L<<" "<<B<<" "<<Z<<endl;
		}

		ioFile.close();  //一定要关闭文件
	}
}
