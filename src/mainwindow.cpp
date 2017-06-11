/************************************************************************/
/* 主界面源文件															*/
/*2013.10.16 11:48														*/
/*黄俊																	*/
/************************************************************************/
#include "mainwindow.h"
#include <QtGui>
#include "Coordinates2MapConverter.h"
#include "ReadMapFileOperation.h"
#include "ShortAllocate.h"
#include <cmath>
#include "paraminput.h"
#include "WriteComMapFile.h"
#include "APlanThread.h"
#include "ProgressBarShow.h"
#include "DataConvertThread.h"
#include "ComMapThread.h"
#include "XmlParse.h"
#include "StandardizeConstraint.h"
#include "CDibBitMap.h"
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	//初始化
	CreateActions();
	CreateMenus();
	CreateToolBars();

	//设置界面标题和图标
	setWindowTitle("电力线路规划演示软件");
	setWindowIcon(QIcon(":/Resources/system.png"));


	//初始化图片显示器
	imgVer = new ImageViewer(this);
	setCentralWidget(imgVer);

	//初始化内存映射大小初始为1400*1400个字节便于调试时观察是否有错误也可增大
	memoryViewSize = 1600*1600;

	//初始化工具栏菜单栏的状态

	//规划线程初始化
	athread = new APlanThread;
	connect(athread , SIGNAL(finished()) , this , SLOT(APlanThreadFinished()));

	//进度条
	prgresAPlan = new ProgressBarShow(this);//A*

	//数据转换线程
	dcThread = new DataConvertThread;
	connect(dcThread , SIGNAL(finished()) , this , SLOT(DataConvertThreadFinished()));
	//数据转换进度条
	prgresDataCvt = new ProgressBarShow(this);

	//组合图生成线程
	comMapThread = new ComMapThread;
	prgresComMap = new ProgressBarShow(this);
	connect(comMapThread , SIGNAL(finished()) , this , SLOT(ComMapThreadFinished()));
}

void MainWindow::ComMapThreadFinished()
{
	prgresComMap->hide();
	prgresComMap->Stop();

	//检查线程结束标记
	if (isEndThread)
	{
		comMapThread->errcode = 3;
		isEndThread = false;
	}

	switch(comMapThread->errcode)
	{
	case 0:
		QMessageBox::information(this , "提示" , "组合图生成成功" , QMessageBox::Yes);
		break;
	case 1:
		QMessageBox::information(this , "提示" , "初始化组合图失败" , QMessageBox::Yes);
		break;
	case 2:
		QMessageBox::information(this , "提示" , "生成组合图失败" , QMessageBox::Yes);
		break;

	case 3:
		QMessageBox::information(this , "提示" , "线程被终止！" , QMessageBox::Yes);
		break;

	default:
		QMessageBox::warning(this,"错误","未知类型的错误!");
	}

	comMapThread->errcode = 0;
}

void MainWindow::DataConvertThreadFinished()
{
	prgresDataCvt->hide();
	prgresDataCvt->Stop();

	//检查线程结束标记
	if (isEndThread)
	{
		dcThread->errcode = 3;
		isEndThread = false;
	}

	switch(dcThread->errcode)
	{
	case 0:
		QMessageBox::information(this , "提示" , "原始数据转换成MAP图成功\n打开此图即可！" , QMessageBox::Yes);
		break;
	case 1:
		QMessageBox::information(this , "提示" , "原始数据转换成MAP图失败！" , QMessageBox::Yes);
		break;

	case 3:
		QMessageBox::information(this , "提示" , "线程被终止！" , QMessageBox::Yes);
		break;

	default:
		QMessageBox::warning(this,"错误","未知类型的错误!");
	}

	dcThread->errcode = 0;
}

void MainWindow::APlanThreadFinished()
{
	//影藏进度条
	prgresAPlan->hide();
	prgresAPlan->Stop();

	//检查线程结束标记
	if (isEndThread)
	{
		athread->errcode = 3;
		isEndThread = false;
	}

	switch(athread->errcode)
	{
	case 0:
		QMessageBox::information(this , "提示"  , "A*局部规划完成");
		break;
	case 1:
		QMessageBox::warning(this,"Warning","请打开一个整体规划结果文件!");
		break;

	case 3:
		QMessageBox::information(this , "提示" , "线程被终止！" , QMessageBox::Yes);
		break;

	case 4:
		QMessageBox::information(this , "提示" , "规划失败！" , QMessageBox::Yes);
		break;

	default:
		QMessageBox::warning(this,"错误","未知类型的错误!");
	}

	athread->errcode = 0;//还原
}


