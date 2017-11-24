#include "newdialog.h"
#include "mainwindow.h"
#include "ui_newdialog.h"
#include "Comm.h"


#include <string.h>

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);
   // MainWindow* pParent = (MainWindow*)parentWidget();
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
    //以下是检测本机有几个可用的串口
    for(int i = 254; i >= 0; i--)
    {
        QString str("COM");
        str += QString::number(i);
        if(QSerialPortInfo(str).isValid())
        {
            ui->comboBoxCOM_NUM->insertItem(0,str);
        }
    }
}

NewDialog::~NewDialog()
{
    delete ui;
}

void NewDialog::on_pushButtonOK_clicked()//点击触发
{
    char lip[IP_LEN] ={}, rip[IP_LEN] ={};
    int lport = 0, rport = 0;

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
        COMM co;
        if(ui->radioButtonTCPS->isChecked())
        {
            co.set_TCPS(lip,lport);
        }
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
