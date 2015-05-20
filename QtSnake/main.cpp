#include "csnakecore.h"
#include <QtCore/QTextCodec>
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));

	CSnakeCore w;
	w.show();
	return a.exec();
}
