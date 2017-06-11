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
	//���߳̽�����־��Ϊ��
	isEndThread = false;

	//��ȡ�����յ�
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
		vector<GroundPoint>::iterator last,beg,end;  // ���õ����� 
		LeaderVector.push_back(startPoint);  //ѹ�����
		last = LeaderVector.end();
		beg = GuidePointVector.begin();
		end = GuidePointVector.end();
		LeaderVector.insert(last,beg,end);   //����������
		LeaderVector.push_back(endPoint);    //ѹ���յ�
	}
	else   //ֱ����A*�滮�����յ�
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