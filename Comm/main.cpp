#include "mainwindow.h"
#include "Comm.h"
#include <QApplication>

Comm gcomm;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
