/************************************************************************/
/* 全局规划用线程                                                                     */
/************************************************************************/
#pragma once
#include <QtGui>
class WholePlanThread :
	public QThread
{
	Q_OBJECT
public:
	WholePlanThread(void);
	~WholePlanThread(void);

	//错误代码
	int errcode;
	QString fileName;

public:
	void run();
};

