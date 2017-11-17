#include "global_defines.h"
#include "laserPulse.h"
#include <Qdebug.h>

laserPulse::laserPulse(QObject *parent) : QObject(parent)
{
    connect(&Laserpulsethread,SIGNAL(responsePulse(QString)),this,SLOT(receive_response(QString)));
    connect(&Laserpulsethread,SIGNAL(Pulse_PortNotOpen()),this,SLOT(portError()));
    connect(&Laserpulsethread,SIGNAL(timeoutPulse()),this,SLOT(timeout()));

    powerSet = true;
    fire = false;
    close = false;
}

void laserPulse::beginPulseLaser(const double &s)
{
    pulsePower = s;
    senddata = QByteArray::fromHex("AA 55 C1 01 01 01 00");
    Laserpulsethread.transaction(PulseLaserComPort,senddata);
    fire = true;
}

void laserPulse::setPulsePower(const int &s)
{
    QString key = QString("%1").arg(s,4,16,QLatin1Char('0')).toUpper();
    bool ok;
    int aa = key.left(2).toInt(&ok,16)+key.right(2).toInt(&ok,16);
    QString key2 =QString("%1").arg(aa,4,16,QLatin1Char('0')).toUpper();
    QString power="AA 55 C3 02 "+key.right(2)+" "+key.left(2)+" "+key2.right(2)+" "+key2.left(2);

    senddata = QByteArray::fromHex(power.toLatin1());
    Laserpulsethread.transaction(PulseLaserComPort,senddata);
    powerSet = false;
}

void laserPulse::closePulseLaser()
{
    senddata = QByteArray::fromHex("AA 55 C1 01 00 00 00");
    Laserpulsethread.transaction(PulseLaserComPort,senddata);
    close = true;
}

void laserPulse::receive_response(const QString &temp)
{
    if(!powerSet)
    {
        QString powerAnswer=temp.mid(8,2);
        if(powerAnswer != "00")
        {
            errorCode = QString::fromLocal8Bit("���弤�����������ô���");
            emit this->laserPulseError(errorCode);
        }
        else
        {
            powerSet = true;
            emit this->laserWorkRight();
            qDebug()<<QString::fromLocal8Bit("���弤�����������óɹ�");
        }
    }
    else
    {
        if(fire||close)
        {
            if(fire)
            {
                if(temp == "55aac101000000")
                {
                    setPulsePower((int)(pulsePower*100));       //������
                    qDebug()<<QString::fromLocal8Bit("���弤�����򿪳ɹ�");
                }
                else
                {
                    errorCode = QString::fromLocal8Bit("���弤�������쳣");
                    emit this->laserPulseError(errorCode);
                }
                fire = false;
            }
            else
            {
                if(temp == "55aac101000000")
                {
                    emit this->pulseCloseReady();
                    qDebug()<<"pulse close right;";
                }
                else
                {
                    errorCode = QString::fromLocal8Bit("���弤�����ر��쳣");
                    emit this->laserPulseError(errorCode);
                }
                close = false;
            }
        }
        else
        {
            QString checkAnswer = temp.mid(8,2);
            if(checkAnswer == "00")
            {
                errorCode = QString::fromLocal8Bit("���弤�������쳣");
                emit this->laserPulseError(errorCode);
            }
            else
            {
                QString checkAnswer=temp.mid(10,2);
                if(checkAnswer != "00")
                {
                    bool ok;
                    int s = checkAnswer.right(1).toInt(&ok,16);
                    QString key =QString("%1").arg(s,4,2,QLatin1Char('0'));
                    if(key.left(1)=="1")
                        errorCode.append(QString::fromLocal8Bit("�����¶��쳣;"));
                    if(key.right(1)=="1")
                        errorCode.append(QString::fromLocal8Bit("ģ���¶��쳣;"));
                    if(key.mid(1,1)=="1")
                        errorCode.append(QString::fromLocal8Bit("���빦���쳣;"));
                    if(key.mid(2,1)=="1")
                        errorCode.append(QString::fromLocal8Bit("�������弤�����¶��쳣;"));
                    emit this->laserPulseError(errorCode);
                }
                else
                {
                    qDebug()<<QString::fromLocal8Bit("���弤������������");
                }
            }
        }
    }
}

void laserPulse::checkLaser()
{
    senddata = QByteArray::fromHex("AA 55 D3 00 00 00");
    Laserpulsethread.transaction(PulseLaserComPort,senddata);
}

void laserPulse::portError()
{
    errorCode = QString::fromLocal8Bit("���弤�������ڴ��쳣");
    emit this->laserPulseError(errorCode);
}

void laserPulse::timeout()
{
    errorCode = QString::fromLocal8Bit("���弤�����������ݶ�ȡ�쳣");
    emit this->laserPulseError(errorCode);
}