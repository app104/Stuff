#ifndef COMM_H
#define COMM_H

#include <QtNetwork>
#include <QMainWindow>

#define IP_LEN 16
#define BUF_LEN 1024

#define TYPE_TCPS        100 //TCP Server
#define TYPE_TCPA        101 //TCP Server Accept
#define TYPE_TCPC        102 //TCP Client
#define TYPE_UDP         103 //UDP
#define TYPE_MS          104 //Multicast Source
#define TYPE_MR          105 //Multicast Receive, add into the group
#define TYPE_IGMP        106 //IGMP, for ping

#define TYPE_SSLS        110 //SSL Server
#define TYPE_SSLSA       111 //SSL Server Accept
#define TYPE_SSLC        112 //SSL Client

#define TYPE_SERIAL      200 //Serial 串口

class MainWindow;
class COMM: public QObject
{
    Q_OBJECT
//function
public:
    COMM();
    ~COMM();
    void set_TCPS(char* lip, int lport);
    void set_TCPC(char* rip, int rport);
    void set_UDP(char* lip, int lport,char* rip, int rport);
    void set_MultS(char* lip, int lport,char* mip, int mport);
    void set_MultC(char* lip, int lport,char* cip, int cport);
    void SetTimer(int interval);
    void StopTimer();
protected:
private:
    void init();
    void timerEvent(QTimerEvent * event);
public:
    static int        SEQ;
    int               NO;         //序号
    int               TYPE;       //通讯类型
    char              LIP[IP_LEN];    //本地端口
    int               LPORT;      //本地端口
    char              RIP[IP_LEN];    //远端端口
    int               RPORT;      //远端端口
    void*             sock;       //socket 类的指针
    int               TIMEID;  //定时发送的时间ID
    char              buf[BUF_LEN];
    int               buf_size;
protected:

private:

public slots:
    void TCPS_newConnection();
    void TCPS_acceptError(QAbstractSocket::SocketError socketError);
    void TCP_stateChanged(QAbstractSocket::SocketState socketState);
    void Disconnected();
    void ReadData();
    void WriteData(const QString& buf);

signals:

};


int is_valid_ip(char* ip);
#endif // COMM_H
