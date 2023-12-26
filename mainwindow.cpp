#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <memory>
#include <QSqlQuery>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomContextMenu(QPoint)));
    timer.setInterval(5000);
    timer.setSingleShot(false);
    connect(&timer, SIGNAL(timeout()), this, SLOT(showInf()));
    on_actionConnect_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::slotCustomContextMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    QAction *blockUser = new  QAction(tr("Block user"), this);
    QAction *unblockUser = new  QAction(tr("Unblock user"), this);
    connect(blockUser, SIGNAL(triggered()), this, SLOT(slotBlockUser()));
    connect(unblockUser, SIGNAL(triggered()), this, SLOT(slotUnblockUser()));
    menu->addAction(blockUser);
    menu->addAction(unblockUser);
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::slotBlockUser()
{
    QModelIndex myIndex = ui->tableView->currentIndex();
    QVariant myData = ui->tableView->model()->data(myIndex ,Qt::DisplayRole);
    db->blockUser(myData.toString());
}

void MainWindow::slotUnblockUser()
{
    QModelIndex myIndex = ui->tableView->currentIndex();
    QVariant myData = ui->tableView->model()->data(myIndex ,Qt::DisplayRole);
    db->unblockUser(myData.toString());
}


void MainWindow::on_actionConnect_triggered()
{

    myTcp = new TcpServer;
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    db = new Database;
    showInf();
    timer.start();
}

void MainWindow::showInf()
{
    ui->textBrowser->setText(db->showDb(users));
    QList<QStandardItem *> items;
    QStandardItemModel* model = new QStandardItemModel();
    QString tmp1, tmp2, tmp3;
    int i = 0;
    while ( i < users.length())
    {
        for (;users[i] != '\t'; i++)
            if (users[i] != '\t')
                tmp1 += users[i];
        i++;
        for (;users[i] != '\t'; i++)
            if (users[i] != '\t')
                tmp2 += users[i];
        i++;
        for (;users[i] != '\n'; i++)
            if (users[i] != '\n')
                tmp3 += users[i];
        i++;
        items.append(new QStandardItem(tmp1));
        items.append(new QStandardItem(tmp2));
        items.append(new QStandardItem(tmp3));
        model->appendRow(items);
        ui->tableView->setModel(model);
        items.clear();
        tmp1.clear();
        tmp2.clear();
        tmp3.clear();
    }
}
