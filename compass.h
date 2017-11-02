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
    QByteArray senddata;                                   //需要发送的字节数组
    double toangle(const QString &c);                     //将接收的数据转为角度信息
    void showResponse(const QByteArray &s);
    void processError(const QString &s);
    compassThread workthread;

};

#endif // COMPASS_H
