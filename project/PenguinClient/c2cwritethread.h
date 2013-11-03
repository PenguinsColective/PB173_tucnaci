#ifndef C2CWRITETHREAD_H
#define C2CWRITETHREAD_H

#include <QThread>

class C2CWriteThread : public QThread
{
    Q_OBJECT
public:
    explicit C2CWriteThread(QObject *parent = 0);
    ~C2CWriteThread();

    void startOutput(const QString &hostName, const quint16 port);
    void run();

signals:
    void error(int socketError, const QString &message);

public slots:
    
private:
    QString hostName;
    quint16 port;
};

#endif // C2CWRITETHREAD_H