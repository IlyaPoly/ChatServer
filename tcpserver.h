#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "database.h"
#include <vector>
#include <QThread>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
signals:

public slots:
    void slotNewConnect();
private:
    QTcpServer s;
    std::vector<Database> dbvec;
    Database* db;
    std::vector<std::thread> working;
};

#endif // TCPSERVER_H
