#include "mainwindow.h"
#include "Comm.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    COMM x;
    COMM y;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
