#ifndef DEVICESCONTROL_H
#define DEVICESCONTROL_H

#include <QObject>
#include <adq214.h>
#include <compass.h>
#include <motor.h>
#include <laserpulse.h>
#include <laserseed.h>
#include <global_defines.h>
#include <ADQAPI.h>
#include <QDebug>
#include <QTimer>
#include <QString>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QFileInfo>
#include <QDataStream>
#include <QDateTime>
#include <dispsettings.h>

class DevicesControl : public QObject
{
    Q_OBJECT
public:
    explicit DevicesControl(QObject *parent = nullptr);
    void startAction(SOFTWARESETTINGS settings);
    void stopAction();

signals:
    void vectorVelocityReady(double *vectorVelocity);

public slots:

private slots:
    void showCompassAngle(const double &s);
    void checkMotorAngle(const double &s);
    void getPosition();
    void checkMotor();
    void errorSolve();
    void laserErrorHint(const QString &s);
    void readyToMove();
    void openLaser();
    void closeLaser();
    void checkMove();
    void pulse_laser_opened_fcn();
    void On_ControlTimer_TimeOut();         //�ɼ����̿�������
    void timeStart();

    void Generate_freqAxis();               // ����Ƶ��������
    void LOSVelocityCal(const int heightNum, const int totalSpecPoints, const int objSpecPoints, const double lambda, const double *freqAxis, const double *specData);
    void Init_Buffers();

    void SaveSpec_FileHead();
    void SaveSpec_AddData();

private:
    compass Compass;
    motor Motor;
    laserSeed LaserSeed;
    laserPulse LaserPulse;
    ADQ214 adq;

    QTimer *timeOclock;
    QTimer *ControlTimer;       // ̽����̿��Ƽ�ʱ��

    bool moveNorth, checkReady;
    double headAngle, motorPX0;
    double currentMotorAngle;    //����ĵ�ǰλ�ã��Ƿ����motorPX0��
    bool stopped;				// �ɼ�δ����״̬
    bool stop_now;              // ���GUI��������ControlTimerֹͣ�ɼ�������ȶ
    int capture_counter;        // ̽�ⷽ�������
    QDateTime Start_Time;           // ��ʼʱ�䣬���ڶ�ʱ̽��ģʽ
    Control_State State;
    bool readyToCollect;        //----ָʾ����Ƿ��Ѿ�ֹͣ���ﵽָ��λ��
    bool isPulseLaserOpened;
    SOFTWARESETTINGS mysetting;

    double freqAxis [nFFT_half];    // Ƶ����
    double *losVelocity;            // �������ֵ
    double *aomSpec;
    double *specArray;

    QDateTime CaptureTime;          // ��ǰ����Ĳɼ�ʱ��
    QString SpecFileName;
    void Create_DataFolder();       // ���ݴ洢�ļ��еĴ���
};

#endif // DEVICESCONTROL_H
