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
    sl.clear();
    connect(this,SIGNAL(s_new_channal()),this,SLOT(new_channal()));
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
                it->sock = new QTcpServer;
                ((QTcpServer*)it->sock)->listen(QHostAddress(it->LIP),(it->LPORT));
                connect(((QTcpServer*)it->sock),SIGNAL(newConnection()),this,SLOT(TCPS_newConnection()));
                break;
            default:
                break;
            }
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
    NO++;
    mutex.lock();
    sl.append(co);
    mutex.unlock();
    emit this->s_new_channal();
}
void COMM::TCPS_newConnection()
{
    qDebug()<< "TCPS_newConnection";
}
void COMM::TCPS_acceptError(QAbstractSocket::SocketError socketError)
{
    qDebug()<< "TCPS_newConnection" << socketError;
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
