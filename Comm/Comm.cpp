#include "Comm.h"
#include "mainwindow.h"
#include <string.h>
Qt::HANDLE COMM::tid = 0;
int COMM::tid_run = 1;
QList<SCOMM> COMM::sl;
COMM * COMM::fcomm = NULL;
QMutex COMM::mutex;
int COMM::NO;
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
        connect(this,\
                SIGNAL(s_delete_channel(int)),\
                this,
                SLOT(delete_channel(int)));
    }
    else
    {
        connect(this,SIGNAL(s_new_channel(SCOMM*)),fcomm,SLOT(new_channel(SCOMM*)),Qt::QueuedConnection); //用于通知线程,来了一个新的通道
    }
    if(tid == 0)//没有线程在跑
    {
        sl.clear();
        this->start();
        connect(this,SIGNAL(s_tid_quit()),this,SLOT(tid_quit()),Qt::QueuedConnection);
    }
}
void COMM::new_channel(SCOMM * s)
{
    s->NO = (this->NO)++;
    mutex.lock();
    sl.push_back(*s);
    delete s;
    mutex.unlock();
}
void COMM::delete_channel(int id)
{
    qDebug() << u8"关闭通道" << id<<QThread::currentThreadId();
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin(); it != sl.end();it++)
    {
        it->mark |= 1; //此通道将会在run中被关掉
    }
    mutex.unlock();
}
void COMM::set_channel(QList<SCOMM>::iterator it) //在独立线程中运行
{
    char tname[][32] = {
        u8"TCP Server",
        u8"TCPA",
        u8"TCPC",
        u8"UDP",
    };
    switch (it->TYPE)
    {
    case TYPE_TCPS:
        #define s_tcps ((QTcpServer*)(it->sock))
        static QTcpServer* s = new QTcpServer(this);
        it->sock = s;
        if(true == s_tcps->listen((QHostAddress)(it->LIP),quint16(it->LPORT)))
        {
            qDebug()<< u8"线程ID" <<this->currentThreadId();
            connect(s,\
                    SIGNAL(newConnection()),\
                    fcomm,SLOT(TCPS_newConnection()),Qt::DirectConnection);
            connect(s,\
                    SIGNAL(acceptError(QAbstractSocket::SocketError)),\
                    fcomm,\
                    SLOT(TCPS_acceptError(QAbstractSocket::SocketError)),\
                    Qt::DirectConnection);
            QString str = QString(u8"%1:%2").arg(it->LIP).arg(it->LPORT);
            emit gui->s_tableItem_add(it->NO,0,0,QString(u8"成功，打开本地TCPS服务:") + str);
            emit gui->s_treeItem_add(it->NO,TYPE_TCPS,QString(tname[0]),\
                    QStringList(u8"TCPS")<<str);
        }
        else
        {
            emit gui->s_tableItem_add(it->NO,0,0,\
                                 QString(u8"错误，不能打开本地TCPS服务： %1:%2").arg(it->LIP).arg(it->LPORT));
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
    SCOMM* tcps = new SCOMM;
    memset(tcps,0,sizeof(SCOMM));
    tcps->TYPE = TYPE_TCPS;
    strncpy( tcps->LIP,lip,IP_LEN);
    tcps->LPORT = lport;
    emit s_new_channel(tcps);
}
void COMM::TCPS_newConnection()
{
    qDebug() << "新的TCPS连接";
    emit gui->s_tableItem_add(this->NO,0,0,QString(u8"TCPA"));
}
void COMM::TCPS_acceptError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "新的TCPS连接错误" <<socketError;
}
void COMM::run()
{
    if(tid)
    {
        qDebug() << u8"已经有线程在运行了";
        this->exit();
    }
    tid = this->currentThreadId();
    qDebug()<< u8"线程开始" <<this->currentThreadId() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << this->tid;
    while (tid_run) {
        qDebug()<< u8"在线程中" <<this->currentThreadId() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        mutex.lock();
        //检查有没有新的通道,或没有用的通道
        for(QList<SCOMM>::iterator i = sl.begin(); i != sl.end(); i++)
        {
            if(i->sock == NULL)//新通道
            {
                set_channel(i);
                continue;
            }
            else if(i->mark & 0x01) //关闭通道
            {
                switch (i->TYPE)
                {
                case TYPE_TCPS:
                    ((QTcpServer*)(i->sock))->close();
                    emit gui->s_tableItem_add(i->NO,0,0,QString(u8"关闭通道%1 %2:%3").arg(i->NO).arg(i->LIP).arg(i->LPORT));
                    break;
                default:
                    continue;//继续for循环
                }
                sl.erase(i); //此句和下一句一起用，不可删
                break;
            }
        }
        mutex.unlock();
        this->sleep(1);

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
