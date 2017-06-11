#include "WholePlanThread.h"
#include "DataStruct.h"
#include "GL_GolbalPlan.h"

WholePlanThread::WholePlanThread(void)
{
	errcode = 0;//成功
}


WholePlanThread::~WholePlanThread(void)
{
}

void WholePlanThread::run()
{
	//将线程结束标志置为假
	isEndThread = false;

	//读取起点终点链表
	std::list<NavigationNode>ETList;
	
	//从文件读取数据
	fstream ioFile;
	string path = fileName.toStdString();
	NavigationNode temp;
	ioFile.open(path,ios::in | ios::binary);
	char NodeType[64];
	double L,B,Z;
	int cx,cy;
	ioFile>>NodeType;
	if(strcmp(NodeType,"任务文件") != 0 )
	{
		//失败
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
	ioFile.close();  //一定要关闭文件


	//开始整体规划
	GL_GolbalPlan m_WholePlan;
	m_WholePlan.loadTasks_GL(ETList);  //传入任务
	if(!m_WholePlan.run())
	{
		errcode =4;
	}
}
