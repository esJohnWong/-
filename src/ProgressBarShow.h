#pragma once
#include <QtGui>
class ProgressBarShow :
	public QDialog
{
	Q_OBJECT
public:
	ProgressBarShow(QWidget * parent = NULL);
	~ProgressBarShow(void);

	QProgressBar * progressBar; 

	//当前进度条对应的进程
	QThread * thread;//方便通过进度条结束线程
	QLabel * TotalTimeLabel;
	QLCDNumber *Totaltime;

	//计算总时间
	QTime * timeLeave;
	QTimer * timerShow;

	int timeElapse;//已经逝去的时间
protected:
	//关闭事件
	void closeEvent(QCloseEvent *event);

public slots:
	void timeShow();//显示时间

public://重新开启计时器
	void Start();
	void Stop();//停止计时
};

