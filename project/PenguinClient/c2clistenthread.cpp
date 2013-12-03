#include "c2clistenthread.h"
#include "c2ctcp.h"
#include "./../messageenvelop.h"
#include <iostream>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
namespace PenguinClient
{
C2CListenThread::C2CListenThread(QObject *parent) :
    QThread(parent), quit(false)
{
    port = CLIENT_PEARL_HARBOR_PORT;
}

C2CListenThread::~C2CListenThread(){

}

void C2CListenThread::startListener(const QHostAddress &hostName, QString key){
    qDebug() << "Listener started";
    QMutexLocker locker(&mutex);
    this->key = key;
    this->hostName = hostName;

    if (!isRunning()){
        start();
    } else {
         cond.wakeOne();
    }
}

void C2CListenThread::run(){

    server.startServer(key, hostName, port);
    qDebug() <<  (server.errorString());    
    exec();
}

void C2CListenThread::endConnection(){
    server.deleteLater();
}

void ListenServer::incomingConnection(qintptr socketDescriptor){
    C2CTcpListen *thread = new C2CTcpListen(key, socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void ListenServer::startServer(QString key, const QHostAddress hostName, const quint16 port)
{
    this->key = key;
    if(!this->listen(hostName, port))
    {
        std::cerr << "Could not start server";

    }
}

ListenServer::ListenServer(QObject *parent):
    QTcpServer(parent)
{
}
}
