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
    int               mark; //此值为1时表示要删除此通道
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
    void run()  override;
    void set_channel(QList<SCOMM>::iterator it);
public:

protected:

private:
    static Qt::HANDLE tid;
    static int tid_run;
    static QList<SCOMM> sl;
    static COMM * fcomm; //第一个COMM
    static QMutex mutex;
    static int NO;
public slots:
    void tid_quit(){tid_run = 0;}
private slots:
    void new_channel(SCOMM *s);
    void delete_channel(int id); //通道删除有延时，最多1s
    void TCPS_newConnection();
    void TCPS_acceptError(QAbstractSocket::SocketError socketError);
signals:
    void s_tid_quit();
    void s_new_channel(SCOMM * s);
    void s_delete_channel(int id);
    void s_TCPS_connecting();
};


int is_valid_ip(char* ip);
#endif // COMM_H
