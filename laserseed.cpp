#include "laserseed.h"
#include <Qdebug.h>
laserSeed::laserSeed(QObject *parent) : QObject(parent)
{
    connect(&Laserseedthread,SIGNAL(responseSeed(QString)),this,SLOT(receive_response(QString)));
    connect(&Laserseedthread,SIGNAL(seed_PortNotOpen()),this,SLOT(portError()));
    connect(&Laserseedthread,SIGNAL(timeoutSeed()),this,SLOT(timeout()));

    laserPort="COM8";
    powerSet=true;
    fire=false;
    close=false;
}

void laserSeed::beginSeedLaser()
{
    qDebug() << "begin Seed laser";
    StringToHex("AA 55 C1 01 01 01 00",senddata);
    Laserseedthread.transaction(laserPort,senddata);
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
    Laserseedthread.transaction(laserPort,senddata);
    powerSet=false;
}

void laserSeed::closeSeedLaser()
{
    close=true;
    StringToHex("AA 55 C1 01 00 00 00",senddata);
    Laserseedthread.transaction(laserPort,senddata);

}

void laserSeed::receive_response(const QString &temp)
{
    if(!powerSet)
    {
        QString powerAnswer=temp.mid(8,2);
        if(powerAnswer!="00")
        {
            errorCode="种子源激光器功率设置错误";
            emit this->laserSeedError(errorCode);
        }
        else
        {
            powerSet=true;
            if(openPulse)
            {
                emit this->seedOpenReady();
                openPulse=false;
            }
            qDebug()<<QString::fromLocal8Bit("种子源激光器功率设置成功");
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
                    setSeedPower(1000); //打开正常
                    qDebug()<<QString::fromLocal8Bit("种子源激光器打开正常");
                }
                else
                {
                    errorCode="种子源激光器打开异常";
                    emit this->laserSeedError(errorCode);
                }
                fire=false;
            }
            else
            {
                if(temp=="55aac101000000")
                {
                    emit this->laserColseRight();
                    qDebug()<<"种子源激光器关闭正常";
                }
                else
                {
                    errorCode="种子源激光器关闭异常";
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
                errorCode="种子源激光器打开异常";
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
                    {errorCode.append("泵浦温度异常;");}
                    if(key.right(1)=="1")
                    {errorCode.append("模块温度异常;");}
                    if(key.mid(1,1)=="1")
                    {errorCode.append("输入功率异常;");}
                    if(key.mid(2,1)=="1")
                    {errorCode.append("种子激光器温度异常;");}
                    emit this->laserSeedError(errorCode);
                }
                else
                {
                    qDebug()<<QString::fromLocal8Bit("种子源激光器工作正常");
                }
            }
        }
    }
}

void laserSeed::checkLaser()
{
    StringToHex("AA 55 D3 00 00 00",senddata);
    Laserseedthread.transaction(laserPort,senddata);
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
    errorCode="种子源激光器串口打开异常";
    emit this->laserSeedError(errorCode);
}

void laserSeed::timeout()
{
    errorCode="种子源激光器串口数据读取异常";
    emit this->laserSeedError(errorCode);
}
