#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include"motorthread.h"

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
public slots:
private slots:
    void timeout();
    void portError();
     void receive_response(const QString &s);
private:
    QString portname,Order_str,AC,DC,PR,PA,SP;
    //       串口名   发送命令 加减速度 相对/绝对移动 零点原始位置  电机转动初始位置 接收数据
    motorthread thread_port;
//    bool defaultset,direction,handle_PX,check_PX,PR_move,first_move;

    //                     默认设置    转向      等待获取位置 等待检查位置  确定PR、PA转动  判断初次转动
};

#endif // MOTOR_H



