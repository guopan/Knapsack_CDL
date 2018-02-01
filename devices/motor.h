#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include "serialportthread.h"

class motor : public QObject
{
    Q_OBJECT
public:
    explicit motor(QObject *parent = nullptr);
    void prepare();
    void moveAbsolute(const double &a);
    void moveRelative(const double &a);
    void position();
    void checkMove();
    void motorQuit();
signals:
    void motorAngle(const double &s);
    void motorPrepareOk();
    void beginMove();
    void moveReady();
    void motorError();
    void motorClosed();
public slots:
private slots:
    void timeout();
    void portError();
    void receive_response(const QByteArray &responseInfo);
private:
    QString portname, Order_str, AC, DC, PR, PA, SP;
    //    串口名   发送命令 加减速度 相对/绝对移动 转速
    serialportThread motorThread;
    int baud, waittimeout, waitForReadyReadTime;
};

#endif // MOTOR_H



