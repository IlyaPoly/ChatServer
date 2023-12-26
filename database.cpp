#include "database.h"

Database::Database(qintptr descr)
{
    dbName = QString::number(descr);;
    _descr = descr;
    mydb = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL", dbName));
    mydb->setHostName("127.0.0.1");
    mydb->setPort(5432);
    mydb->setDatabaseName("consolechatdb");
    mydb->setUserName("postgres");
    mydb->setPassword("465hg2s_Ilya");
    bool ok = mydb->open();
    if (ok)
        qDebug("db connected");
}
Database::Database()
{
    dbName = "Server";
    mydb = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL", dbName));
    mydb->setHostName("127.0.0.1");
    mydb->setPort(5432);
    mydb->setDatabaseName("postgres");
    mydb->setUserName("postgres");
    mydb->setPassword("465hg2s_Ilya");
    bool ok = mydb->open();
    if (ok)
        qDebug("db connected");
}

QString& Database::showDb(QString& users)
{
    users.clear();
    tmp.clear();
    QSqlQuery query(*mydb);
    query.exec("select id_sender, id_recipient, message  from public.message left join chat on (chat_id = id)");
    while (query.next())
        tmp += query.value(0).toString() + "\t" + query.value(1).toString() + "\t" + query.value(2).toString() + "\n";
    query.exec("SELECT * from public.client");
    while (query.next())
        users += query.value(0).toString() + "\t" + query.value(1).toString() + "\t" + query.value(4).toString() + "\n";
    return tmp;
}

bool Database::blockUser(const QString &login)
{
    QSqlQuery query(*mydb);
    tmp = "Update public.client set is_active = 'blocked' where public.client.login = '" + login + "'";
    query.exec(tmp);
    return true;
}

bool Database::unblockUser(const QString &login)
{
    QSqlQuery query(*mydb);
    tmp = "Update public.client set is_active = 'notBlocked' where public.client.login = '" + login + "'";
    query.exec(tmp);
    return true;
}


void Database::initThread()
{
    _sock = new QTcpSocket;
    _sock->setSocketDescriptor(_descr);
    connect(_sock, SIGNAL(readyRead()), this, SLOT(work()),Qt::DirectConnection);
}



void Database::messToParam(std::string* param, const char* data, int countParam)
{
    int j = 0;
    for (auto i = 0; i < strlen(data); i++)
    {
        if (data[i] == ' ' && (j < countParam - 1))
            j++;
        else
            param[j] += data[i];
    }
}

void Database::work()
{
        _sock->read(message, sizeof(message));
        qDebug(message);
        act = message[0];
        strcpy(message,message+1);
        switch (act - 48)
        {
        case 1:
        {
            regChat(message);
            break;
        }
        case 2:
        {
            signIn(message);
            break;
        }
        case 3:
        {
            writeMessage(message);
            break;
        }
        case 4:
        {
            usersList();
            break;
        }
        case 5:
        {
            dispChat();
            break;
        }
        case 0:
        {
            if (id[0] != '0')
            {
                memset(id, 0, sizeof(id));
                ans[0] = '1';
                std::cout << "Quit!";
                break;
            } else return;
        }
        default:
            break;
        }
        if (act != '4' && act != '5')
            _sock->write(ans, sizeof(ans));
        _sock->waitForReadyRead(1000);
        memset(message, 0, MESSAGE_LENGTH);
        memset(ans, 0, MESSAGE_LENGTH);
}

void Database::regChat(const char* data)
{
    QSqlQuery query(*mydb);
    std::string userParam[3] = {};
    messToParam(userParam, data, 3);
    char snd[1000] = {};
    std::string tmp = {};
    tmp = "SELECT * FROM client WHERE login ='" + userParam[1] + "'";
    query.exec(tmp.c_str());
    if (query.next())
    {
        strcpy(ans, "0Login is busy!");
        return;
    } else
    {
        tmp = "INSERT INTO public.client (login,firstname,lastname,is_active) VALUES ('" +  userParam[1] + "','" + userParam[0] + "',NULL,'notBlocked');"
                "UPDATE public.client_save set pass ='" + userParam[2] + "' where public.client_save.client_id = (select id from public.client where login ='" + userParam[1] + "');";
        qDebug(tmp.c_str());
        query.exec(tmp.c_str());
        tmp = "SELECT id from client where login = '" + userParam[1] + "'";
        qDebug(tmp.c_str());
        query.exec(tmp.c_str());
        query.next();
        strcpy(id, query.value(0).toString().toLatin1().data());
        ans[0] = '1';
    }

}

