#include "imageviewer.h"
#include <QtGui>


ImageViewer::ImageViewer(QWidget * parent):QScrollArea(parent)
{
	//设置图片属性
	imgEditor = new ImageEditor(this);
	imgEditor->setBackgroundRole(QPalette::Dark);
	imgEditor->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	//设置滚动窗口属性
	setBackgroundRole(QPalette::Dark);
	setFocusPolicy(Qt::NoFocus);
	setFrameStyle(QFrame::NoFrame);
	setWidget(imgEditor);
}

ImageViewer::~ImageViewer(void)
{
}
