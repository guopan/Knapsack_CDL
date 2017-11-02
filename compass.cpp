#include "global_defines.h"
#include "compass.h"
#include <QDebug>
#include <QByteArray>

compass::compass(QObject *parent) : QObject(parent)
{
    connect(&workthread, &compassThread::response, this, &compass::showResponse);
    connect(&workthread, &compassThread::error, this, &compass::processError);
    connect(&workthread, &compassThread::timeout, this, &compass::processError);
}

void compass::read()
{
    //要发送的命令
    QByteArray str = "6804000408";
    senddata=QByteArray::fromHex(str);
    workthread.transaction(CompassComPort,senddata);
}

double compass::toangle(const QString &c)
{
    double answer;
    QString sign = c.mid(0,1);                    //截取符号位，整数位和小数位
    QString round1 = c.mid(1,1);
    QString round2 = c.mid(2,2);
    QString decimal = c.mid(4,2);
    answer = 100*round1.toDouble()+round2.toDouble()+0.01*decimal.toDouble(); //数据整合为实际角度
    if(sign.toInt() == 1)
    {
        answer = -1*answer;               //符号位为1时取负
    }
    return answer;
}

void compass::showResponse(const QByteArray &s)
{
    double angle;
    QString temp = s.toHex();             //接收信号转16进制

    QString a,b,c;
    a = temp.mid(8,6);                     //截取接收信号 pitch roll head
    b = temp.mid(14,6);
    c = temp.mid(20,6);

    angle = toangle(c);
    //    qDebug()<<"result="<<result;
    emit this->compassAngle(angle);
}

void compass::processError(const QString &s)
{
    qDebug()<<"compass error:"<<s;
}
