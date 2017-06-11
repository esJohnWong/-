
#include "DataStruct.h"


// ȫ�ֱ���
double G_MAPL0;               // ȫ�ֵ�ͼ���Ͻǵľ���
double G_MAPB0;               // ȫ�ֵ�ͼ���Ͻǵ�γ��
int G_WIDTH;                  // ȫ�ֹ滮��ͼ�Ŀ��
int G_HEIGHT;                 // ȫ�ֹ滮��ͼ�ĸ߶�
DWORD G_dwGran = 65536;       // ϵͳ��������
HANDLE G_hGctDataFilemap;     // ԭʼ�߳����ݵ�ͼ���ڴ��ļ�ӳ����
HANDLE G_hDxppDataFilemap;    // ԭʼ����ƥ�����ݵ�ͼ���ڴ��ļ�ӳ����
__int64 fileSize; //�߳�ͼ�ļ��Ĵ�С

double G_RESOLVE = 20.0;        // ȫ�ֹ滮��ͼ�ķֱ��� 
double factofthreat = 0.1;               //��в����ϵ��
int tt  = 0;
ErrorType errStr = DefError;          //��������

//Լ������  add at 10-30 
double minl = 200;          //���������ڵ�֮�����С����
double maxl = 400;         //���������ڵ�֮���������
double maxhangle = PI/4;      //�����ڵ�֮������ת��Ƕ�(���ȱ�ʾ)
double maxvangle = PI/4;      //�����ڵ�֮�������¶ȣ����ȱ�ʾ��


