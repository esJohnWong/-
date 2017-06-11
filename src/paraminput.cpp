#include "paraminput.h"
#include "DataStruct.h"
#include <QtGui>

ParamInput::ParamInput(QWidget * parent):QDialog(parent)
{
	QLabel * label1 = new QLabel("���ڵ����С����" , this);
	QLabel * label2 = new QLabel("���ڵ��������" , this);
	QLabel * label3 = new QLabel("���ڵ�����Ƕ�" , this);
	QLabel * label4 = new QLabel("���ڵ������¶�" , this);

	QLabel * label1Hint = new QLabel("200~400��" , this);
	QLabel * label2Hint = new QLabel("401~600��" , this);
	QLabel * label3Hint = new QLabel("15  ~  90��" , this);
	QLabel * label4Hint = new QLabel("15  ~  90��" , this);

	okBtn = new QPushButton("ȷ��" , this);
	cancelBtn = new QPushButton("ȡ��" , this);
	connect(okBtn , SIGNAL(clicked()) , this , SLOT(accept()));
	connect(cancelBtn , SIGNAL(clicked()) , this , SLOT(reject()));

	minlEdit = new QLineEdit("200.0" , this);
	maxlEdit = new QLineEdit("400.0" , this);
	maxhangleEdit = new QLineEdit("45.0" , this);
	maxvangleEdit = new QLineEdit("45.0" , this);

	QHBoxLayout * layout1 = new QHBoxLayout;
	layout1->addWidget(label1);
	layout1->addWidget(minlEdit);
	layout1->addWidget(label1Hint);

	QHBoxLayout * layout2 = new QHBoxLayout;
	layout2->addWidget(label2);
	layout2->addWidget(maxlEdit);
	layout2->addWidget(label2Hint);

	QHBoxLayout * layout3 = new QHBoxLayout;
	layout3->addWidget(label3);
	layout3->addWidget(maxhangleEdit);
	layout3->addWidget(label3Hint);

	QHBoxLayout * layout4 = new QHBoxLayout;
	layout4->addWidget(label4);
	layout4->addWidget(maxvangleEdit);
	layout4->addWidget(label4Hint);

	QHBoxLayout * layout5 = new QHBoxLayout;
	layout5->addWidget(okBtn);
	layout5->addWidget(cancelBtn);

	QVBoxLayout * minlayout = new QVBoxLayout;
	minlayout->addLayout(layout1);
	minlayout->addLayout(layout2);
	minlayout->addLayout(layout3);
	minlayout->addLayout(layout4);
	minlayout->addLayout(layout5);

	setLayout(minlayout);

	setWindowTitle("�������");
}


ParamInput::~ParamInput(void)
{
}

bool ParamInput::GetParam(double &minl,  double &maxl, double &maxhangle,double &maxvangle )
{
	minl = minlEdit->text().toDouble();
	maxl = maxlEdit->text().toDouble();
	maxhangle = maxhangleEdit->text().toDouble();
	maxvangle = maxvangleEdit->text().toDouble();

	if (minl >= 200 && minl <= 400)
	{
		minl /= 20;
	}
	else 
	{
		QMessageBox::information(this , "��ʾ" , "���벻�Ϸ�");
		return false;
	}

	if (maxl  >= 400 && maxl <= 600)
	{
		maxl /= 20;
	}
	else 
	{
		QMessageBox::information(this , "��ʾ" , "���벻�Ϸ�");
		return false;
	}

	if (maxhangle >= 15 && maxhangle <= 90)
	{
		maxhangle *= 180/PI;
	}
	else 
	{
		QMessageBox::information(this , "��ʾ" , "���벻�Ϸ�");
		return false;
	}

	if (maxvangle >= 15 && maxvangle <= 90)
	{
		maxvangle *= 180/PI;
	}
	else 
	{
		QMessageBox::information(this , "��ʾ" , "���벻�Ϸ�");
		return false;
	}

	return true;
}
