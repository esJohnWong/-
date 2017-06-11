/************************************************************************/
/* 数据转换线程                                                                     */
/************************************************************************/
#pragma once
#include <QtGui>
#include "DataConvertVersion2.h"

class DataConvertThread :
	public QThread
{
	Q_OBJECT
public:
	DataConvertThread(void);
	~DataConvertThread(void);

	QString fileNameOrg;
	QString fileNameNew;
	int errcode;
	CDataConvertVersion2 dataConvert;

public:
	void run();
};

