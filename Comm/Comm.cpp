#include "Comm.h"
#include "mainwindow.h"
#include <string.h>
COMM::COMM()
{
    init();
}

COMM::~COMM()
{

}
void COMM::init()
{
    NO = 0;
    sl.clear();
    connect(this,SIGNAL(s_new_channal()),this,SLOT(new_channal()),Qt::QueuedConnection);
    connect(this,SIGNAL(s_delete_channel(int)),this,SLOT(delete_channel(int)),Qt::QueuedConnection);
    connect(this,SIGNAL(s_new_timer(int,int)),this,SLOT(new_timer(int,int)),Qt::QueuedConnection);
    connect(this,SIGNAL(s_delete_timer(int)),this,SLOT(delete_timer(int)),Qt::QueuedConnection);
}
void COMM::timerEvent(QTimerEvent * event)
{
    qDebug()<<"timerEvent";
    //mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if(event->timerId() == it->TIMEID)
        {
            if(it->TYPE == TYPE_TCPA || it->TYPE == TYPE_TCPC)
            {
                emit TCP_Write(it->NO,QString(u8"22 33 44 55 66"));
            }
        }
    }
   // mutex.unlock();
}

void COMM::new_timer(int id, int interval)
{
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if(id == it->NO && it->TIMEID == 0)
        {
            it->TIMEID = startTimer(interval);
        }
    }
    mutex.unlock();
}
void COMM::delete_timer(int id)
{
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if(id == it->NO && it->TIMEID != 0)
        {
            killTimer(id);
            it->TIMEID = 0;
        }
    }
    mutex.unlock();
}