MainWindow::~MainWindow()
{

}

void MainWindow::CreateActions()				
{
	//数据转换
	actOrgDataToMap = new QAction(QIcon(":/Resources/orgtomap.png"), "原始数据转换", this);
	actOrgDataToMap->setShortcut(tr("Ctrl+N"));
	connect(actOrgDataToMap, SIGNAL(triggered()), this, SLOT(OrgDataToMap()));

	//打开工程文件
	actOpenMapData = new QAction(QIcon(":/Resources/openmap.png"), "打开MAP文件",this);                              
	actOpenMapData->setShortcut(tr("Ctrl+O"));
	connect(actOpenMapData, SIGNAL(triggered()), this, SLOT(OpenMapData()));

	//退出系统
	actQuit = new QAction(QIcon(":/Resources/quit.png"), "退出系统", this);
	actQuit->setShortcut(tr("Ctrl+Q"));
	connect(actQuit, SIGNAL(triggered()), this, SLOT(Close()));

	//线路规划
	actBasicAStarPlan = new QAction(QIcon(":/Resources/baseastar.png"),"基本A*规划", this);
	connect(actBasicAStarPlan, SIGNAL(triggered()), this, SLOT(BasicAStarPlan()));

	//效果演示
	actDisplay = new QAction(QIcon(":/Resources/display.png"),"效果演示", this);
	connect(actDisplay, SIGNAL(triggered()), this, SLOT(Display()));

	//组合图生成
	actCreatComMap = new QAction(QIcon(":/Resources/commap.png"),"组合图生成", this);
	connect(actCreatComMap, SIGNAL(triggered()), this, SLOT(CreatComMap()));

	//引导A*规划
	actGuidAStarPlan = new QAction(QIcon(":/Resources/guidastar.png"),"引导A*规划" , this);
	connect(actGuidAStarPlan , SIGNAL(triggered()) , this , SLOT(GuidAStarPlan()));

	//关闭工程
	actClose = new QAction(QIcon(":/Resources/closepoj.png"),"关闭工程" , this);
	connect(actClose , SIGNAL(triggered()) , this , SLOT(CloseProject()));

	//画引导点
	actGuidPoint = new QAction(QIcon(":/Resources/guidpoint.png"),"添加引导点", this);
	connect(actGuidPoint , SIGNAL(triggered()) , this , SLOT(SetGuidPoint()));

	//约束图层
	actTask = new QAction(QIcon(":/Resources/confile.png"),"加载约束图层", this);
	connect(actTask , SIGNAL(triggered()) , this , SLOT(OpenTask()));
}

void MainWindow::GuidAStarPlan()
{
	QString fileName = QFileDialog::getOpenFileName(this,"打开需要规划的任务文件",
		".\\Task",tr("XML Files (*.XML *.xml)"));

	if(fileName.isEmpty())
	{
		return ;
	}

	//changed by hj 2013.11.11
	athread->fileName = fileName;
	athread->IsGuidePlan =true;
	athread->start();//线程开始

	//显示进度条
	prgresAPlan->Start();//计时开始
	prgresAPlan->thread = athread;
	prgresAPlan->setWindowTitle("A*规划正在进行中，请稍等...");
	prgresAPlan->exec();

	//将画禁飞区威胁区关闭
	imgVer->imgEditor->isForT = -1;
}

void MainWindow::OpenTask()
{
	QString fileName = QFileDialog::getOpenFileName(this , "打开约束文件" , ".\\Task\\AutoRouteParameter.xml" , "*.xml;;*.*");

	if (fileName.isEmpty())
	{
		return;
	}

	XmlParse xml;//将xml文件信息读出
	imgVer->imgEditor->forbidAndThreatVector.clear();
	if (!xml.readConstrain((char *)fileName.toStdString().c_str() , imgVer->imgEditor->forbidAndThreatVector))
	{
		QMessageBox::information(this , "提示" , "解析xml文件失败" , QMessageBox::Yes);
		return;
	}
	
	//读起始点
	GroundPoint ptS , ptE;
	if (!xml.readETPoint((char *)fileName.toStdString().c_str() , ptS , ptE))
	{
		return ;
	}

	StandardizeConstraint standcos;//坐标转换，生成外接矩形
	iPoint tmp;
	standcos.PointLBToSC(ptS , tmp);
	imgVer->imgEditor->start.setX(tmp.x);
	imgVer->imgEditor->start.setY(tmp.y);
	standcos.PointLBToSC(ptE , tmp);
	imgVer->imgEditor->end.setX(tmp.x);
	imgVer->imgEditor->end.setY(tmp.y );
	//转换坐标
	for (int i = 0 ; i < imgVer->imgEditor->forbidAndThreatVector.size() ; i++)
 	{
 		standcos.Standardize(imgVer->imgEditor->forbidAndThreatVector[i]);
 	}
	imgVer->imgEditor->update();

	XmlFileName = fileName;
}

