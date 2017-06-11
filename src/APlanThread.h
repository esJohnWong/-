/************************************************************************/
/* A*规划用线程                                                                     */
/************************************************************************/
#pragma once
#include <QtGui>
class APlanThread :
	public QThread
{
	Q_OBJECT
public:
	APlanThread(void);
	~APlanThread(void);

	//错误代码
	int errcode;
	QString fileName;
	bool IsGuidePlan;

public:
	void run();
};

