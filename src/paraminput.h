/************************************************************************/
/* ��������Ի���		*/
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
	//��ȡ����Ĳ���
	bool GetParam(double &minl,  double &maxl, double &maxhangle,double &maxvangle )  ; 
private:
	QPushButton * okBtn;
	QPushButton * cancelBtn;
	QLineEdit * minlEdit; //���������ڵ�֮�����С����
	QLineEdit * maxlEdit;	//���������ڵ�֮���������
	QLineEdit * maxhangleEdit;	//�����ڵ�֮������ת��Ƕ�
	QLineEdit * maxvangleEdit;	//�����ڵ�֮�������¶�
};

