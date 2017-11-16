#ifndef COMPASSTHREAD_H
#define COMPASSTHREAD_H
#include<QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QtSerialPort/QSerialPort>

class compassThread : public QThread
{
    Q_OBJECT
public:
    explicit compassThread(QObject *parent = nullptr);
    ~compassThread();
    void transaction(const QString &portName, const QByteArray &request);
    void run() Q_DECL_OVERRIDE;
signals:
    void response(const QByteArray &s);
    void error(const QString &s);
    void timeout(const QString &s);
private:
    QString portName;
    QByteArray request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};

#endif // COMPASSTHREAD_H
