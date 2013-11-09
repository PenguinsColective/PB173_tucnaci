#include "serverthread.h"

#include <QDataStream>

#include "../messageenvelop.h"

namespace PenguinServer
{

ServerThread::ServerThread(qintptr socketDescriptor, SharedList *list, QObject *parent) :
    QThread(), s(), socketDescriptor(socketDescriptor), list(list),
    pending(0), available(true), isInitialized(false)
{


}

void ServerThread::run()
{
    s = new QTcpSocket(this);
    if(!s->setSocketDescriptor(socketDescriptor))
    {
        emit error(s->error());
    }

    connect(s, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::QueuedConnection);
    connect(s, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);

    exec();

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
    s->waitForDisconnected();


}

void ServerThread::initialize()
{
    isInitialized = true;
    MessageEnvelop e;
    QDataStream str(s);

    str >> e;


    if(e.getRequestType() != SEND_LOGIN_TO_SERVER)
    {
        sendError("The Connection is bad Mkay. You should try it again M'Kay");
        emit error(s->error());
        s->deleteLater();
        emit disconnected();
        return;
    }



    ConnectedClient * c = new ConnectedClient(s->peerAddress(), e.getName(),
                                              s->peerPort());

    if(!list->addClient(c))
    {
        sendError("The connection already exists M'kay");
        emit error(s->error());
    }
    this->name = e.getName();
    list->callAllClients();



}

void ServerThread::requestCall(const QString & login)
{
    list->callClient(login, name);
}

void ServerThread::readyRead()
{
    if(!isInitialized)
    {
        initialize();
        return;
    }

    QDataStream input(s);

    MessageEnvelop e;
    input >> e;

    switch (e.getRequestType())
    {
    case SEND_LOGIN_TO_SERVER:
    case PING:
    case OK:
        return;
    case REQUEST_CALL_TO_CLIENT_FROM_SERVER:
        return requestCall(data);
    case REQUEST_CLIENT_LIST_FROM_SERVER:
        return sendError("The List will be sent M'kay");

    case END_OF_CALL_TO_CLIENT:
    case ERROR_SERVER_RESPONSE:
        return sendError("Only me can communicate on port 666 MUHEHEHE");
    default:
        return sendError("Unrecognized messsage type M'kay");
    }


}

void ServerThread::disconnected()
{
    list->removeClient(name);
    s->deleteLater();
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

//    QString name = cli->getName();
//    QHostAddress address = cli->getIpAddr();
//    qint16 port = cli->getPort();

    QByteArray block;
    QDataStream str(&block, QIODevice::WriteOnly);

    MessageEnvelop e(reason);
    e.setAddr(cli->getIpAddr());
    e.setName(cli->getName());
    e.setPort(cli->getPort());

//    str << (qint16) 0;
//    str << reason
//        << name << " " << address << " " <<  port;
//    str.device()->seek(0);
//    str << (qint16) (block.size() - sizeof(qint16));

    str << e;

    s->write(block);
}

void ServerThread::askNewConnection(ConnectedClient * cli)
{
    sendAClient(SEND_INCOMMING_CALL_TO_CLIENT,cli);
}

void ServerThread::connectionOnSuccess(ConnectedClient *cli)
{
    sendAClient(SEND_SUCCESS_RESPONSE_TO_COMMUNICATION, cli);
}

void ServerThread::distributeClients(QList<QString> list)
{
//    QList<QString>::Iterator it;

    QByteArray block;
    QDataStream str(&block, QIODevice::WriteOnly);
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
}

}// namespace PenguinServer
