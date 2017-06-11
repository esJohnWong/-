
#include "GL_GolbalPlan.h"


GL_GolbalPlan::GL_GolbalPlan()
{
	srand((int)time(0));   //�漴���ӣ������ظ���ͬ�漴��
	this->SEED=5;          //ÿ���������������
	this->num=0;             //��¼���·��������
}


GL_GolbalPlan::~GL_GolbalPlan()
{

}


//ȫ�ֹ滮��ʵ�ֹ�����ں���
bool GL_GolbalPlan::run()
{
	GetTransInfo_GL();  //����滮����
	initialize_GL();    //��ʼ��
	if(2 == SEED)   //���������������Ŀ�����ýϽ���ֱ�ӽ��ɾֲ��滮���� 
	{
		OutPutData_GL();
		return true; 
	}

	evaluate_GL();      //�������к���������������
	
	//��ȡĩλ��̭�ķ�������ѡ�����,ֱ����̭����ĩ��λ���壬����ȫ��������һ��
	int generationc=0;     //�Ŵ��㷨�Ĵ���
	int Maxiter=MAXGEN;    //��������
	int breakCount=0;      //����ѭ����ֹ�����ж�
	double lastCost=MAXNUM;
	num = 0;
	
	while(generationc<=Maxiter)
	{
		//������߳̽����򲻽��м���
		if (isEndThread)
		{
			break;
		}

		//�������ӣ��ڱ�ѡ����һ���ĸ������漴��ѡ���������������н���
		//�������ֱ���滻����̭��λ�ã������������ۣ�������Բ�ȡ���ַ�ʽ
		crossover_GL();

		//�������ӣ���һ��������ѡ�����������б��죬��������ԭ�����壬ä���첻�����Ÿ�����ʩ�У���0����
		//����������Ը��������������ۣ����췽ʽ���Ը�������֪ʶ������
		mutate_GL(); 

		//���������¶�Ⱥ�尴��������
		reOrder_GL();

		//������ֹ�������ж�:�������Ϊ��;�ҳ���Maxiter/10;������ֹ;
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
	//.................................�Ŵ��㷨����...............................
	
	
	OutPutData_GL();  //�������

	//�ͷ�����Ķ�̬�ڴ�
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


//���ݸ����ķ���㣬Ŀ��������滮��ص�������Ϣ
void GL_GolbalPlan::GetTransInfo_GL()
{
	//����滮�������
	int leftup_x,leftup_y,rightdown_x,rightdown_y;
	leftup_x= min(emitPoint.X,targetPoint.X) - 20;   //����һ����������ֹ�滮���
	leftup_y= min(emitPoint.Y,targetPoint.Y) - 20;
	rightdown_x = max(emitPoint.X, targetPoint.X) + 20;
	rightdown_y = max(emitPoint.Y, targetPoint.Y) + 20;

	transinfo.leftupX = leftup_x;
	transinfo.leftupY = leftup_y;
	transinfo.width = rightdown_x -leftup_x;
	transinfo.height = rightdown_y- leftup_y;
}


//��ɶԳ�ʼ�ļ����������еĳ�ʼ����
//��ʼ��Ϊ�漴����N�������㣨�����ڹ滮�����X����������
//����NΪ:����㵽Ŀ������/L������LΪһ���鳤�ȣ�
void GL_GolbalPlan::initialize_GL()
{
	//ȷ���������еĳ���
	double ETDitance=PLAN_Distance(&emitPoint,&targetPoint);
	if(ETDitance<=400) //�滮����С��400����
	{
		//ֱ�ӽ����ֲ��滮������Ŀ���
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

	//��ʼ����Ⱥ
	for(int i=0;i<NOOFROUTES;i++)
	{   
		//Ϊ������������洢�ռ�
		planningRoutes[i]=(genoType *)malloc(sizeof(genoType));//ÿ����������SEED��������,���ó�ָ��һ�������ṹָ�룬���������Ľ���
		planningRoutes[i]->genes=(EarthPoint *)malloc((SEED)*sizeof(EarthPoint));
		for(int j=0;j<SEED;j++)
		{
			planningRoutes[i]->genes[j].L=0;      //�����ֵ0
			planningRoutes[i]->genes[j].B=0;      //�����ֵ0
		}
	}


	int interval = transinfo.width/(SEED-1);  //��ʼ���ӵļ��
	int Width = transinfo.width;              //�滮���ο�Ŀ��
	int Height = transinfo.height;            //�滮���ο�ĸ߶�
    int rx = transinfo.leftupX;
	int ry = transinfo.leftupY;

	//������һ����Ⱥ
	for(int i=0;i<NOOFROUTES;i++)
	{
		//��ÿ����������㸳ͬ��ֵ
		planningRoutes[i]->genes[0].L = emitPoint.X;
		planningRoutes[i]->genes[0].B = emitPoint.Y;

		for(int j=1;j<SEED-1;j++)
		{
			//����һ��interval�����漴��
			if(emitPoint.X < targetPoint.X)
			{
				planningRoutes[i]->genes[j].L=rx+interval*(j-1)+rand()%interval;
				planningRoutes[i]->genes[j].B=ry+rand()%Height;
			
				//����ڽ������������������
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

				//����ڽ������������������
				while(PLAN_IsInForbid(planningRoutes[i]->genes[j].L,planningRoutes[i]->genes[j].B))
				{
					planningRoutes[i]->genes[j].L=emitPoint.X-interval*(j-1)-rand()%interval;
					planningRoutes[i]->genes[j].B=ry+rand()%Height;
				}
			}
		}

		//��ÿ������Ŀ��㸳ͬ��ֵ
		planningRoutes[i]->genes[SEED-1].L=targetPoint.X;
		planningRoutes[i]->genes[SEED-1].B=targetPoint.Y;
		planningRoutes[i]->totalcost=0.0;
		orderByX_GL(planningRoutes[i]);   //��X��������

	} //end for
}


// ����Ƕ�
double GL_GolbalPlan::calAngle_GL(EarthPoint * frontNode,EarthPoint* centreNode,EarthPoint* lastNode)//ֱ����õ�ǰ��нǵĻ���
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

 
//�������������
void GL_GolbalPlan::evaluateCostOfARuote_GL(genoType *planningRoute)
{
	double cost= 0.0;
	double dis=0.0;
	NavigationNode prev,next,temp;
	
	//��ͷ��β����һ����������εĴ���ֵ,ֻ�����Ӳ�����ĺ�������
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

	//�յ�Ƕȴ���
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

       
//�����۶�ÿ�����������ܴ��ۣ����մ���ֵ�Ժ�����Ⱥ���ӵ͵�������
void GL_GolbalPlan::evaluate_GL()
{
	for(int i=0;i<NOOFROUTES;i++)
	{
		evaluateCostOfARuote_GL(planningRoutes[i]);
	}

	reOrder_GL();  //�Ժ���������ð������
}

      
//�Ժ�����X����ð���Ÿ���
void GL_GolbalPlan::orderByX_GL(genoType *Track)
{
	//����Ժ�����X����ð���Ÿ������������û�лָ�������б�Ҫ������뿼����ŵ�������
	for(int j=0;j<SEED-2;j++)//����ֻ��0~SEED-2�Ľڵ�����.��ǰSEED-1����X������
	{
		for(int k=0;k<SEED-2-j;k++)//ÿ�ֽ���n��1��j �αȽϣ����n��1��j �ν���
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


//�򵥵Ĵ��м�һ�㽻��
void GL_GolbalPlan::simpleCrossover_GL(genoType *TrackOne,genoType *TrackTwo,genoType *LastOne,genoType *LastTwo)
{
	int crossPoint=0;
	crossPoint=rand()%SEED;
	for(int i=0;i<SEED;i++)
	{
		//ʵ�ִ��м�ĳ��Ľ���
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

	//���п��ܸı������㰴X��������˳��Ĳ������м�Ҫ�Բ�����ĺ�������һ������������
	orderByX_GL(LastOne);
	orderByX_GL(LastTwo);
}

    
//ִ�ж��м�ĳһ���漰����Ľ������
void GL_GolbalPlan::segmentCrossover_GL(genoType *TrackOne,genoType *TrackTwo,genoType *LastOne,genoType *LastTwo)
{ 
	int crossPoint=0;
	crossPoint=rand()%(SEED-1);

	for(int i=0;i<SEED;i++)
	{
		//ʵ�ִ��м�ĳ�εĽ���
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

	//���п��ܸı������㰴X��������˳��Ĳ������м�Ҫ�Բ�����ĺ�������һ������������
	orderByX_GL(LastOne);
	orderByX_GL(LastTwo);
}

   
//��������
void GL_GolbalPlan::crossover_GL()
{
	double ran_P = 0.0;  //0~1�����P
	ran_P = rand()/(double)(RAND_MAX);

	//���������ѡ����������������н������,�������������漴ѡһ����ż�����漴ѡһ��
	//����ס����ǰNOOFROUTES-2����������ѡ
	int Num1=rand()%((NOOFROUTES-2)/2);
	Num1=2*Num1;  //żλ����һ��

	int Num2=rand()%((NOOFROUTES-2)/2);
	Num2=2*Num2+1;  //��λ����һ��

	if(ran_P>0&&ran_P<=0.12)
	{
		simpleCrossover_GL(planningRoutes[Num1],planningRoutes[Num2],planningRoutes[NOOFROUTES-1],planningRoutes[NOOFROUTES-2]);
	}
	else if (ran_P>0.12&&ran_P<=0.15)
	{	
		segmentCrossover_GL(planningRoutes[Num1],planningRoutes[Num2],planningRoutes[NOOFROUTES-1],planningRoutes[NOOFROUTES-2]);
	}
	
	if(ran_P>0&&ran_P<=0.45) //�Ӵ�������
	{
		//�������۽����ĸ���
		evaluateCostOfARuote_GL(planningRoutes[NOOFROUTES-2]);
		evaluateCostOfARuote_GL(planningRoutes[NOOFROUTES-1]);
	}
}


//��һ��Բ������ʵ���漴���Ŷ�
void GL_GolbalPlan::disturbWithinACircle_GL(EarthPoint *centrePoint,int r)
{
	double ran_P=0.0;//0~1�����P
	ran_P = rand() / (double)(RAND_MAX);
	double angle = ran_P*360;   //�����ת��
	ran_P = rand() / (double)(RAND_MAX);
	double Radius= ran_P*r*0.3;	//����뾶
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

//��������
void GL_GolbalPlan::distrub_GL(genoType *Track,int i)
{
	int width=transinfo.width;    //���
	int length=transinfo.height;  //�߶�
	int distrubPoint=0;                //ʩ���Ŷ���λ��
	while(distrubPoint==0||distrubPoint==SEED-1)//Ҫע�⣬�������Ŀ����ǲ��������Ŷ��ģ�����������漴һ��
	{
		distrubPoint=rand()%(SEED-1);
	}

	//����һ����ΧԲ���Ŷ�
	//������һ��ǰ��������������εľ��룬ȡ��С��һ����Ϊ�Ŷ��뾶
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

	//��һ��Բ������ʵ���漴���Ŷ�
	disturbWithinACircle_GL(&Track->genes[distrubPoint],r);

	//���п��ܸı������㰴X��������˳��Ĳ������м�Ҫ�Բ�����ĺ�������һ������������
	orderByX_GL(Track);
}

 
//ƽ������
void GL_GolbalPlan::smooth_GL(genoType *Track)
{
	//�漴��һ�����ƽ������
	int distrubPoint=0;

	while(distrubPoint==0)//Ҫע�⣬�������Ŀ����ǲ��������Ŷ��ģ�����������漴һ��
	{
		distrubPoint=rand()%(SEED-1);
	}

	//��������������ε�����ԲԲ������ƽ������
	double x1,y1,x2,y2,x3,y3;
	double a,b,c;
	x1=Track->genes[distrubPoint-1].L;y1=Track->genes[distrubPoint-1].B;
	x2=Track->genes[distrubPoint].L;y2=Track->genes[distrubPoint].B;
	x3=Track->genes[distrubPoint+1].L;y3=Track->genes[distrubPoint+1].B;

	a=distanc_GL(Track->genes[distrubPoint+1],Track->genes[distrubPoint]);
	b=distanc_GL(Track->genes[distrubPoint-1],Track->genes[distrubPoint+1]);
	c=distanc_GL(Track->genes[distrubPoint-1],Track->genes[distrubPoint]);

	//ƽ���������������ε�����
	Track->genes[distrubPoint].L=(int)((a*x1+b*x2+c*x3)/(a+b+c));
	Track->genes[distrubPoint].B=(int)((a*y1+b*y2+c*y3)/(a+b+c));

	//���п��ܸı������㰴X��������˳��Ĳ������м�Ҫ�Բ�����ĺ�������һ������������
	orderByX_GL(Track);
}

     
//���������
double GL_GolbalPlan::distanc_GL(EarthPoint point1,EarthPoint point2)
{
	double distance=0;
	distance=(point1.L-point2.L)*(point1.L-point2.L)+(point1.B-point2.B)*(point1.B-point2.B);
	distance=sqrt(distance);
	return distance;
}

  
//������Ϊһ��ֱ���ϵĸ�������ֱ��
bool GL_GolbalPlan::applyProjection_GL(int beginPoint,int endPoint,genoType *Track)//����ֱ��
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


//�������ӣ�
void GL_GolbalPlan::stretch_GL(genoType *Track)
{
	int beginPoint=0;//������촦��Ŀ�ʼ
	int endPoint=0;//������촦�����ĩ
	int counter=0;
	int tag=0;//�Ƿ��������ı�Ҫ��ֻ�ҵ�һ��������������,�Ѿ�������Ľ���ⷢ����ƽ�ǣ��ᱻ��������

	for(int i=1;i<SEED-1;i++)//�������Ŀ�����û�нǶȵģ����м���Ⲣ�������췶Χ
	{	
		double angleOfI=fabs(calAngle_GL(&Track->genes[i],&Track->genes[i],&Track->genes[i])-PI);
		if(0.0001<angleOfI&&angleOfI<=0.05)//�ӽ�ƽ�ǵ����ȷ�Χ��
		{

			if(tag==0)//��һ��������ƽ�ǡ�
			{tag=1;
			beginPoint=i;
			endPoint=i;
			}
			else if(tag==1)//�Ѿ����ǵ�һ����
			{
				endPoint=i;
			}
		}
		else
		{//�������ǡ�ƽ�ǡ����������:
			if(tag==0) continue;//��û���ҵ�����ξ�ֱ��������
			else//�����������öν���λ
			{
				tag=2;
				break;//��������εĲ���
			}

		}
	}//end for

	if(tag!=0)//��������ı�Ҫ
	{
		applyProjection_GL(beginPoint,endPoint,Track);//����ֱ��
	}

	orderByX_GL(Track);
}


//����������ܵ��ȷ���
void GL_GolbalPlan::mutate_GL()
{
	double ran_P=0.0;//0~1�����P
	int count=rand()%3;
	while(--count!=-1)
	{
		ran_P = rand()/(double)(RAND_MAX);
	}

	//���������ѡ��������������б������,�������������漴ѡһ����ż�����漴ѡһ��
	//����ס����ǰNOOFROUTES-2����������ѡ
	count=rand()%3;
	int Num1;
	Num1=rand()%((NOOFROUTES-2)/2);
	if(Num1==0)
	{
		Num1++;
	}
	Num1=2*Num1; //żλ����һ��

	count=rand()%3;
	int Num2=rand()%((NOOFROUTES-2)/2);
	Num2=2*Num2+1; //��λ����һ��

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
	
	//�������۱����ĸ���
	evaluateCostOfARuote_GL(planningRoutes[Num1]);
	evaluateCostOfARuote_GL(planningRoutes[Num2]);
}

      
//����Ⱥ��������ð������
void GL_GolbalPlan::reOrder_GL()
{
	
	for(int j=0;j<NOOFROUTES-1;j++)
	{
		for(int k=0;k<NOOFROUTES-1-j;k++)//ÿ�ֽ���n��1��j �αȽϣ����n��1��j �ν���
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


//���뷢���Ŀ�����Ϣ���ж������Ƿ�Ϸ�
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

	emitPoint.PointType=1;     //��ʾ�˵�����Ϊ���
	targetPoint.PointType = 3;  //��ʾ�˵�����ΪĿ���
	
	emitPoint.angle = PLAN_LinkAngle(emitPoint.X,emitPoint.Y,targetPoint.X,targetPoint.Y);
	targetPoint.angle = emitPoint.angle;

	// �жϵ�ǰ����ķ�����Ŀ����Ƿ�Ϸ�
	if (!PLAN_CheckEmitTarPoint(&emitPoint, &targetPoint) || 
		!IsInAreaofPoint(emitPoint.X, emitPoint.Y) ||
		!IsInAreaofPoint(targetPoint.X, targetPoint.Y)) 
	{
		errStr = GA_loadTaskErr;
		return false;
	}

	return true;
}

       
//�滮������������(�ɸ�����Ҫ���������Ϻõ����������У�
void GL_GolbalPlan::OutPutData_GL()
{
	NavigationNode temp = DefNavigationNode;  //������
	RouteList Markers;   //һ��·������

	//ȡ���е�10����Ϊ�ο�
	for(int n = 0; n*4<NOOFROUTES; n++)
	{
		int j = n*4;

		//�����һ����������
		Markers.push_back(emitPoint);

		//������ȥ�ظ�
		for(int i=1;i<SEED-1;i++)
		{
			//�ڽ�������ȥ��
			if(PLAN_IsInForbid(planningRoutes[j]->genes[i].L,planningRoutes[j]->genes[i].B))
			{
				continue;
			}

			//������ǽǵ�,�Ϳ���ȥ��
			NavigationNode prev,next;
			double dis = 0.0;
			prev.X = planningRoutes[j]->genes[i-1].L;
			prev.Y = planningRoutes[j]->genes[i-1].B;
			next.X = planningRoutes[j]->genes[i].L;
			next.Y = planningRoutes[j]->genes[i].B;
			dis = PLAN_Distance(&prev,&next);
			if(dis < maxl)
			{
				continue; //ȥ��
			}

			prev.X = planningRoutes[j]->genes[i].L;
			prev.Y = planningRoutes[j]->genes[i].B;
			next.X = planningRoutes[j]->genes[i+1].L;
			next.Y = planningRoutes[j]->genes[i+1].B;
			dis = PLAN_Distance(&prev,&next);
			if(dis < maxl)
			{
				continue; //ȥ��
			}

			//ֱ����õ�ǰ��нǵĻ���
			double angle = calAngle_GL(&planningRoutes[j]->genes[i-1],&planningRoutes[j]->genes[i],&planningRoutes[j]->genes[i+1]);
			if(angle == PI)
			{
				continue;//ȥ��
			}

			//ƽ���������ھֲ��滮
			while(angle < PI*2/3)
			{
				//��������������ε�����ԲԲ������ƽ������
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

				//ƽ���������������ε�����
				planningRoutes[j]->genes[i].L=(int)((a*x1+b*x2+c*x3)/(a+b+c));
				planningRoutes[j]->genes[i].B=(int)((a*y1+b*y2+c*y3)/(a+b+c));

				if(PLAN_IsInForbid(planningRoutes[j]->genes[i].L,planningRoutes[j]->genes[i].B))
				{
					continue;
				}

				angle = calAngle_GL(&planningRoutes[j]->genes[i-1],&planningRoutes[j]->genes[i],&planningRoutes[j]->genes[i+1]);
			}

			//���п��ܸı������㰴X��������˳��Ĳ������м�Ҫ�Բ�����ĺ�������һ������������
			orderByX_GL(planningRoutes[j]);

			//�����������������
			if(!GetZofPoint(temp.X,temp.Y,temp.Z, 0, G_hGctDataFilemap))
			{
				continue;
			}
			temp.X = (planningRoutes[j]->genes[i]).L;
			temp.Y = (planningRoutes[j]->genes[i]).B;
			temp.PointType = 2;   //��ʾ�˵�Ϊ������
			Markers.push_back(temp);
		}
		Markers.push_back(targetPoint);   //Ŀ���һ����������


		//���滮���д���ļ�
		double L,B,Z;
		char buf[64];
		sprintf(buf,"WholePlan\\·��_%d.dat",n+1);   
		fstream ioFile;
		ioFile.open(buf,ios::out);
		ioFile<<"����滮���"<<endl;
		while(!Markers.empty())
		{
			temp = Markers.front();
			Markers.pop_front();
			IJtoLB(temp.X,temp.Y,G_RESOLVE,L,B);
			Z = (double)temp.Z;
			ioFile<<L<<" "<<B<<" "<<Z<<endl;
		}

		ioFile.close();  //һ��Ҫ�ر��ļ�
	}
}
