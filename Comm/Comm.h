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



class MainWindow;
class COMM: public QMainWindow
{
    Q_OBJECT

public:
    int               NO;         //序号
    int               TYPE;       //通讯类型
    char              LIP[IP_LEN];    //本地端口
    int               LPORT;      //本地端口
    char              RIP[IP_LEN];    //远端端口
    int               RPORT;      //远端端口
    COMM*             prev;       //指向前一个class Comm
    COMM*             next;       //指向后一个class Comm
    QTcpSocket*       tcpsock;
    QTcpServer*       server;

    static int        Count;      //通讯序号,只加不减
    static COMM*      last;       //指向最后一个class Comm
    static QMutex     mutex;      //通讯列表的互斥锁,
    COMM(); //
    COMM(int type,char* lip,int lport,char* rip ,int rport);//tcps tcpc 的构造函数
    COMM( QTcpSocket* sock,int type,char* lip,int lport,char* rip ,int rport);
    void destory(){delete this;} //实现COMM类的自删除，
    int COMM::init_net(int type,char* lip,int lport,char* rip ,int rport);
private:
    //因为要实现类的自删除
    ~COMM(); //据说把析构函数设为private，就不能像 COMM x;这样声明了，只能用new
    //virtual void run();
    void init();
    QString* str_ip();
    QString* str_lip();
    QString* str_rip();
private slots:
    int TCPS_new();
    int TCP_read();
    int TCP_write(const QString& buf);
    int TCP_connect();
    int TCP_disconnect();
    void TCP_setconnected();
    void TCP_setdisconnected();
    void TCP_error(QAbstractSocket::SocketError socketError);
signals:
    void s_tableAddItem(const QString &, const QString &, const QString &, const QString &);
    void s_treeAddItem(int, int, QStringList&); //void treeAddItem(int type, int id, QStringList& qinfo);
    void s_treeDelItem(int);

    void s_TCP_disconnect();
    int  s_TCP_write(const QString&);
};


int is_valid_ip(char* ip);
extern COMM* Comm;
#endif // COMM_H
