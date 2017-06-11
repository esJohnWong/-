/************************************************************************/
/*  ͼƬ������������Ҫ��ͼʱ��������Ӵ���                                                                    */
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
	void SetImage(QString imgName); //����Ҫ��ʾ��ͼƬ
	QImage GetImage() const{ return image; } 
	QSize sizeHint() const; 

	//�ж��ǻ�������������в��
	int isForT;

	//չʾ·��  add by mj at 11-15
	std::vector<iPoint> ShowRoute;  

	//���滮��·��
	void DrawPath(QPainter & painter , std::vector<iPoint> &route);
	//�����ɷ���
	void DrawForbid(QPainter & painter , iPolygon & forbid);
	void DrawThreat(QPainter & painter , iPolygon &threat);

	void DrawBeginAndEnd(QPainter & painter , QPoint &pt);

	void DrawGuidPoint(QPainter & painter , std::vector<iPoint> &guid);
protected: 
	void paintEvent(QPaintEvent *event); //�Ի洦��

	//����¼����ڻ�ý�������в������
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent ( QMouseEvent  * event);
private: 
	QImage image; //ͼƬ
	//int zoom; //��������
	//������갴�º͵���������
	QPoint beginPoint;

public:
	//����������
	std::vector<iPoint> guidPoint;

	//��·�����յ�
	QPoint start , end;
	//��в����������Ϣ
	vector<iPolygon> forbidAndThreatVector;
};

