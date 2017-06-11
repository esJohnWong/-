/************************************************************************/
/*  图片管理器，当需要画图时在这里添加处理                                                                    */
/************************************************************************/

#pragma once
#include <QWidget>
#include "DataStruct.h"

class ImageEditor :
	public QWidget
{
	Q_OBJECT
public:
	ImageEditor(QWidget * parent = NULL);
	~ImageEditor(void);
public: 
	void SetImage(QString imgName); //设置要显示的图片
	QImage GetImage() const{ return image; } 
	QSize sizeHint() const; 

	//判断是画禁飞区还是威胁区
	int isForT;

	//展示路径  add by mj at 11-15
	std::vector<iPoint> ShowRoute;  

	//画规划的路径
	void DrawPath(QPainter & painter , std::vector<iPoint> &route);
	//画禁可飞区
	void DrawForbid(QPainter & painter , iPolygon & forbid);
	void DrawThreat(QPainter & painter , iPolygon &threat);

	void DrawBeginAndEnd(QPainter & painter , QPoint &pt);

	void DrawGuidPoint(QPainter & painter , std::vector<iPoint> &guid);
protected: 
	void paintEvent(QPaintEvent *event); //自绘处理

	//鼠标事件用于获得禁飞区威胁区数据
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent ( QMouseEvent  * event);
private: 
	QImage image; //图片
	//int zoom; //缩放因子
	//保存鼠标按下和弹起的坐标点
	QPoint beginPoint;

public:
	//引导点序列
	std::vector<iPoint> guidPoint;

	//线路起点和终点
	QPoint start , end;
	//威胁区禁飞区信息
	vector<iPolygon> forbidAndThreatVector;
};

