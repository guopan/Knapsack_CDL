#include "controlthread.h"


QT_USE_NAMESPACE

controlthread::controlthread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
}

controlthread::~controlthread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void controlthread::transaction()
{
    QMutexLocker locker(&mutex);
    if (!isRunning())
    {
         start();
    }
    else
    {
       cond.wakeOne();
    }
}

void controlthread::run()
{

}


