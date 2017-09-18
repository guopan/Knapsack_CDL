#ifndef laserpulsethread_H
#define laserpulsethread_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QByteArray>

class laserpulsethread : public QThread
{
    Q_OBJECT
public:
    laserpulsethread(QObject *parent = 0);
    ~laserpulsethread();

    void transaction(const QString &portName, const QByteArray &request);
    void run();

signals:
    void Pulse_PortNotOpen();
    void responsePulse(const QString &s);
    void timeoutPulse();


private:
    QString portName;
    QByteArray request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};
#endif // laserpulsethread_H
