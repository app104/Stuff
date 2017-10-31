#include "Comm.h"
#include <string.h>

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



void COMM::init()
{
    switch (TYPE) {
    case TYPE_TCPS:

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


void COMM::run()
{

}
