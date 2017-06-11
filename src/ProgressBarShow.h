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

	//��ǰ��������Ӧ�Ľ���
	QThread * thread;//����ͨ�������������߳�
	QLabel * TotalTimeLabel;
	QLCDNumber *Totaltime;

	//������ʱ��
	QTime * timeLeave;
	QTimer * timerShow;

	int timeElapse;//�Ѿ���ȥ��ʱ��
protected:
	//�ر��¼�
	void closeEvent(QCloseEvent *event);

public slots:
	void timeShow();//��ʾʱ��

public://���¿�����ʱ��
	void Start();
	void Stop();//ֹͣ��ʱ
};

