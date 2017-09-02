#include "mainwindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow  w; // 先执行构造函数
    w.setWindowTitle(QString::fromLocal8Bit("大气风场"));
    QFont font = w.font();
    font.setPointSize(16);
    w.setFont(font);
    w.show();

//    w.showMaximized();
//    w.showFullScreen();
    w.setMinimumSize(1024, 768);
    return a.exec();

}
