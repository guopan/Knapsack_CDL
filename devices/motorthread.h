#ifndef MOTORTHREAD_H
#define MOTORTHREAD_H
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class motorthread : public QThread
{
    Q_OBJECT
public:
    motorthread(QObject *parent = 0);
    ~motorthread();

    void transaction(const QString &portName, const QString &request);
    void run();

signals:
    void S_PortNotOpen();
    void response2(const QString &s);
    void timeout2();


private:
    QString portName;
    QString request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};

#endif // MOTORTHREAD_H








