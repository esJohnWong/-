/************************************************************************/
/* ������ͷ�ļ�															*/
/*2013.10.16 11:48														*/
/*�ƿ�																	*/
/************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "imageviewer.h"
#include "imagebuf.h"

//Add by mj at 10-28
#include "DataStruct.h"
#include "APlanning.h"


class ParamInput;
class QAction;
class QMenu;
class QToolBar;
class QMessageBox;
class WholePlanThread;
class APlanThread;
class ProgressBarShow;
class DataConvertThread;
class ComMapThread;

class MainWindow:public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget * parent = NULL);
	~MainWindow();

private:
	//�˵��� begin
	QMenu * menuDataConvert;					//���̹���
	QMenu * menuConstraintCondition;			//Լ������
	QMenu * menuAStarPlan;					//��·�滮
	QMenu * menuDisplay;					//�滮Ч����ʾ
	QMenu *	menuCreateComMap;					//Ԥ����
	//�˵��� end

	//������
	QToolBar * toolBar;

	//�˵��͹��������� begin
	QAction * actOrgDataToMap;			//����ת��
	QAction * actOpenMapData;			//��MAP�ļ�
	QAction * actQuit;						//�˳�ϵͳ
	QAction * actBasicAStarPlan;			//����A*��·�滮
	QAction * actGuidAStarPlan;				//����A*�滮
	QAction * actDisplay;					//Ч����ʾ
	QAction * actCreatComMap;				//���ͼ����
	QAction * actClose;						//�رչ���
	QAction * actGuidPoint;					//��������
	QAction * actTask;			//����Լ���ļ�
	//�˵��͹��������� end

	//ÿ���ڴ�ӳ��Ĵ�С
	int memoryViewSize;

private slots:
		//�˵����͹������ۺ��� begin
		void Close();						//�رչ���
		void OrgDataToMap();				//ԭʼ����ת��
		void OpenMapData();				//��map�ļ�
		void BasicAStarPlan();					//����A*��·�滮
		void GuidAStarPlan();					//����A*�滮
		void Display();						//Ч����ʾ
		void CreatComMap();					//Ԥ�������ͼ����
		void CloseProject();				//�رչ���
		void SetGuidPoint();				//��������
		void OpenTask();			//����Լ���ļ�
		//�˵����͹������ۺ��� end

private:
	//�����ʼ������ begin
	void CreateActions();					//��ʼ������
	void CreateMenus();						//��ʼ���˵�
	void CreateToolBars();					//��ʼ��������
	//�����ʼ������ end

	//ͼƬ������
	ImageViewer * imgVer;
	
	//ͼƬ�ڴ�
	ImageBuf imgBuf;
	
protected:
	//ϵͳ�ر��¼�
	void closeEvent(QCloseEvent *event);

public:
	//�ڴ�ӳ����ͼ
	bool MemoryMapView(HANDLE hFileMap , __int64 begin , int size , short * &address);

public:
	//�滮�߳�
	APlanThread * athread;
	

public slots://����滮�߳���ɺ��֪ͨ
	void APlanThreadFinished();

	//����ת���߳����֪ͨ
	void DataConvertThreadFinished();

	//���ͼ�������֪ͨ
	void ComMapThreadFinished();

public:
	//�滮������
	ProgressBarShow *prgresAPlan;

	//����ת��������
	ProgressBarShow *prgresDataCvt;

	//����ת���߳�
	DataConvertThread * dcThread;

public:
	//���ͼ�����߳�
	ComMapThread *comMapThread;
	ProgressBarShow *prgresComMap;

	//
	QString XmlFileName;

};
#endif // MAINWINDOW_H
