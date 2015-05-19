#include "csnakecore.h"
#include <QtCore/QTextCodec>
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

	CSnakeCore w;
	w.show();
	return a.exec();
}
