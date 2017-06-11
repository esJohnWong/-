#include "ComMapThread.h"
#include "CreateComMap.h"
#include "StandardizeConstraint.h"
#include "XmlParse.h"
#include "APlanning.h"

ComMapThread::ComMapThread(void)
{
	errcode = 0;//成功
}


ComMapThread::~ComMapThread(void)
{
}

void ComMapThread::run()
{
	//将线程结束标志置为假
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

	//写引导点
	vector<GroundPoint> guidPoint;
	//转换坐标
	StandardizeConstraint stdcons;
	if(!stdcons.GuidOrResConvertToLBCoordinate(vctGuidPoint , guidPoint))
	{
		errcode = 2;
		return;
	}

	XmlParse xmlGuid;//写入xml文件对象
	if(!xmlGuid.writeGuidePointToXml((char *)XmlFileName.toStdString().c_str(), guidPoint))
	{
		//errcode = 2;
		return;
	}
}
