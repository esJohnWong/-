
#include "APlanning.h"
#include "ReadMapFileOperation.h"
#include "WriteComMapFile.h"
#include "StandardizeConstraint.h"

APlanning::APlanning()
{
	//��Ա�����ĳ�ʼ��
	m_pCurEmit = new NavigationNode;
	*m_pCurEmit = DefNavigationNode;				// ��ǰ��������

	m_pCurTarget = new NavigationNode;   
	*m_pCurTarget = DefNavigationNode;              // ��ǰ�����Ŀ���

	MinCostNode = new NavigationNode;
	*MinCostNode = DefNavigationNode;

	minl = 200/G_RESOLVE;          //���������ڵ�֮�����С����
	maxl = 400/G_RESOLVE;         //���������ڵ�֮���������
	factofthreat = 0.1;    //��в����ϵ��
	ShowList.clear();      //·��չʾ�����ʼ��
}


APlanning::~APlanning()
{
// 	delete m_pCurEmit;
// 	delete m_pCurTarget;
// 	delete MinCostNode;
}


//run()���ú���:
//1. ����չģ�顿PLAN_ExpandNaviNode
//2. �����ۼ��㡿
//3. ����������
// ����:
// 1. Ŀǰ��һ��һ�ĺ�·�滮 ,���ü򻯷��� û��������������������  
// ��ή���ⲿ�ֵĳ�����չ��,��������ĺ�·�滮 ���õĴ����Ƚ���
// 2. �߶ȹ滮Ӧ�ÿ���
bool APlanning::run()
{
	//-----------------------------------------------------------------------
	// ��ʼ��open��closed����open����뷢��㣬close��Ϊ��
	if(!InitAStar())
	{
		errStr = InitErr;
		return false;
	}

// 	fstream ioFile;
// 	ioFile.open("MinCost.dat",ios::out);
// 	ioFile<<"MinCost"<<endl;

	//-----------------------------------------------------------------------
	// A*�㷨�滮ѭ��
	// OPEN��Ϊ�վͼ���ִ�й滮�߳� 
	while(!OpenList.empty())
	{

		//������߳̽����򲻽��м���
		if (isEndThread)
		{
			break;
		}

		// ��ǰ����open���д�����С�ĵ�(open���д�����С�ĵ�������ǰ�棬�Ѿ��ź���)
		MinCostNode = OpenList.front();

 //ioFile<<20*MinCostNode->X<<" "<<20*MinCostNode->Y<<" "<<MinCostNode->Z<<endl;		

		// ��Open���е����˵�
		OpenList.pop_front(); 
	
		//�жϴ˵��Ƿ�ΪĿ��㣨�������滮��ɣ��ɹ��˳���
		if(PlanIsOK(MinCostNode))
		{
			return true;
		}

		//��ǰ���ǵ����㣬�����Ըõ���Ϊ�������
		if(IsTempTar(MinCostNode))
		{
			continue;
		}


		//-----------------------------------------------------------------------
		// ����չģ�顿��ʼ��չ���������Ĵ˸���С���۽ڵ�
		if(!PLAN_ExpandNaviNode(MinCostNode,OpenList,CloseList,m_pCurEmit,m_pCurTarget))
		{
			continue;
		}
	}//end of while

//ioFile.close();

	errStr = ExpErr;
 	return false;  //TODO:���ش������OpenList�ѿգ��滮��ʱ��
}


