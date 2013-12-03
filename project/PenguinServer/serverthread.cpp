#include "serverthread.h"

#include <QDataStream>
#include <fstream>
#include <polarssl/sha2.h>



namespace PenguinServer
{

ServerThread::ServerThread(qintptr socketDescriptor, SharedList *list, SqlConnection *conn, QObject *parent) :
    QThread(parent), s(), socketDescriptor(socketDescriptor), list(list), database(conn),
    pending(), available(true), isInitialized(false), c(0)
{

    qDebug() << socketDescriptor;
}

void ServerThread::run()
{
    QFile keyFile("./server.key");
    s = new QSslSocket();

    if(!s->setSocketDescriptor(socketDescriptor))
    {
        emit error(s->error());
    }

    if(!keyFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "File was not opened";
    }

    QSslKey key(&keyFile, QSsl::Rsa, QSsl::Pem ,QSsl::PrivateKey);

    s->setPrivateKey(key);
    s->setLocalCertificate("./server.crt",QSsl::Pem);
    s->addCaCertificates("./ca.crt", QSsl::Pem);
    //s->addCaCertificates("./server.crt",QSsl::Pem);
    connect(s, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::QueuedConnection);
    connect(s, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);
    connect(s, SIGNAL(encrypted()), this, SLOT(ready()),Qt::QueuedConnection);
    connect(s, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(errorm(QList<QSslError>)),Qt::DirectConnection);
    s->startServerEncryption();


    qDebug() << "started listening";

    exec();

}

void ServerThread::ready()
{
    qDebug() << "ok";
}

void ServerThread::registerNewClient(MessageEnvelop &e)
{
    if(database->existsUser(e.getName().toStdString()))
    {
        sendError("The Username already exists M'kay, pick another one M'kay");
        emit error(this->s->error());
        return;
    }
    std::ifstream rand("/dev/urandom",std::ios::binary);
    char * newSalt = new char[8];
    rand.read(newSalt, 8);
    rand.close();
    char * corrSalt = getAscii85(newSalt, 8);
    delete[] newSalt;


    std::string s(e.getPassword().toStdString()), qCorrSalt(corrSalt);
    s = s + qCorrSalt;

    free(corrSalt);
    unsigned char hash[32];
    char *printableHash;

    sha2((unsigned char *) s.c_str(), s.length(), hash, 0);
    printableHash = getAscii85((char*) hash, 32);
    QString pass(printableHash), Qsalt(qCorrSalt.c_str());
    try
    {
        database->insertUser(e.getName().toStdString(), pass.toStdString(),
                         Qsalt.toStdString());
    }
    catch(SqlConnection::SqlException e)
    {
        sendError("The user was not added");
        emit error(this->s->error());
        return;
    }

    QByteArray b;
    QDataStream outStr(&b, QIODevice::WriteOnly);
    MessageEnvelop ret(REGISTER_APROOVED);

    isInitialized = false;
    outStr << ret;

    this->s->write(b);





}

bool ServerThread::verify(const QString & name, const QString & password)
{
    if(!database->existsUser(name.toStdString()))
    {
        return false;
    }
    auto info = database->getUserByName(name.toStdString());

    QString s(password);
    s = s + info.getSalt();
    unsigned char hash[32];
    sha2((unsigned char*) s.toUtf8().constData(), s.length(), hash, 0);
    char * base = getAscii85((char*) hash, 32);
    QString hashed(base);
    free(base);
    if(hashed != info.getPass())
    {
        return false;
    }

    return true;


}

void ServerThread::sendError(QString str)
{
    QByteArray bl;
    QDataStream outStr(&bl, QIODevice::WriteOnly);

    MessageEnvelop e(ERROR_SERVER_RESPONSE);
    e.setName(str);

    outStr << e;

    s->write(bl);
    s->disconnectFromHost();
    //s->waitForDisconnected();





}

void ServerThread::initialize()
{
    isInitialized = true;
    MessageEnvelop e;
    QDataStream str(s);
    try
    {
        str >> e;
    }
    catch(MessageException e)
    {

        return;
    }

    qDebug() << e.getRequestType() << " initialize on data " << e.getName();

    if(e.getRequestType() == REGISTER_TO_SERVER)
    {
        registerNewClient(e);
        return;
    }

    if(e.getRequestType() != SEND_LOGIN_TO_SERVER)
    {
        sendError("The Connection is bad Mkay. You should try it again M'Kay");
        emit error(s->error());
        //s->deleteLater();
        //emit disconnected();
        return;
    }
    bool result = false;
    try
    {
        result = verify(e.getName(), e.getPassword());
    }
    catch (SqlConnection::SqlException e)
    {
        sendError("The username is not in the database M'kay");
        emit error(s->error());
        return;
    }

    if(!result)
    {
        sendError("The password is bad M'kay");
        emit error(s->error());
        return;
    }

    ConnectedClient * c = new ConnectedClient(s->peerAddress(), e.getName(),
                                              s->peerPort(), this);

    qDebug() << "The host " << e.getName()
             << " on address " <<   s->peerAddress() << " on port " << s->peerPort();
    c->init(this);
    qDebug() << "opened connection to " << e.getName();
    if(!list->addClient(c))
    {
        sendError("The connection already exists M'kay");
        emit error(s->error());
    }
    this->c = c;
    this->name = e.getName();
    list->callAllClients();



}

void ServerThread::requestCall(const QString & login)
{
    list->callClient(login, name, SEND_INCOMMING_CALL_TO_CLIENT);
    pending = login;
    available = false;
}

void ServerThread::sendConnectionDenied()
{

    list->callClient(pending, name, SEND_DENIED_RESPONSE_TO_COMMUNICATION);
    available = true;
}

void ServerThread::ConnectionGranted(QString key)
{
    this->c->SetAesKey(key);
    list->callClient(pending, name, SEND_SUCCESS_RESPONSE_TO_COMMUNICATION);
}

void ServerThread::logout()
{
    //list->removeClient(name);

//    MessageEnvelop e(SEND_LOGOUT_RESPONSE);
//    e.setName(name);

//    QByteArray block;
//    QDataStream str(&block, QIODevice::WriteOnly);
//    str << e;

//    s->write(block);
    s->disconnectFromHost();
    //s->waitForDisconnected();

}

void ServerThread::readyRead()
{
        if(!isInitialized)
    {
        initialize();
        return;
    }

    QDataStream input(s);
    s->waitForReadyRead();
    MessageEnvelop e;
    try
    {
        input >> e;
    }
    catch(MessageException e)
    {
        qDebug() << "Hackers are strong "
                 << "But I'm stronger";
        return;
        //sendError("Telnet Cannot kill me");
    }

    switch (e.getRequestType())
    {
    case SEND_LOGIN_TO_SERVER:
    case PING:
    case OK:
        return;
    case REQUEST_CALL_TO_CLIENT_FROM_SERVER:
        qDebug() << "Connection init";
        return requestCall(e.getName());
    case REQUEST_CLIENT_LIST_FROM_SERVER:
        return sendError("The List will be sent M'kay");
    case SEND_DENIED_RESPONSE_TO_COMMUNICATION:
        sendConnectionDenied();
        return;
    case SEND_SUCCESS_RESPONSE_TO_COMMUNICATION:
        qDebug() << "Connection Granted";
        ConnectionGranted(e.getPassword());
        return;

    case SEND_LOGOUT_REQUEST:
        logout();
        return;
    case END_OF_CALL_TO_CLIENT:
    case ERROR_SERVER_RESPONSE:
        return sendError("Only me can communicate on port 666 MUHEHEHE");
    default:
        return sendError("Unrecognized messsage type M'kay");
    }


}

void ServerThread::disconnected()
{
    qDebug() << "Disconnected" << name;
    if(isInitialized)
    {
        list->removeClient(name);
    }
    emit s->deleteLater();
    //emit deleteLater();
    exit(0);

}

void ServerThread::connectionDenied(ConnectedClient *cli)
{
    QString name = cli->getName();
    QByteArray block;
    QDataStream str(&block, QIODevice::WriteOnly);

    MessageEnvelop e(SEND_DENIED_RESPONSE_TO_COMMUNICATION);
    e.setName(name);

    str << e;

    s->write(block);


}

void ServerThread::sendAClient(qint16 reason, ConnectedClient * cli)
{

    QMutexLocker l(&m);
//    QString name = cli->getName();
//    QHostAddress address = cli->getIpAddr();
//    qint16 port = cli->getPort();
    qDebug() << "Sending client to" << cli->getName()
             << " from " << name;
    QByteArray block;
    QDataStream str(&block, QIODevice::WriteOnly);

    MessageEnvelop e(reason);
    e.setAddr(cli->getIpAddr());
    e.setName(cli->getName());
    e.setPort(cli->getPort());
    if(reason == SEND_SUCCESS_RESPONSE_TO_COMMUNICATION)
    {
        e.setPassword(cli->getAndDestroyKey());
    }

//    str << (qint16) 0;
//    str << reason
//        << name << " " << address << " " <<  port;
//    str.device()->seek(0);
//    str << (qint16) (block.size() - sizeof(qint16));

    str << e;

    s->write(block);


}

void ServerThread::errorm(const QList<QSslError> & errors)
{
    qDebug("someError");
}

void ServerThread::askNewConnection(ConnectedClient * cli)
{
    qDebug() << "Asking new connection from " << name
             << " to " << cli->getName();
    pending = cli->getName();
    sendAClient(SEND_INCOMMING_CALL_TO_CLIENT,cli);
}

void ServerThread::connectionOnSuccess(ConnectedClient *cli)
{
    qDebug() << "Succeeded connection between " << name
             << " and " << cli->getName();
    sendAClient(SEND_SUCCESS_RESPONSE_TO_COMMUNICATION, cli);
}

void ServerThread::distributeClients(QList<QString> list)
{
//    QList<QString>::Iterator it;

    qDebug() << " sending clients to " << name;
    QByteArray block;
    QDataStream str(&block, QIODevice::Append);
//    str << (qint16) 0;
//    for(it = list.begin(); it != list.end(); it++)
//    {
//        if(*it == name) continue;
//        str << *it << " ";

//    }
//    str.device()->seek(0);
//    str << (qint16) (block.size() - sizeof(qint16));

    MessageEnvelop e(SEND_CLIENT_LIST_TO_CLIENT);
    e.setClients(list);

    str << e;

    s->write(block);

    while(s->encryptedBytesToWrite() != 0)
    {
        msleep(100);
    }

}

}// namespace PenguinServer
