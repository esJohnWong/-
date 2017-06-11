/************************************************************************/
/* ������Դ�ļ�															*/
/*2013.10.16 11:48														*/
/*�ƿ�																	*/
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
	//��ʼ��
	CreateActions();
	CreateMenus();
	CreateToolBars();

	//���ý�������ͼ��
	setWindowTitle("������·�滮��ʾ���");
	setWindowIcon(QIcon(":/Resources/system.png"));


	//��ʼ��ͼƬ��ʾ��
	imgVer = new ImageViewer(this);
	setCentralWidget(imgVer);

	//��ʼ���ڴ�ӳ���С��ʼΪ1400*1400���ֽڱ��ڵ���ʱ�۲��Ƿ��д���Ҳ������
	memoryViewSize = 1600*1600;

	//��ʼ���������˵�����״̬

	//�滮�̳߳�ʼ��
	athread = new APlanThread;
	connect(athread , SIGNAL(finished()) , this , SLOT(APlanThreadFinished()));

	//������
	prgresAPlan = new ProgressBarShow(this);//A*

	//����ת���߳�
	dcThread = new DataConvertThread;
	connect(dcThread , SIGNAL(finished()) , this , SLOT(DataConvertThreadFinished()));
	//����ת��������
	prgresDataCvt = new ProgressBarShow(this);

	//���ͼ�����߳�
	comMapThread = new ComMapThread;
	prgresComMap = new ProgressBarShow(this);
	connect(comMapThread , SIGNAL(finished()) , this , SLOT(ComMapThreadFinished()));
}

void MainWindow::ComMapThreadFinished()
{
	prgresComMap->hide();
	prgresComMap->Stop();

	//����߳̽������
	if (isEndThread)
	{
		comMapThread->errcode = 3;
		isEndThread = false;
	}

	switch(comMapThread->errcode)
	{
	case 0:
		QMessageBox::information(this , "��ʾ" , "���ͼ���ɳɹ�" , QMessageBox::Yes);
		break;
	case 1:
		QMessageBox::information(this , "��ʾ" , "��ʼ�����ͼʧ��" , QMessageBox::Yes);
		break;
	case 2:
		QMessageBox::information(this , "��ʾ" , "�������ͼʧ��" , QMessageBox::Yes);
		break;

	case 3:
		QMessageBox::information(this , "��ʾ" , "�̱߳���ֹ��" , QMessageBox::Yes);
		break;

	default:
		QMessageBox::warning(this,"����","δ֪���͵Ĵ���!");
	}

	comMapThread->errcode = 0;
}

void MainWindow::DataConvertThreadFinished()
{
	prgresDataCvt->hide();
	prgresDataCvt->Stop();

	//����߳̽������
	if (isEndThread)
	{
		dcThread->errcode = 3;
		isEndThread = false;
	}

	switch(dcThread->errcode)
	{
	case 0:
		QMessageBox::information(this , "��ʾ" , "ԭʼ����ת����MAPͼ�ɹ�\n�򿪴�ͼ���ɣ�" , QMessageBox::Yes);
		break;
	case 1:
		QMessageBox::information(this , "��ʾ" , "ԭʼ����ת����MAPͼʧ�ܣ�" , QMessageBox::Yes);
		break;

	case 3:
		QMessageBox::information(this , "��ʾ" , "�̱߳���ֹ��" , QMessageBox::Yes);
		break;

	default:
		QMessageBox::warning(this,"����","δ֪���͵Ĵ���!");
	}

	dcThread->errcode = 0;
}

void MainWindow::APlanThreadFinished()
{
	//Ӱ�ؽ�����
	prgresAPlan->hide();
	prgresAPlan->Stop();

	//����߳̽������
	if (isEndThread)
	{
		athread->errcode = 3;
		isEndThread = false;
	}

	switch(athread->errcode)
	{
	case 0:
		QMessageBox::information(this , "��ʾ"  , "A*�ֲ��滮���");
		break;
	case 1:
		QMessageBox::warning(this,"Warning","���һ������滮����ļ�!");
		break;

	case 3:
		QMessageBox::information(this , "��ʾ" , "�̱߳���ֹ��" , QMessageBox::Yes);
		break;

	case 4:
		QMessageBox::information(this , "��ʾ" , "�滮ʧ�ܣ�" , QMessageBox::Yes);
		break;

	default:
		QMessageBox::warning(this,"����","δ֪���͵Ĵ���!");
	}

	athread->errcode = 0;//��ԭ
}


MainWindow::~MainWindow()
{

}