void Database::signIn(const char* data)
{
    QSqlQuery query(*mydb);
    std::string userParam[2] = {}, tmp;
    messToParam(userParam, data, 2);
    query.exec("SELECT * FROM client;");
    if (!query.next())
    {
        strcpy(ans, "0Register users not found!");
        return;
    }
    tmp = "SELECT id, login, is_active FROM client WHERE login = '" + userParam[0] + "' and id ="
                                                                          " (select client_id from public.client_save where pass = '" + userParam[1] + "'and client_id = id)";
    query.exec(tmp.c_str());
    if (query.next())
    {
        if (query.value(2).toString() == "blocked")
        {
            strcpy(ans, "0You are blocked!!");
                return;
        }
        ans[0] = '1';
        strcpy(id, query.value(0).toString().toLatin1().data());
    } else
        strcpy(ans, "0Wrong login/pass!");
}

void Database::writeMessage(const char* data)
{
    QSqlQuery query(*mydb);
    std::string mess[2] = {};
    messToParam(mess, data, 2);
    bool find = false;
    QString id_rec = {};
    std::string t1,t2, tmp;
    t1 = id;
    tmp = "Select is_active from public.client where id = '" + t1 + "'";
    query.exec(tmp.c_str());
    query.next();
        if (query.value(0).toString() == "blocked")
    {
        strcpy(ans, "0You are blocked!");
        return;
    }
    tmp = "SELECT * FROM public.client WHERE login = '" + mess[0] + "'";
    query.exec(tmp.c_str());
    if (query.next() && mess[0] != "all")
    {
        find = true;
        id_rec = query.value(0).toString();
    }
    if (id_rec != id && (find || mess[0] == "all"))
    {
        if (mess[0] == "all")
            t2 = "2";
        else
            t2 = id_rec.toStdString();
        tmp = "insert into public.chat (id_sender, id_recipient) values ('" + t1 + "', '" + t2 + "'); insert into public.message (chat_id, message) values (currval('chat_id_seq'::regclass),'" + mess[1].c_str() + "');";
        query.exec(tmp.c_str());
        qDebug(query.lastQuery().toLatin1().data());
        ans[0] = '1';
    }
    else
        strcpy(ans, "0Recipient is not found!");
}

void Database::usersList()
{
    QSqlQuery query(*mydb);
    QString mess = "4", buf = {};
    QString tmp = "SELECT * FROM public.client";
    query.exec(tmp);
    qDebug(query.lastQuery().toLatin1().data());
    query.next();
    while (query.next())
    {
        if (query.value(0).toString() == id)
            buf = " (this account)";
        else
            buf = "";
        mess += "Login : '" + query.value(1).toString() + "' " + buf + "\n";
    }
    if (copyUsers == mess)
        _sock->write("1", sizeof("1"));
    else
    {
    _sock->write(mess.toLatin1().data(), mess.length());
    qDebug("User list sended");
    copyUsers = mess;
    _sock->waitForReadyRead(1000);
    }
}

void Database::dispChat()
{
    QSqlQuery query(*mydb), query2(*mydb), query3(*mydb);
    QString message = "5";
    QString t1 = id;
    QString tmp = "select id_sender, id_recipient, message  from public.message left join chat on (chat_id = id)"
                      "where (id_recipient = '"+ t1 + "' or id_sender = '"+ t1 + "' or id_recipient = '2')";
    query.exec(tmp);
    query.next();
    while(query.next())
    {
        message += "\nFrom : ";
        t1 = query.value(0).toString();
        tmp = "select login from client where id ='" + t1 + "';";
        query2.exec(tmp);
        query2.next();
        message += query2.value(0).toString() + "\nTo : ";
        t1 = query.value(1).toString();
        tmp = "select login from client where id ='" + t1 + "';";
        query3.exec(tmp);
        query3.next();
        message += query3.value(0).toString() + "\nText : " + query.value(2).toString();
    }
    if (copyMess == message)
        _sock->write("1", sizeof("1"));
    else
    {
    _sock->write(message.toLatin1().data(), message.length());
    _sock->waitForReadyRead(1000);
    copyMess = message;
    qDebug("Messages sended");
    }
}
