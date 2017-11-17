#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QtWidgets>
#include "mainwindow.h"
#include "Comm.h"
namespace Ui {
class NewDialog;
}

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(QWidget *parent = 0);
    void destory(){delete this;}

private:
    ~NewDialog();
void setenable( int num);

private slots:
    void on_pushButtonOK_clicked();//点击触发
    void on_radioButtonTCPS_toggled(bool checked); //变位触发

    void on_radioButtonTCPC_toggled(bool checked);

    void on_radioButtonUDP_toggled(bool checked);

    void on_radioButtonMulticast_toggled(bool checked);

    void on_radioButtonSerial_toggled(bool checked);

signals:

private:
    Ui::NewDialog *ui;

};

#endif // NEWDIALOG_H
