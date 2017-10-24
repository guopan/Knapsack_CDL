#include "laserseed.h"
#include "global_defines.h"
#include <Qdebug.h>

laserSeed::laserSeed(QObject *parent) : QObject(parent)
{
    connect(&Laserseedthread,SIGNAL(responseSeed(QString)),this,SLOT(receive_response(QString)));
    connect(&Laserseedthread,SIGNAL(seed_PortNotOpen()),this,SLOT(portError()));
    connect(&Laserseedthread,SIGNAL(timeoutSeed()),this,SLOT(timeout()));

//    laserPort="COM1";
    powerSet=true;
    fire=false;
    close=false;
}

void laserSeed::beginSeedLaser(const double &SeedPower,const double &PulsePower)
{
    qDebug() << "begin Seed laser";
    seedPower=SeedPower;
    pulsePower=PulsePower;
    StringToHex("AA 55 C1 01 01 01 00",senddata);
    Laserseedthread.transaction(SeedLaserComPort,senddata);
    fire=true;
    openPulse=true;
}

void laserSeed::setSeedPower(const int &s)
{
    QString key =QString("%1").arg(s,4,16,QLatin1Char('0')).toUpper();
    bool ok;
    int aa=key.left(2).toInt(&ok,16)+key.right(2).toInt(&ok,16);
    QString key2 =QString("%1").arg(aa,4,16,QLatin1Char('0')).toUpper();
    QString power="AA 55 C3 02 "+key.right(2)+" "+key.left(2)+" "+key2.right(2)+" "+key2.left(2);
    StringToHex(power,senddata);
    Laserseedthread.transaction(SeedLaserComPort,senddata);
    powerSet=false;
}

void laserSeed::closeSeedLaser()
{
    close=true;
    StringToHex("AA 55 C1 01 00 00 00",senddata);
    Laserseedthread.transaction(SeedLaserComPort,senddata);

}

void laserSeed::receive_response(const QString &temp)
{
    if(!powerSet)
    {
        QString powerAnswer = temp.mid(8,2);
        if(powerAnswer!="00")
        {
            errorCode=QString::fromLocal8Bit("����Դ�������������ô���");
            emit this->laserSeedError(errorCode);
        }
        else
        {
            powerSet=true;
            if(openPulse)
            {
                emit this->seedOpenReady(pulsePower);
                openPulse = false;
            }
            qDebug()<<QString::fromLocal8Bit("����Դ�������������óɹ�");
        }
    }
    else
    {
        if(fire||close)
        {
            if(fire)
            {
                if(temp=="55aac101000000")
                {
//                    setSeedPower(1000); //������
                    setSeedPower((int)(seedPower*1000));
                    qDebug()<<QString::fromLocal8Bit("����Դ������������");
                }
                else
                {
                    errorCode="����Դ���������쳣";
                    emit this->laserSeedError(errorCode);
                }
                fire=false;
            }
            else
            {
                if(temp=="55aac101000000")
                {
                    emit this->laserColseRight();
                    qDebug()<<"seed close right";
                }
                else
                {
                    errorCode="����Դ�������ر��쳣";
                    emit this->laserSeedError(errorCode);
                }
                close=false;
            }

        }
        else
        {
            QString checkAnswer=temp.mid(8,2);
            if(checkAnswer=="00")
            {
                errorCode="����Դ���������쳣";
                emit this->laserSeedError(errorCode);
            }
            else
            {
                QString checkAnswer=temp.mid(10,2);
                if(checkAnswer!="00")
                {
                    bool ok;
                    int s=checkAnswer.right(1).toInt(&ok,16);
                    QString key =QString("%1").arg(s,4,2,QLatin1Char('0'));
                    if(key.left(1)=="1")
                    {errorCode.append("�����¶��쳣;");}
                    if(key.right(1)=="1")
                    {errorCode.append("ģ���¶��쳣;");}
                    if(key.mid(1,1)=="1")
                    {errorCode.append("���빦���쳣;");}
                    if(key.mid(2,1)=="1")
                    {errorCode.append("���Ӽ������¶��쳣;");}
                    emit this->laserSeedError(errorCode);
                }
                else
                {
                    qDebug()<<QString::fromLocal8Bit("����Դ��������������");
                }
            }
        }
    }
}

void laserSeed::checkLaser()
{
    StringToHex("AA 55 D3 00 00 00",senddata);
    Laserseedthread.transaction(SeedLaserComPort,senddata);
}

char laserSeed::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

void laserSeed::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

void laserSeed::portError()
{
    errorCode="����Դ���������ڴ��쳣";
    emit this->laserSeedError(errorCode);
}

void laserSeed::timeout()
{
    errorCode="����Դ�������������ݶ�ȡ�쳣";
    emit this->laserSeedError(errorCode);
}
