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
    //Ҫ���͵�����
    QByteArray str = "6804000408";
    senddata=QByteArray::fromHex(str);
    workthread.transaction(CompassComPort,senddata);
}

double compass::toangle(const QString &c)
{
    double answer;
    QString sign = c.mid(0,1);                    //��ȡ����λ������λ��С��λ
    QString round1 = c.mid(1,1);
    QString round2 = c.mid(2,2);
    QString decimal = c.mid(4,2);
    answer = 100*round1.toDouble()+round2.toDouble()+0.01*decimal.toDouble(); //��������Ϊʵ�ʽǶ�
    if(sign.toInt() == 1)
    {
        answer = -1*answer;               //����λΪ1ʱȡ��
    }
    return answer;
}

void compass::showResponse(const QByteArray &s)
{
    double angle;
    QString temp = s.toHex();             //�����ź�ת16����

    QString a,b,c;
    a = temp.mid(8,6);                     //��ȡ�����ź� pitch roll head
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