void MainWindow::SetGuidPoint()
{
	//清空容器为引导做准备
	imgVer->imgEditor->guidPoint.clear();
	imgVer->imgEditor->isForT = 1;
}

void MainWindow::CloseProject()
{
	//换一张平凡图
	imgVer->imgEditor->SetImage("");
	imgVer->imgEditor->isForT = -1;
	//关闭先前与图有关的句柄
	CloseHandle(G_hGctDataFilemap);
	CloseHandle(G_hZhtMapping);
	//释放先前的图片内存
	imgBuf.Delete();

	//初始化工具栏菜单栏的状态

	//清理初始点和终点
	imgVer->imgEditor->start = QPoint(0 , 0);
	imgVer->imgEditor->end = QPoint(0 , 0);
	//清空数据
}

void MainWindow::CreateMenus()					
{
	//工程管理菜单
	menuDataConvert = menuBar()->addMenu("数据转换");
	menuDataConvert->addAction(actOrgDataToMap);
	menuDataConvert->addAction(actOpenMapData);
	menuDataConvert->addAction(actClose);
	menuDataConvert->addSeparator();
	menuDataConvert->addAction(actQuit);

	//约束条件菜单
	menuConstraintCondition = menuBar()->addMenu("约束条件");
	menuConstraintCondition->addAction(actTask);
	menuConstraintCondition->addAction(actGuidPoint);
	
	//预处理
	menuCreateComMap = menuBar()->addMenu("组合图");
	menuCreateComMap->addAction(actCreatComMap);

	//线路规划
	menuAStarPlan = menuBar()->addMenu("线路规划");
	menuAStarPlan->addAction(actBasicAStarPlan);
	menuAStarPlan->addAction(actGuidAStarPlan);

	//效果演示
	menuDisplay = menuBar()->addMenu("效果演示");
	menuDisplay->addAction(actDisplay);

}	

void MainWindow::CreateToolBars()				
{
	//主工具栏
	toolBar = addToolBar("toolBar");

	//数据转换
	toolBar->addAction(actOrgDataToMap);
	toolBar->addAction(actOpenMapData);
	toolBar->addAction(actClose);
	toolBar->addSeparator();

	//约束条件
	toolBar->addAction(actTask);
	toolBar->addAction(actGuidPoint);
	toolBar->addSeparator();

	//生成组合图
	toolBar->addAction(actCreatComMap);
	toolBar->addSeparator();

	//路径规划A*
	toolBar->addAction(actBasicAStarPlan);
	toolBar->addAction(actGuidAStarPlan);
	toolBar->addSeparator();

	//效果演示
	toolBar->addAction(actDisplay);
	toolBar->addSeparator();

	//退出系统
	toolBar->addAction(actQuit);
	toolBar->show();
}

//新建工程即将原始数据转换成map图
void MainWindow::OrgDataToMap()
{
	QString fileName = QFileDialog::getOpenFileName(this , "打开原始文件" , ".\\dataorg\\testdata.txt" , "*.txt;;*.*");

	if (fileName.isEmpty())
	{
		return;
	}

	//将原始数据转换成高程图
	QString newFileName = QFileDialog::getSaveFileName(this , "新建MAP文件" , ".\\datamap\\gct.map" , "*.map;;*.*");

	if (newFileName.isEmpty())
	{
		return;
	}

	//线程开始
	dcThread->fileNameOrg = fileName;
	dcThread->fileNameNew = newFileName;
	dcThread->start();

	//显示进度条
	prgresDataCvt->Start();//计时开始
	prgresDataCvt->thread = dcThread;
	prgresDataCvt->setWindowTitle("正在进行数据转换请稍后...");
	prgresDataCvt->exec();

}

