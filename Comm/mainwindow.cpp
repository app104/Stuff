#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "Comm.h"
double COLUMN =  (1.0/3.0);         //Column
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPalette palette;
    palette.setBrush(this->backgroundRole(), Qt::gray);
    this->setPalette(palette);

}

MainWindow::~MainWindow()
{
    delete ui;
}

class NewDialog;
void MainWindow::on_actionnew_triggered()
{
    //NewDialog dialog;
    NewDialog* nd = new NewDialog(this);
    nd->show();
}

void MainWindow::on_actionquit_triggered()
{
    this->close();
}
//bool MainWindow::event(QEvent *event)
//{
//    qDebug() << "get an event" << event->type();
//    return true;
//}


void MainWindow::resizeEvent(QResizeEvent* event)
{
    qDebug() << "get an event" << event->type() <<COLUMN;
    if(event->type() == QEvent::Resize)
    {
        int x = event->size().width();
        int y = event->size().height() - ui->mainToolBar->height() - ui->statusBar->height()-ui->menuBar->height() ;
        int x1 = x*COLUMN;
        int x2 = ui->line->width();
        ui->treeView->setGeometry(0, 0, x1, y);
        ui->line->setGeometry(x1, 0, x2, y);
        ui->listView->setGeometry(x1+x2, 0, x - x1 -x2, y);
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "get an event" << event->type()<<"   " <<COLUMN;
    if(event->type() == QEvent::MouseMove)
    {
        int xMouse = event->globalX() - this->x();
        int x = this->width();
        int y = this->height() - ui->mainToolBar->height() - ui->statusBar->height()-ui->menuBar->height() ;
        if(xMouse < x ) COLUMN = double(xMouse) / double(x); else return;
        int x1 = x*COLUMN;
        int x2 = ui->line->width();
        ui->treeView->setGeometry(0, 0, x1, y);
        ui->line->setGeometry(x1, 0, x2, y);
        ui->listView->setGeometry(x1+x2, 0, x - x1 -x2, y);
    }
    qDebug() << "COLUMN"  <<COLUMN;
}
