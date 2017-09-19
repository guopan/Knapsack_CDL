#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "compass.h"
#include "motor.h"
#include "motorthread.h"
#include "adq214.h"
#include "global_defines.h"
#include "settingfile.h"
#include <paradialog.h>

#include <QDebug>
#include "display/wind_display.h"
#include <QLabel>
#include <QTimer>
#include <QWidget>
#include "mainwindow.h"
#include <QToolBar>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include <usertoolbar.h>
#include <admintoolbar.h>
#include <laserpulse.h>
#include <laserseed.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    int H_low;
    int H_high;

    double H_speed[10];
    double H_direction[10];
    double V_speed[10];
    double Height_values[10];

signals:
    void data_changed();
    void size_changed();

private slots:
    void on_startButton_clicked();
    void action_set_triggered();			//设置
    void action_start_Triggered();          //启动、暂停按钮
    void action_quit_triggered();           //退出

    void On_ControlTimer_TimeOut();         //采集过程控制主体
    void checkMove();

    void changeData();                      //测试用，显示数据刷新

    void toolBarControlTimerOutFcn();


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


    void Save_Spec2File();
    void on_pushButton_test_clicked();

private:
    Ui::MainWindow *ui;
    // GUI界面
    UserToolBar *userToolBar;
    AdminToolBar *adminToolBar;
    bool isUserToolBarShowed;
    bool isAdminToolBarShowed;
    void showToolBar(bool isUserToolBarShowed, bool isAdminToolBarShowed);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QTimer *toolBarControlTimer;
    QTimer *doubleAltKeyPressedClassifyTimer;

    // 外设器件（激光器、采集卡、电机、罗盘）
    compass Compass;
    void showCompassAngle(const double &s);
    void checkMotorAngle(const double &s);
    void timeStart();                   // ZM？？
    void getPosition();
    void checkMotor();
    void errorSolve();
    void laserErrorHint(const QString &s);

    motor Motor;
    void readyToMove();
    bool moveNorth, checkReady;
    double headAngle, motorPX0;
    QTimer *timeOclock;

    ADQ214 adq;

    // 数据显示
    wind_display *DisplaySpeed;
    void resizeEvent(QResizeEvent * event);
    QTimer *TestTimer;          // 仅测试用

    //过程控制
    QTimer *ControlTimer;       // 探测过程控制计时器
    bool stopped;				// 采集未启动状态
    bool stop_now;              // 命令：GUI用来控制ControlTimer停止采集，待商榷
    int capture_counter;        // 探测方向计数器
    QTime Start_Time;           // 开始时间，用于定时探测模式
    Control_State State;


    //参数配置
    paraDialog *ParaSetDlg;             //参数设置对话框
    ACQSETTING mysetting;
    settingfile m_setfile;

    void openLaser();
    void closeLaser();
    laserSeed LaserSeed;
    laserPulse LaserPulse;
    void Create_DataFolder();			//数据存储文件夹的创建
};

#endif // MAINWINDOW_H


