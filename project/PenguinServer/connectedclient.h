#ifndef CONNECTEDCLIENT_H
#define CONNECTEDCLIENT_H


#include <QMutex>
#include <QtNetwork>
#include <QDataStream>
#include "serverthread.h"
namespace PenguinServer
{

class ServerThread; //forward declaration


/**
 * @brief The ConnectedClient class
 *        Class represents data of client in system.
 *        Is connected to ServerThread which is operating all incomming and outcomming data
 *
 */
class ConnectedClient : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief ConnectedClient constructor
     * @param ipAddr Address of the client
     * @param name name of the client
     * @param port port which is used to communication
     * @param parent QObject
     */
    ConnectedClient(QHostAddress ipAddr, QString name, qint16 port, ServerThread *parent =0);

    virtual ~ConnectedClient() { }

    /**
     * @brief getName
     * @return name
     */
    QString getName();

    /**
     * @brief getPort
     * @return port number
     */
    qint16 getPort();

    /**
     * @brief getIpAddr
     * @return ip address
     */
    QHostAddress getIpAddr() ;

    void SetAesKey(const QString & key);

    QString getAndDestroyKey();

    /**
     * @brief callRequest
     *        Calls request - emits signal, to Thread to send data from another client
     * @param reqId ID of request more in ../messageenvelop.h
     * @param cli client whose data mus be sent
     */
    void callRequest(int reqId, ConnectedClient*cli);

    /**
     * @brief sendList
     *        Sends list of clients to this client - emits signal
     * @param list data to be sent
     */
    void sendList(QList<QString> list);

    /**
     * @brief init
     *        Initializes the conectivity to parent thread, connects this signalst to
     *        parent->slots
     * @param parent
     */
    void init(ServerThread *parent);


signals:

    /**
     * @brief sendUpdatedList
     * @param s
     */
    void sendUpdatedList(QList<QString> s);

    /**
     * @brief requestConnection
     */
    void requestConnection(ConnectedClient*);

    /**
     * @brief denyConnection
     */
    void denyConnection(ConnectedClient*);

    /**
     * @brief allowConnection
     */
    void allowConnection(ConnectedClient*);

private:

    QHostAddress ipaddress;
    qint16 port;
    QString name;

    QString aesKey;

    QMutex mutex;

};

}
#endif // CONNECTEDCLIENT_H
