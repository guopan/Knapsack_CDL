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
    void action_set_triggered();			//����
    void action_start_Triggered();          //��������ͣ��ť
    void action_quit_triggered();           //�˳�

    void On_ControlTimer_TimeOut();         //�ɼ����̿�������
    void checkMove();

    void changeData();                      //�����ã���ʾ����ˢ��

    void toolBarControlTimerOutFcn();


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void Save_Spec2File();
    void on_pushButton_test_clicked();

private:
    Ui::MainWindow *ui;
    // GUI����
    UserToolBar *userToolBar;
    AdminToolBar *adminToolBar;
    bool isUserToolBarShowed;
    bool isAdminToolBarShowed;
    void showToolBar(bool isUserToolBarShowed, bool isAdminToolBarShowed);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QTimer *toolBarControlTimer;
    QTimer *doubleAltKeyPressedClassifyTimer;

    // �������������������ɼ�������������̣�
    compass Compass;
    void showCompassAngle(const double &s);
    void checkMotorAngle(const double &s);
    void timeStart();                   // ZM����
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

    // ������ʾ
    wind_display *DisplaySpeed;
    void resizeEvent(QResizeEvent * event);
    QTimer *TestTimer;          // ��������

    //���̿���
    QTimer *ControlTimer;       // ̽����̿��Ƽ�ʱ��
    bool stopped;				// �ɼ�δ����״̬
    bool stop_now;              // ���GUI��������ControlTimerֹͣ�ɼ�������ȶ
    int capture_counter;        // ̽�ⷽ�������
    QTime Start_Time;           // ��ʼʱ�䣬���ڶ�ʱ̽��ģʽ
    Control_State State;


    //��������
    paraDialog *ParaSetDlg;             //�������öԻ���
    ACQSETTING mysetting;
    settingfile m_setfile;

    void openLaser();
    void closeLaser();
    laserSeed LaserSeed;
    laserPulse LaserPulse;
    void Create_DataFolder();			//���ݴ洢�ļ��еĴ���
    bool readyToCollect;         //----ָʾ����Ƿ��Ѿ�ֹͣ���ﵽָ��λ��
};

#endif // MAINWINDOW_H


