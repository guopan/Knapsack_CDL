#ifndef LASERSEED_H
#define LASERSEED_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include<QTimer>
#include <QByteArray>
#include "laserseedthread.h"
#include "laserpulse.h"

class laserSeed : public QObject
{
    Q_OBJECT
public:
    explicit laserSeed(QObject *parent = nullptr);
    void beginSeedLaser(const double &SeedPower,const double &PulsePower);
    void setSeedPower(const int &s);
    void closeSeedLaser();
    void checkLaser();
private slots:
    void receive_response(const QString &temp);
    void portError();
    void timeout();


signals:
//    void powerReady();
    void laserSeedError(QString &s);
    void seedOpenReady(const double &SeedPower);
    void laserColseRight();
private:
    QByteArray senddata;
    QString laserPort,errorCode;
    char ConvertHexChar(char ch);
    void StringToHex(QString str, QByteArray &senddata);
    bool powerSet, fire, close, openPulse;
    laserseedthread Laserseedthread;
    double seedPower,pulsePower;
};

#endif // LASERSEED_H
