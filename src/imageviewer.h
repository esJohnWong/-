/************************************************************************/
/* 图片显示器，当要添加显示功能(界面)时在这里添加处理                          */
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
	ImageEditor * imgEditor;//需要显示的图片窗口
};

