/************************************************************************/
/* ȫ�ֹ滮���߳�                                                                     */
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

	//�������
	int errcode;
	QString fileName;

public:
	void run();
};

