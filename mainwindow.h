#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include "tcpserver.h"
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void slotCustomContextMenu(const QPoint &pos);
    void slotBlockUser();
    void slotUnblockUser();
    void on_actionConnect_triggered();
    void showInf();

private:
    Ui::MainWindow *ui;
    TcpServer* myTcp = nullptr;
    QString users;
    QTimer timer;
    Database* db;

};


#endif // MAINWINDOW_H
