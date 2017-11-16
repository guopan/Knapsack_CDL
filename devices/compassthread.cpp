#include "compassthread.h"
#include <QtSerialPort/QSerialPort>

#include <QTime>

#include <QDebug>

QT_USE_NAMESPACE

compassThread::compassThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
}

compassThread::~compassThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void compassThread::transaction(const QString &portName, const QByteArray &request)
{
    qDebug() << "Compass port name: " << portName;
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->request = request;
    //! [3]
    if (!isRunning())
    {
         start();
    }
    else
    {
       cond.wakeOne();
    }
}

void compassThread::run()
{
    bool currentPortNameChanged = false;
    mutex.lock();
    QString currentPortName;
    if (currentPortName != portName)
    {
        currentPortName = portName;
        currentPortNameChanged = true;
    }
    int currentWaitTimeout = 1000;
    waitTimeout=3000;
    QByteArray currentRequest = request;
    mutex.unlock();
    QSerialPort serial;
    while (!quit)
    {
        if (currentPortNameChanged)
        {
            serial.close();
            serial.setPortName(currentPortName);
            if (!serial.open(QIODevice::ReadWrite))
            {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
            serial.setBaudRate(QSerialPort::Baud9600);			//波特率
            serial.setDataBits(QSerialPort::Data8);				//数据位
            serial.setParity(QSerialPort::NoParity);			//校验位
            serial.setStopBits(QSerialPort::OneStop);			//停止位
            serial.setFlowControl(QSerialPort::NoFlowControl);	//流控制
        }
        QByteArray requestData = currentRequest;
        serial.write(requestData);

        if (serial.waitForBytesWritten(waitTimeout))                        //有字节写入时就开启，时间终止后跳出
        {
             if (serial.waitForReadyRead(currentWaitTimeout))                //有信号可读时就开启，时间终止后跳出
             {
                QByteArray responseData = serial.readAll();                  //先读取一次
                while (serial.waitForReadyRead(30))                          // 如果在20s内还有可读数据就继续读取
                {
                    responseData += serial.readAll();                        //将读取信号拼接为完整的接收信号
                }
                 emit this->response(responseData);
            }
             else
             {
                emit timeout(tr("Wait read response timeout %1")
                    .arg(QTime::currentTime().toString()));
             }

        }
        else
        {
            emit timeout(tr("Wait write request timeout %1")
                         .arg(QTime::currentTime().toString()));
        }

        mutex.lock();
        cond.wait(&mutex);
       if (currentPortName != portName)
       {
            currentPortName = portName;
            currentPortNameChanged = true;
       }
       else
       {
            currentPortNameChanged = false;
       }
        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
    }
}
