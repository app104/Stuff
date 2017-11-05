#include "Comm.h"
#include <string.h>

#include <QMessageBox>
#include <QThread>
COMM*  Comm = NULL;           //通讯列表

int COMM::Count = 0;
COMM* COMM::last  = NULL;
QMutex COMM::mutex;
Qt::HANDLE COMM::tid = 0;
COMM::COMM()
{
    if(Comm == NULL)
    {
        Comm = this;
        prev = NULL;
        next = NULL;
    }
    else
    {
        last->next = this;
        this->prev = last;
    }
    last = this;
    NO = Count ++;
}
COMM::~COMM()
{
    mutex.lock();
    if (last == this ) //在序列尾部
    {
        if(this->prev != NULL) //序列中不是只有一个
        {
            prev->next = NULL;
            last = prev;
        }
        else
        {
            Comm = NULL;
            last = NULL;
            tid = 0;
        }
    }
    else
    {
        prev->next = next;
        next->prev = prev;
    }
    Count --;
    mutex.unlock();
}
int COMM::init()
{
    switch (TYPE) {
    case TYPE_TCPS:
        server = new QTcpServer;
        if(server == NULL) return -1;
        if(!server->listen(QHostAddress(LIP),quint16(LPORT)))
        {
            server->close();
            return -1;
        }
        connect(server,SIGNAL(newConnection()),this,SLOT(TCPS_new()));
        break;
    case TYPE_TCPA:

        break;
    case TYPE_TCPC:

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


    return 0;
}

int COMM::init_net(int type,char* lip,int lport,char* rip ,int rport)
{
    mutex.lock();
    for (COMM* p = Comm; p != NULL; p = p->next )
    {
        switch (type) {
        case TYPE_TCPS:
            if(strncmp(lip,p->LIP,strlen(p->LIP)) == 0 && p->LPORT == lport) return -1;
            strncpy(LIP,lip,IP_LEN -1);
            TYPE = type;
            LPORT = lport;
            if(this->init()) return -1;
            break;
        case TYPE_TCPC:
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
    }
//    TYPE = type;
//    strncpy(LIP,lip,IP_LEN -1);
//    LPORT = lport;
    strncpy(RIP,rip,IP_LEN -1);
    RPORT = rport;
    mutex.unlock();
    //if (tid == NULL) this->start();
    return 0;
}

int COMM::TCPS_new()
{
    qDebug() << "TCP Server: "<< LIP << ":"<< LPORT << ", Get New Connection";
    tcpsock = server->nextPendingConnection();
    if (tcpsock == NULL) return -1;
//    COMM* comm = new COMM;
//    comm->TYPE = TYPE_TCPA;
    connect(tcpsock,SIGNAL(readyRead()),this,SLOT(TCP_read()));
    connect(tcpsock,SIGNAL(disconnected()), this,SLOT(TCP_close()));
    //gui->tableAddItem(NULL,NULL,NULL,u8"TCP Server 接收了一个新的连接");
    return 0;
}

int COMM::TCP_read()
{
    QString data(tcpsock->readAll());
    qDebug() << data;
    return 0;
}
int COMM::TCP_close()
{
    tcpsock->close();
    qDebug() << u8"关闭连接";
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