//��ȡ����
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
			//�жϸ߳�ֵ�Ƿ�Ϸ�
			int Z;
			if(!GetZofPoint(temp.X, temp.Y, Z, 0, G_hGctDataFilemap) || Z < 0)
			{
				continue;
			}

			//�ж��������Ƿ��ڽ�����
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
// �������ܣ�������ǰ�������жϸ������Ƿ�Ϸ�
//           ��ʼ��open��closed����; open����뷢��㣬close��Ϊ��; 
// �����������ǰ������(���������������Ŀ���)
// �����������ǰ�����Open��Close��
bool APlanning::InitAStar()
{
// 	*m_pCurEmit = DefNavigationNode;
// 	*m_pCurTarget = DefNavigationNode;
// 	*MinCostNode = DefNavigationNode;
	factofthreat = 0.1;

	//ȡ����ǰ����������յ�
	if(!TaskList.empty())
	{
		*m_pCurEmit = TaskList.front();
		m_pCurEmit->PointType = 1;   //���øõ�Ϊ���
		m_pCurEmit->pFather = NULL;  //�ÿձ���·����˷
		TaskList.pop_front();

		*m_pCurTarget = TaskList.front();

		//���ҵ�����Ŀ�꣬���治��������
		if(3 == m_pCurTarget->PointType)   
		{
			TaskList.pop_front(); 
		}
	}
	

	//���������Ŀ������ߵĽǶ�
	m_pCurEmit->angle = PLAN_LinkAngle(m_pCurEmit->X, m_pCurEmit->Y,m_pCurTarget->X,m_pCurTarget->Y);
	m_pCurTarget->angle = m_pCurEmit->angle;


	//-----------------------------------------------------------------------
	// ���ܳ��ֵ����������Խ�硢�߶�Υ�������ڽ�����or��в��
	// TODO��������Ȼ�����˶�ȡ���θ߳�ͼ��Zֵ,û�в��ö�ȡ��ϵ���ͼ
	// TODO: �жϵ�ǰ����ķ�����Ŀ���ƽ�������Ƿ�Խ��

	// �жϵ�ǰ����ķ�����Ŀ����Ƿ�Ϸ�
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

	// TODO: �жϵ�ǰ����ķ�����Ŀ����Ƿ��ںϷ��Ľ�����or��в����
	// �������Ŀ��㶼����в����,�������в����ϵ��
	if(IsInThreat(m_pCurEmit,m_pCurTarget))
	{
		factofthreat = 0.001;
	}

	//-------------------------------------------------------------------------
	// �㷨��ʼǰ����������, ���A*�㷨��Ҫ�õ���open��close��
	OpenList.clear();
	CloseList.clear();
	TempList.clear();

	//-------------------------------------------------------------------------
	// ����ǰ����������뵽��ǰ�����OPEN����ע�⣺��������ָ��·������!
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


// ��������:PLAN_PLANEDOK �жϵ�ǰ���Ƿ���Ŀ���
//TODO:�����ǰ����Ŀ������˷�ҳ��滮·��
bool APlanning::PlanIsOK(pNaviNode pCurNode)
{
	//�жϴ˵��Ƿ�ΪĿ���
	//�ڵ����ͣ�0:Ĭ�� 1:���  2:������ 3:Ŀ��㣩
	if(3 == pCurNode->PointType)  
	{	
		pNaviNode p;      //��˷�ҳ��滮·��
		p = pCurNode;
		while(p != NULL)
		{
			TempList.push_front(*p);  //����ǰ�ڵ�ѹ��·��չʾ����
			p = p->pFather;  
		}

		list<NavigationNode>::iterator last,beg,end;  // ���õ����� 
		last = ShowList.end();
		beg = TempList.begin();
		end = TempList.end();
		ShowList.insert(last,beg,end);

		//����OpenList��CloseList�ٷ���
		OpenList.clear();
		CloseList.clear();

		return true;
	}
	return false;
}


//�жϵ�ǰ���Ƿ��ǵ�����
//TODO:�����ǰ���ǵ��������˷�ҳ��滮·��
bool APlanning::IsTempTar(pNaviNode pCurNode)  
{
	//�ڵ����ͣ�0:Ĭ�� 1:���  2:������ 3:Ŀ��㣩
	if(2 == pCurNode->PointType)  
	{								 
		pNaviNode p;      //��˷�ҳ��滮·��
		p = pCurNode;
		while(p != NULL)
		{
			TempList.push_front(*p);  //����ǰ�ڵ�ѹ��·��չʾ����
			p = p->pFather;  
		}

		list<NavigationNode>::iterator last,beg,end;  // ���õ����� 
		last = ShowList.end();
		beg = TempList.begin();
		end = TempList.end();
		ShowList.insert(last,beg,end);
	
		InitAStar();   //���³�ʼ����Ϊ��һ�ι滮��׼��
		return true;
	}
	return false;
}


//���A*�ֲ��滮���
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

//��ȡ���������Ϣ
void APlanning::getParaminput(double &minlength, //���������ڵ�֮�����С����
	double &maxlength,               //���������ڵ�֮���������
	double &maxhang,                 //�����ڵ�֮������ת��Ƕ�(���ȱ�ʾ)
	double &maxvang)                 //�����ڵ�֮�������¶ȣ����ȱ�ʾ��
{
	minl = minlength/G_RESOLVE;
	maxl = maxlength/G_RESOLVE;
	maxhangle = RADIAN(maxhang);
	maxvangle = RADIAN(maxvang);
}

//��ȡ��ͼ��Ϣ
bool APlanning::getMapInfo(LPCWSTR szFileName)            // ȫ�ֹ滮��ͼ�ķֱ��� 
{
	ReadMapFileOperation rmf;
	//��ԭʼ�ļ�

	int iLengthFileName = WideCharToMultiByte(CP_ACP , 0 , szFileName , -1 , 
		NULL , 0 , NULL , NULL);
	char * szFileaNameA = new char[iLengthFileName];
	WideCharToMultiByte(CP_ACP , 0 , szFileName , iLengthFileName , 
		szFileaNameA , iLengthFileName , NULL , NULL);

	if (!rmf.OpenFileByRead(szFileaNameA))
		return false;

	delete []szFileaNameA;

	MapFileHeader mfh;
	//���ļ�ͷ
	if (!rmf.ReadMapFileHeader(mfh))
	{
		return false;
	}

	// add by mj at 10-28
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);
	G_dwGran = sinf.dwAllocationGranularity;    //��ʼ��ϵͳ��������
	G_MAPL0 = mfh.xllcorner;               // ȫ�ֵ�ͼ���Ͻǵľ���
	G_MAPB0 = mfh.yllcorner;               // ȫ�ֵ�ͼ���Ͻǵ�γ��
	G_WIDTH = mfh.nclos;                   // ȫ�ֹ滮��ͼ�Ŀ��
	G_HEIGHT = mfh.nrows; // ȫ�ֹ滮��ͼ�ĸ߶�
	G_RESOLVE = mfh.cellsize; //add by hj 2013.10.31


	if(!CreateGctDataMap(szFileName , G_hGctDataFilemap))
	{
		return false;
	}

	return true;
}

