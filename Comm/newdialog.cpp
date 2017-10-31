#include "newdialog.h"
#include "ui_newdialog.h"
#include <QString>
#include <QByteArray>
#include <string.h>

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);
}

NewDialog::~NewDialog()
{
    delete ui;
}

void NewDialog::on_pushButtonOK_clicked()//点击触发
{
    char lip[IP_LEN], rip[IP_LEN];
    int lport, rport;
    if(ui->radioButtonTCPS->isChecked())
    {
        QString* t = ui->comboBoxLIP->currentText();
        QByteArray ba = t->toLatin1();
        strncpy(lip,ba.data(), IP_LEN-1);
    }else if()
    {

    }
    accept();
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
