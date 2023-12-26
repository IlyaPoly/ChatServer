#include "tcpserver.h"
#include <QTimer>

TcpServer::TcpServer(QObject *parent)
    : QObject{parent}
{
    s.listen(QHostAddress::Any, 30000);
    connect(&s, SIGNAL(newConnection()), this, SLOT(slotNewConnect()));
}

void TcpServer::slotNewConnect()
{

    qDebug("Connect");
    Database* dbnew = new Database(s.nextPendingConnection()->socketDescriptor());
    QThread *th = new QThread(this);
    dbnew->moveToThread(th);
    th->start();
    QTimer::singleShot(50, dbnew, SLOT(initThread()));
}