//打开工程即打开已存在的map图
void MainWindow::OpenMapData()
{
	QString fileName = QFileDialog::getOpenFileName(this , "打开MAP文件" , ".\\datamap\\gct.map" , "*.map;;*.*");

	if (fileName.isEmpty())
	{
		return;
	}

	//关闭先前的工程
	CloseProject();
	ReadMapFileOperation rmf;
	//打开原始文件
	if (!rmf.OpenFileByRead(fileName.toStdString().c_str()))
		return ;

	MapFileHeader mfh;
	//读文件头
	if (!rmf.ReadMapFileHeader(mfh))
	{
		QMessageBox::information(this , "提示" , "打开MAP文件失败" , QMessageBox::Yes);
		return;
	}
	rmf.Closefp();
	// add by mj at 10-28
// 	SYSTEM_INFO sinf;
// 	GetSystemInfo(&sinf);
// 	G_dwGran = sinf.dwAllocationGranularity;    //初始化系统分配粒度
// 	G_MAPL0 = mfh.xllcorner;               // 全局地图左上角的经度
// 	G_MAPB0 = mfh.yllcorner;               // 全局地图左上角的纬度
// 	G_WIDTH = mfh.nclos;                   // 全局规划地图的宽度
// 	G_HEIGHT = mfh.nrows; // 全局规划地图的高度
// 	G_RESOLVE = mfh.cellsize; //add by hj 2013.10.31

	//每次读多少字节
	memoryViewSize = mfh.nclos * sizeof(short);

	//为地图数据准备内存
	CShortAllocate shortBuf;
	if (!shortBuf.Allocate(memoryViewSize/sizeof(short)))
	{
		QMessageBox::information(this , "提示" , "数据过大无法分配内存" , QMessageBox::Yes);
		return;
	}

	APlanning apl;
	if(!apl.getMapInfo(fileName.toStdWString().c_str()))
	{
		QMessageBox::information(this , "提示" , "获取地图信息失败" , QMessageBox::Yes);
		return ;
	}

// 	if(!CreateGctDataMap(fileName.toStdWString().c_str() , G_hGctDataFilemap))
// 	{
// 		QMessageBox::information(this , "提示" , "创建内存映射失败" , QMessageBox::Yes);
// 		return;
// 	}

	//cnt计算一共要取多少次
	int cnt1 = mfh.nclos*mfh.nrows*sizeof(short)/memoryViewSize;
	int t = cnt1 - 1  ; 

	int dataSize = memoryViewSize;//每次映射的数据长度

	//为图片分配临时内存
	int extra = 0;
	if (mfh.nclos % 4)
	{
		extra = 4 - mfh.nclos%4;
	}

	int iImageWidth = mfh.nclos +extra;//对齐
	BYTE * pbImageBuf = new BYTE[mfh.nrows*iImageWidth];

	if (pbImageBuf == NULL)
	{
		QMessageBox::information(this , "提示" , "分配地图内存失败" , QMessageBox::Yes);
		return ;
	}

	ZeroMemory(pbImageBuf , mfh.nrows*iImageWidth);

	__int64 iDataSizeMore = fileSize - memoryViewSize*cnt1-sizeof(MapFileHeader);

	//新图写了多少行
	int iNewRows = 0;

	while (t >= 0)
	{	//逐次读取原始数据

		if(!MemoryMapView(G_hGctDataFilemap , sizeof(MapFileHeader) + memoryViewSize*t + iDataSizeMore , dataSize, shortBuf.m_pData))
		{
			QMessageBox::information(this , "提示" , "读取地图数据失败" , QMessageBox::Yes);
			return;
		}

		//将高程值转换成图片颜色
		for (int i = (dataSize/sizeof(short)) / mfh.nclos - 1 ; i >= 0 ; i--)
		{
			for (int j = 0 ; j < mfh.nclos ; j++)
			{
				int k = i * mfh.nclos + j;
				if (mfh.NODATA_value == shortBuf.m_pData[k] || 0 == shortBuf.m_pData[k])
				{
					pbImageBuf[iNewRows*iImageWidth + j] = 0;
				}
				else
				{
					UCHAR fillData = fabs((/*log10*/((double)shortBuf.m_pData[k]-mfh.sMin+1)*255/ /*log10*/((double)mfh.sMax-mfh.sMin+1)));
					pbImageBuf[iNewRows*iImageWidth + j] = fillData;
				}
			}
			iNewRows++;
		}
		t--;
	}


	if (iDataSizeMore != 0)
	{
		if(!MemoryMapView(G_hGctDataFilemap , sizeof(MapFileHeader), iDataSizeMore , shortBuf.m_pData))
		{
			QMessageBox::information(this , "提示" , "内存映射失败" , QMessageBox::Yes);
			return ;
		}

		//将高程值转换成图片颜色
		for (int i = (iDataSizeMore/sizeof(short)) / mfh.nclos - 1 ; i >= 0 ; i--)
		{
			for (__int64 j = 0 ; j < mfh.nclos ; j++)
			{
				int k = i * mfh.nclos + j;
				if (mfh.NODATA_value == shortBuf.m_pData[k] || 0 == shortBuf.m_pData[k])
				{
					pbImageBuf[iNewRows*iImageWidth + j] = 0;
				}
				else
				{
					long fillData = fabs((/*log10*/((double)shortBuf.m_pData[k]-mfh.sMin+1)*255/ /*log10*/((double)mfh.sMax-mfh.sMin+1)));
					pbImageBuf[iNewRows*iImageWidth + j] = fillData;
				}
			}
			iNewRows++;
		}
	}

	CDibBitMap bitmap;
	if (!bitmap.AllocateMap(mfh.nrows*iImageWidth))
	{
		QMessageBox::information(this , "提示" , "分配地图内存失败" , QMessageBox::Yes);
		return ;
	}

	//创建位图
	BITMAPFILEHEADER bmpFileHeader;
	BITMAPINFO * bmpMapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD)];

	bmpFileHeader.bfType = 0x4d42;
	bmpFileHeader.bfReserved1 = bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + 
		sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
	bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + mfh.nrows*iImageWidth;

	bmpMapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpMapInfo->bmiHeader.biWidth = iImageWidth;
	bmpMapInfo->bmiHeader.biHeight = mfh.nrows;
	bmpMapInfo->bmiHeader.biPlanes = 1;
	bmpMapInfo->bmiHeader.biBitCount = 8;
	bmpMapInfo->bmiHeader.biCompression = BI_RGB;
	bmpMapInfo->bmiHeader.biSizeImage = mfh.nrows*iImageWidth;
	bmpMapInfo->bmiHeader.biXPelsPerMeter =
		bmpMapInfo->bmiHeader.biYPelsPerMeter = 0;
	bmpMapInfo->bmiHeader.biClrUsed = 256;
	bmpMapInfo->bmiHeader.biClrImportant = 256;

	ZeroMemory(bmpMapInfo->bmiColors , 256*sizeof(RGBQUAD));


	for (int i = 0 ; i < 256 ; i++)
	{
		bmpMapInfo->bmiColors[i].rgbBlue = (BYTE)i;
		bmpMapInfo->bmiColors[i].rgbGreen = (BYTE)i;
		bmpMapInfo->bmiColors[i].rgbRed = (BYTE)i;
	}

	if(!bitmap.CreateBitMap(bmpFileHeader , bmpMapInfo , pbImageBuf , sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD) , mfh.nrows*iImageWidth))
	{
		QMessageBox::information(this , "提示" , "创建位图失败" , QMessageBox::Yes);
		return ;
	}

	QString str = ".\\mapimage\\project.bmp";
	if (!bitmap.WriteMapData(str.toStdString().c_str() , mfh.nrows*iImageWidth))
	{
		QMessageBox::information(this , "提示" , "写地图数据失败" , QMessageBox::Yes);
		return;
	}	

	imgVer->imgEditor->SetImage(str);
	
	//设置工具栏，菜单栏
}

