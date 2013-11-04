#include "serverlistener.h"

namespace PenguinServer
{

ServerListener::ServerListener(QObject *parent) :
    QTcpServer(parent)
{
    if(!this->listen(QHostAddress::Any, 27173))
    {

    }
}

void ServerListener::incomingConnection(qintptr handle)
{
    ServerThread * thread = new ServerThread(handle, list, this);

    connect(thread, SIGNAL(finished()), thread, SLOT(deletelater()));
}

}//namespace PenguinServer
