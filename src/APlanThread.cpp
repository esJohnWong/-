#include "APlanThread.h"
#include "DataStruct.h"
#include "APlanning.h"
#include "XmlParse.h"

APlanThread::APlanThread(void)
{
	errcode = 0;
	IsGuidePlan = false;
}


APlanThread::~APlanThread(void)
{
}

void APlanThread::run()
{ 
	//将线程结束标志置为假
	isEndThread = false;

	//读取起点和终点
	vector<GroundPoint>LeaderVector;
	vector<GroundPoint>GuidePointVector;
	XmlParse xml;
	GroundPoint startPoint,endPoint;
	double minlength,maxlength,maxhang,maxvang;
	char path[200]; 
	sprintf(path,"%s",fileName.toStdString().c_str());
	xml.readETPoint(path,startPoint,endPoint);
	xml.readParam(path,minlength,maxlength,maxhang,maxvang);
	if(IsGuidePlan)
	{
		xml.readGuidePointForPlan(path,GuidePointVector);
		vector<GroundPoint>::iterator last,beg,end;  // 设置迭代器 
		LeaderVector.push_back(startPoint);  //压入起点
		last = LeaderVector.end();
		beg = GuidePointVector.begin();
		end = GuidePointVector.end();
		LeaderVector.insert(last,beg,end);   //插入引导点
		LeaderVector.push_back(endPoint);    //压入终点
	}
	else   //直接用A*规划起点和终点
	{
		LeaderVector.push_back(startPoint);
	    LeaderVector.push_back(endPoint);
	}

	
	APlanning m_plan;
	m_plan.getTask(LeaderVector);
	m_plan.getParaminput(minlength,maxlength,maxhang,maxvang);
	if(!m_plan.run())
	{
		errcode =4;
		return ;
	}

	vector<GroundPoint>ResultVector;
	if(!m_plan.OutPutData(ResultVector))
	{
		errcode =4;
		return ;
	}
	xml.writeResultToXml(path,ResultVector);
}