void MainWindow::CreateActions()				
{
	//����ת��
	actOrgDataToMap = new QAction(QIcon(":/Resources/orgtomap.png"), "ԭʼ����ת��", this);
	actOrgDataToMap->setShortcut(tr("Ctrl+N"));
	connect(actOrgDataToMap, SIGNAL(triggered()), this, SLOT(OrgDataToMap()));

	//�򿪹����ļ�
	actOpenMapData = new QAction(QIcon(":/Resources/openmap.png"), "��MAP�ļ�",this);                              
	actOpenMapData->setShortcut(tr("Ctrl+O"));
	connect(actOpenMapData, SIGNAL(triggered()), this, SLOT(OpenMapData()));

	//�˳�ϵͳ
	actQuit = new QAction(QIcon(":/Resources/quit.png"), "�˳�ϵͳ", this);
	actQuit->setShortcut(tr("Ctrl+Q"));
	connect(actQuit, SIGNAL(triggered()), this, SLOT(Close()));

	//��·�滮
	actBasicAStarPlan = new QAction(QIcon(":/Resources/baseastar.png"),"����A*�滮", this);
	connect(actBasicAStarPlan, SIGNAL(triggered()), this, SLOT(BasicAStarPlan()));

	//Ч����ʾ
	actDisplay = new QAction(QIcon(":/Resources/display.png"),"Ч����ʾ", this);
	connect(actDisplay, SIGNAL(triggered()), this, SLOT(Display()));

	//���ͼ����
	actCreatComMap = new QAction(QIcon(":/Resources/commap.png"),"���ͼ����", this);
	connect(actCreatComMap, SIGNAL(triggered()), this, SLOT(CreatComMap()));

	//����A*�滮
	actGuidAStarPlan = new QAction(QIcon(":/Resources/guidastar.png"),"����A*�滮" , this);
	connect(actGuidAStarPlan , SIGNAL(triggered()) , this , SLOT(GuidAStarPlan()));

	//�رչ���
	actClose = new QAction(QIcon(":/Resources/closepoj.png"),"�رչ���" , this);
	connect(actClose , SIGNAL(triggered()) , this , SLOT(CloseProject()));

	//��������
	actGuidPoint = new QAction(QIcon(":/Resources/guidpoint.png"),"���������", this);
	connect(actGuidPoint , SIGNAL(triggered()) , this , SLOT(SetGuidPoint()));

	//Լ��ͼ��
	actTask = new QAction(QIcon(":/Resources/confile.png"),"����Լ��ͼ��", this);
	connect(actTask , SIGNAL(triggered()) , this , SLOT(OpenTask()));
}

void MainWindow::GuidAStarPlan()
{
	QString fileName = QFileDialog::getOpenFileName(this,"����Ҫ�滮�������ļ�",
		".\\Task",tr("XML Files (*.XML *.xml)"));

	if(fileName.isEmpty())
	{
		return ;
	}

	//changed by hj 2013.11.11
	athread->fileName = fileName;
	athread->IsGuidePlan =true;
	athread->start();//�߳̿�ʼ

	//��ʾ������
	prgresAPlan->Start();//��ʱ��ʼ
	prgresAPlan->thread = athread;
	prgresAPlan->setWindowTitle("A*�滮���ڽ����У����Ե�...");
	prgresAPlan->exec();

	//������������в���ر�
	imgVer->imgEditor->isForT = -1;
}

void MainWindow::OpenTask()
{
	QString fileName = QFileDialog::getOpenFileName(this , "��Լ���ļ�" , ".\\Task\\AutoRouteParameter.xml" , "*.xml;;*.*");

	if (fileName.isEmpty())
	{
		return;
	}

	XmlParse xml;//��xml�ļ���Ϣ����
	imgVer->imgEditor->forbidAndThreatVector.clear();
	if (!xml.readConstrain((char *)fileName.toStdString().c_str() , imgVer->imgEditor->forbidAndThreatVector))
	{
		QMessageBox::information(this , "��ʾ" , "����xml�ļ�ʧ��" , QMessageBox::Yes);
		return;
	}
	
	//����ʼ��
	GroundPoint ptS , ptE;
	if (!xml.readETPoint((char *)fileName.toStdString().c_str() , ptS , ptE))
	{
		return ;
	}

	StandardizeConstraint standcos;//����ת����������Ӿ���
	iPoint tmp;
	standcos.PointLBToSC(ptS , tmp);
	imgVer->imgEditor->start.setX(tmp.x);
	imgVer->imgEditor->start.setY(tmp.y);
	standcos.PointLBToSC(ptE , tmp);
	imgVer->imgEditor->end.setX(tmp.x);
	imgVer->imgEditor->end.setY(tmp.y );
	//ת������
	for (int i = 0 ; i < imgVer->imgEditor->forbidAndThreatVector.size() ; i++)
 	{
 		standcos.Standardize(imgVer->imgEditor->forbidAndThreatVector[i]);
 	}
	imgVer->imgEditor->update();

	XmlFileName = fileName;
}

