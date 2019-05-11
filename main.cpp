#include "dialog.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QString a1 = QString("123");
//    QByteArray b = a1.toLatin1();

    Dialog::getInstance()->show();
    return a.exec();
}
