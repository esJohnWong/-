#include "ProgressBarShow.h"
#include "DataStruct.h"

ProgressBarShow::ProgressBarShow(QWidget * parent /*= NULL*/):QDialog(parent)
{
	progressBar = new QProgressBar(this);
	progressBar->setMaximum(0);

	QHBoxLayout * layout1 = new QHBoxLayout;
	layout1->addWidget(progressBar);
	

	//add by hj2013.11.12
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint & ~Qt::WindowContextHelpButtonHint); 


	setFixedSize(300,85); 
	setWindowIcon(QIcon(":/Resources/system.png"));
	
	//
	setStyle(QStyleFactory::create("CDE"));
	setPalette(style()->standardPalette());


	//
	TotalTimeLabel = new QLabel(this);    
	TotalTimeLabel->setText("逝去的时间");
	Totaltime = new QLCDNumber(17,this);  
	Totaltime->setSegmentStyle(QLCDNumber::Flat);

	QHBoxLayout * layout2 = new QHBoxLayout;
	layout2->addWidget(TotalTimeLabel  );
	layout2->addWidget(Totaltime);
	layout2->addStretch();

	QVBoxLayout * Layout = new QVBoxLayout;
	Layout->addLayout(layout1 );
	Layout->addLayout(layout2 );

	setLayout(Layout);
	thread = NULL;

	//
	timeLeave = new QTime;
	timerShow = new QTimer(this);
	connect(timerShow , SIGNAL(timeout()) , SLOT(timeShow()));
}

void ProgressBarShow::Start()
{
	timeLeave->start();
	timerShow->start(1000);
	Totaltime->display(QString("0"));
}

void ProgressBarShow::Stop()
{
	timerShow->stop();
}

void ProgressBarShow::timeShow()
{
	timeElapse = timeLeave->elapsed();//计算从最后一次start到现在的时间毫秒

	//两分钟规划超时结束
// 	if (timeElapse >= 15*60*1000)
// 	{
// 		isEndThread = true;
// 	}

	int Hour = 0 , minute = 0,second=0;

	timeElapse = timeElapse/1000;   // 单位 秒 

	Hour = timeElapse / 3600;

	if( timeElapse % 3600 <60) 
	{
		minute = 0;
		second = timeElapse%3600;
	}
	else 
	{
		minute = timeElapse%3600/60;
		second = timeElapse%3600%60;
	}

	QTime time;
	time.setHMS(Hour,minute,second,0);

	QString timeStr = time.toString("hh:mm:ss");

	Totaltime->display(timeStr);

}

ProgressBarShow::~ProgressBarShow(void)
{
}

void ProgressBarShow::closeEvent(QCloseEvent *event)
{
	if(QMessageBox::No == QMessageBox::question(this , "提示" , "是否确定结束当前线程？" , QMessageBox::Yes|QMessageBox::No))
	{
		event->ignore();
		return;
	}

	if (NULL != thread)
	{
		if (thread->isRunning())
		{//设置线程结束标记
			isEndThread = true;
		}
	}
	
	event->accept();
}

