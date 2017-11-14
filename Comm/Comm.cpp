#include "Comm.h"
#include "mainwindow.h"
#include <string.h>

#include <QMessageBox>
#include <QMainWindow>
COMM* Comm = NULL;
int COMM::Count = 0;
COMM* COMM::last  = NULL;
QMutex COMM::mutex;

COMM::COMM()
{
    init();
}
COMM::COMM(int type,char* lip,int lport,char* rip ,int rport)
{
    init();
    init_net(type,lip,lport,rip ,rport);
}
COMM::COMM( QTcpSocket* sock,int type,char* lip,int lport,char* rip ,int rport)
{
    init();
    tcpsock = sock;
    init_net(type, lip, lport, rip , rport);
    if (type ==TYPE_TCPA &&sock != NULL)
    {
        TCP_connect();
    }
}
COMM::~COMM()
{
    disconnect(this,\
            SIGNAL(s_tableAddItem(QString,QString,QString,QString)),\
            gui,\
            SLOT(tableAddItem(QString,QString,QString,QString)));
    disconnect(this,\
            SIGNAL(s_treeAddItem(int, int, QStringList&)),\
            gui,\
            SLOT(treeAddItem(int, int, QStringList&)));
    disconnect(this,\
            SIGNAL(s_treeDelItem(int)),\
            gui,\
            SLOT(treeDelItem(int)));
    disconnect(this,\
            SIGNAL(s_SETtimer(int)),\
            this,\
            SLOT(SETtimer(int)));
    disconnect(this,\
            SIGNAL(s_STOPtimer()),\
            this,\
            SLOT(STOPtimer()));

    if (TIMEID) killTimer(TIMEID);
    qDebug ()<< QThread::currentThreadId();
    mutex.lock();
    if( Comm == last) //序列中只有一个元素
    {
        Comm = NULL;
        last = NULL;
    }
   else
    {
        if (last == this ) //在序列尾部
        {
            last = prev;
            prev->next = NULL;
        }
        else
        {
            if(Comm == this) //序列中第一个
            {
                Comm = next;
                next->prev = NULL;

            }
            else
            {
                prev->next = next;
                next->prev = prev;
            }

        }
    }

    mutex.unlock();
}
void COMM::init()
{
    TIMEID = 0;
    //因为子线程不能直接写主窗口，所以只能通过SIGNAL-SLOT来改变主窗口
    connect(this,\
            SIGNAL(s_tableAddItem(QString,QString,QString,QString)),\
            gui,\
            SLOT(tableAddItem(QString,QString,QString,QString)));
    connect(this,\
            SIGNAL(s_treeAddItem(int, int, QStringList&)),\
            gui,\
            SLOT(treeAddItem(int, int, QStringList&)));
    connect(this,\
            SIGNAL(s_treeDelItem(int)),\
            gui,\
            SLOT(treeDelItem(int)));
    connect(this,\
            SIGNAL(s_SETtimer(int)),\
            this,\
            SLOT(SETtimer(int)));
    connect(this,\
            SIGNAL(s_STOPtimer()),\
            this,\
            SLOT(STOPtimer()));
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
        this->next = NULL;
    }
    last = this;
    NO = Count ++;
}

