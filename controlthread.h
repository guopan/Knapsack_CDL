#ifndef CONTROLTHREAD_H
#define CONTROLTHREAD_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class controlthread : public QThread
{
public:
    explicit controlthread(QObject *parent = nullptr);
    ~controlthread();
    void transaction();
    void run() Q_DECL_OVERRIDE;
signals:

private:

};

#endif // CONTROLTHREAD_H
