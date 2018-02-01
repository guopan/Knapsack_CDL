#include "global_defines.h"
#include "motor.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

motor::motor(QObject *parent) : QObject(parent)
{
    connect(&motorThread,SIGNAL(response(QByteArray)),this,SLOT(receive_response(QByteArray)));
    connect(&motorThread,SIGNAL(PortNotOpen()),this,SLOT(portError()));
    connect(&motorThread,SIGNAL(timeout()),this,SLOT(timeout()));

    baud = 19200;
    waittimeout = 300;
    waitForReadyReadTime = 80;
}

void motor::prepare()
{
    portname = MotorComPort;
    Order_str = "VR;";

    motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);   //获取版本后依次上电--设置加减速度--设置速度--发送上电完成信号
}

void motor::moveAbsolute(const double &a)
{
    QString anglePA = QString::number(a*524288/360,'f',2);
    qDebug()<<"a="<<anglePA;
    Order_str = "PA="+anglePA+";";

    motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
}

void motor::moveRelative(const double &a)
{
    QString anglePR=QString::number(a*524288/360,'f',2);
    Order_str = "PR="+anglePR+";";

    motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
}

void motor::position()
{
    Order_str = "PX;";

    motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
}

void motor::checkMove()
{
    Order_str = "MS;";

    motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
}

void motor::portError()
{
    qDebug()<<"motor no open";
}

void motor::timeout()
{
    qDebug()<<"motor timeout";
}

void motor::motorQuit()
{
    Order_str = "MO=0;";
    motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
}

void motor::receive_response(const QByteArray &responseInfo)
{
    QString responseStr = QString(responseInfo);
    if(responseStr.left(2) == "MO")                       //电机是否关闭
    {
        if(responseStr.left(4) == "MO=1")
        {
            Order_str = "AC=1e7;";

            motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
        }
        else
        {
            emit this->motorClosed();
            qDebug()<<"motor close";
        }
    }
    if(responseStr.left(2) == "AC")
    {
        Order_str = "DC=1e7;";

        motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
    }
    if(responseStr.left(2) == "DC")
    {
        Order_str = "SP=524288;";

        motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
    }
    if(responseStr.left(2) == "MS")
    {
        if(responseStr.left(4)=="MS;0")
        {
            emit this->moveReady();
        }
        else
        {
            if(responseStr.left(4)=="MS;3")
            {
                emit this->motorError();
            }
        }
    }
    if(responseStr.left(2) == "PA")
    {
        Order_str = "BG;";

        motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
        emit this->beginMove();
    }
    if(responseStr.left(2) == "PR")
    {
        Order_str = "BG;";

        motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
        emit this->beginMove();
    }
    if(responseStr.left(2) == "PX")
    {


        QString ret = responseStr.split(";").at(1);	//PX值
        int retVal = ret.toInt();
        int temp = retVal & 0x7ffff;
        double angle = (double) temp/524288*360.0;

        while(angle<0)
            angle = angle + 360;

        while(angle>360)
            angle = angle - 360;
        emit this->motorAngle(angle);
    }
    if(responseStr.left(2) == "SP")
    {
        emit this->motorPrepareOk();
    }
    if(responseStr.left(10) == "VR;Whistle")
    {
        Order_str = ";";

        motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
    }
    if(responseStr.left(1) == ";")
    {
        qDebug() << "return ;;;;;;;;;;;;;;;;;;;;;;;;;;";
        Order_str = "MO=1;";

        motorThread.transaction(portname,Order_str.toLatin1(),baud,waittimeout,waitForReadyReadTime);
    }
}