void MainWindow::SetGuidPoint()
{
	//�������Ϊ������׼��
	imgVer->imgEditor->guidPoint.clear();
	imgVer->imgEditor->isForT = 1;
}

void MainWindow::CloseProject()
{
	//��һ��ƽ��ͼ
	imgVer->imgEditor->SetImage("");
	imgVer->imgEditor->isForT = -1;
	//�ر���ǰ��ͼ�йصľ��
	CloseHandle(G_hGctDataFilemap);
	CloseHandle(G_hZhtMapping);
	//�ͷ���ǰ��ͼƬ�ڴ�
	imgBuf.Delete();

	//��ʼ���������˵�����״̬

	//�����ʼ����յ�
	imgVer->imgEditor->start = QPoint(0 , 0);
	imgVer->imgEditor->end = QPoint(0 , 0);
	//�������
}

void MainWindow::CreateMenus()					
{
	//���̹���˵�
	menuDataConvert = menuBar()->addMenu("����ת��");
	menuDataConvert->addAction(actOrgDataToMap);
	menuDataConvert->addAction(actOpenMapData);
	menuDataConvert->addAction(actClose);
	menuDataConvert->addSeparator();
	menuDataConvert->addAction(actQuit);

	//Լ�������˵�
	menuConstraintCondition = menuBar()->addMenu("Լ������");
	menuConstraintCondition->addAction(actTask);
	menuConstraintCondition->addAction(actGuidPoint);
	
	//Ԥ����
	menuCreateComMap = menuBar()->addMenu("���ͼ");
	menuCreateComMap->addAction(actCreatComMap);

	//��·�滮
	menuAStarPlan = menuBar()->addMenu("��·�滮");
	menuAStarPlan->addAction(actBasicAStarPlan);
	menuAStarPlan->addAction(actGuidAStarPlan);

	//Ч����ʾ
	menuDisplay = menuBar()->addMenu("Ч����ʾ");
	menuDisplay->addAction(actDisplay);

}	

void MainWindow::CreateToolBars()				
{
	//��������
	toolBar = addToolBar("toolBar");

	//����ת��
	toolBar->addAction(actOrgDataToMap);
	toolBar->addAction(actOpenMapData);
	toolBar->addAction(actClose);
	toolBar->addSeparator();

	//Լ������
	toolBar->addAction(actTask);
	toolBar->addAction(actGuidPoint);
	toolBar->addSeparator();

	//�������ͼ
	toolBar->addAction(actCreatComMap);
	toolBar->addSeparator();

	//·���滮A*
	toolBar->addAction(actBasicAStarPlan);
	toolBar->addAction(actGuidAStarPlan);
	toolBar->addSeparator();

	//Ч����ʾ
	toolBar->addAction(actDisplay);
	toolBar->addSeparator();

	//�˳�ϵͳ
	toolBar->addAction(actQuit);
	toolBar->show();
}

//�½����̼���ԭʼ����ת����mapͼ
void MainWindow::OrgDataToMap()
{
	QString fileName = QFileDialog::getOpenFileName(this , "��ԭʼ�ļ�" , ".\\dataorg\\testdata.txt" , "*.txt;;*.*");

	if (fileName.isEmpty())
	{
		return;
	}

	//��ԭʼ����ת���ɸ߳�ͼ
	QString newFileName = QFileDialog::getSaveFileName(this , "�½�MAP�ļ�" , ".\\datamap\\gct.map" , "*.map;;*.*");

	if (newFileName.isEmpty())
	{
		return;
	}

	//�߳̿�ʼ
	dcThread->fileNameOrg = fileName;
	dcThread->fileNameNew = newFileName;
	dcThread->start();

	//��ʾ������
	prgresDataCvt->Start();//��ʱ��ʼ
	prgresDataCvt->thread = dcThread;
	prgresDataCvt->setWindowTitle("���ڽ�������ת�����Ժ�...");
	prgresDataCvt->exec();

}