void COMM::new_channal()
{
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if(it->sock == NULL)
        {
            switch (it->TYPE) {
            case TYPE_TCPS:
#define tcps_sock ((QTcpServer*)(it->sock))
                it->sock = new QTcpServer;
                if(tcps_sock->listen(QHostAddress(it->LIP),(it->LPORT)))
                {
                    connect(tcps_sock,SIGNAL(newConnection()),this,SLOT(TCPS_newConnection()),Qt::QueuedConnection);
                    connect(tcps_sock,\
                            SIGNAL(acceptError(QAbstractSocket::SocketError)),\
                            this,\
                            SLOT(TCPS_acceptError(QAbstractSocket::SocketError)));
                    QString str = QString(u8"本地%1:%2").arg(QString(it->LIP)).arg(it->LPORT);
                    emit gui->s_tableItem_add(it->NO,0,0,QString(u8"Success:打开TCPS服务,%1").arg(str));
                    emit gui->s_treeItem_add(it->NO,TYPE_TCPS, QString("TCP Server"),QStringList("TCPS")<<str);
                }
                else
                {
                    emit gui->s_tableItem_add(it->NO,0,0,\
                                              QString(u8"Fault:不能打开TCPS服务,本地%1:%2")\
                                              .arg(QString(it->LIP))\
                                              .arg(it->LPORT));
                }
#undef tcps_sock
                break;
            case TYPE_TCPC:
                break;
            default:
                break;
            }
        }
    }
    mutex.unlock();
}
void COMM::delete_channel(int id)
{
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if(it->NO == id)
        {
            if(it->TIMEID) killTimer(it->TIMEID);
            switch (it->TYPE) {
            case TYPE_TCPS:
#define tcps_sock ((QTcpServer*)(it->sock))
                tcps_sock->close();
                emit gui->s_tableItem_add(it->NO,0,0,\
                                          QString(u8"Notice:关闭TCPS服务,本地%1:%2")\
                                          .arg(QString(it->LIP))\
                                          .arg(it->LPORT));
#undef  tcps_sock
                break;
            default:
                break;
            }
            sl.erase(it);
            break; //跳出循环
        }
    }
    mutex.unlock();
}
void COMM::set_TCPS(char* lip, int lport)
{
    SCOMM co = {};
    co.TYPE = TYPE_TCPS;
    strncpy(co.LIP,lip,IP_LEN);
    co.LPORT = lport;
    co.NO = NO++;
    mutex.lock();
    sl.append(co);
    mutex.unlock();
    emit this->s_new_channal();
}
void COMM::TCPS_newConnection()
{
    qDebug()<< "TCPS_newConnection";
    SCOMM sc = {};
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if(it->TYPE == TYPE_TCPS && ((QTcpServer*)(it->sock))->hasPendingConnections())
        {
            sc.sock = ((QTcpServer*)(it->sock))->nextPendingConnection();
            sc.TYPE = TYPE_TCPA;
    #define tcpa_sock ((QTcpSocket*)(sc.sock))
            strncpy(sc.LIP,tcpa_sock->localAddress().toString().toLocal8Bit().data(),IP_LEN);
            sc.LPORT = tcpa_sock->localPort();
            strncpy(sc.RIP,tcpa_sock->peerAddress().toString().toLocal8Bit().data(),IP_LEN);
            sc.RPORT = tcpa_sock->peerPort();
            sc.NO = NO++;
            sl.append(sc);
    #undef  tcpa_sock
            break;
        }
    }
    mutex.unlock();
    if(sc.sock) //有tcp server accept
    {
        connect(((QTcpSocket*)(sc.sock)),SIGNAL(readyRead()),this,SLOT(TCP_Read()),Qt::QueuedConnection);
        connect(this,SIGNAL(s_TCP_Write(int ,const QString& )),this,SLOT(TCP_Write(int ,const QString& )),Qt::QueuedConnection);
        connect(((QTcpSocket*)(sc.sock)),SIGNAL(disconnected()),this,SLOT(TCP_disconnected()),Qt::QueuedConnection);
        QString str = QString(u8"本地%1:%2,远端%3:%4").arg(QString(sc.LIP)) .arg(sc.LPORT).arg(sc.RIP).arg(sc.RPORT);
        emit gui->s_tableItem_add(sc.NO,0,0,\
                                  QString(u8"Success:打开TCPS服务,%1").arg(str));
        emit gui->s_treeItem_add(sc.NO,TYPE_TCPA,\
                                 QString("TCP Server Accept"),\
                                 QStringList("TCPA")<<str);
    }

}
void COMM::TCPS_acceptError(QAbstractSocket::SocketError socketError)
{
    qDebug()<< "TCPS_newConnection" << socketError;
}
void COMM::TCP_Read()
{
    qDebug()<< "TCP_Read" ;
    QString buf;
    int id = -1;
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if((it->TYPE == TYPE_TCPA || it->TYPE == TYPE_TCPC))
        {
            buf= ((QTcpSocket*)(it->sock))->readAll();
            id = it->NO;
            break;
        }
    }
    mutex.unlock();
    if(buf.size())
    {
        emit gui->s_tableItem_add(id,2,1,QString(buf));
    }
}
void COMM::TCP_Write(int id,const QString& buf)
{
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if(id == it->NO && (it->TYPE == TYPE_TCPA || it->TYPE == TYPE_TCPC))
        {
            ((QTcpSocket*)(it->sock))->write(buf.toLocal8Bit().data());
            break;
        }
    }
    mutex.unlock();
    emit gui->s_tableItem_add(id,1,1,buf);
}
void COMM::TCP_disconnected()
{
    qDebug()<< "TCP_disconnected" ;
    mutex.lock();
    for(QList<SCOMM>::iterator it = sl.begin();it!=sl.end();it++)
    {
        if((it->TYPE == TYPE_TCPA || it->TYPE == TYPE_TCPC))
        {
            ((QTcpSocket*)(it->sock))->close();
            sl.erase(it);
            emit gui->s_tableItem_add(it->NO,0,0,\
                                      QString(u8"Notice:关闭TCP连接,本地%1:%2,远端%3:%4")\
                                      .arg(QString(it->LIP)) .arg(it->LPORT)\
                                        .arg(QString(it->RIP)) .arg(it->RPORT));
            break;
        }
    }
    mutex.unlock();
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