///////////////////////////////////////////////////////////
/////////A*线路规划   add by mj at 10-28
void MainWindow::BasicAStarPlan()
{
	QString fileName = QFileDialog::getOpenFileName(this,"打开需要规划的任务文件",
		".\\Task",tr("XML Files (*.XML *.xml)"));

	if(fileName.isEmpty())
	{
		return ;
	}

	//changed by hj 2013.11.11
	athread->fileName = fileName;
	athread->IsGuidePlan =false;
	athread->start();//线程开始

	//显示进度条
	prgresAPlan->Start();//计时开始
	prgresAPlan->thread = athread;
	prgresAPlan->setWindowTitle("A*规划正在进行中，请稍等...");
	prgresAPlan->exec();

	//将画禁飞区威胁区关闭
	imgVer->imgEditor->isForT = -1;
}

//////////////////////////////////////////////////////////////




void MainWindow::Display()
{
	QString fileName = QFileDialog::getOpenFileName(this,"打开需要演示的文件",
		"Task",tr("Data Files (*.xml)"));

	if(fileName.isEmpty())
	{
		return ;
	}

	//将画禁飞区威胁区关闭
	imgVer->imgEditor->isForT = -1;

	//清空数据，从新读取
	imgVer->imgEditor->ShowRoute.clear();

	//从xml文件读出规划结果
	XmlParse xmlres;
	vector<GroundPoint> res , guid;
	GroundPoint ptS , ptE;
	//读出结果
	xmlres.readResultFromXml((char *)fileName.toStdString().c_str() , res);
	xmlres.readGuidePointFromXml((char *)fileName.toStdString().c_str() , guid);
	xmlres.readETPoint((char *)fileName.toStdString().c_str() , ptS , ptE);
	//情空以前的
	imgVer->imgEditor->ShowRoute.clear();
	imgVer->imgEditor->guidPoint.clear();

	//转换坐标
	StandardizeConstraint standres;
	standres.GuidOrResConvertToScreenCoordinate(res , imgVer->imgEditor->ShowRoute);
	standres.GuidOrResConvertToScreenCoordinate(guid , imgVer->imgEditor->guidPoint);
	/*standres.PointLBToSC(ptS , imgVer->imgEditor->start);*/

	imgVer->imgEditor->update();
}

