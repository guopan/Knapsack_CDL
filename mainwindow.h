#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "compass.h"
#include "motor.h"
#include "motorthread.h"
#include "adq214.h"

#include <QDebug>
#include "heights_label.h"
#include "direction_needle.h"
#include "speed_bar.h"
#include "wind_display.h"
#include <QLabel>
#include <QTimer>
#include <QWidget>
#include "mainwindow.h"
#include <QToolBar>
#include <QAction>
#include <QMouseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTimer *timer;

    int H_low;
    int H_high;

    double H_speed[10];
    double H_direction[10];
    double V_speed[10];
    double Height_values[10];
signals:
    void data_changed();
private slots:

//    void on_readCompassButton_clicked();

    void on_startButton_clicked();

    void checkMove();

    void changeData();
    void quitActionTriggered();

    void toolBarControlTimerOutFcn();
private:
    Ui::MainWindow *ui;
    compass Compass;
    void showCompassAngle(const double &s);
    void checkMotorAngle(const double &s);
    void timeStart();
    void getPosition();
    void checkMotor();

    motor Motor;
    void readyToMove();
    bool moveNorth, checkReady;
    double headAngle, motorPX0;
    QTimer *timeOclock;
    QTimer *toolBarControlTimer;

//    motorthread Motorthread;
//    compassThread CompassThread;

    ADQ214 adq;
    int perTime;     //每次转动的角度，先假设为每60°采一组数

    QWidget *widget;

    wind_display *DisplaySpeed;
    QGridLayout *Glayout;

    QToolBar *mainToolBar;
    QAction *quitAction;
    bool isToolBarShowed;
    void showToolBar(bool isToolBarShowed);

    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