//�򿪹��̼����Ѵ��ڵ�mapͼ
void MainWindow::OpenMapData()
{
	QString fileName = QFileDialog::getOpenFileName(this , "��MAP�ļ�" , ".\\datamap\\gct.map" , "*.map;;*.*");

	if (fileName.isEmpty())
	{
		return;
	}

	//�ر���ǰ�Ĺ���
	CloseProject();
	ReadMapFileOperation rmf;
	//��ԭʼ�ļ�
	if (!rmf.OpenFileByRead(fileName.toStdString().c_str()))
		return ;

	MapFileHeader mfh;
	//���ļ�ͷ
	if (!rmf.ReadMapFileHeader(mfh))
	{
		QMessageBox::information(this , "��ʾ" , "��MAP�ļ�ʧ��" , QMessageBox::Yes);
		return;
	}
	rmf.Closefp();
	// add by mj at 10-28
// 	SYSTEM_INFO sinf;
// 	GetSystemInfo(&sinf);
// 	G_dwGran = sinf.dwAllocationGranularity;    //��ʼ��ϵͳ��������
// 	G_MAPL0 = mfh.xllcorner;               // ȫ�ֵ�ͼ���Ͻǵľ���
// 	G_MAPB0 = mfh.yllcorner;               // ȫ�ֵ�ͼ���Ͻǵ�γ��
// 	G_WIDTH = mfh.nclos;                   // ȫ�ֹ滮��ͼ�Ŀ��
// 	G_HEIGHT = mfh.nrows; // ȫ�ֹ滮��ͼ�ĸ߶�
// 	G_RESOLVE = mfh.cellsize; //add by hj 2013.10.31

	//ÿ�ζ������ֽ�
	memoryViewSize = mfh.nclos * sizeof(short);

	//Ϊ��ͼ����׼���ڴ�
	CShortAllocate shortBuf;
	if (!shortBuf.Allocate(memoryViewSize/sizeof(short)))
	{
		QMessageBox::information(this , "��ʾ" , "���ݹ����޷������ڴ�" , QMessageBox::Yes);
		return;
	}

	APlanning apl;
	if(!apl.getMapInfo(fileName.toStdWString().c_str()))
	{
		QMessageBox::information(this , "��ʾ" , "��ȡ��ͼ��Ϣʧ��" , QMessageBox::Yes);
		return ;
	}

// 	if(!CreateGctDataMap(fileName.toStdWString().c_str() , G_hGctDataFilemap))
// 	{
// 		QMessageBox::information(this , "��ʾ" , "�����ڴ�ӳ��ʧ��" , QMessageBox::Yes);
// 		return;
// 	}

	//cnt����һ��Ҫȡ���ٴ�
	int cnt1 = mfh.nclos*mfh.nrows*sizeof(short)/memoryViewSize;
	int t = cnt1 - 1  ; 

	int dataSize = memoryViewSize;//ÿ��ӳ������ݳ���

	//ΪͼƬ������ʱ�ڴ�
	int extra = 0;
	if (mfh.nclos % 4)
	{
		extra = 4 - mfh.nclos%4;
	}

	int iImageWidth = mfh.nclos +extra;//����
	BYTE * pbImageBuf = new BYTE[mfh.nrows*iImageWidth];

	if (pbImageBuf == NULL)
	{
		QMessageBox::information(this , "��ʾ" , "�����ͼ�ڴ�ʧ��" , QMessageBox::Yes);
		return ;
	}

	ZeroMemory(pbImageBuf , mfh.nrows*iImageWidth);

	__int64 iDataSizeMore = fileSize - memoryViewSize*cnt1-sizeof(MapFileHeader);

	//��ͼд�˶�����
	int iNewRows = 0;

	while (t >= 0)
	{	//��ζ�ȡԭʼ����

		if(!MemoryMapView(G_hGctDataFilemap , sizeof(MapFileHeader) + memoryViewSize*t + iDataSizeMore , dataSize, shortBuf.m_pData))
		{
			QMessageBox::information(this , "��ʾ" , "��ȡ��ͼ����ʧ��" , QMessageBox::Yes);
			return;
		}

		//���߳�ֵת����ͼƬ��ɫ
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
			QMessageBox::information(this , "��ʾ" , "�ڴ�ӳ��ʧ��" , QMessageBox::Yes);
			return ;
		}

		//���߳�ֵת����ͼƬ��ɫ
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
		QMessageBox::information(this , "��ʾ" , "�����ͼ�ڴ�ʧ��" , QMessageBox::Yes);
		return ;
	}

	//����λͼ
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
		QMessageBox::information(this , "��ʾ" , "����λͼʧ��" , QMessageBox::Yes);
		return ;
	}

	QString str = ".\\mapimage\\project.bmp";
	if (!bitmap.WriteMapData(str.toStdString().c_str() , mfh.nrows*iImageWidth))
	{
		QMessageBox::information(this , "��ʾ" , "д��ͼ����ʧ��" , QMessageBox::Yes);
		return;
	}	

	imgVer->imgEditor->SetImage(str);
	
	//���ù��������˵���
}

