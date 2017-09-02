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
//    QSerialPort serial;
//    QString portName;
//    QTimer *time_clock;                                     //定时器
    void StringToHex(QString str, QByteArray &senddata);   //转换为16进制
    char ConvertHexChar(char ch);
    QByteArray senddata;                                   //需要发送的字节数组
    double toangle(const QString &c);                     //将接收的数据转为角度信息
//    QSerialPort serial;
//    workthread athread;
   /* bool isRunning; */                                     //指示定时器工作状态
//    void timeOut();
    void showResponse(const QByteArray &s);
     void processError(const QString &s);
     compassThread workthread;

};

#endif // COMPASS_H
