/************************************************************************/
/* A*�滮���߳�                                                                     */
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

	//�������
	int errcode;
	QString fileName;
	bool IsGuidePlan;

public:
	void run();
};

