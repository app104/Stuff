#ifndef COMM_H
#define COMM_H

#include <QtNetwork>
#include <QMainWindow>

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

typedef struct _SCOMM_
{
    int               NO;         //序号
    int               TYPE;       //通讯类型
    char              LIP[IP_LEN];    //本地端口
    int               LPORT;      //本地端口
    char              RIP[IP_LEN];    //远端端口
    int               RPORT;      //远端端口
    void*             sock;       //socket 类的指针
    int               TIMEID;  //定时发送的时间ID
}SCOMM;


class MainWindow;
class COMM: public QThread
{
    Q_OBJECT
//function
public:
    COMM();
    ~COMM();
    void set_TCPS(char* lip, int lport);
protected:
private:
    void init();
public:

protected:

private:
    QList<SCOMM> sl;
    QMutex mutex;
    int NO;

private slots:
    void new_channal();
    void delete_channel(int id);
    void TCPS_newConnection();
    void TCPS_acceptError(QAbstractSocket::SocketError socketError);
    void TCP_disconnected();
    void TCP_Read();
    void TCP_Write(int id,const QString& buf);
signals:
    void s_new_channal();
    void s_delete_channel(int id);
    void s_TCP_Write(int id,const QString& buf);
};


int is_valid_ip(char* ip);
#endif // COMM_H
