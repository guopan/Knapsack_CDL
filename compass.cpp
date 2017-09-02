#include "compass.h"
#include <QDebug>

compass::compass(QObject *parent) : QObject(parent)
{
    connect(&workthread, &compassThread::response, this, &compass::showResponse);
    connect(&workthread, &compassThread::error, this, &compass::processError);
    connect(&workthread, &compassThread::timeout, this, &compass::processError);
}

void compass::read()
{
    //要发送的命令
    QString str="6804000408";
    StringToHex(str,senddata);
    workthread.transaction("COM3",senddata);

}

void compass::StringToHex(QString str, QByteArray &senddata)
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

char compass::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

double compass::toangle(const QString &c)
{
    double answer;
    QString sign=c.mid(0,1);                    //截取符号位，整数位和小数位
    QString round1=c.mid(1,1);
    QString round2=c.mid(2,2);
    QString decimal=c.mid(4,2);
    answer=100*round1.toDouble()+round2.toDouble()+0.01*decimal.toDouble(); //数据整合为实际角度
    if(sign.toInt()==1)
    {
        answer=-1*answer;               //符号位为1时取负
    }
    return answer;
}



void compass::showResponse(const QByteArray &s)
{
    double angle;
    QString temp= s.toHex();             //接收信号转16进制

    QString a,b,c;
    a=temp.mid(8,6);                     //截取接收信号 pitch roll head
    b=temp.mid(14,6);
    c=temp.mid(20,6);

    angle=toangle(c);
//    qDebug()<<"result="<<result;
    emit this->compassAngle(angle);
}

void compass::processError(const QString &s)
{
    qDebug()<<"compass error:"<<s;
}


