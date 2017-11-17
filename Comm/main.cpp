#include "mainwindow.h"
#include "Comm.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    COMM *comm = new COMM;
    if(comm == NULL) return 1;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
