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
protected:
private:
    virtual void resizeEvent(QResizeEvent *event); //窗口改变大小时调用此函数
    virtual void mouseMoveEvent(QMouseEvent *event); //鼠标按下状态中，移动的时候调用
//    voidmousePressEvent(QMouseEvent*event);//按下
//    voidmouseMoveEvent(QMouseEvent*event);//按下移动
//    voidmouseReleaseEvent(QMouseEvent*event);//松开
//    void mouseDoubleClickEvent(QMouseEvent*event);//双击
//    voidwheelEvent(QWheelEvent*event);//滚轮
    int treeView_init();
    int tableView_init();

private slots:
    void on_actionnew_triggered();
    void on_actionquit_triggered();
    void on_tabWidget_currentChanged(int index);

    void tableItem_add(int id,int dir,int type, const QString& data);
    void tableItem_del();
    void tableView_rightMenu(const QPoint &pos);  //右键信号槽函数

    void treeItem_add(int id, int type, const QString & info, const QStringList& ql);
    void treeItem_del(int id);
    void on_treeView_doubleClicked(const QModelIndex &index);//treeView有双击事件

signals:
    void s_tableItem_add(int id,int dir,int type, const QString& data);

    void s_treeItem_add(int id,int type, const QString & info,const QStringList& ql);
    void s_treeItem_del(int id);

public:
    QList<COMM*> qcomm; //每个new的COMM全部压边这个qcomm中. 不考虑emit引起的数据同步问题.
private:
    Ui::MainWindow *ui;
    QStandardItemModel* mtree;
    QStandardItemModel* mtable;
    QMenu * qm_table; //tableView的右键菜单
};
extern MainWindow * gui;
#endif // MAINWINDOW_H
