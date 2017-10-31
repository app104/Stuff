﻿#ifndef COMM_H
#define COMM_H
#include <QMutex>
#include <QThread>

#define IP_LEN 32

#define TYPE_TCPS        100 //TCP Server
#define TYPE_TCPA        101 //TCP Server Accept
#define TYPE_TCPC        102 //TCP Client
#define TYPE_UDP         103 //UDP
#define TYPE_UDPBS       104 //UDP Broadcast Source
#define TYPE_UDPBR       105 //UDP Broadcast Receive
#define TYPE_MS          106 //Multicast Source
#define TYPE_MR          107 //Multicast Receive, add into the group
#define TYPE_IGMP        109 //IGMP, for ping

#define TYPE_SSLS        110 //SSL Server
#define TYPE_SSLSA       111 //SSL Server Accept
#define TYPE_SSLC        112 //SSL Client

#define TYPE_SERIAL      200 //Serial 串口

class COMM: public QThread
{
public:
    COMM();
    ~COMM();
    void COMM::init_net(int type,char* lip,int lport,char* rip ,int rport);

    virtual void run();
private:

protected:
    int              NO;         //序号
    int              TYPE;       //通讯类型
    char             LIP[IP_LEN];    //本地端口
    int              LPORT;      //本地端口
    char             RIP[IP_LEN];    //远端端口
    int              RPORT;      //远端端口
    COMM*            prev;       //指向前一个class Comm
    COMM*            next;       //指向后一个class Comm
public:
    static int       Count;      //通讯序号
    static COMM*     last;       //指向最后一个class Comm
    static QMutex    mutex;      //通讯列表的互斥锁,
};

extern COMM*  Comm;           //通讯列表

#endif // COMM_H