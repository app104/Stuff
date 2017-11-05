#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QtWidgets>
#include "mainwindow.h"
namespace Ui {
class NewDialog;
}

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(QWidget *parent = 0);
    ~NewDialog();

private:
void setenable( int num);

private slots:


    void on_pushButtonOK_clicked();

    void on_radioButtonTCPS_toggled(bool checked);

    void on_radioButtonTCPC_toggled(bool checked);

    void on_radioButtonUDP_toggled(bool checked);

    void on_radioButtonMulticast_toggled(bool checked);

    void on_radioButtonSerial_toggled(bool checked);

private:
    Ui::NewDialog *ui;
    MainWindow* pParent;
};

#endif // NEWDIALOG_H
