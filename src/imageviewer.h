/************************************************************************/
/* ͼƬ��ʾ������Ҫ�����ʾ����(����)ʱ��������Ӵ���                          */
/***********************************************************************/
#pragma once
#include <QScrollArea>
#include "imageeditor.h"

class ImageViewer :
	public QScrollArea
{
	Q_OBJECT
public:
	ImageViewer(QWidget * parent = NULL);
	~ImageViewer(void);

public:
	ImageEditor * imgEditor;//��Ҫ��ʾ��ͼƬ����
};