bool APlanning::InitComMap(
	char * szMapFileName ,//���ͼ������
	__int64 i64MapHeightInPixel ,//���ͼ�߶����ص�
	__int64 i64MapWdithInPixel)	//���ͼ��� ���ص�
{
	WriteComMapFile wcmf;
	if (!wcmf.WrtieAllData(szMapFileName , i64MapHeightInPixel , i64MapWdithInPixel))
	{
		return false;
	}
	return true;
}

bool APlanning::CreateComMap(
	TCHAR * szMapFileName , //���ͼ����
	vector<iPolygon> &forbidAndThreatVector)//��������в����Ϣ �����Ѿ�ת��
{
	//��ʼ�����ͼ
	int needlen = WideCharToMultiByte( CP_ACP, 0, szMapFileName, -1, NULL, 0, NULL, NULL );
	PCHAR pAsciiChar = (PCHAR)HeapAlloc(GetProcessHeap() , 0 , needlen);
	WideCharToMultiByte( CP_ACP, 0, szMapFileName , -1, pAsciiChar , needlen, NULL, NULL );

	if(!InitComMap(pAsciiChar , G_HEIGHT , G_WIDTH))
	{
		return false;
	}
	HeapFree(GetProcessHeap() , 0 , pAsciiChar);

	//ӳ���ڴ��ļ�
	if(!CreateGctDataMap(szMapFileName , G_hZhtMapping))
	{
		return false;
	}

	StandardizeConstraint standcos;//ת��������
	//���û��ת�����ڴ�ת��
	// 	for (int i = 0 ; i < forbidAndThreatVector.size() ; i++)
	// 	{
	// 		standcos.Standardize(forbidAndThreatVector[i]);
	// 	}

	//����������в��д�뵽���ͼ
	for(int i = 0 ; i < forbidAndThreatVector.size() ; i++)
	{
		//ӳ��һ����Ӿ��εĴ�С
		__int64 begin = forbidAndThreatVector[i].rectangle.ly;
		int row = forbidAndThreatVector[i].rectangle.height;
		int errOfStart;
		void * pBuf = ReadComMapData(G_hZhtMapping , begin , row , errOfStart);

		short * buf = (short *)((PUCHAR)pBuf + errOfStart);

		if (pBuf == NULL)
		{
			return false;
		}

		//�߽��ж�
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

		//�ж�һ�����Ƿ��ڶ������
		StandardizeConstraint standCons;

		for (int j = 0 ; j < row ; j++)
		{
			int y = j + forbidAndThreatVector[i].rectangle.ly;
			for (int k = tmp ; k < length ; k++)
			{
				iPoint pt;
				pt.x = k;
				pt.y = y;

				//�㲻�ڶ������
				if (!standCons.APointIsInPolygon(pt , forbidAndThreatVector[i]))
				{
					continue;
				}

				short * tmpBuf = buf + j*G_WIDTH + k;
				if (forbidAndThreatVector[i].level == 1 || forbidAndThreatVector[i].level == 2)
				{//������
					*tmpBuf = MAKESHORT(forbidAndThreatVector[i].level , FORBID_FLY_AREA);
				}
				else
				{//��в��
					*tmpBuf = MAKESHORT(forbidAndThreatVector[i].level , THREAT_FLY_AREA);
				}
			}
		}
		FlushViewOfFile(pBuf , 0);//���޸ĵ�����д�����
		UnmapViewOfFile(pBuf);//�ر�ӳ��
	}

	return true;
}