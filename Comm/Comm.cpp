#include "Comm.h"
#include "mainwindow.h"
#include <string.h>
Qt::HANDLE COMM::tid = 0;
int COMM::tid_run = 1;
QList<SCOMM> COMM::sl;
COMM * COMM::fcomm = NULL;
QMutex COMM::mutex;
COMM::COMM()
{
    init();
}

COMM::~COMM()
{
    if( tid != 0) //有个线程在跑
    {
        if(fcomm == this) // 如果第一个线程要退出,那么线程就退出
        {
            emit s_tid_quit();
            this->wait();
            tid = 0;
            fcomm = NULL;
        }
    }
}
void COMM::init()
{
    if (fcomm == NULL)
    {
        fcomm = this;
        connect(this,SIGNAL(s_tableItem_add(const QStringList&)),gui,SLOT(tableItem_add(const QStringList&)));
        connect(this,\
                SIGNAL(s_treeItem_add(int, const QString &,const QStringList&)),\
                gui,\
                SLOT(treeItem_add(int, const QString &,const QStringList&)));
    }
    else
    {
        connect(this,SIGNAL(s_new_channel(SCOMM)),fcomm,SLOT(new_channel(SCOMM)),Qt::QueuedConnection); //用于通知线程,来了一个新的通道
    }
    if(tid == 0)
    {
        sl.clear();
        this->start();
        connect(this,SIGNAL(s_tid_quit()),this,SLOT(tid_quit()),Qt::QueuedConnection);
    }
}
void COMM::new_channel(const SCOMM & s)
{
    mutex.lock();
    sl.push_back(s);
    mutex.unlock();
}

void COMM::set_channel(QList<SCOMM>::iterator it) //在独立线程中运行
{
#define s_tcps ((QTcpServer*)sock)
    switch (it->TYPE) {
    case TYPE_TCPS:
        s_tcps = new QTcpServer();
        if(true == s_tcps->listen((QHostAddress)(it->LIP),quint16(it->LPORT)))
        {
            connect(this,SIGNAL(newConnection()),this,SLOT(TCPS_newConnection()),Qt::QueuedConnection);
            connect(this,\
                    SIGNAL(acceptError(QAbstractSocket::SocketError socketError)),\
                    this,\
                    SLOT(TCPS_acceptError(QAbstractSocket::SocketError socketError)));
        }
        else
        {
            emit s_treeItem_add(it->NO,QString(u8"TCP Server"),);
            emit s_tableItem_add();
        }
        break;
    case TYPE_TCPA:
        break;
    default:
        break;
    }
}
void COMM::set_TCPS(char* lip, int lport)
{
    SCOMM tcps = {};
    tcps.TYPE = TYPE_TCPS;
    strncmp( tcps.LIP,lip,IP_LEN);
    tcps.LPORT = lport;
    emit s_new_channel(tcps);
}
void COMM::TCPS_newConnection()
{
    qDebug << "新的TCPS连接";
}

void COMM::run()
{
    while (tid_run) {
        mutex.lock();
        //检查有没有新的通道,或没有用的通道
        for(QList<SCOMM>::iterator i = sl.begin(); i != sl.end(); i++)
        {
            if(i->sock == NULL)//新通道
            {
                set_channel(i);
            }
        }
        mutex.unlock();
        this->sleep(1);
        qDebug()<< u8"在线程中" <<this->currentThreadId() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    }
}

int is_valid_ip(char* ip)
{
    int a = 0, b = 0, c = 0, d = 0;
    int len = sscanf(ip,"%d.%d.%d.%d",&a,&b,&c,&d);
    if(len == 4)
    {
        if (a >= 0 && a <= 255 && \
            b >= 0 && b <= 255 && \
            c >= 0 && c <= 255 && \
            d >= 0 && d <= 255 )
        {
            return 1;
        }
    }
    return 0;
}