int COMM::init_net(int type,char* lip,int lport,char* rip ,int rport)
{
    mutex.lock();
    for (COMM* p = Comm; p != NULL; p = p->next )
    {
        switch (type)
        {
        case TYPE_TCPS:
        case TYPE_UDP:
            if(strncmp(lip,p->LIP,strlen(p->LIP)) == 0 && p->LPORT == lport) { mutex.unlock();return -1;}
            break;
        default:
            break;
        }
    }
    TYPE = type;
    switch (TYPE)
    {
    case TYPE_TCPS:
        strncpy(LIP,lip,IP_LEN -1);
        LPORT = lport;
        server = new QTcpServer;
        if(server == NULL) return -1;
        if(!server->listen(QHostAddress(LIP),quint16(LPORT)))
        {
            server->close();
            mutex.unlock();
            QString * str = str_lip();
            emit s_tableAddItem(NULL,NULL,NULL,QString(u8"新建TCP Server通道失败 ")+ *str);
            delete str;
            //QThread::sleep(1);
            this->destory();
            return -1;
        }
        else
        {
            connect(server,SIGNAL(newConnection()),this,SLOT(TCPS_new()));
            connect(this,SIGNAL(s_TCP_disconnect()),this,SLOT(TCP_disconnect()));
            QString *str = str_lip();
            emit s_tableAddItem(QString(u8"通道") + QString::number(NO), \
                                NULL,NULL,QString(u8"新建TCP Server通道")+ *str);
            QStringList sl;
            sl.append(*str);
            emit s_treeAddItem(TYPE_TCPS,NO,sl);
            delete str;
        }

        break;
    case TYPE_TCPA:
        strncpy(LIP,lip,IP_LEN -1);
        strncpy(RIP,rip,IP_LEN -1);
        LPORT = lport;
        RPORT = rport;
        break;
    case TYPE_TCPC:
        strncpy(RIP,rip,IP_LEN -1);
        RPORT = rport;
        qDebug() << RIP << RPORT;
        tcpsock = new QTcpSocket;
        tcpsock->connectToHost(QHostAddress(RIP),quint16(RPORT),QTcpSocket::ReadWrite);
        connect(tcpsock,SIGNAL(connected()),this,SLOT(TCP_connect()));
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
    mutex.unlock();
    return 0;
}

int COMM::TCPS_new()
{
    qDebug() << "TCP Server: "<< LIP << ":"<< LPORT << ", Get New Connection";

    QTcpSocket* sock = server->nextPendingConnection();
    if (sock == NULL) return -1;
    char lip[IP_LEN],rip[IP_LEN];
    int lport, rport;
    strncpy(lip, sock->localAddress().toString().toLocal8Bit().data(),IP_LEN);
    lport = sock->localPort();
    strncpy(rip, sock->peerAddress().toString().toLocal8Bit().data(),IP_LEN);
    rport = sock->peerPort();
    COMM* comm = new COMM(sock,TYPE_TCPA,lip,lport,rip,rport);
    if(comm == NULL) emit s_tableAddItem(QString(u8"通道") + QString::number(comm->NO),NULL,NULL,QString(u8"内存不够tcpa"));

    return 0;
}

int COMM::TCP_read()
{
    emit this->s_tableAddItem(QString(u8"通道") + QString::number(this->NO),\
                              QString(u8"接收"),\
                              QString(u8"报文"),tcpsock->readAll());
    return 0;
}
int COMM::TCP_write(const QString &buf)
{
    emit this->s_tableAddItem(QString(u8"通道") + QString::number(NO),\
                              QString(u8"发送"),\
                              QString(u8"报文"),buf);
    int write_len = tcpsock->write(buf.toLocal8Bit());
    if(write_len != buf.size())
    {
        emit this->s_tableAddItem(QString(u8"通道") + QString::number(NO),NULL,
                                  QString(u8"提示"),QString(u8"仅发送了:") + QString::number(write_len));
    }
    return write_len;
}
int COMM::TCP_connect()
{
    strncpy(this->LIP,this->tcpsock->localAddress().toString().toLocal8Bit().data(),IP_LEN);
    this->LPORT = this->tcpsock->localPort();
    strncpy(this->RIP,this->tcpsock->peerAddress().toString().toLocal8Bit().data(),IP_LEN);
    this->RPORT = this->tcpsock->peerPort();
    TCP_setconnected();

    set_notice();
    return 0;
}
int COMM::TCP_disconnect()
{
    QString* str;
    if(TYPE == TYPE_TCPS)
    {
       str = str_lip();
        disconnect(server,SIGNAL(newConnection()),this,SLOT(TCPS_new()));
        disconnect(this,SIGNAL(s_TCP_disconnect()),this,SLOT(TCP_disconnect()));
        server->close();
        qDebug() << u8"关闭连接 TCP Server";
    }
    else if(TYPE == TYPE_TCPA || TYPE == TYPE_TCPC)
    {
        str = str_ip();
        TCP_setdisconnected();
        tcpsock->close();
    }
    emit this->s_tableAddItem(QString(u8"通道") + QString::number(this->NO),\
                              NULL, NULL,QString(u8"关闭连接:")+ *str);
    emit s_treeDelItem(NO);
    delete str;
    this->destory();
    qDebug() << u8"关闭连接";
    return 0;
}

void COMM::TCP_error(QAbstractSocket::SocketError socketError)
{
    emit s_tableAddItem(QString(u8"通道")+ QString::number(this->NO),\
                        NULL,NULL,\
                        QString(u8"QT错误号:") +QString::number(socketError) + u8" " + tcpsock->errorString());
    emit this->s_treeDelItem(NO);
    TCP_setdisconnected();
    tcpsock->close();
    this->destory();
}

QString* COMM::str_ip()
{
    QString * str = new QString;
    str->append(u8"本地:"),str->append(LIP), str->append(u8":"), str->append(QString::number(this->LPORT)),
    str->append(u8" 远端:"),str->append(RIP), str->append(u8":"),str->append(QString::number(this->RPORT));
    return str;
}
QString* COMM::str_lip()
{
    QString * str = new QString;
    str->append(u8"本地:"),str->append(LIP), str->append(u8":"), str->append(QString::number(this->LPORT));
    return str;
}
QString* COMM::str_rip()
{
    QString * str = new QString;
    str->append(u8"远端:"),str->append(RIP), str->append(u8":"),str->append(QString::number(this->RPORT));
    return str;
}
void COMM::TCP_setconnected()
{
    connect(tcpsock,SIGNAL(readyRead()), this,SLOT(TCP_read()));
    connect(tcpsock,SIGNAL(disconnected()), this,SLOT(TCP_disconnect()));
    connect(this,SIGNAL(s_TCP_disconnect()), this,SLOT(TCP_disconnect()));
    connect(this,SIGNAL(s_TCP_write(const QString&)), this,SLOT(TCP_write(const QString&)));
    connect(tcpsock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(TCP_error(QAbstractSocket::SocketError)));
}
void COMM::TCP_setdisconnected()
{
    disconnect(tcpsock,0,0,0);
    disconnect(this,SIGNAL(s_TCP_disconnect()), this,SLOT(TCP_disconnect()));
    disconnect(this,SIGNAL(s_TCP_write(const QString&)), this,SLOT(TCP_write(const QString&)));
}
void COMM::SETtimer(int interval)
{

    TIMEID = startTimer(interval);
    qDebug() <<"startTimer" << interval << TIMEID;
}
void COMM::STOPtimer()
{
    qDebug() <<"killTimer" << TIMEID;
    if(TIMEID != 0) this-> killTimer(TIMEID);
    TIMEID = 0;
}
void COMM::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == TIMEID)
    {
        switch (TYPE) {
        case TYPE_TCPA:
        case TYPE_TCPC:
            TCP_write(QString(u8"11 22 33 44 55 88"));
            break;
        default:
            break;
        }
    }
}
void COMM::COMM::set_notice()
{
    QString* str = str_ip();
    char cstr[][64] =
    {
        u8"TCP Server Accept成功连入：",
        u8"TCP Client成功连接到服务端：",
    };

    emit this->s_tableAddItem(QString(u8"通道") + QString::number(NO),NULL,NULL,QString(cstr[TYPE- TYPE_TCPA]) + *str);

    QStringList sl(*str);
    sl.append(QString(u8"发送"));
    sl.append(QString(u8"11 22 33 44 55 66"));
    sl.append(QString(u8"定时发送停止"));
    sl.append(QString(u8"1000"));
    emit this->s_treeAddItem(TYPE,NO,sl);
    delete str;
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
