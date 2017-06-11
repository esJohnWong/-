#include "WholePlanThread.h"
#include "DataStruct.h"
#include "GL_GolbalPlan.h"

WholePlanThread::WholePlanThread(void)
{
	errcode = 0;//�ɹ�
}


WholePlanThread::~WholePlanThread(void)
{
}

void WholePlanThread::run()
{
	//���߳̽�����־��Ϊ��
	isEndThread = false;

	//��ȡ����յ�����
	std::list<NavigationNode>ETList;
	
	//���ļ���ȡ����
	fstream ioFile;
	string path = fileName.toStdString();
	NavigationNode temp;
	ioFile.open(path,ios::in | ios::binary);
	char NodeType[64];
	double L,B,Z;
	int cx,cy;
	ioFile>>NodeType;
	if(strcmp(NodeType,"�����ļ�") != 0 )
	{
		//ʧ��
		errcode = 1;
		return;
	}
	
	ioFile.getline(NodeType, 64, '\n');
	while(!ioFile.eof())
	{
		ioFile>>L>>B>>Z;
		if(ioFile.eof())
		{
			break;
		}

		LBtoIJ(L,B,G_RESOLVE,cx,cy);
		temp.X = cx;
		temp.Y = cy;
		temp.Z = (int)Z;
		ETList.push_back(temp);
	}
	ioFile.close();  //һ��Ҫ�ر��ļ�


	//��ʼ����滮
	GL_GolbalPlan m_WholePlan;
	m_WholePlan.loadTasks_GL(ETList);  //��������
	if(!m_WholePlan.run())
	{
		errcode =4;
	}
}
