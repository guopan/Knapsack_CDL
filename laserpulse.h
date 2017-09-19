#ifndef laserPulse_H
#define laserPulse_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include<QTimer>
#include <QByteArray>
#include "laserpulsethread.h"
#include "laserseed.h"

class laserPulse : public QObject
{
    Q_OBJECT
public:
    explicit laserPulse(QObject *parent = nullptr);
    void beginPulseLaser();
    void setPulsePower(const int &s);
    void closePulseLaser();
private slots:
    void checkLaser();
    void receive_response(const QString &temp);
    void portError();
    void timeout();


signals:
//    void powerReady();
    void laserPulseError(QString &s);
    void pulseCloseReady();
    void laserWorkRight();
private:    
    QByteArray senddata;
    QString laserPort,errorCode;
    char ConvertHexChar(char ch);
    void StringToHex(QString str, QByteArray &senddata);
    bool powerSet,fire,close;
    laserpulsethread Laserpulsethread;
};

#endif // laserPulse_H
