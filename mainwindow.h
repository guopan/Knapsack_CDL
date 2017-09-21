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

    double H_speed[Max_nLayers];
    double H_direction[Max_nLayers];
    double V_speed[Max_nLayers];
    double Height_values[Max_nLayers];

signals:
    void data_changed();
    void size_changed();                    // ���ڵ���ʵʱ������ʾ��widget�߶�

private slots:
    void action_set_triggered();			//����
    void action_start_Triggered();          //��������ͣ��ť
    void action_quit_triggered();           //�˳�

    void pulse_laser_opened_fcn();

    void On_ControlTimer_TimeOut();         //�ɼ����̿�������
    void checkMove();

    void changeData();                      //�����ã���ʾ����ˢ��

    // GUI�������
    void toolBarControlTimerOutFcn();
    void on_startButton_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_test_clicked();

    // �ļ��洢
    void SaveSpec_FileHead();
    void SaveSpec_AddData();

    // ���ݼ���
    void Generate_freqAxis();               // ����Ƶ��������
    void LOSVelocityCal(const int heightNum, const int totalSpecPoints, const int objSpecPoints, const double lambda, const double *freqAxis, const double *specData);
    void Init_Buffers();
    void UpdateHeightsValue();

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
    double currentMotorAngle;    //����ĵ�ǰλ�ã��Ƿ����motorPX0��
    QTimer *timeOclock;

    void openLaser();
    void closeLaser();
    laserSeed LaserSeed;
    laserPulse LaserPulse;

    ADQ214 adq;

    // ������ʾ
    wind_display *DisplaySpeed;
    void resizeEvent(QResizeEvent * event);
    QTimer *TestTimer;          // ��������

    //���ݼ���
    double freqAxis [nFFT_half];    // Ƶ����
    double *losVelocity;            // �������ֵ
    double *aomSpec;
    double *specArray;

    //���̿���
    QTimer *ControlTimer;       // ̽����̿��Ƽ�ʱ��
    bool stopped;				// �ɼ�δ����״̬
    bool stop_now;              // ���GUI��������ControlTimerֹͣ�ɼ�������ȶ
    int capture_counter;        // ̽�ⷽ�������
    QDateTime Start_Time;           // ��ʼʱ�䣬���ڶ�ʱ̽��ģʽ
    Control_State State;
    bool readyToCollect;        //----ָʾ����Ƿ��Ѿ�ֹͣ���ﵽָ��λ��

    //��������
    paraDialog *ParaSetDlg;         // �������öԻ���
    ACQSETTING mysetting;
    settingfile m_setfile;

    //���ݼ�¼
    QDateTime CaptureTime;          // ��ǰ����Ĳɼ�ʱ��
    QString SpecFileName;
    void Create_DataFolder();       // ���ݴ洢�ļ��еĴ���
<<<<<<< HEAD
    bool isPulseLaserOpened;
=======
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
};

#endif // MAINWINDOW_H
