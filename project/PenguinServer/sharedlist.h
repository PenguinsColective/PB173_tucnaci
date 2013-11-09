#ifndef SHAREDLIST_H
#define SHAREDLIST_H

#include "connectedclient.h"
#include "serverthread.h"

#include <QMutex>
#include <QList>
#include <QMap>


namespace PenguinServer
{

class ConnectedClient;

class SharedList: public QObject
{
    Q_OBJECT
public:
    SharedList();

    bool addClient(ConnectedClient*);

    bool removeClient(const QString &);

    void callAllClients();

    void callClient(const QString & destName, const QString & srcName);



private:
    QMap<QString, ConnectedClient*> clients;
    QMutex mutex;
};

}//namespace PenguinServer
#endif // SHAREDLIST_H
