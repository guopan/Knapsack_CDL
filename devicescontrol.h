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
    void Create_DataFolder();       // 数据存储文件夹的创建

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
    void On_ControlTimer_TimeOut();         //采集过程控制主体
    void timeStart();

    void Generate_freqAxis();               // 生成频率坐标轴
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
    QTimer *ControlTimer;           // 探测过程控制计时器

    bool moveNorth, checkReady;
    double headAngle, motorPX0;
    double currentMotorAngle;       //电机的当前位置，是否就是motorPX0？
    bool stopped;                   // 采集未启动状态
    bool stop_now;                  // 命令：GUI用来控制ControlTimer停止采集，待商榷
    uint capture_counter;           // 探测方向计数器
    QDateTime Start_Time;           // 开始时间，用于定时探测模式
    Control_State State;
    bool readyToCollect;            //----指示电机是否已经停止并达到指定位置
    bool isPulseLaserOpened;
    SOFTWARESETTINGS mysetting;

    int nRB_ovlp;                   // 修正了overlap之后的距离门数，不包括噪声和镜面
    double freqAxis [nFFT_half];    // 频率轴
    double *losVelocity;            // 径向风速值
    double *aomSpec;
    double *specArray;

    VectorXd azimuthAngle;
    MatrixXd losVelocityMat;
    double *hVelocity;
    double *hAngle;
    double *vVelocity;
    double Height_values[Max_nLayers];

    QDateTime CaptureTime;          // 当前方向的采集时间
    QString SpecFileName;
    QString VeloFileName;

    void quitControlTimer();
    void quitLaser();
};

#endif // DEVICESCONTROL_H
