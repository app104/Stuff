﻿#include "newdialog.h"
#include "mainwindow.h"
#include "ui_newdialog.h"
#include "Comm.h"
#include <QString>
#include <QByteArray>
#include <QMessageBox>
 #include <QtNetwork>

#include <string.h>

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);
    MainWindow* pParent = (MainWindow*)parentWidget();
    connect(this,\
            SIGNAL(s_tableAddItem(QString,QString,QString,QString)),\
            pParent,\
            SLOT(tableAddItem(QString,QString,QString,QString)));
    connect(this,\
            SIGNAL(s_treeAddItem(int, int, QStringList&)),\
            pParent,\
            SLOT(treeAddItem(int, int, QStringList&)));

    //以下为获取所有本地网卡的IPv4地址, 并加到本地列表中
    foreach(QHostAddress addr, QNetworkInterface::allAddresses())
    {
        if(addr.protocol() == QAbstractSocket::IPv4Protocol)
        {
            QString ip = addr.toString();
            if(ip.contains("169.254.")) continue; //去掉保留地址
            ui->comboBoxLIP->insertItem(0,ip);
        }
    }

}

NewDialog::~NewDialog()
{
    qDebug() << "~NewDialog( )";
    MainWindow* pParent = (MainWindow*)parentWidget();
    disconnect(this,\
            SIGNAL(s_tableAddItem(QString,QString,QString,QString)),\
            pParent,\
            SLOT(tableAddItem(QString,QString,QString,QString)));
    disconnect(this,\
               SIGNAL(s_treeAddItem(int, int, QStringList&)),\
               pParent,\
               SLOT(treeAddItem(int, int, QStringList&)));
    delete ui;
}

void NewDialog::on_pushButtonOK_clicked()//点击触发
{
    char notice[][64]=
    {
        u8"新建TCP Server通道 ",
        u8"新建TCP Server Accept通道 ",
        u8"新建TCP Client通道 ",
        u8"新建UDP通道 ",
        u8"新建xxx通道 ",
    };
    char lip[IP_LEN] ={}, rip[IP_LEN] ={};
    int lport = 0, rport = 0;
    int type = -1;
    if(ui->radioButtonTCPS->isChecked() || ui->radioButtonTCPC->isChecked())
    {
        strncpy(lip,ui->comboBoxLIP->currentText().toLatin1().data(), IP_LEN-1);
        if(! is_valid_ip(lip)) {QMessageBox::warning(this,"Warning","Local IP invalid");return;}
        strncpy(rip,ui->comboBoxRIP->currentText().toLatin1().data(), IP_LEN-1);
        if(! is_valid_ip(rip)) {QMessageBox::warning(this,"Warning","Remote IP invalid");return;}
        lport = ui->lineEditLPORT->text().toInt();
        if(lport > 65535) {QMessageBox::warning(this,"Warning","Local Port invalid");return;}
        rport = ui->lineEditRPORT->text().toInt();
        if(rport > 65535) {QMessageBox::warning(this,"Warning","Remote Port invalid");return;}
        if(ui->radioButtonTCPS->isChecked()) type = TYPE_TCPS; else type = TYPE_TCPC;
        COMM* comm = new COMM(type,lip,lport,rip,rport);
        QString ipinfo (QString(u8"本地:")+ lip + u8":" +QString::number(lport));
        QStringList sl(ipinfo);
        emit s_treeAddItem(comm->TYPE,comm->NO,sl);
        emit s_tableAddItem(QString(u8"通道") + QString::number(comm->NO),NULL,NULL, QString(notice[comm->TYPE - TYPE_TCPS]) + ipinfo);
    }
    this->destory();
   // accept();
}

void NewDialog::setenable( int num)
{
    switch(num)
    {
    case 0:
    case 1:
    case 2:
        ui->comboBoxLIP->setEnabled(true);
        ui->comboBoxRIP->setEnabled(true);
        ui->lineEditLPORT->setEnabled(true);
        ui->lineEditRPORT->setEnabled(true);
        ui->radioButtonMulticastS->setEnabled(false);
        ui->radioButtonMulticastC->setEnabled(false);
        ui->lineEditMPORT->setEnabled(false);
        ui->comboBoxCOM_NUM->setEnabled(false);
        ui->comboBoxCOM_BAUD->setEnabled(false);
        ui->comboBoxCOM_DATABIT->setEnabled(false);
        ui->comboBoxCOM_STOPBIT->setEnabled(false);
        ui->comboBoxCOM_CHECK->setEnabled(false);
        break;
    case 3:
        ui->comboBoxLIP->setEnabled(true);
        ui->comboBoxRIP->setEnabled(true);
        ui->lineEditLPORT->setEnabled(true);
        ui->lineEditRPORT->setEnabled(true);
        ui->radioButtonMulticastS->setEnabled(true);
        ui->radioButtonMulticastC->setEnabled(true);
        ui->lineEditMPORT->setEnabled(true);
        ui->comboBoxCOM_NUM->setEnabled(false);
        ui->comboBoxCOM_BAUD->setEnabled(false);
        ui->comboBoxCOM_DATABIT->setEnabled(false);
        ui->comboBoxCOM_STOPBIT->setEnabled(false);
        ui->comboBoxCOM_CHECK->setEnabled(false);
        break;
    case 4:
        ui->comboBoxLIP->setEnabled(false);
        ui->comboBoxRIP->setEnabled(false);
        ui->lineEditLPORT->setEnabled(false);
        ui->lineEditRPORT->setEnabled(false);
        ui->radioButtonMulticastS->setEnabled(false);
        ui->radioButtonMulticastC->setEnabled(false);
        ui->lineEditMPORT->setEnabled(false);
        ui->comboBoxCOM_NUM->setEnabled(true);
        ui->comboBoxCOM_BAUD->setEnabled(true);
        ui->comboBoxCOM_DATABIT->setEnabled(true);
        ui->comboBoxCOM_STOPBIT->setEnabled(true);
        ui->comboBoxCOM_CHECK->setEnabled(true);
        break;
    default:
        break;
    }
}


void NewDialog::on_radioButtonTCPS_toggled(bool checked) //变位触发
{
    if(checked == true) setenable( 0);
}

void NewDialog::on_radioButtonTCPC_toggled(bool checked)
{
    if(checked == true) setenable( 1);
}

void NewDialog::on_radioButtonUDP_toggled(bool checked)
{
    if(checked == true) setenable( 2);
}

void NewDialog::on_radioButtonMulticast_toggled(bool checked)
{
    if(checked == true) setenable( 3);
}

void NewDialog::on_radioButtonSerial_toggled(bool checked)
{
    if(checked == true) setenable( 4);
}