void MainWindow::CreatComMap()
{
	//获得组合图文件名
	QString newFileName = QFileDialog::getSaveFileName(this , "打开原始组合图" , ".\\datazht\\zht.zmp" , "*.zmp;;*.*");

	if (newFileName.isEmpty())
	{
		return;
	}

	//QString XmlFileName = QFileDialog::getSaveFileName(this , "打开XML文件" , ".\\Task\\AutoRouteParameter.xml" , "*.xml;;*.*");

// 	if (XmlFileName.isEmpty())
// 	{
// 		return;
// 	}

	//启动线程
	comMapThread->beginPoint = imgVer->imgEditor->start;
	comMapThread->endPoint = imgVer->imgEditor->end;
	comMapThread->fileName = newFileName;
	comMapThread->forbidAndThreatVector = imgVer->imgEditor->forbidAndThreatVector;
	comMapThread->start();
	comMapThread->XmlFileName = XmlFileName;
	comMapThread->vctGuidPoint = imgVer->imgEditor->guidPoint;

	//显示进度条
	prgresComMap->Start();//计时开始
	prgresComMap->thread = comMapThread;
	prgresComMap->setWindowTitle("组合图正在生成，请稍后...");
	prgresComMap->exec();

	//设置工具栏菜单栏状态
	actBasicAStarPlan->setEnabled(true);

	//将画禁飞区威胁区关闭
	imgVer->imgEditor->isForT = -1;
}

/************************************************************************/
/* 打开高程图时使用内存映射													*/
/*add by hj  2013.10.30																		*/
/************************************************************************/
bool MainWindow::MemoryMapView(HANDLE hFileMap , __int64 begin , int size , short * &address)
{
	__int64 start = begin/G_dwGran*G_dwGran;
	int errorStatr = begin - start;
	DWORD highOffset = start>>32;
	DWORD lowOffset = start&0xffffffff;

	LPVOID pVoid = MapViewOfFile(hFileMap,
		FILE_MAP_READ ,
		highOffset , 
		lowOffset , 
		size+errorStatr//注意这个errorStart一定要加上否则超内存，然后就是一天坐着黯然伤神。。。。。
		);

	if (NULL == pVoid)
	{// 出错处理
		return false;
	}

	CopyMemory((void *)address , (BYTE*)pVoid+errorStatr , size);

	UnmapViewOfFile(pVoid);  // 必须撤销映射
	return true;
}

void MainWindow::Close()
{
	//确认是否退出
	close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	//提示用户是否退出
	QMessageBox msgBox(QMessageBox::Question,"电力线路规划演示软件","您确定要退出吗？"
		,QMessageBox::Yes|QMessageBox::No,this,Qt::Dialog);

	switch (msgBox.exec()) 
	{
	case QMessageBox::Yes:
		event->accept();		
		break;
	case QMessageBox::No:
		event->ignore();
	default:
		event->ignore();
		break;
	}
}
