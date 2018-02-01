#ifndef COMPASS_H
#define COMPASS_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

#include <QByteArray>
#include "serialportthread.h"

class compass : public QObject
{
    Q_OBJECT

public:
    explicit compass(QObject *parent = nullptr);
    void read();


signals:
    void compassAngle(const double &s);
public slots:
private:
    QByteArray senddata;                                   //��Ҫ���͵��ֽ�����
    double toangle(const QString &c);                     //�����յ�����תΪ�Ƕ���Ϣ
    void showResponse(const QByteArray &s);
    void processError();
    serialportThread compassThread;
    int baud,waittimeout,waitForReadyReadTime;

};
#endif // COMPASS_H
