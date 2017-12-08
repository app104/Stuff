#include "Comm.h"
#include "mainwindow.h"
#include <string.h>
int COMM::SEQ = 0;
COMM::COMM()
{
    init();
}

COMM::~COMM()
{
    if(sock)
    {
        switch (TYPE)
        {
        case TYPE_TCPS:
            ((QTcpServer*)sock)->close();
            emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:关闭TCPS L%1:%2").arg(LIP).arg(LPORT));
            break;
        case TYPE_TCPA:
        case TYPE_TCPC:
            ((QTcpSocket*)(sock))->close();
        default:
            break;
        }
        sock = 0;
    }
    if(TIMEID)
    {
        killTimer(TIMEID);
        TIMEID = 0;
    }
}
void COMM::init()
{
    sock = 0;
    TIMEID = 0;
    buf_size = 0;
}
void COMM::timerEvent(QTimerEvent * event)
{
    qDebug()<< "timerEvent" << event;
    WriteData(buf);
}

void COMM::SetTimer(int interval)
{
    if(TIMEID == 0) TIMEID = startTimer(interval);

}
void COMM::StopTimer()
{
    if(TIMEID)
    {
        killTimer(TIMEID);
        TIMEID = 0;
    }
}


void COMM::set_TCPS(char* lip, int lport)
{
    NO = SEQ ++;
    TYPE = TYPE_TCPS;
    strncpy(LIP,lip,IP_LEN);
    LPORT = lport;
    sock = new QTcpServer(this);
    QString str = QString(u8"L:%1:%2").arg(LIP).arg(LPORT);
    if(((QTcpServer*)sock)->listen(QHostAddress(LIP),quint16(LPORT)) == false)
    {
        gui->s_tableItem_add(NO, 0, 0, QString(u8"ERROR:Can\'t open TCP Server %1").arg(str));
        return;
    }
    else
    {
        gui->s_tableItem_add(NO, 0, 0, QString(u8"NOTICE:Open TCP Server %1").arg(str));
        gui->s_treeItem_add(NO,TYPE_TCPS,QString(u8"TCP Server"),QStringList()<<"TCPS"<< str);
        connect(((QTcpServer*)sock),SIGNAL(newConnection()),this,SLOT(TCPS_newConnection()),Qt::AutoConnection);
        connect(((QTcpServer*)sock),\
                SIGNAL(acceptError(QAbstractSocket::SocketError)),\
                this,\
                SLOT(TCPS_acceptError(QAbstractSocket::SocketError)),Qt::AutoConnection);
    }
}


void COMM::TCPS_newConnection()
{
        qDebug()<< "TCPS_newConnection";

        COMM* comm = new COMM;
        comm->sock= ((QTcpServer*)sock)->nextPendingConnection();
        comm->NO = SEQ++;
        comm->TYPE = TYPE_TCPA;
        strncpy(comm->LIP,((QTcpSocket*)(comm->sock))->localAddress().toString().toLocal8Bit().data(),IP_LEN);
        comm->LPORT = ((QTcpSocket*)(comm->sock))->localPort();
        strncpy(comm->RIP,((QTcpSocket*)(comm->sock))->peerAddress().toString().toLocal8Bit().data(),IP_LEN);
        comm->RPORT = ((QTcpSocket*)(comm->sock))->peerPort();
        gui->qcomm.append(comm); // 不考虑数据同步问题

        connect(((QTcpSocket*)(comm->sock)),\
                SIGNAL(stateChanged(QAbstractSocket::SocketState)),\
                comm,\
                SLOT(TCP_stateChanged(QAbstractSocket::SocketState)));
        QString ips = QString("L:%1:%2,R:%3:%4").arg(comm->LIP).arg(comm->LPORT).arg(comm->RIP).arg(comm->RPORT);
        connect(((QTcpSocket*)(comm->sock)),SIGNAL(readyRead()),comm,SLOT(ReadData()));
        emit gui->s_tableItem_add(comm->NO,0,0,QString(u8"NOTICE:TCPS L:%1:%2接收到新连接 %3").arg(comm->LIP).arg(comm->LPORT).arg(ips));
        emit gui->s_treeItem_add(comm->NO,TYPE_TCPA,QString("TCP Accepted"),QStringList("TCPA")<<ips);
}
void COMM::set_TCPC(char* rip, int rport)
{
    TYPE = TYPE_TCPC;
    NO = SEQ++;
    strncpy(RIP,rip,IP_LEN);
    RPORT = rport;
    sock = new QTcpSocket(this);
    connect(((QTcpSocket*)sock),\
            SIGNAL(stateChanged(QAbstractSocket::SocketState)),\
            this,\
            SLOT(TCP_stateChanged(QAbstractSocket::SocketState)));
    ((QTcpSocket*)sock)->connectToHost(QHostAddress(rip),quint16(rport));

}

