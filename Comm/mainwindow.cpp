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
    ui->treeView->setEditTriggers(QTableView::NoEditTriggers);
    ui->treeView->setSelectionBehavior(QTableView::SelectRows);

    mtable = new QStandardItemModel(ui->tableView);
    mtable->setColumnCount(5);
    QStringList slist;
    slist << u8"时间" << u8"通道" <<u8"方向" << u8"类型" << u8"报文";
    int c[] = {80,40,40,40,300};
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
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
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

void MainWindow::treeAddItem(int type, int id, QStringList& qinfo) //qinfo 数量需要为奇数个,第一个为通道信息,剩下的连续成对
{
#define _TREEITEM_NUM_ 4
    type -= TYPE_TCPS;
    if(type >= _TREEITEM_NUM_) return;
    typedef struct _TREEITEM_{
        char cpic[32];
        char ctype[32];
        char cinfo[32];
    }TREEITEM;
    TREEITEM treeitem[] ={
        {":/res/tcps.png", "TCPS", "TCP Server"},  // tcp server
        {":/res/tcpa.png", "TCPA", "TCP Server Accept"},  //tcp server accept
        {":/res/tcpc.png", "TCPC", "TCP Client"}, //tcp client
        {":/res/udp.png", "UDP", "UDP"}, //UDP
    };
    QStringList sl; //0: 图片信息,1:通道信息,
    sl.append(treeitem[type].cpic);
    sl.append(QString::number(id));
    sl.append(treeitem[type].cinfo);
    sl.append(treeitem[type].ctype);
    sl += qinfo;
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
    item->appendRow(new QStandardItem(QString(u8"关闭")));
    mtree->appendRow(item);
    mtree->setItem(mtree->indexFromItem(item).row(),1,new QStandardItem(sl.at(2)));
}

void MainWindow::treeDelItem(int ID)
{
    for(int i = 0; i != mtree->rowCount(); i++)
    {
        if(ID == mtree->item(i,0)->text().toInt())
        {
            mtree->removeRow(i);
            break;
        }
    }
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
    ui->tableView->verticalScrollBar()->setValue(ui->tableView->verticalScrollBar()->maximum());//滚动到最底位
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

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QModelIndex pp = index.parent();
    int id = pp.data().toInt();
   // qDebug() << u8"p"<< pp.row()<< pp.column() << u8"index"<< index.row()<<index.column()<<index.child(index.row(),1);
    if(index.column() == 0)
    {
        if(index.data().toString().compare(QString(u8"发送")) == 0)
        {
            Comm->mutex.lock();
            int x = Comm->Count;
            int i = 0; //防止程序跑飞
            QModelIndex qq = pp.child(index.row(),1);
            for(COMM* p = Comm; p != NULL && i != x; i++, p = p->next)
            {
                if(p->NO == id)
                {
                    switch(p->TYPE)
                    {
                    case TYPE_TCPS:
                    case TYPE_TCPA:
                    case TYPE_TCPC:
                        emit p->s_TCP_write(qq.data().toString());
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }
            Comm->mutex.unlock();
            return;
        }
        else if(index.data().toString().compare(QString(u8"关闭")) == 0) //关闭当前的通道
        {
            Comm->mutex.lock();
            for(COMM* p = Comm; p != NULL; p = p->next)
            {
                if(p->NO == id)
                {
                    switch(p->TYPE)
                    {
                    case TYPE_TCPS:
                    case TYPE_TCPA:
                    case TYPE_TCPC:
                        qDebug ()<< QThread::currentThreadId();
                        Comm->mutex.unlock(); //因为emit可能是类似于中断的机制(emit执行时线程ID是一样的，所以先在这unlock()吧)
                        emit p->s_TCP_disconnect();
                        return;
                    default:
                        break;
                    }
                    break;
                }
            }
            Comm->mutex.unlock();
        }
        else if(index.data().toString().compare(QString(u8"定时发送停止")) == 0)
        {
            QStandardItem* item = mtree->itemFromIndex(index);
            item->setText(QString(u8"定时发送开始"));

            Comm->mutex.lock();
            for(COMM* p = Comm; p != NULL; p = p->next)
            {
                if(p->NO == id)
                {
                    switch(p->TYPE)
                    {
                    case TYPE_TCPA:
                    case TYPE_TCPC:
                        qDebug()<<u8"定时发送开始";
                        emit p->s_SETtimer(pp.child(index.row(),1).data().toInt());
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }
            Comm->mutex.unlock();
        }
        else if(index.data().toString().compare(QString(u8"定时发送开始")) == 0)
        {
            QStandardItem* item = mtree->itemFromIndex(index);
            item->setText(QString(u8"定时发送停止"));
            Comm->mutex.lock();
            for(COMM* p = Comm; p != NULL; p = p->next)
            {
                if(p->NO == id)
                {
                    switch(p->TYPE)
                    {
                    case TYPE_TCPA:
                    case TYPE_TCPC:
                        qDebug()<<u8"定时发送停止";
                        emit p->s_STOPtimer();
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }
            Comm->mutex.unlock();
        }
    }
    else if(index.column() == 1)
    {
        if(pp.child(index.row(),0).data().toString().compare(QString(u8"发送")) == 0) //双击修改发送的报文
        {
            bool isOK;
            QString text = QInputDialog::getText(this, u8"输入", u8"请输入报文内容",QLineEdit::Normal,
                                                               "11 22 33 44 55",
                                                               &isOK);
            if(isOK)
            {
                QStandardItem* item = mtree->itemFromIndex(index);
                item->setText(text);
            }
        }
        else if(pp.child(index.row(),0).data().toString().startsWith(QString(u8"定时发送")))
        {
            bool isOK;
            QString text = QInputDialog::getText(this, u8"输入", u8"请输入定时发送的时间间隔(单位:ms)",QLineEdit::Normal,
                                                               "1000",
                                                               &isOK);
            if(isOK)
            {
                QStandardItem* item = mtree->itemFromIndex(index);
                item->setText(text);
                QModelIndex id_new = pp.child(index.row(),0);
                if(id_new.data().toString().compare(u8"定时发送开始") == 0)
                {
                    on_treeView_doubleClicked(id_new);
                }
                on_treeView_doubleClicked(id_new);
            }
        }
    }

}
