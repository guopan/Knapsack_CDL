#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include "laserseedthread.h"

laserseedthread::laserseedthread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
}

laserseedthread::~laserseedthread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void laserseedthread::transaction(const QString &portName, const QByteArray &request)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->waitTimeout = 3000;
    this->request = request;
    if(!isRunning())
        start();
    else
        cond.wakeOne();
}

void laserseedthread::run()
{
    bool currentPortNameChanged = false;

    mutex.lock();
    QString currentPortName;
    if(currentPortName != portName)
    {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = waitTimeout;
    QByteArray currentRequest = request;

    mutex.unlock();
    QSerialPort serial;
    while(!quit)
    {
        if(currentPortNameChanged)
        {
            serial.close();
            serial.setPortName(currentPortName);
            if(!serial.open(QIODevice::ReadWrite))
            {
                emit this->seed_PortNotOpen();						//串口未成功打开，信号返回到主程序
                return;
            }
            serial.setBaudRate(QSerialPort::Baud9600);			//波特率
            serial.setDataBits(QSerialPort::Data8);				//数据位
            serial.setParity(QSerialPort::NoParity);			//校验位
            serial.setStopBits(QSerialPort::OneStop);			//停止位
            serial.setFlowControl(QSerialPort::NoFlowControl);	//流控制
        }
        serial.write(currentRequest);
        if(serial.waitForBytesWritten(waitTimeout))
        {
            if(serial.waitForReadyRead(currentWaitTimeout))
            {
                QByteArray responseData = serial.readAll();
                while(serial.waitForReadyRead(30))
                    responseData += serial.readAll();
                QString response=responseData.toHex();
                emit this->responseSeed(response);
            }
            else
            {
                emit this->timeoutSeed();
                return;
            }
        }
        else
        {
            emit this->timeoutSeed();
            return;
        }

        mutex.lock();
        cond.wait(&mutex);
        if(currentPortName != portName)
        {
            currentPortName = portName;
            currentPortNameChanged = true;
        }
        else
            currentPortNameChanged = false;

        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
    }
}
