#include "ComMapThread.h"
#include "CreateComMap.h"
#include "StandardizeConstraint.h"
#include "XmlParse.h"
#include "APlanning.h"

ComMapThread::ComMapThread(void)
{
	errcode = 0;//�ɹ�
}


ComMapThread::~ComMapThread(void)
{
}

void ComMapThread::run()
{
	//���߳̽�����־��Ϊ��
	isEndThread = false;
// 	CreateComMap comMap;
// 	if (!comMap.Create(
// 		(TCHAR *)fileName.toStdWString().c_str() , 
// 		G_hZhtMapping , 
// 		G_HEIGHT ,
// 		G_WIDTH ,
// 		forbidAndThreatVector))
// 	{
// 		errcode = 2;
// 		return;
// 	}


	APlanning apl;
	if(!apl.CreateComMap((TCHAR *)fileName.toStdWString().c_str() , forbidAndThreatVector))
	{
		errcode = 2;
		return;
	}

	//д������
	vector<GroundPoint> guidPoint;
	//ת������
	StandardizeConstraint stdcons;
	if(!stdcons.GuidOrResConvertToLBCoordinate(vctGuidPoint , guidPoint))
	{
		errcode = 2;
		return;
	}

	XmlParse xmlGuid;//д��xml�ļ�����
	if(!xmlGuid.writeGuidePointToXml((char *)XmlFileName.toStdString().c_str(), guidPoint))
	{
		//errcode = 2;
		return;
	}
}
