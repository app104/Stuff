#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "Comm.h"
double COLUMN =  (1.0/3.0);         //Column
MainWindow * gui;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gui = this;
    //改变窗口背景
    QPalette palette;
    palette.setBrush(this->backgroundRole(), Qt::gray);
    this->setPalette(palette);
    //改变treeView;
    mtree = new QStandardItemModel(ui->treeView);
    mtree->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("项目名")<<QStringLiteral("信息"));
    ui->treeView->setModel(mtree);
    mtable = new QStandardItemModel(ui->tableView);
    mtable->setColumnCount(5);
    QStringList slist;
    slist << u8"时间" << u8"通道" <<u8"方向" << u8"类型" << u8"报文";
    int c[] = {100,50,50,50,500};
    for(int i = 0; i != 5; i++)
    {
        mtable->setHeaderData(i,Qt::Horizontal,slist.at(i));
    }

    ui->tableView->setModel(mtable);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    for(int i = 0; i != 5; i++)
    {
        ui->tableView->setColumnWidth(i,c[i]);
    }
//    COMM* comm = new COMM;
//    connect(Comm,SIGNAL(s_tableAddItem(QString,QString,QString,QString)),this,SLOT(tableAddItem(QString,QString,QString,QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete mtree;
    delete mtable;

}

class NewDialog;
void MainWindow::on_actionnew_triggered()
{
    NewDialog* nd = new NewDialog(this);
    nd->show();
}

void MainWindow::on_actionquit_triggered()
{
    this->close();
}

void MainWindow::treeAddItem(COMM *comm)
{
    QStringList sl; //0: 图片信息,1:通道信息,
    switch(comm->TYPE)
    {
    case TYPE_TCPS:
        sl.append(":/res/tcps.png");
        sl.append (u8"通道" + QString::number(comm->NO));
        sl.append("TCP Server");
        sl.append(u8"TCPS");  //第三个是第一行信息
        sl.append(QString(u8"本地:")+ comm->LIP + QString::number(comm->LPORT));
        break;
    case TYPE_TCPA:
        break;
    case TYPE_TCPC:
        break;
    case TYPE_UDP:
        break;
    default:
        break;
    }

    QStandardItem* item = new QStandardItem(QIcon(sl.at(0)), sl.at(1));
    QList<QString>::Iterator it = sl.begin() + 3,itend = sl.end();
    int i = 0;
    while (1) {
        if(it >= itend) break;
        item->appendRow(new QStandardItem(*it));
        it ++;
        if(it >= itend) break;
        item->setChild(i++,1,new QStandardItem(*it));
        it ++;
    }
    mtree->appendRow(item);
    mtree->setItem(mtree->indexFromItem(item).row(),1,new QStandardItem(sl.at(3)));
}

void MainWindow::treeDelItem(int ID)
{
    int x = ID;
    x = 0;
}

void MainWindow::tableAddItem(const QString &channel, const QString &dir, const QString &type, const QString &data)
{
//    static int x = 0;
//    qDebug() << u8"收到tableAddItem的调用" << x++;
    int rownum = mtable->rowCount();
    mtable->setItem(rownum,0,new QStandardItem(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
    if(channel.size()) mtable->setItem(rownum,1,new QStandardItem(channel));
    if(dir.size()) mtable->setItem(rownum,2,new QStandardItem(dir));
    if(type.size()) mtable->setItem(rownum,3,new QStandardItem(type));
    if(data.size()) mtable->setItem(rownum,4,new QStandardItem(data));
}
void MainWindow::tableClear()
{
    mtable->removeRows(0,mtable->rowCount());//Item的new已经在此delete了,不用手动delete
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
        ui->tableView->setGeometry(x1+x2, 0, x - x1 -x2, y);
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
        ui->tableView->setGeometry(x1+x2, 0, x - x1 -x2, y);
    }
    qDebug() << "COLUMN"  <<COLUMN;
}
