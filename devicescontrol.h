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
#include <eigen/Eigen>
#include <Eigen/Dense>
#include <dswf.h>
using namespace Eigen;

class DevicesControl : public QObject
{
    Q_OBJECT
public:
    explicit DevicesControl(QObject *parent = nullptr);
    void startAction(SOFTWARESETTINGS settings);
    void stopAction();
    void Create_DataFolder();       // ���ݴ洢�ļ��еĴ���

signals:
    void hVelocityReady(double *hVelocity);
    void hAngleReady(double *hAngle);
    void vVelocityReady(double *vVelocity);

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

    void CalHeightsValues();
    void SaveSpec_FileHead();
    void SaveSpec_AddData();
    void SaveVelo_AddData();
    void SaveVelo_FileHead();

private:
    compass Compass;
    motor Motor;
    laserSeed LaserSeed;
    laserPulse LaserPulse;
    ADQ214 adq;

    QTimer *motorCheckTimer;
    QTimer *ControlTimer;           // ̽����̿��Ƽ�ʱ��

    bool moveNorth, checkReady;
    double headAngle, motorPX0;
    double currentMotorAngle;       //����ĵ�ǰλ�ã��Ƿ����motorPX0��

    bool stop_now;                  // ���GUI��������ControlTimerֹͣ�ɼ�������ȶ
    uint capture_counter;           // ̽�ⷽ�������
    QDateTime Start_Time;           // ��ʼʱ�䣬���ڶ�ʱ̽��ģʽ
    Control_State State;
    bool readyToCollect;            //----ָʾ����Ƿ��Ѿ�ֹͣ���ﵽָ��λ��

    SOFTWARESETTINGS mysetting;

    int nRB_ovlp;                   // ������overlap֮��ľ��������������������;���
    double freqAxis [nFFT_half];    // Ƶ����
    double *losVelocity;            // �������ֵ
    double *aomSpec;
    double *specArray;

    VectorXd azimuthAngle;
    MatrixXd losVelocityMat;
    double *hVelocity;
    double *hAngle;
    double *vVelocity;
    double Height_values[Max_nLayers];

    QDateTime CaptureTime;          // ��ǰ����Ĳɼ�ʱ��
    QString SpecFileName;
    QString VeloFileName;

//    void quitControlTimer();
//    void quitLaser();
};

#endif // DEVICESCONTROL_H