void COMM::set_UDP(char *lip, int lport, char* rip, int rport)
{
    sock = new QUdpSocket(this);
    NO = SEQ++;
    TYPE = TYPE_UDP;
    strncpy(LIP,lip,IP_LEN);
    LPORT = lport;
    strncpy(RIP,rip,IP_LEN);
    RPORT = rport;
    if(lport)  // 如果本地端口不是0,那么就要bind本地端口
    {
        ((QUdpSocket*)sock)->bind(quint16(lport),QUdpSocket::ReuseAddressHint);
        connect(((QUdpSocket*)sock), SIGNAL(readyRead()),
                      this, SLOT(ReadData()));
    }
    QString sip = QString("R:%1:%2,L:%3:%4").arg(LIP).arg(LPORT).arg(RIP).arg(RPORT);
    emit gui->s_treeItem_add(NO,TYPE_UDP,QString("UDP"),QStringList("UDP")<<sip);
    emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:UDP创建成功:%1").arg(sip));
}
void COMM::set_MultS(char* lip, int lport,char* mip, int mport)
{
    qDebug()<<lip<<lport<<mip<<mport;
    sock = new QUdpSocket(this);
    NO = SEQ++;
    TYPE = TYPE_UDP;
    strncpy(LIP,lip,IP_LEN);
    LPORT = lport;
    strncpy(RIP,mip,IP_LEN);
    RPORT = mport;
    ((QUdpSocket*)sock)->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);//禁止本机接收
    ((QUdpSocket*)sock)->joinMulticastGroup(QHostAddress(mip));//这句是关键，加入组播地址
    if(lport)  // 如果本地端口不是0,那么就要bind本地端口
    {
        ((QUdpSocket*)sock)->bind(quint16(lport),QUdpSocket::ReuseAddressHint);
        connect(((QUdpSocket*)sock), SIGNAL(readyRead()),
                      this, SLOT(ReadData()));
    }
    QString sip = QString("R:%1:%2,L:%3:%4").arg(LIP).arg(LPORT).arg(RIP).arg(RPORT);
    emit gui->s_treeItem_add(NO,TYPE_UDP,QString("UDPMS"),QStringList("UDPMS")<<sip);
    emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:UDP组播源创建成功:%1").arg(sip));
}
void COMM::set_MultC(char* lip, int lport,char* cip, int cport)
{
    qDebug()<<lip<<lport<<cip<<cport;
    sock = new QUdpSocket(this);
    NO = SEQ++;
    TYPE = TYPE_UDP;
    strncpy(LIP,lip,IP_LEN);
    LPORT = lport;
    strncpy(RIP,cip,IP_LEN);
    RPORT = cport;
    if(lport)  // 如果本地端口不是0,那么就要bind本地端口
    {
        ((QUdpSocket*)sock)->bind(quint16(lport),QUdpSocket::ReuseAddressHint);
        connect(((QUdpSocket*)sock), SIGNAL(readyRead()),
                      this, SLOT(ReadData()));
    }
    QString sip = QString("R:%1:%2,L:%3:%4").arg(LIP).arg(LPORT).arg(RIP).arg(RPORT);
    emit gui->s_treeItem_add(NO,TYPE_UDP,QString("UDPMS"),QStringList("UDPMS")<<sip);
    emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:UDP组播源创建成功:%1").arg(sip));
}
void COMM::TCPS_acceptError(QAbstractSocket::SocketError socketError)
{
    qDebug()<< "TCPS_acceptError" << socketError;
    Disconnected();
}

