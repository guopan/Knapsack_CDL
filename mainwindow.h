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

    void checkMove();

    void changeData();
    void quitActionTriggered();
    void action_start_Triggered();

    void toolBarControlTimerOutFcn();

    void action_set_triggered();					//设置

private:
    Ui::MainWindow *ui;
    compass Compass;
    void showCompassAngle(const double &s);
    void checkMotorAngle(const double &s);
    void timeStart();
    void getPosition();
    void checkMotor();
    void errorSolve();

    motor Motor;
    void readyToMove();
    bool moveNorth, checkReady;
    double headAngle, motorPX0;
    QTimer *timeOclock;

    ADQ214 adq;

    wind_display *DisplaySpeed;

    QTimer *toolBarControlTimer;
    QTimer *doubleAltKeyPressedClassifyTimer;
    QTimer *TestTimer;
    QTimer *IntervalTimer;      // 定时探测模式下的间隔计时器
    QTimer *GroupTimer;         // 定时探测模式下的探测计时器
    UserToolBar *userToolBar;
    AdminToolBar *adminToolBar;
    bool isUserToolBarShowed;
    bool isAdminToolBarShowed;
    void showToolBar(bool isUserToolBarShowed, bool isAdminToolBarShowed);

    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void resizeEvent(QResizeEvent * event);

    paraDialog *ParaSetDlg;
    ACQSETTING mysetting;
    settingfile m_setfile;
    bool stopped;									//停止采集
};

#endif // MAINWINDOW_H