///////////////////////////////////////////////////////////
/////////A*��·�滮   add by mj at 10-28
void MainWindow::BasicAStarPlan()
{
	QString fileName = QFileDialog::getOpenFileName(this,"����Ҫ�滮�������ļ�",
		".\\Task",tr("XML Files (*.XML *.xml)"));

	if(fileName.isEmpty())
	{
		return ;
	}

	//changed by hj 2013.11.11
	athread->fileName = fileName;
	athread->IsGuidePlan =false;
	athread->start();//�߳̿�ʼ

	//��ʾ������
	prgresAPlan->Start();//��ʱ��ʼ
	prgresAPlan->thread = athread;
	prgresAPlan->setWindowTitle("A*�滮���ڽ����У����Ե�...");
	prgresAPlan->exec();

	//������������в���ر�
	imgVer->imgEditor->isForT = -1;
}

//////////////////////////////////////////////////////////////




void MainWindow::Display()
{
	QString fileName = QFileDialog::getOpenFileName(this,"����Ҫ��ʾ���ļ�",
		"Task",tr("Data Files (*.xml)"));

	if(fileName.isEmpty())
	{
		return ;
	}

	//������������в���ر�
	imgVer->imgEditor->isForT = -1;

	//������ݣ����¶�ȡ
	imgVer->imgEditor->ShowRoute.clear();

	//��xml�ļ������滮���
	XmlParse xmlres;
	vector<GroundPoint> res , guid;
	GroundPoint ptS , ptE;
	//�������
	xmlres.readResultFromXml((char *)fileName.toStdString().c_str() , res);
	xmlres.readGuidePointFromXml((char *)fileName.toStdString().c_str() , guid);
	xmlres.readETPoint((char *)fileName.toStdString().c_str() , ptS , ptE);
	//�����ǰ��
	imgVer->imgEditor->ShowRoute.clear();
	imgVer->imgEditor->guidPoint.clear();

	//ת������
	StandardizeConstraint standres;
	standres.GuidOrResConvertToScreenCoordinate(res , imgVer->imgEditor->ShowRoute);
	standres.GuidOrResConvertToScreenCoordinate(guid , imgVer->imgEditor->guidPoint);
	/*standres.PointLBToSC(ptS , imgVer->imgEditor->start);*/

	imgVer->imgEditor->update();
}

void MainWindow::CreatComMap()
{
	//������ͼ�ļ���
	QString newFileName = QFileDialog::getSaveFileName(this , "��ԭʼ���ͼ" , ".\\datazht\\zht.zmp" , "*.zmp;;*.*");

	if (newFileName.isEmpty())
	{
		return;
	}

	//QString XmlFileName = QFileDialog::getSaveFileName(this , "��XML�ļ�" , ".\\Task\\AutoRouteParameter.xml" , "*.xml;;*.*");

// 	if (XmlFileName.isEmpty())
// 	{
// 		return;
// 	}

	//�����߳�
	comMapThread->beginPoint = imgVer->imgEditor->start;
	comMapThread->endPoint = imgVer->imgEditor->end;
	comMapThread->fileName = newFileName;
	comMapThread->forbidAndThreatVector = imgVer->imgEditor->forbidAndThreatVector;
	comMapThread->start();
	comMapThread->XmlFileName = XmlFileName;
	comMapThread->vctGuidPoint = imgVer->imgEditor->guidPoint;

	//��ʾ������
	prgresComMap->Start();//��ʱ��ʼ
	prgresComMap->thread = comMapThread;
	prgresComMap->setWindowTitle("���ͼ�������ɣ����Ժ�...");
	prgresComMap->exec();

	//���ù������˵���״̬
	actBasicAStarPlan->setEnabled(true);

	//������������в���ر�
	imgVer->imgEditor->isForT = -1;
}

/************************************************************************/
/* �򿪸߳�ͼʱʹ���ڴ�ӳ��													*/
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
		size+errorStatr//ע�����errorStartһ��Ҫ���Ϸ����ڴ棬Ȼ�����һ��������Ȼ���񡣡�������
		);

	if (NULL == pVoid)
	{// ������
		return false;
	}

	CopyMemory((void *)address , (BYTE*)pVoid+errorStatr , size);

	UnmapViewOfFile(pVoid);  // ���볷��ӳ��
	return true;
}

void MainWindow::Close()
{
	//ȷ���Ƿ��˳�
	close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	//��ʾ�û��Ƿ��˳�
	QMessageBox msgBox(QMessageBox::Question,"������·�滮��ʾ���","��ȷ��Ҫ�˳���"
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
