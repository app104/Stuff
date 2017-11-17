#include "Comm.h"
#include "mainwindow.h"
#include <string.h>

COMM::COMM()
{
    this->start();
}

COMM::~COMM()
{

}
void COMM::run()
{
    qDebug()<< u8"在线程中" <<this->currentThreadId();
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