// ���������ߵĽǶ�(����)
double PLAN_LinkAngle(int L1,int B1,int L2,int B2)
{
	// L1, B1Ϊ��ʼ��,����Ϊ-PI��PI
	double angle = 0.0;    // ������������߽Ƕ�
	double dB = B2-B1;
	double dL = L2-L1;

	// ��ֹ��������
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


//��ⷢ��㡢Ŀ����Ƿ�߶ȺϷ�
bool PLAN_CheckEmitTarPoint(pNaviNode pcurEmit, pNaviNode pcurtarget)
{
	int Z;	// ��ⷢ��� Ŀ����Ƿ�߶ȺϷ�

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

		//        return true;  // ע�⣺���ﲻ�ܷ����� ��ΪĿ��㻹û�м��
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
/* �����߳�ͼ�ļ��ڴ�ӳ�� szFileName Ϊ�ļ�·�� �� hFileMapΪ�������ڴ�ӳ����		*/
/*add by hj 2013.10.30													*/
/************************************************************************/
bool CreateGctDataMap(LPCWSTR szFileName , HANDLE &hMapFile )
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	G_dwGran = sysInfo.dwAllocationGranularity;//�õ�ϵͳ�ڴ��������

	//����Ҫӳ����ļ�
	HANDLE hFile = CreateFile(szFileName , 
		GENERIC_READ | GENERIC_WRITE , 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL ,
		OPEN_EXISTING ,
		FILE_ATTRIBUTE_NORMAL ,
		NULL
		);

	if (INVALID_HANDLE_VALUE == hFile)
	{//������
		return false;
	}

	//����ӳ��
	//if(hMapFile != NULL)
	//{
	   // CloseHandle(hMapFile);//�ȹص���ǰ��
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


// �õ�ĳ��ĸ߳�ֵ��X,YΪȫ��ͼ����ֵ isInt�ж���Ҫ��ȡint�ͻ���shor��
// hFileMapΪ��Ӧ�ڴ�ӳ��ľ��
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
	dataMemoryStart = start/G_dwGran*G_dwGran;//�ڴ�ӳ�����ʵλ��
	int errorOfStart =start - dataMemoryStart ;//�ڴ�ӳ�����ʼ�����ʵ��ʼ������
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
	{// ������
		return false;
	}

	short *tmps = new short;
	void * tmpBuf = (PUCHAR)pVoid+errorOfStart;

	CopyMemory(tmps , tmpBuf , sizeof(short));
	Z = *tmps;

	delete tmps;

	UnmapViewOfFile(pVoid);  // ���볷��ӳ��
	return true;
}


bool LBtoIJ(double L,     // ��ǰ��ľ���      
	double B,     // ��ǰ���γ��
	double Level, // ��ǰʹ�õĵ�ͼ�ֱ��� // �������Ҫ��������Ū��
	int& I,       // �ڸ����ֱ����µ�ȫ������I(����)λ��
	int& J        // �ڸ����ֱ����µ�ȫ������J(����)λ��
	)
{
	// ע��I,J��L,B�Ƕ��������Ͻ�Ϊ��ʼ���
	// ��ע��I,J��L,B�Ķ�Ӧ��ϵ
	// ������ʾ��ʱ����ܻ����Խ�磨��Ҫ��ʾΪ��Ч���ݣ�����Խ��
	// ��ͬ������ת��
	// �����ؽ�����������ȡֵ
	if (Level < 0) 
	{
		return  false;
	}

	I = (int)(L - G_MAPL0)/Level + 0.5;
	J = (int)(G_MAPB0 - B)/Level + 0.5;

	return true;
}


// ����ǰ��ĵ�ǰ�ֱ�����ȫ����������ת���ɾ�γ��
bool IJtoLB(int I,        // �ڸ����ֱ����µ�ȫ������I(����)λ��
	int J,        // �ڸ����ֱ����µ�ȫ������J(����)λ��
	double Level, // ��ǰʹ�õķֱ��� 				
	double& L,    // ��ǰ��ľ���      
	double& B     // ��ǰ���γ��								
	)
{
	// ע��I,J��L,B�Ƕ��������Ͻ�Ϊ��ʼ���
	// ��ע��I,J��L,B�Ķ�Ӧ��ϵ
	// ������ʾ��ʱ����ܻ����Խ�磨��Ҫ��ʾΪ��Ч���ݣ�����Խ��
	// ��ͬ������ת��

	if (Level < 0) 
	{
		return  false;
	}

	L = G_MAPL0 + I*Level;
	B = G_MAPB0 - J*Level;

	return true;
}


// �ж��Ƿ��ڴ��ͼ��
bool IsInAreaofPoint(int x, int y)
{
	if (x<0 || x > G_WIDTH || y<0 || y > G_HEIGHT)
	{
		return false;
	}
	return true;
}


// ��������֮���ƽ�����
double PLAN_Distance(pNaviNode prev, pNaviNode next)
{
	// ע����ʱ�Դ�������µ�ŷʽ�������
	return sqrt((double)(prev->X-next->X)*(prev->X-next->X)
		+ (prev->Y-next->Y)*(prev->Y-next->Y));

}


// �������ռ��֮��ľ���
double PLAN_KJDistance(pNaviNode prev, pNaviNode next)
{
	return sqrt((double)(prev->X - next->X)*(prev->X - next->X)
		+ (prev->Y - next->Y)*(prev->Y - next->Y)
		+ (prev->Z - next->Z)*(prev->Z - next->Z));
}


// �жϵ��Ƿ��ڽ�������
bool PLAN_IsInForbid(int CurNodeX, int CurNodeY)
{
	//-------------------------------------------------------------------------
	//�жϽڵ�CurNode�Ƿ��ڽ�������

	//�ж��Ƿ��ڷ�Χ��
	if( !IsInAreaofPoint(CurNodeX, CurNodeY) )
	{
		//������ڷ�Χ�⣬���ش���
		return false;
	}

	int Z;  //ȡ���õ����Ϣ
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


// �жϵ��Ƿ�����в����
bool PLAN_IsInThreat(int CurNodeX, int CurNodeY)
{

	//-------------------------------------------------------------------------
	//�жϽڵ�CurNode�Ƿ�����в����

	//�ж��Ƿ��ڷ�Χ��(TODO���ж��Ƿ��ڽ�����ʱ�Ѿ�����)
	
	int Z;  //ȡ���õ����Ϣ
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


// �ж�Ŀ����Ƿ�������ƥ��������
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


//  ��չ������
bool PLAN_ExpandNaviNode(pNaviNode pCurNode,    // ��ǰ��չ�ڵ� 
	pRouteList& OpenList,
	std::set<EarthPoint>& CloseList,
	pNaviNode pEmiNode,	    // ��ǰ�����
	pNaviNode pTarNode )	// ��ǰĿ���
{
	SearchAreaParameter SAP;  // ��չ�ķ�Χ����
	PLAN_CalSAP(pCurNode, SAP);
	
	// �ж�Ŀ����Ƿ��ڴ���������
	//TODO: ����ڻ�Ҫ�ж��ܷ�ֱ���뵱ǰ�����ӣ����ж��Ƿ񾭹�������
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
	//���ָ�����Ϊ����������������  
	EarthPoint outputpoint;  // �������ĵ�����
	double UnitAngle ;          // ������Ƕ����ȴ�С
	double UnitDistance ;       // ��������������ȴ�С

	// �����Ȼ�����������
	UnitAngle = 40/G_RESOLVE/minl;
	UnitDistance = 40/G_RESOLVE;

	double CurAngle = SAP.MinAngle;
	double CurRadius = SAP.MinRadius;
	
	//-------------------------------------------------------------------------
	// ����������  
	// ���Ƕ�ѭ�� �ڲ�뾶ѭ�� TODO
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
			// ����������ɹ�����д��ۼ��㡣
			
			//����ƽ�����, �߶ȴ�������չ���жϼ���
			double Cost = PLAN_Cost(pEmiNode,pCurNode,pTempNode, pTarNode);


			// ��䵱ǰ��չ�����ĵ�
			pTempNode->t_cost = Cost;
			pTempNode->pFather = pCurNode;
			pTempNode->PointType = 0;

			//-----------------------------------------------------------------
			// ������չ���������OPEN�����в�����
			PLAN_NaviNodeInsertToOpenList(pTempNode, OpenList,CloseList);

		}   // end for Radius

	}  // end for Angle

	return true;
}


/******************************************************************************
��  ��: ���ۼ���ĺ��ĺ���
����ƽ����۲��жϴ˶�ά�������Ƿ񾭹�������
��  ע: �ֱ��������ۣ�Ȼ������ۼӣ�
******************************************************************************/
double PLAN_Cost(pNaviNode pEmiNode,pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode)
{
	// �����ܴ��۵ĳ�ʼ��
	double cost = 0.0;  

	// ��һ�������� ���ۼ��京�����£�
	// g_cost : SAS��֪����
	// h_cost : SASԤ������
	// f_cost : SASƽ�����
	// t_cost : SAS�ܴ���
	// p_cost : Ԥ��
	// HP_cost: �߶ȴ���
	// ���� t_cost = f_cost + HP_cost = (g_cost+h_cost) + HP_cost
	// �����ɾ��� �Ƿ���в�������������� 



	// ------------------------------------------------------------------------
	// ���� ÿһ��������� �� ����ϵ��
	double cost_dis, factor_dis; //distance
	double cost_thr, factor_thr; //threat and forbid
	double cost_lia, factor_lia; //link-angle
	double cost_for, factor_for; //forbid in future route


	// ------------------------------------------------------------------------
	// ��ʼ�� ÿһ����� �� ����ϵ��
	cost_dis = 0.0;
	cost_thr = 0.0;
	cost_lia = 0.0;
	cost_for = 0.0;

	factor_dis = 2.0;   //factor_dis = 2.0;
	factor_thr = 0.5;   //factor_thr = 0.5;
	factor_lia = 1.0;
	factor_for = 1.0;


	// ------------------------------------------------------------------------
	// ��ÿһ�����

	//-------------------------------------------------------------------------
	// 1 �������
	cost_dis = PLAN_Cost_Dis( pEmiNode,  pCurNode, pTempNode,  pTarNode);

	//-------------------------------------------------------------------------
	// 2 �������������в������, ����ͨ����������Ҫ�ϸ��ų�,��ʸ����ʽ�������������в��    
	cost_thr = PLAN_Cost_Threat_Forbid( pEmiNode,  pCurNode, pTempNode,  pTarNode);  
	

	// ------------------------------------------------------------------------
	// 3��չ��ķ�����õ��Ŀ�����߷���ĽǶȴ���
	cost_lia = PLAN_Cost_LinkAngle( pEmiNode,  pCurNode, pTempNode,  pTarNode);
	if (cost_lia <= -0.5) // cost_lia = -1.000000 ע�⾫�� ��ʾ�Ƕȴ���120
	{
		cost = -1;
		return cost;
	}
	
	//-------------------------------------------------------------------------    
	// 4 ���ƵĽ���������
	double dis_f = PLAN_Distance(pCurNode, pTarNode);	//Distance in Future
	double Angle_CT = PLAN_LinkAngle(pCurNode->X,pCurNode->Y, pTarNode->X,pTarNode->Y); 

	//����ȡ��
	int STEP = 60/G_RESOLVE;
	int nn = (int) (dis_f/STEP);
	int n_counter = 0;
	EarthPoint outputPoint = DefEarthPoint;        // ���߲�����
	EarthPoint outputPoint1 = DefEarthPoint;        // ���߲�����

	//��ǰ�㵽Ŀ������߲���
	for (int ii = 1; ii <= nn; ii++)
	{
		// �����ǰ�����������
		if (!PLAN_NextNode(pCurNode->X, pCurNode->Y, outputPoint, ii*STEP, Angle_CT))
		{
			break;
		}

		//�жϲ������Ƿ��ڽ�����
		if (PLAN_IsInForbid(outputPoint.L, outputPoint.B))
		{
			n_counter++;
		}
	}

	
	//��ǰ���ӳ��߲���
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
	// ����ܴ��۵���ͼ���

	cost = factor_dis*cost_dis 
		+ factor_thr*cost_thr 
		+ factor_lia*cost_lia 
		+ factor_for*cost_for;

	return cost;
}


//����ֵ�� -1�������˽�������0.5��δ������������δ������в����������ֵ��ͨ����в���Ĵ���ֵ
double PLAN_Cost_Threat_Forbid(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode)
{

	//-------------------------------------------------------------------------
	// �������������в������, ����ͨ����������Ҫ�ϸ��ų�,��ʸ����ʽ�������������в��
	double cost_t_f = 0.0;

	if (PLAN_IsInThreat(pCurNode->X, pCurNode->Y) ||
		PLAN_IsInThreat(pTempNode->X, pTempNode->Y))
	{
		double TotalLength = PLAN_Distance(pCurNode, pTempNode);
		int STEP = 60/G_RESOLVE;
		int nn = (int)(TotalLength/STEP);   // �ֳ�nn��,ǿ�Ƴ�int��
		EarthPoint outputPoint  = DefEarthPoint;  // �����������ƽ������

		// ������Ϊ�˼�����в�������
		for (int ii = 1; ii <= nn; ii++)
		{
			// ������ǰ�����������
			if (PLAN_NextNode(pCurNode->X,pCurNode->Y,outputPoint,ii*STEP,pTempNode->angle))
			{
				// �жϵ�ǰ�������ϲ����㾭����в���Ĵ���
				double TD = 0.0;  // ��ǰ�������뵱ǰ��в�����ĵ����(���ؾ���)
				int level = 1;    // ��ǰ��в������в�ȼ�

				if (PLAN_CrossThreatenIJ(outputPoint.L, outputPoint.B, TD,level))  
				{                
					cost_t_f += level*ThreatFactor*factofthreat;  // TODO: here level = threatArea[circleInt].level; 
				}  
			}   // end of if PLAN_NextNode

		} // end of for (����)
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
	// ��չ����Ŀ���֮��ĽǶȴ���

	// ��ǰ��չ�� �� Ŀ������߽Ƕ�
	double Angle_Link = PLAN_LinkAngle(pTempNode->X,pTempNode->Y, pTarNode->X,pTarNode->Y);

	// ��ǰ��չ��Ƕ� �� ��ǰ��ͬĿ�����߽Ƕȵ�ƫ��
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

	// תΪ���Ƕ�
	if (crossAngle<=0.1)
	{
		crossAngle += 2*fabs(crossAngle);
	}

	// �ų�0�ȵ����
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


// ���ۺ�����ÿһ��
double PLAN_Cost_Dis(pNaviNode pEmiNode, pNaviNode pCurNode, pNaviNode pTempNode, pNaviNode pTarNode)
{
	double targetdistance = PLAN_Distance(pTempNode, pTarNode);
	return fabs(targetdistance);
}


// ����ƽ����һ��ֱ���ϵ���һ��,���ھ���϶�
// û�п��Ǵ�������Ӱ��,������ԭ��Ϊ���Ͻ�
bool  PLAN_NextNode(int prevL, int prevB, EarthPoint &next, double distance,double angle)
{
	// ע������ķ����Ƿ��з���(��������Ϊ��׼��˳ʱ��Ϊ������ʱ�븺)
	
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


// ����չ�Ľڵ����OPEN����ͬʱ�������н��д������򣬽�������С�ĵ������ǰ��
void PLAN_NaviNodeInsertToOpenList(pNaviNode pTempNode,pRouteList &OpenList,std::set<EarthPoint>&CloseList)
{

	// �����������openlist
	pRouteList::iterator first,last;  // ���õ����� 
	first = OpenList.begin();
	last = OpenList.end();

	// ����չʱ��������ڵ���Ӧ���Ѿ����¸ı�����
	while (first != last) 
	{
		double t=(*first)->t_cost;
		if( (pTempNode->t_cost) < t)
		{
			OpenList.insert(first, pTempNode);  // ���ҵ����Լ���Ľڵ�ͼ���OPENLIST
			break;
		}
		++first;
	}

	// �Ѿ�ѭ����OPEN�����,�Լ��Ĵ��������ô�ͷ��������
	if(first == last)
	{
		OpenList.push_back(pTempNode);
	}

	EarthPoint tmp;
	tmp.L = pTempNode->X;
	tmp.B = pTempNode->Y;
	CloseList.insert(tmp);
}


// ���㵱ǰ��չ����������
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


//   �ж�����֮���ܷ����ӣ������ӵĻ��ͽ�������
bool PLAN_BuildTrack(pNaviNode pCurNode,pNaviNode NextNode)
{
	int Z1, Z2;   //ȡ�������Ϣ

	//�ж��¶��Ƿ�С��maxvangle
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
	//�ж��Ƿ񾭹�������
	double distance = PLAN_Distance(pCurNode, NextNode);	
	int nn = (int)distance + 0.5;  //����ȡ��
	EarthPoint outputPoint = DefEarthPoint;   // ���߲�����

	for (int ii = 1; ii <= nn; ii+=3)
	{
		// �����ǰ�����������
		if (!PLAN_NextNode(pCurNode->X, pCurNode->Y, outputPoint, ii,NextNode->angle))
		{
			return false;
		}

		//�жϸ߳�ֵ�Ƿ�Ϸ�
		int Z3;
		if(!GetZofPoint(outputPoint.L, outputPoint.B,Z3,0,G_hGctDataFilemap) || Z3 < 0)
		{
			return false;
		}

		//�жϲ������Ƿ��ڽ�����
		if(PLAN_IsInForbid(outputPoint.L, outputPoint.B))
		{
			return false;
		}
	}

	//NextNode->Z = Z2;
	NextNode->HP_cost = dh;  //�Ը߶Ȳ���Ϊ�߶ȴ���

	return true;
}


bool PLAN_CrossThreatenIJ(int I,int J, double& TD, int& level)
{
	int Z;     //ȡ���ĵ����Ϣֵ
	if(!GetZofPoint(I,J,Z,1,G_hZhtMapping))
	{
		return false;
	}
	level = GetThreatType(Z);
	return true;	
}


//���ع滮��������
string GetPlanError()
{
	string str;
	switch(errStr)
	{
	case 0: 
		str = "ϵͳ����";
		break;
	case 1: 
		str = "�ֲ��滮��ȡ����ʧ��";
		break;
	case 2: 
		str = "�ֲ��滮��ʼ��ʧ��";
		break;
	case 3: 
		str = "�ֲ��滮��չ��ʧ��";
		break;
	case 4: 
		str = "����滮ʧ��";
		break;
	}
	return str;
}


//add by hj 2013.11.3 begin----------------------------------
HANDLE G_hZhtMapping = INVALID_HANDLE_VALUE;			//���ͼ�ļ�ӳ����

//add by hj 2013.12.20

//�õ����ͼ�ڴ�ӳ��beginΪ��ʼ���к� rowΪҪӳ������� , errorOfStart Ϊ��Ҫӳ����ڴ���ʼ��ַ
//��ʵ���ڴ�ӳ����ʼ��ַ��ƫ����
void * ReadComMapData(HANDLE hFileMap , __int64 begin , int &row , int &errorOfStart)
{
	//�õ�ÿ�����ݵĿ�� ���ֽڼ�
	int width = G_WIDTH * sizeof(unsigned short);

	//�߽��ж�
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
	errorOfStart =begin*width -  start;//�ڴ�ӳ�����ʼ�����ʵ��ʼ������
	DWORD highOffset = start>>32;
	DWORD lowOffset = start&0xffffffff;

	//
	LPVOID pVoid = MapViewOfFile(hFileMap,
		FILE_MAP_ALL_ACCESS ,
		highOffset , 
		lowOffset , 
		row*width+errorOfStart);

	if (NULL == pVoid)
	{// ������
		return NULL;
	}
	return pVoid;
}
bool isEndThread = false;

//add by hj 2013.11.3 end----------------------------------
