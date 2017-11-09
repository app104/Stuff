#include "Comm.h"
#include "mainwindow.h"
#include <string.h>

#include <QMessageBox>
#include <QMainWindow>

int COMM::Count = 0;
COMM* COMM::first  = NULL;
COMM* COMM::last  = NULL;
QMutex COMM::mutex;

COMM::COMM()
{
    init();
}
COMM::COMM(int type,char* lip,int lport,char* rip ,int rport)
{
    init();
    init_net(type,lip,lport,rip ,rport);
}
COMM::COMM( QTcpSocket* sock,int type,char* lip,int lport,char* rip ,int rport)
{
    init();
    tcpsock = sock;
    init_net(type, lip, lport, rip , rport);
    if (type ==TYPE_TCPA &&sock != NULL)
    {
        connect(tcpsock,SIGNAL(readyRead()), this,SLOT(TCP_read()));
        connect(tcpsock,SIGNAL(disconnected()), this,SLOT(TCP_close()));
    }
}
COMM::~COMM()
{
    disconnect(this,\
            SIGNAL(s_tableAddItem(QString,QString,QString,QString)),\
            gui,\
            SLOT(tableAddItem(QString,QString,QString,QString)));
    disconnect(this,\
            SIGNAL(s_treeAddItem(int, int, QStringList&)),\
            gui,\
            SLOT(treeAddItem(int, int, QStringList&)));
    disconnect(this,\
            SIGNAL(s_treeDelItem(int)),\
            gui,\
            SLOT(treeDelItem(int)));
    mutex.lock();
    if( first == last) //序列中只有一个元素
    {
        first = NULL;
        last = NULL;
    }
   else
    {
        if (last == this ) //在序列尾部
        {
            last = prev;
            prev->next = NULL;
        }
        else
        {
            if(first == this) //序列中第一个
            {
                first = next;
                next->prev = NULL;

            }
            else
            {
                prev->next = next;
                next->prev = prev;
            }

        }
    }

    mutex.unlock();
}
void COMM::init()
{

    //因为子线程不能直接写主窗口，所以只能通过SIGNAL-SLOT来改变主窗口
    connect(this,\
            SIGNAL(s_tableAddItem(QString,QString,QString,QString)),\
            gui,\
            SLOT(tableAddItem(QString,QString,QString,QString)));
    connect(this,\
            SIGNAL(s_treeAddItem(int, int, QStringList&)),\
            gui,\
            SLOT(treeAddItem(int, int, QStringList&)));
    connect(this,\
            SIGNAL(s_treeDelItem(int)),\
            gui,\
            SLOT(treeDelItem(int)));
    if(first == NULL)
    {
        first = this;
        prev = NULL;
        next = NULL;
    }
    else
    {
        last->next = this;
        this->prev = last;
        this->next = NULL;
    }
    last = this;
    NO = Count ++;
}

int COMM::init_net(int type,char* lip,int lport,char* rip ,int rport)
{
    mutex.lock();
    for (COMM* p = first; p != NULL; p = p->next )
    {
        switch (type)
        {
        case TYPE_TCPS:
        case TYPE_UDP:
            if(strncmp(lip,p->LIP,strlen(p->LIP)) == 0 && p->LPORT == lport) { mutex.unlock();return -1;}
            break;
        case TYPE_UDP:
            if(strncmp(lip,p->LIP,strlen(p->LIP)) == 0 && p->LPORT != 0 && p->LPORT == lport ) { mutex.unlock();return -1;}
            break;
        default:
            break;
        }
    }
    TYPE = type;
    switch (TYPE)
    {
    case TYPE_TCPS:
        strncpy(LIP,lip,IP_LEN -1);
        LPORT = lport;
        server = new QTcpServer;
        if(server == NULL) return -1;
        if(!server->listen(QHostAddress(LIP),quint16(LPORT)))
        {
            server->close();
            mutex.unlock();
            return -1;
        }
        connect(server,SIGNAL(newConnection()),this,SLOT(TCPS_new()));
        break;
    case TYPE_TCPA:
        strncpy(LIP,lip,IP_LEN -1);
        strncpy(RIP,rip,IP_LEN -1);
        LPORT = lport;
        RPORT = rport;
        break;
    case TYPE_TCPC:
        strncpy(RIP,rip,IP_LEN -1);
        RPORT = rport;
        qDebug() << RIP << RPORT;
        tcpsock = new QTcpSocket;
        tcpsock->connectToHost(QHostAddress(RIP),quint16(RPORT),QTcpSocket::ReadWrite);
        if (tcpsock->state() == QAbstractSocket::UnconnectedState)
        {
            emit s_tableAddItem(QString(u8"通道ID")+ QString::number(this->NO),NULL,NULL,QString(u8"连接失败"));
            mutex.unlock();
            return -1;
        }
        connect(tcpsock,SIGNAL(connected()),this,SLOT(TCP_connect()));
        break;
    case TYPE_UDP:

        break;
    case TYPE_UDPBS:

        break;
    case TYPE_UDPBR:

        break;

    default:
        break;
    }
    mutex.unlock();
    return 0;
}

