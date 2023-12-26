#ifndef DATABASE_H
#define DATABASE_H
#include <QTcpSocket>
#include <vector>
#include <thread>
#include <iostream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <qstring.h>
#include <QObject>

#define MESSAGE_LENGTH 1024

class Database : public QObject
{
    Q_OBJECT
public:
    Database(qintptr descr);
    Database();
    void regChat(const char* data);
    void signIn(const char* data);
    void writeMessage(const char* data);
    void usersList();
    void dispChat();
    void messToParam(std::string* param, const char* data, int countParam);
    QString& showDb(QString& users);
    bool blockUser(const QString& login);
    bool unblockUser(const QString& login);

private slots:
    void initThread();
    void work();

private:
    QTcpSocket* _sock;
    qintptr _descr;
    char message[MESSAGE_LENGTH]{};
    char act;
    char ans[MESSAGE_LENGTH]{};
    char id[10] = {};
    QSqlDatabase* mydb;
    QString tmp = {}, copyUsers = {}, copyMess = {};
    QString dbName;
};

#endif // DATABASE_H
