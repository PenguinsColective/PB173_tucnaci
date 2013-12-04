#include "clientbackgroundmanager.h"\

namespace PenguinClient
{



ClientBackgroundManager::ClientBackgroundManager(QWidget *parent)
    : QWidget(parent)
{
    serverIpLabel = new QLabel(tr("&Server IP:"));
    loginLabel = new QLabel(tr("&Login:"));
    passwdLabel = new QLabel(tr("&Password:"));

    serverIpEdit = new QLineEdit(tr("192.168.15.4"));

    loginEdit = new QLineEdit(tr("client"));
    passwdEdit = new QLineEdit(tr(""));
    passwdEdit->setEchoMode(QLineEdit::Password);

    serverIpLabel->setBuddy(serverIpEdit);
    loginLabel->setBuddy(loginEdit);
    passwdLabel->setBuddy(passwdEdit);

    loginButton = new QPushButton(tr("Login"));
    loginButton->setEnabled(false);

    registerButton = new QPushButton(tr("Register"));
    registerButton->setEnabled(false);

    listHeaderlabel = new QLabel(tr("Seznam online klientu"));
    listHeaderlabel->setEnabled(false);

    myNewWidget = new QWidget();
    insideArea = new QVBoxLayout(myNewWidget);
    myNewWidget->setLayout(insideArea);

    logoutButton = new QPushButton(tr("Logout"));
    logoutButton->setDefault(false);
    logoutButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));
    quitButton->setDefault(false);
    quitButton->setEnabled(false);

    connect(serverIpEdit, SIGNAL(textChanged(QString)), this,
            SLOT(enableSubmitButton()));
    connect(loginEdit, SIGNAL(textChanged(QString)), this,
            SLOT(enableSubmitButton()));
    connect(loginButton, SIGNAL(clicked()), this, SLOT(initLogin()));
    connect(registerButton, SIGNAL(clicked()), this, SLOT(initRegister()));
    connect(logoutButton, SIGNAL(clicked()), this, SLOT(logout()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    //to server
    connect(this, SIGNAL(sendDataToServer(MessageEnvelop&)),
            &myClient2ServerThread, SLOT(sendMessageToServer(MessageEnvelop&)), Qt::DirectConnection);
    //from server
    connect(&myClient2ServerThread, SIGNAL(error(int, QString)),
            this, SLOT(displayError(int, QString)));
    connect(&myClient2ServerThread, SIGNAL(clientList(const QList<QString>)),
            this, SLOT(displayClientList(const QList<QString>)), Qt::DirectConnection);
    connect(&myClient2ServerThread,
            SIGNAL(incommingCall(const QString, const QHostAddress, const quint16, const quint16)),
            this, SLOT(incommingCall(const QString, const QHostAddress, const quint16, const quint16)),
            Qt::DirectConnection);
    connect(&myClient2ServerThread,
            SIGNAL(successResponseCall(const QString, const QHostAddress, const quint16, const quint16)),
            this, SLOT(successResponseCall(const QString, const QHostAddress, const quint16, const quint16)),
            Qt::DirectConnection);

    /*test*/callAnotherClient1 = new QPushButton(tr("call 1. client"));
    /*test*/callAnotherClient1->setDefault(true);
    /*test*/callAnotherClient1->setEnabled(true);
    /*test*/callAnotherClient2 = new QPushButton(tr("call 2. client"));
    /*test*/callAnotherClient2->setDefault(true);
    /*test*/callAnotherClient2->setEnabled(true);
    /*test*/connect(callAnotherClient1, SIGNAL(clicked()), this, SLOT(call1()));
    /*test*/connect(callAnotherClient2, SIGNAL(clicked()), this, SLOT(call2()));


    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(serverIpLabel, 0, 0);
    mainLayout->addWidget(serverIpEdit, 0, 1);
    mainLayout->addWidget(loginLabel, 1, 0);
    mainLayout->addWidget(loginEdit, 1, 1);
    mainLayout->addWidget(passwdLabel, 2, 0);
    mainLayout->addWidget(passwdEdit, 2, 1);
    mainLayout->addWidget(loginButton, 3, 0);
    mainLayout->addWidget(registerButton, 3, 1);

    mainLayout->addWidget(listHeaderlabel, 4, 0);
    mainLayout->addWidget(myNewWidget, 5, 0);
    /*test*/mainLayout->addWidget(callAnotherClient1, 6, 0);
    /*test*/mainLayout->addWidget(callAnotherClient2, 6, 1);
    mainLayout->addWidget(logoutButton, 7, 0);
    mainLayout->addWidget(quitButton, 7, 1);

    setLayout(mainLayout);


    setWindowTitle(tr("Penguin Client"));
    serverIpEdit->setFocus();
}

ClientBackgroundManager::~ClientBackgroundManager() {
    myClient2ServerThread.terminate();
}

void ClientBackgroundManager::initLogin() {
    QString adress = serverIpEdit->text();
    quint16 port = SERVER_VIETNAM_WAR_PORT;
    login = loginEdit->text();
    QString passwd = passwdEdit->text();

    myClient2ServerThread.initThread(adress, port, login, passwd, false);

    //maybe wait for reply from server, but not for testing
    /*test*/listHeaderlabel->setEnabled(true);
    /*test*/logoutButton->setEnabled(true);
    /*test*/quitButton->setEnabled(true);
}

void ClientBackgroundManager::initRegister() {
    QString adress = serverIpEdit->text();
    quint16 port = SERVER_VIETNAM_WAR_PORT;
    login = loginEdit->text();
    QString passwd = passwdEdit->text();

    myClient2ServerThread.initThread(adress, port, login, passwd, true);

    //maybe wait for reply from server, but not for testing
    /*test*/listHeaderlabel->setEnabled(true);
    /*test*/logoutButton->setEnabled(true);
    /*test*/quitButton->setEnabled(true);
}

void ClientBackgroundManager::enableSubmitButton() {
    bool enable = (!serverIpEdit->text().isEmpty() && !loginEdit->text().isEmpty());
    loginButton->setEnabled(enable);
    registerButton->setEnabled(enable);
}

void ClientBackgroundManager::displayClientList(const QList<QString> list) {
    qDebug() << "Called displayClientList";
    QString str;
    //release buttons
    foreach (QPushButton* button, clientListButtons) {
        button->close();
        //delete button;
    }
    //clientListButtons.clear();
    //create new
    foreach(str, list) {
        qDebug() << "Showing client " << str;
        /*QPushButton* clientCallButton = new QPushButton(str);
        insideArea->addWidget(clientCallButton);
        clientCallButton->setObjectName(str);
        connect(clientCallButton, SIGNAL(clicked()), this, SLOT(callClient()));
        clientListButtons.push_back(clientCallButton);
        insideArea->update();*/
    }
}

void ClientBackgroundManager::call1() {
    MessageEnvelop call(REQUEST_CALL_TO_CLIENT_FROM_SERVER);
    call.setName("client1");
    emit sendDataToServer(call);
}

void ClientBackgroundManager::call2() {
    MessageEnvelop call(REQUEST_CALL_TO_CLIENT_FROM_SERVER);
    call.setName("client2");
    emit sendDataToServer(call);
}

void ClientBackgroundManager::callClient() {
    QObject* sendedFrom = sender();
    qDebug() << "Called callClient" << sendedFrom->objectName();
    //send message to server
    MessageEnvelop call(REQUEST_CALL_TO_CLIENT_FROM_SERVER);
    call.setName(sendedFrom->objectName());
    emit sendDataToServer(call);
    //myClient2ServerThread.sendMessageToServer(call);
    //GUI

}

void ClientBackgroundManager::incommingCall(const QString name,
                                            const QHostAddress IP) {

    //info about incomming call
    qDebug() << "Called incommingCall " << name << " IP " << IP;
    //QStringList list = from.split(" ");

    //generating keys for threads
    std::ifstream rand("/dev/urandom",std::ios::binary);
    char *keyCharArray = new char[256];
    rand.read(keyCharArray, 256);
    rand.close();
    QString keyString(keyCharArray);
    delete[] keyCharArray;
    QString firstHalf = keyString.mid(0, 128);
    QString secondHalf = keyString.mid(128, 128);


    MessageEnvelop sendData(SEND_SUCCESS_RESPONSE_TO_COMMUNICATION);
    sendData.setPassword(keyString);
    emit sendDataToServer(sendData);

    //listen thread
    /*test*/myClient2ClientListenThread = new C2CListenThread();
    /*test*/myClient2ClientListenThread->startListener(IP, firstHalf);

    sleep(1); //wait for startListener init all right
    //write thread
    qDebug() << "write start";
    /*test*/myClient2ClientWriteThread = new C2CWriteThread();
    /*test*/myClient2ClientWriteThread->startOutput(IP, secondHalf);

    //GUI
    /*QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Hovor s " + name,
                                      "Chcete navazat hovor s hovor " + name + " Answer?",
                                      QMessageBox::Yes|QMessageBox::No);
    if(reply) { //answer
        MessageEnvelop sendData(SEND_SUCCESS_RESPONSE_TO_COMMUNICATION);
        emit sendDataToServer(sendData);

        myClient2ClientListenThread = new C2CListenThread();
        myClient2ClientListenThread->startListener(IP);

        QMessageBox::StandardButton really;
        really = QMessageBox::question(this, "Zapnete si mikrofon",
                                      "Zapli jste mikrofon?",
                                      QMessageBox::Yes|QMessageBox::No);
        if(really) {
            myClient2ClientWriteThread = new C2CWriteThread();
            myClient2ClientWriteThread->startOutput(IP, port);
            QMessageBox msgBox;
            msgBox.setText("You are speaking to" + name);
            msgBox.setStandardButtons(QMessageBox::Cancel);
            msgBox.exec();
            if(QMessageBox::Save) {
                MessageEnvelop sendData(END_OF_CALL_FROM_CLIENT);
                emit sendDataToServer(sendData);
            }
        }

    }
    else {//deny
        MessageEnvelop sendData(SEND_DENIED_RESPONSE_TO_COMMUNICATION);
        emit sendDataToServer(sendData);
        //myClient2ServerThread.sendMessageToServer(sendData);
    }*/
}

void ClientBackgroundManager::successResponseCall(const QString name,
             const QHostAddress IP, QString aesKey) {

    qDebug() << "Called successResponseCall " << name << " IP " << IP;
    ///*test*/QString notParsedClientData = "karlos 127.0.0.1 1234";
    //QStringList list = from.split(" ");

    QString firstHalf = aesKey.mid(0, 128);
    QString secondHalf = aesKey.mid(128, 256);

    /*test*/myClient2ClientListenThread = new C2CListenThread();
    /*test*/myClient2ClientListenThread->startListener(IP, firstHalf);

    sleep(1);
    qDebug() << "write start";
    /*test*/myClient2ClientWriteThread = new C2CWriteThread();
    /*test*/myClient2ClientWriteThread->startOutput(IP, secondHalf);
}

void ClientBackgroundManager::incomingEndOfCall() {
    qDebug() << "Called incomingEndOfCall";
    //lets exterminate Luke!!!
    myClient2ClientWriteThread->~C2CWriteThread();
    myClient2ClientListenThread->~C2CListenThread();
}

void ClientBackgroundManager::displayError(int socketError, const QString &message) {
    qDebug() << "Socket error " << socketError << message;

    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Blocking Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Blocking Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Blocking Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(message));
    }
}


void ClientBackgroundManager::logout() {
    MessageEnvelop sendData(SEND_LOGOUT_REQUEST);
    emit sendDataToServer(sendData);

//    myClient2ServerThread.disconnect();
//    submitButton->setEnabled(true);
}

}//end of namespace PenguinClient

