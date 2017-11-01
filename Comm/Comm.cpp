#include "Comm.h"
#include <string.h>

#include <QMessageBox>
COMM*  Comm = NULL;           //通讯列表

int COMM::Count = 0;
COMM* COMM::last  = NULL;
QMutex COMM::mutex;

COMM::COMM()
{
    mutex.lock();
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
    mutex.unlock();
}
COMM::~COMM()
{
    mutex.lock();
    if (last == this ) //在序列尾部
    {
        if(this->prev != NULL) //序列中不是只有一个
        prev->next = NULL;
        last = prev;
    }
    else
    {
        prev->next = next;
        next->prev = prev;
    }
    Count --;
    mutex.unlock();
}

void COMM::init_net(int type,char* lip,int lport,char* rip ,int rport)
{
    TYPE = type;
    strncpy(LIP,lip,IP_LEN -1);
    LPORT = lport;
    strncpy(RIP,rip,IP_LEN -1);
    RPORT = rport;
}

void COMM::run()
{
    int retval = 0;
    switch (TYPE) {
    case TYPE_TCPS:
        retval = run_TCPS();
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
    case TYPE_MS:

        break;
    case TYPE_MR:

        break;
    case TYPE_IGMP:

        break;
    case TYPE_SSLS:

        break;
    case TYPE_SSLSA:

        break;
    case TYPE_SSLC:

        break;
    case TYPE_SERIAL:

        break;
    default:
        break;
    }
}


int COMM::run_TCPS()
{
    QTcpServer* server = new QTcpServer;
    if(!server->listen(QHostAddress(LIP),quint16(LPORT)))
    {
        QMessageBox::warning(NULL,"warning","can't listen");
        return -1;
    }
    connect(server,SIGNAL(newConnection()), this,SLOT(run_TCPA()));
   // connect(server,SIGNAL(server->newConnection()),this,SLOT(run_TCPA()));
    this->sleep(500000);
    return 0;
}
void COMM::run_TCPA()
{
    socket = new QTcpSocket;
    int i = 0;
    while(i++ != 10)
    {
        socket->waitForReadyRead(1000);
        NetRead();
    }
    //QObject::connect(socket, &QTcpSocket::readyRead, this, &COMM::NetRead);

}
int COMM::NetRead()
{
    QByteArray buffer;
    //读取缓冲区数据
    buffer = socket->readAll();
    if(!buffer.isEmpty())
     {
         QMessageBox::warning(NULL,"get new data",tr(buffer));
     }
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
