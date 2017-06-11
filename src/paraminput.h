/************************************************************************/
/* 参数输入对话框		*/
/*add by hj 2013.11.1*/
/************************************************************************/
#pragma once
#include <QDialog>

class QPushButton;
class QLineEdit;
class QString;
class QWidget;

class ParamInput :public QDialog
{
	Q_OBJECT
public:
	ParamInput(QWidget * parent = NULL);
	~ParamInput(void);
	//获取输入的参数
	bool GetParam(double &minl,  double &maxl, double &maxhangle,double &maxvangle )  ; 
private:
	QPushButton * okBtn;
	QPushButton * cancelBtn;
	QLineEdit * minlEdit; //任意两个节点之间的最小距离
	QLineEdit * maxlEdit;	//任意两个节点之间的最大距离
	QLineEdit * maxhangleEdit;	//两个节点之间的最大转弯角度
	QLineEdit * maxvangleEdit;	//两个节点之间的最大坡度
};

