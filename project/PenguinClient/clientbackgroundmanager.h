#ifndef CLIENTBACKGROUNDMANAGER_H
#define CLIENTBACKGROUNDMANAGER_H

#include "clientserverthread.h"
#include "c2clistenthread.h"
#include "c2cwritethread.h"
#include "../messageenvelop.h"

#include <QWidget>
#include <QtWidgets>
#include <QScrollArea>
#include <unistd.h>
#include <fstream>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QAction;


namespace PenguinClient
{

class ClientBackgroundManager : public QWidget
{
    Q_OBJECT
public:
    ClientBackgroundManager(QWidget *parent = 0);
    ~ClientBackgroundManager();

private:
    ClientServerThread myClient2ServerThread;
    C2CListenThread *myClient2ClientListenThread;
    C2CWriteThread *myClient2ClientWriteThread;
    QList<QString> myClientList;
    QString login;


    //init window
    QLabel *serverIpLabel;
    QLabel *loginLabel;
    QLabel *passwdLabel;
    QLineEdit *serverIpEdit;
    QLineEdit *loginEdit;
    QLineEdit *passwdEdit;
    QPushButton *registerButton;
    QPushButton *loginButton;
    //main window
    QLabel *listHeaderlabel;
    QVBoxLayout *insideArea;
    QWidget *myNewWidget;
    QWidget *widget;
    QList<QPushButton*> clientListButtons;
    /*test*/QPushButton *callAnotherClient1;
    /*test*/QPushButton *callAnotherClient2;
    QPushButton *logoutButton;
    QPushButton *quitButton;


signals:
    void sendDataToServer(MessageEnvelop &data);

private slots:
    //SLOTS FOR INTERNAL SIGNALS
    /**
     * @brief call1 slot for button callAnotherClient1, sends call request for client1 to server
     * internal signal used, call outern method
     */
    /*test*/void call1();
    /**
     * @brief call2 slot for button callAnotherClient2, sends call request for client2 to server
     * actually is not used, because dynamical buttons are not working
     * internal signal used, call outern method
     */
    /*test*/void call2();
    /**
     * @brief callClient slot for dynamicaly generated buttons, send call request to server
     * actually is not used, because dynamical buttons are not working
     * internal signal used, call outern method
     * ACTUALLY UNUSED - substitued by call1/2
     */
    void callClient();
    /**
     * @brief init slot for login button, initialize serverClient thread
     * after pushing submit button, allows to click logout and quit buttons
     * login and IP is read from QLineEdit, port is SERVER_VIETNAM_WAR_PORT
     * client is registered
     * internal signal used, call outern method
     */
    void initLogin();
    /**
     * @brief init slot for register button, initialize serverClient thread
     * after pushing submit button, allows to click logout and quit buttons
     * login and IP is read from QLineEdit, port is SERVER_VIETNAM_WAR_PORT
     * client is not registered - send registration data first
     * internal signal used, call outern method
     */
    void initRegister();
    /**
     * @brief logout slot for logout button, send logout to server and kill serverClient thread
     * internal signal used, call outern method
     */
    void logout();
    /**
     * @brief enableSubmitButton allow to click logout and quit
     * internal signal used, call internall method
     */
    void enableSubmitButton();


public slots:
    //SLOTS FOR EXTERNAL SIGNAL
    /**
     * @brief displayClientList shows list of ClientServerThread
     * @param[in] list QList<QString> of clients logins
     * actually are dymamical generated buttons inactive
     * client list is printed by qDebug(), static buttons just for 2 clients
     */
    void displayClientList(const QList<QString> list);
    /**
     * @brief incommingCall 1. part of initialazing communication between 2 clients
     * @param[in] name login of client, which initialize call
     * @param[in] IP IP of client, which initialize call
     * @param[in] hisPort port of client, which initialize call
     * @param[in] myPort port of this user, got from socket for communication to server
     */
    void incommingCall(const QString name, const QHostAddress IP);
    /**
     * @brief successResponseCall 2. part of initialazing communication between 2 clients
     * @param[in] name login of client, which initialize call
     * @param[in] IP IP of client, which initialize call
     * @param[in] hisPort port of client, which initialize call
     * @param[in] myPort port of this user, got from socket for communication to server
     */
    void successResponseCall(const QString name, const QHostAddress IP,
                       QString aesKey);
    /**
     * @brief incomingEndOfCall call ended, I terminate threads between clients
     */
    void incomingEndOfCall();
    /**
     * @brief displayError print error of TcpSocket message
     * @param socketError number of error
     * @param message string version error
     * external signal is used, call internal method
     */
    void displayError(int socketError, const QString &message);

};

}//end of namespace PenguinClient

#endif // CLIENTBACKGROUNDMANAGER_H
