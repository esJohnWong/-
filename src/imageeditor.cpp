#include "imageeditor.h"
#include <QtGui>

ImageEditor::ImageEditor(QWidget * parent):QWidget(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);  
	isForT = -1;//代表不画任何区域
	start = end = QPoint(0 , 0);
}


ImageEditor::~ImageEditor(void)
{
}

QSize ImageEditor::sizeHint() const 
{ 
	return image.size(); 
} 

void ImageEditor::DrawForbid(QPainter & painter , iPolygon & forbid)
{
	painter.setPen(QPen(Qt::red , 1));
	QPointF * forbidPoint = new QPointF[forbid.ptPolygon.size()];

	for (int i = 0 ; i < forbid.ptPolygon.size(); i++)
	{
		forbidPoint[i].setX(forbid.ptPolygon[i].x);
		forbidPoint[i].setY(forbid.ptPolygon[i].y);
	}
	painter.drawPolygon(forbidPoint , forbid.ptPolygon.size());
}

void ImageEditor::DrawThreat(QPainter & painter , iPolygon &threat)
{
	painter.setPen(QPen(Qt::yellow , 1));

	QPointF * threatPoint = new QPointF[threat.ptPolygon.size()];

	for (int i = 0 ; i < threat.ptPolygon.size(); i++)
	{
		threatPoint[i].setX(threat.ptPolygon[i].x);
		threatPoint[i].setY(threat.ptPolygon[i].y);
	}
	painter.drawPolygon(threatPoint , threat.ptPolygon.size());
}

void ImageEditor::mouseReleaseEvent ( QMouseEvent  * event)
{
	if (Qt::LeftButton != event->button())
		return;

	switch (isForT)
	{
	case -1:
		return;
	case 1:
		iPoint pt;
		pt.x = beginPoint.rx();
		pt.y = beginPoint.ry();
		guidPoint.push_back(pt);
		break;

	default:
		return;
	}
	update();
}

void ImageEditor::mousePressEvent(QMouseEvent * event)
{
	if (Qt::LeftButton != event->button())
		return;

	switch (isForT)
	{
	case -1:
		return;
	case 1:
		beginPoint = event->pos();
		break;
	default:
		return;
	}
}


void ImageEditor::SetImage(QString imgName) 
{ 
	image.load(imgName);
 	resize(image.size());//改变窗体的大小以便完全显示图片
	update(); 
	updateGeometry(); 
} 

void ImageEditor::DrawBeginAndEnd(QPainter & painter , QPoint &pt)
{
	painter.setPen(QPen(Qt::green , 1));
	painter.setBrush(QBrush(Qt::green));

	QPointF pta[3] = {QPointF(pt.rx() , pt.ry()-10),QPointF(pt.rx() - 10 , pt.ry()+10) , QPointF(pt.rx() + 10 , pt.ry()+10)};
	painter.drawPolygon(pta , 3);
}

void ImageEditor::paintEvent(QPaintEvent * /*event*/) 
{ 
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	if (!image.isNull())  
	{ 
		painter.drawImage(QPoint(0, 0), image);
	}

	//画规划路线
	if (!ShowRoute.empty())
	{
		DrawPath(painter , ShowRoute);
	}

	if (forbidAndThreatVector.size() != 0)
	{
		for (int i = 0 ; i < forbidAndThreatVector.size() ; i++)
		{	//画禁飞区
			if (forbidAndThreatVector[i].level == 1 || forbidAndThreatVector[i].level == 2)
			{
				DrawForbid(painter , forbidAndThreatVector[i]);
			}
			else//画威胁区
			{
				DrawThreat(painter , forbidAndThreatVector[i]);
			}
		}
	}

	if (!start.isNull())
	{
		DrawBeginAndEnd(painter , start);
	}

	if (!end.isNull())
	{
		DrawBeginAndEnd(painter , end);
// 		QPainterPath path;
// 		path.moveTo(start);
// 		path.lineTo(end);
// 		painter.drawPath(path);
	}

	if (!guidPoint.empty())
	{
		DrawGuidPoint(painter , guidPoint);
	}
}

void ImageEditor::DrawPath(QPainter & painter ,std::vector<iPoint> &route)
{
	if (route.empty())
	{
		return;
	}

	//changed by hj 2013.11.19
	painter.setPen(QPen(Qt::blue , 1));
	QPainterPath path;
	QPoint pt;
	pt.setX(route[0].x);
	pt.setY(route[0].y);
	path.moveTo(pt);

	for(int i = 1 ; i < route.size() ; i++)
	{
		pt.setX(route[i].x);
		pt.setY(route[i].y);
		if(route[i].x == route[0].x && route[i].y == route[0].y)
		{
			path.moveTo(pt);
		}
		else
		{
		    path.lineTo(pt);
		}
	}

	painter.drawPath(path);
}


void ImageEditor::DrawGuidPoint(QPainter & painter , std::vector<iPoint> &guid)
{
	painter.setPen(QPen(Qt::darkBlue, 1));
	painter.setBrush(QBrush(Qt::darkBlue));

	for (int i = 0 ; i < guid.size() ; i++)
	{
		painter.drawEllipse(QPoint(guid[i].x , guid[i].y) , 4 , 4);
	}
}