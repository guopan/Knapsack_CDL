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

//SXXX.YYS Ϊ����λ��0 ����1 ����XXX Ϊ��λ����ֵ��YY Ϊ��λλС��ֵ���� 10 26 87 ��ʾ-26.87o
double compass::toangle(const QString &str_Angle)
{
    double answer;
    QString sign = str_Angle.mid(0,1);                    //��ȡ����λ������λ��С��λ
    QString integers = str_Angle.mid(1,3);
    QString decimals = str_Angle.mid(4,2);
    answer = integers.toDouble()+0.01*decimals.toDouble(); //��������Ϊʵ�ʽǶ�
    if(sign.toInt() == 1)
        answer = -1*answer;               //����λΪ1ʱȡ��
    return answer;
}

void compass::showResponse(const QByteArray &s)
{
    double headAngle;
    QString temp = s.toHex();           //�����ź�ת16����

    QString str_pitch, str_roll, str_head;
    //��ȡ�����ź� pitch roll head
    str_pitch = temp.mid(8,6);
    str_roll = temp.mid(14,6);
    str_head = temp.mid(20,6);

    headAngle = toangle(str_head);
    //    qDebug()<<"result="<<result;
    emit this->compassAngle(headAngle);
}

void compass::processError(const QString &s)
{
    qDebug()<<"compass error:"<<s;
}
