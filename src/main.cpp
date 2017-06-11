#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"
#include <QIcon>
#include <QString>

int main(int argc, char *argv[])
{
	// 中文支持
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	QApplication app(argc, argv);

	//Q_INIT_RESOURCE(PowerLinePlan);

	QFont TimesFont("Times New Roman", 10, QFont::Normal);
	app.setFont(TimesFont);

	MainWindow mainWin;
	mainWin.showMaximized();
	return app.exec();

}
