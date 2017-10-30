#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newdialog.h"
#include "Comm.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

class NewDialog;
void MainWindow::on_actionnew_triggered()
{
    //NewDialog dialog;
    NewDialog* nd = new NewDialog(this);
    nd->show();
}

void MainWindow::on_actionquit_triggered()
{
    this->close();
}
