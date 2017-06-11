/************************************************************************/
/* 主界面头文件															*/
/*2013.10.16 11:48														*/
/*黄俊																	*/
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
	//菜单栏 begin
	QMenu * menuDataConvert;					//工程管理
	QMenu * menuConstraintCondition;			//约束条件
	QMenu * menuAStarPlan;					//线路规划
	QMenu * menuDisplay;					//规划效果演示
	QMenu *	menuCreateComMap;					//预处理
	//菜单栏 end

	//工具栏
	QToolBar * toolBar;

	//菜单和工具栏动作 begin
	QAction * actOrgDataToMap;			//数据转换
	QAction * actOpenMapData;			//打开MAP文件
	QAction * actQuit;						//退出系统
	QAction * actBasicAStarPlan;			//基本A*线路规划
	QAction * actGuidAStarPlan;				//引导A*规划
	QAction * actDisplay;					//效果演示
	QAction * actCreatComMap;				//组合图生成
	QAction * actClose;						//关闭工程
	QAction * actGuidPoint;					//画引导点
	QAction * actTask;			//加载约束文件
	//菜单和工具栏动作 end

	//每次内存映射的大小
	int memoryViewSize;

private slots:
		//菜单栏和工具栏槽函数 begin
		void Close();						//关闭工程
		void OrgDataToMap();				//原始数据转换
		void OpenMapData();				//打开map文件
		void BasicAStarPlan();					//基础A*线路规划
		void GuidAStarPlan();					//引导A*规划
		void Display();						//效果演示
		void CreatComMap();					//预处理组合图生成
		void CloseProject();				//关闭工程
		void SetGuidPoint();				//画引导点
		void OpenTask();			//加载约束文件
		//菜单栏和工具栏槽函数 end

private:
	//界面初始化函数 begin
	void CreateActions();					//初始化动作
	void CreateMenus();						//初始化菜单
	void CreateToolBars();					//初始化工具栏
	//界面初始化函数 end

	//图片管理器
	ImageViewer * imgVer;
	
	//图片内存
	ImageBuf imgBuf;
	
protected:
	//系统关闭事件
	void closeEvent(QCloseEvent *event);

public:
	//内存映射视图
	bool MemoryMapView(HANDLE hFileMap , __int64 begin , int size , short * &address);

public:
	//规划线程
	APlanThread * athread;
	

public slots://处理规划线程完成后的通知
	void APlanThreadFinished();

	//数据转换线程完成通知
	void DataConvertThreadFinished();

	//组合图生成完成通知
	void ComMapThreadFinished();

public:
	//规划进度条
	ProgressBarShow *prgresAPlan;

	//数据转换进度条
	ProgressBarShow *prgresDataCvt;

	//数据转换线程
	DataConvertThread * dcThread;

public:
	//组合图生成线程
	ComMapThread *comMapThread;
	ProgressBarShow *prgresComMap;

	//
	QString XmlFileName;

};
#endif // MAINWINDOW_H
