#ifndef COMPASS_H
#define COMPASS_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include<QTimer>

#include <QByteArray>
#include "compassthread.h"

class compass : public QObject
{
    Q_OBJECT

public:
    explicit compass(QObject *parent = nullptr);
    void read();
    //    void stop();

signals:
    void compassAngle(const double &s);
public slots:
private:
    void StringToHex(QString str, QByteArray &senddata);   //ת��Ϊ16����
    char ConvertHexChar(char ch);
    QByteArray senddata;                                   //��Ҫ���͵��ֽ�����
    double toangle(const QString &c);                     //�����յ�����תΪ�Ƕ���Ϣ
    void showResponse(const QByteArray &s);
    void processError(const QString &s);
    compassThread workthread;

};

#endif // COMPASS_H