int COMM::TCPS_new()
{
    qDebug() << "TCP Server: "<< LIP << ":"<< LPORT << ", Get New Connection";

    QTcpSocket* sock = server->nextPendingConnection();
    if (sock == NULL) return -1;
    char lip[IP_LEN],rip[IP_LEN];
    int lport, rport;
    strncpy(lip, sock->localAddress().toString().toLocal8Bit().data(),IP_LEN);
    lport = sock->localPort();
    strncpy(rip, sock->peerAddress().toString().toLocal8Bit().data(),IP_LEN);
    rport = sock->peerPort();
    COMM* comm = new COMM(sock,TYPE_TCPA,lip,lport,rip,rport);

    QString str(u8"本地:");
    str += comm->LIP;
    str += u8":";
    str += QString::number(comm->LPORT);
    str += u8" 远端:";
    str += comm->RIP;
    str += u8":";
    str += QString::number(comm->RPORT);
    QString str2(u8"TCP Server新连入");
    str2+= this->LIP;
    str2 += u8":";
    str2+= QString::number(this->RPORT);
    str2+= u8" ";
    str2 += str;
    emit this->s_tableAddItem(QString(u8"通道") + QString::number(comm->NO),NULL,NULL,str2);
    QStringList sl(str);
    emit this->s_treeAddItem(TYPE_TCPA,comm->NO,sl);
    return 0;
}

int COMM::TCP_read()
{
    emit this->s_tableAddItem(QString(u8"通道") + QString::number(this->NO),\
                              QString(u8"接收"),\
                              QString(u8"报文"),tcpsock->readAll());
    return 0;
}
int COMM::TCP_close()
{
    this->tcpsock->close();

    QString str(u8"本地:");
    str += this->LIP;
    str += u8":";
    str += QString::number(this->LPORT);
    str += u8" 远端:";
    str += this->RIP;
    str += u8":";
    str += QString::number(this->RPORT);
    emit this->s_tableAddItem(QString(u8"通道") + QString::number(this->NO),\
                              NULL, NULL,QString(u8"对侧关闭了连接:")+str);
    emit this->s_treeDelItem(this->NO);
    qDebug() << u8"关闭连接";
    return 0;
}

int COMM::TCP_connect()
{
    strncpy(this->LIP,this->tcpsock->localAddress().toString().toLocal8Bit().data(),IP_LEN);
    this->LPORT = this->tcpsock->localPort();
    strncpy(this->RIP,this->tcpsock->peerAddress().toString().toLocal8Bit().data(),IP_LEN);
    this->RPORT = this->tcpsock->peerPort();
    connect(this->tcpsock,SIGNAL(readyRead()),this,SLOT(TCP_read()));
    connect(this->tcpsock,SIGNAL(disconnected()), this,SLOT(TCP_close()));

    QString str(u8"本地:");
    str += this->LIP;
    str += u8":";
    str += QString::number(this->LPORT);
    str += u8" 远端:";
    str += this->RIP;
    str += u8":";
    str += QString::number(this->RPORT);
    emit this->s_tableAddItem(QString(u8"通道") + QString::number(this->NO),\
                          NULL, NULL,QString(u8"TCP Client成功连上对侧:")+str);
    return 0;
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
