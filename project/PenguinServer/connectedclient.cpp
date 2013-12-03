#include "connectedclient.h"
#include "../messageenvelop.h"
#include "serverthread.h"

#include <algorithm>



namespace PenguinServer
{
ConnectedClient::ConnectedClient(QHostAddress ipAddr, QString name, qint16 port, ServerThread *parent):
     QObject(parent), ipaddress(ipAddr), port(port), name(name)
{

}

void ConnectedClient::init(ServerThread * parent)
{
    connect(this, SIGNAL(sendUpdatedList(QList<QString>)),
             parent, SLOT(distributeClients(QList<QString>)), Qt::DirectConnection);
    connect(this, SIGNAL(denyConnection(ConnectedClient*)),
             parent, SLOT(connectionDenied(ConnectedClient*)), Qt::DirectConnection);
    connect(this, SIGNAL(allowConnection(ConnectedClient*)),
            parent, SLOT(connectionOnSuccess(ConnectedClient*)),Qt::DirectConnection);
    connect(this, SIGNAL(requestConnection(ConnectedClient*)),
            parent, SLOT(askNewConnection(ConnectedClient*)), Qt::DirectConnection);
}

QString ConnectedClient::getName()
{
    mutex.lock();
    QString q = name;
    mutex.unlock();
    return q;
}

QHostAddress ConnectedClient::getIpAddr()
{
    mutex.lock();
    QHostAddress t = ipaddress;
    mutex.unlock();
    return t;
}

qint16 ConnectedClient::getPort()
{
    mutex.lock();
    qint16 retVal = port;
    mutex.unlock();
    return retVal;
}

void ConnectedClient::SetAesKey(const QString &key)
{
    mutex.lock();
    this->aesKey = key;
    mutex.unlock();
}

QString ConnectedClient::getAndDestroyKey()
{
    mutex.lock();
    QString retval(aesKey);
    std::fill(aesKey.begin(), aesKey.end(), '\0');
    mutex.unlock();
    return retval;

}

void ConnectedClient::callRequest(int reqID, ConnectedClient *cli)
{
    switch(reqID)
    {
    case SEND_INCOMMING_CALL_TO_CLIENT:
        emit this->requestConnection(cli);
        return;
    case SEND_SUCCESS_RESPONSE_TO_COMMUNICATION:

        emit this->allowConnection(cli);
        return;
    case SEND_DENIED_RESPONSE_TO_COMMUNICATION:
        emit this->denyConnection(cli);
        return;
    default:
        return;
    }


}

void ConnectedClient::sendList(QList<QString> list)
{
    emit this->sendUpdatedList(list);
}

}// PenguinServer
