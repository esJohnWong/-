#include "imageviewer.h"
#include <QtGui>


ImageViewer::ImageViewer(QWidget * parent):QScrollArea(parent)
{
	//����ͼƬ����
	imgEditor = new ImageEditor(this);
	imgEditor->setBackgroundRole(QPalette::Dark);
	imgEditor->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	//���ù�����������
	setBackgroundRole(QPalette::Dark);
	setFocusPolicy(Qt::NoFocus);
	setFrameStyle(QFrame::NoFrame);
	setWidget(imgEditor);
}

ImageViewer::~ImageViewer(void)
{
}
