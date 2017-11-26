#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "Comm.h"
double COLUMN_0 =  (1.0/3.0);         //Column
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
    qDebug()<< u8"Thread Start main" <<QThread::currentThread() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    comm = new COMM;
    if(comm == NULL) return;

    treeView_init();
    tableView_init();
}

MainWindow::~MainWindow()
{
    delete ui;
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


//bool MainWindow::event(QEvent *event)
//{
//    qDebug() << "get an event" << event->type();
//    return true;
//}

#define TAB_HIGH 25
void MainWindow::resizeEvent(QResizeEvent* event)
{
    qDebug() << "get an event" << event->type() <<COLUMN_0;
    int xtab = event->size().width();
    int ytab = event->size().height() - ui->mainToolBar->height() - ui->statusBar->height()-ui->menuBar->height();
    if(event->type()== QEvent::Resize )
    {
        ui->tabWidget->setGeometry(0,0,xtab,ytab);
        if(ui->tabWidget->currentIndex() == 0 ) // tab标签是: "通讯基本测试"
        {
            int xval = xtab*COLUMN_0;
            int yval = ytab - TAB_HIGH;
            ui->treeView->setGeometry(0,0,xval,yval);
            ui->line_1->setGeometry(xval,0,ui->line_1->width(),yval);
            xval += ui->line_1->width();
            ui->tableView->setGeometry(xval,0,xtab - xval - 4, yval);
        }
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int xMouse = event->globalX() - this->x();
    int xtab = ui->tabWidget->width();
    int ytab = ui->tabWidget->height();
    if(event->type()== QEvent::MouseMove )
    {
        if(ui->tabWidget->currentIndex() == 0 ) // tab标签是: "通讯基本测试"
        {
            if(double(xMouse) < double(xtab)*0.95 ) COLUMN_0 = double(xMouse) / double(xtab); else return;
            int xval = xtab *COLUMN_0;
            int yval = ytab - TAB_HIGH;
            ui->treeView->setGeometry(0,0,xval,yval);
            ui->line_1->setGeometry(xval,0,ui->line_1->width(),yval);
            xval += ui->line_1->width();
            ui->tableView->setGeometry(xval,0,xtab - xval - 4, yval);
        }
    }
    qDebug() << "COLUMN"  <<COLUMN_0;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    int xtab = ui->tabWidget->width();
    int ytab = ui->tabWidget->height();
    if(index == 0 ) // tab标签是: "通讯基本测试"
    {
        int xval = xtab*COLUMN_0;
        int yval = ytab - TAB_HIGH;
        ui->treeView->setGeometry(0,0,xval,yval);
        ui->line_1->setGeometry(xval,0,ui->line_1->width(),yval);
        xval += ui->line_1->width();
        ui->tableView->setGeometry(xval,0,xtab - xval - 4, yval);
    }
}
int MainWindow::treeView_init()
{
    mtree = new QStandardItemModel(ui->treeView);
    mtree->setHorizontalHeaderLabels(QStringList()<<u8"项目名" <<u8"信息");
    ui->treeView->setModel(mtree);
    ui->treeView->setEditTriggers(QTableView::NoEditTriggers);
    ui->treeView->setSelectionBehavior(QTableView::SelectRows);
    ui->treeView->setColumnWidth(0,120);
    connect(this,SIGNAL(s_treeItem_add(int,int,const QString&,const QStringList&)),this, SLOT(treeItem_add(int,int,const QString&,const QStringList&)));

    //emit treeItem_add(5,u8"测试",QStringList() << u8"aaa"<<u8"bbb"<<u8"ccc"<<u8"ddd");
    return 0;
}
int MainWindow::tableView_init()
{
    mtable = new QStandardItemModel(ui->tableView);
    mtable->setColumnCount(5);
    mtable->setHorizontalHeaderLabels(QStringList()<< u8"时间" << u8"通道" <<u8"方向" << u8"类型" << u8"报文");
    int c[] = {80,40,40,40,300};
    ui->tableView->setModel(mtable);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    for(int i = 0; i != 5; i++)
    {
    ui->tableView->setColumnWidth(i,c[i]);
    }
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    connect(this,SIGNAL(s_tableItem_add(int ,int ,int , QString)),this,SLOT(tableItem_add(int ,int ,int , QString )));
//    for(int i = 0; i!=100; i++)
//    {
//        emit s_tableItem_add(QStringList()<<u8"程序启动");
//    }
    //以下新建一个右键菜单
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu); //自定义上下文菜单
    qm_table = new QMenu(ui->tableView);

    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(tableView_rightMenu(QPoint)));
    return 0;
}
void MainWindow::tableItem_add(int id,int dir,int type, const QString& data)
{
    int row = mtable->rowCount();
    mtable->setItem(row,0,new QStandardItem(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
    mtable->setItem(row,1,new QStandardItem(QString::number(id)));
    char dir_str[][16] = {
        u8"发送",
        u8"接收",
    };
    if(dir != 0 && dir <=2)
        mtable->setItem(row,2,new QStandardItem(QString(dir_str[dir - 1])));
    char type_str[][16] = {
        u8"报文",
        u8"信息",
    };
    if(type != 0 && dir <=2)
        mtable->setItem(row,3,new QStandardItem(QString(type_str[type - 1])));
    if(! data.isEmpty())
        mtable->setItem(row,4,new QStandardItem(data));
    //ui->tableView->verticalScrollBar()->setValue(ui->tableView->verticalScrollBar()->maximum());//滚动到最底位
    ui->tableView->scrollToBottom();
}

void MainWindow::tableItem_del()
{
    mtable->removeRows(0,mtable->rowCount());
}

void MainWindow::tableView_rightMenu(const QPoint &pos)
{
    QModelIndex index =  ui->tableView->indexAt(pos);
#define CMENU_NUM 5
    char cmenu[][64] = {
        u8"清空报文",
        u8"暂停\\恢复滚动",
        u8"保存所有报文",
        u8"复制当前行",
        u8"复制当前单元",
    };
    QAction* qa[CMENU_NUM] = {};
    qm_table->clear();
    for (int i = 0; i !=(index.isValid()?CMENU_NUM:3)  ; i++)
    {
        if(qa[i]){delete qa[i];qa[i] = NULL;}
        qa[i] =  new QAction (ui->tableView);
        qa[i]->setText(cmenu[i]);
        qm_table->addAction(qa[i]);
    }

    connect(qa[0], SIGNAL(triggered()), this, SLOT(tableItem_del())); //添加鼠标点击右键菜单调用的函数
    qm_table->exec(QCursor::pos());
    for (int i = 0; i !=(index.isValid()?CMENU_NUM:3)  ; i++)
    {
        if(qa[i]){delete qa[i];}
    }
}

void MainWindow::treeItem_add(int id, int type, const QString & info,const QStringList& ql)
{
    QStandardItem* item = new QStandardItem(QString::number(id));
    int row = 0;
    for(int i = 0; i < ql.size();row ++)
    {
        item->appendRow(new QStandardItem(ql.at(i++)));
        if(i >= ql.size()) break;
        item->setChild(row,1,new QStandardItem(ql.at(i++)));
    }
    if(type == TYPE_TCPA || type == TYPE_TCPC || type == TYPE_UDP)
    {
        item->appendRow(new QStandardItem(u8"发送"));
        item->setChild(row++,1,new QStandardItem(QString(u8"11 22 33 44")));
        item->appendRow(new QStandardItem(QString(u8"定时发送停止")));
        item->setChild(row++,1,new QStandardItem(QString(u8"1000")));
    }

    item->appendRow(new QStandardItem(QString(u8"关闭")));
    mtree->appendRow(item);
    mtree->setItem(mtree->rowCount()-1,1,new QStandardItem(info));
}


void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QModelIndex pf = index.parent();
    if(! pf.isValid() ) return;
    int id = pf.data(0).toInt();
    if(index.column() == 0)
    {
        if(index.data().toString().compare(QString(u8"关闭")) == 0)
        {
            mtree->removeRow(pf.row());
            emit comm->s_delete_channel(id);//删除通道
        }
        else if(index.data().toString().compare(QString(u8"发送")) == 0)
        {
            QString buf = pf.child(index.row(),1).data().toString();
            emit comm->s_TCP_Write(id,buf);
            emit s_tableItem_add(id,1,1,buf);
        }
        else if(index.data().toString().compare(QString(u8"定时发送停止")) == 0)
        {
            QStandardItem* item = mtree->itemFromIndex(index);
            item->setText(QString(u8"定时发送开始"));
            //以下开始定时发送
        }
        else if(index.data().toString().compare(QString(u8"定时发送开始")) == 0)
        {
            QStandardItem* item = mtree->itemFromIndex(index);
            item->setText(QString(u8"定时发送停止"));
            //以下停止定时发送
        }
    }
    else if(index.column() == 1)
    {
        QModelIndex pr = pf.child(index.row(),0);
        if(pr.data().toString().compare(QString(u8"发送")) == 0)
        {
            bool isOK;
            QString text = QInputDialog::getText(this,
                                                 u8"请输入",
                                                 u8"请输入报文内容                                                                    ",
                                                 QLineEdit::Normal,
                                                 u8"11 22 33 44 55",
                                                 &isOK);
            if(isOK)
            {
                QStandardItem* item = mtree->itemFromIndex(index);
                item->setText(text);
            }
        }
        else if(pr.data().toString().startsWith(QString(u8"定时发送")))
        {
            bool isOK;
            QString text = QInputDialog::getText(this,
                                                 u8"请输入",
                                                 u8"请输入定时发送的时间间隔(单位:ms)",
                                                 QLineEdit::Normal,
                                                 u8"1000",
                                                 &isOK);
            if(isOK)
            {
                QStandardItem* item = mtree->itemFromIndex(index);
                item->setText(text);
                if(pr.data().toString().compare(QString(u8"定时发送开始")) == 0)
                {
                    on_treeView_doubleClicked(pr);
                }
                on_treeView_doubleClicked(pr);
            }
        }
    }

}


