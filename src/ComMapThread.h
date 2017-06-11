/************************************************************************/
/* 组合图生成线程                                                                     */
/************************************************************************/

#pragma once
#include <QtGui>
#include "DataStruct.h"
class ComMapThread :
	public QThread
{
	Q_OBJECT
public:
	ComMapThread(void);
	~ComMapThread(void);

	QString fileName;
	QString XmlFileName;
	int errcode;
	vector<iPoint> vctGuidPoint;
	QPoint	beginPoint , endPoint;
	vector<iPolygon> forbidAndThreatVector;
public:
	void run();
};

