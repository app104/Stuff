#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "Comm.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void treeAddItem(COMM* comm);
    void treeDelItem(int ID);

    void tableClear();
   // bool event(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event); //窗口改变大小时调用此函数
    virtual void mouseMoveEvent(QMouseEvent *event); //鼠标按下状态中，移动的时候调用
//    voidmousePressEvent(QMouseEvent*event);//按下
//    voidmouseMoveEvent(QMouseEvent*event);//按下移动
//    voidmouseReleaseEvent(QMouseEvent*event);//松开
//    voidmouseDoubleClickEvent(QMouseEvent*event);//双击
//    voidwheelEvent(QWheelEvent*event);//滚轮
private slots:
    void on_actionnew_triggered();

    void on_actionquit_triggered();
    void tableAddItem(const QString &channel, const QString &dir, const QString &type, const QString & data);

signals:


private:
    Ui::MainWindow *ui;
    QStandardItemModel* mtree;
    QStandardItemModel* mtable;
};
extern MainWindow * gui;
#endif // MAINWINDOW_H