void COMM::TCP_stateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug()<< "TCP_stateChanged" << socketState;
    if(TYPE_TCPC == TYPE)
    {
        switch (socketState) {
        case QAbstractSocket::HostLookupState:
            emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:TCPC HostLookupState R:%1:%2").arg(RIP).arg(RPORT));
            break;
        case QAbstractSocket::ConnectingState:
            emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:TCPC准备连接 R:%1:%2").arg(RIP).arg(RPORT));
            break;
        case QAbstractSocket::ConnectedState:
        {
            strncpy(LIP,((QTcpSocket*)sock)->localAddress().toString().toLocal8Bit().data(),IP_LEN);
            LPORT = ((QTcpSocket*)sock)->localPort();
            connect(((QTcpSocket*)sock),SIGNAL(readyRead()),this,SLOT(ReadData()));
            QString sip = QString("L:%1:%2,R:%3:%4").arg(LIP).arg(LPORT).arg(RIP).arg(RPORT);
            emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:TCPC新连接 %1").arg(sip));
            emit gui->s_treeItem_add(NO,TYPE_TCPA,QString("TCP Client"),QStringList("TCPC")<<sip);
            break;
        }
        case QAbstractSocket::ClosingState:
            Disconnected();
            break;
        case QAbstractSocket::UnconnectedState:
            emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:TCPC连接失败 R:%1:%2").arg(RIP).arg(RPORT));
            for(QList<COMM*>::iterator it = gui->qcomm.begin(); it != gui->qcomm.end(); it++)
            {
                if((*it)->NO== NO)
                {
                    gui->qcomm.erase(it);
                    break;
                }
            }
            ((QTcpSocket*)sock)->close();
            sock = 0;
            delete this;
            break;
        default:
            break;
        }
    }
    else if(TYPE_TCPA == TYPE && QAbstractSocket::ClosingState == socketState)
    {
        Disconnected();
    }
}

void COMM::ReadData()
{
    qDebug()<< "ReadData" ;
    QString buf;
    switch (TYPE) {
    case TYPE_TCPA:
    case TYPE_TCPC:
        buf = ((QTcpSocket*)(sock))->readAll();
        break;
    case TYPE_UDP:
        while (((QUdpSocket*)(sock))->hasPendingDatagrams()) {
                  if(((QUdpSocket*)(sock))->peerAddress()== QHostAddress(RIP) &&\
                     ((QUdpSocket*)(sock))->peerPort() == RPORT)
                  {
                      QNetworkDatagram datagram = ((QUdpSocket*)(sock))->receiveDatagram();
                      buf += datagram.data();
                  }

              }
        break;
    case TYPE_MS:
        qDebug() << "TYPE_MS";
        break;
    case TYPE_MR:
        qDebug() << "TYPE_MR";
        break;
    default:
        break;
    }
    if(buf.size())
    {
        emit gui->s_tableItem_add(NO,2,1,buf);
    }

}
void COMM::WriteData(const QString& buf)
{
    qDebug()<< "TCP_Write" << buf;
    switch (TYPE) {
    case TYPE_TCPA:
    case TYPE_TCPC:
        ((QTcpSocket*)(sock))->write(buf.toLocal8Bit());
        break;
    case TYPE_UDP:
        ((QUdpSocket*)(sock))->writeDatagram(buf.toLocal8Bit(),QHostAddress(RIP),RPORT);
        break;
    default:
        break;
    }
    emit gui->s_tableItem_add(NO,1,1,buf);
}
void COMM::Disconnected()
{
    qDebug()<< "TCP_disconnected" ;
    QString sip;
    switch (TYPE)
    {
    case TYPE_TCPS:
        sip = QString("TCPS L%1:%2").arg(LIP).arg(LPORT);
        break;
    case TYPE_TCPA:
        sip = QString("TCPA L:%1:%2,R:%3:%4").arg(LIP).arg(LPORT).arg(RIP).arg(RPORT);
        disconnect(((QTcpSocket*)sock),\
                SIGNAL(stateChanged(QAbstractSocket::SocketState)),\
                this,\
                SLOT(TCP_stateChanged(QAbstractSocket::SocketState)));
        break;
    case TYPE_TCPC:
        sip = QString("TCPC L:%1:%2,R:%3:%4").arg(LIP).arg(LPORT).arg(RIP).arg(RPORT);
        disconnect(((QTcpSocket*)sock),\
                SIGNAL(stateChanged(QAbstractSocket::SocketState)),\
                this,\
                SLOT(TCP_stateChanged(QAbstractSocket::SocketState)));
        break;
    case TYPE_UDP:
        sip = QString("UDP L:%1:%2,R:%3:%4").arg(LIP).arg(LPORT).arg(RIP).arg(RPORT);
        break;
    default:
        break;
    }
    emit gui->s_tableItem_add(NO,0,0,QString(u8"NOTICE:关闭连接1 %1").arg(sip));
    emit gui->s_treeItem_del(NO);
    //强制删除gui->qcomm中的this, 不管数据的同步问题,谁叫signal-slot-emit类似于中断,又不能暂时禁用呢.
    for(QList<COMM*>::iterator it = gui->qcomm.begin(); it != gui->qcomm.end(); it++)
    {
        if((*it)->NO== NO)
        {
            gui->qcomm.erase(it);
            break;
        }
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

