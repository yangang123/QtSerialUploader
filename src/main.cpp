
#include <QApplication>
#include "mainwindow.h"
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置字体，避免在小尺寸电脑上，窗口显示错位
    QFont font(QStringLiteral("新宋体"));
    font.setPixelSize(12);
    a.setFont(font);
    MainWindow::getInstance()->show();

    return a.exec();
}
