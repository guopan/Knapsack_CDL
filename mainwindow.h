#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "compass.h"
#include "motor.h"
#include "motorthread.h"
#include "adq214.h"
#include "global_defines.h"
#include "settingfile.h"
#include <parametersetdialog.h>

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
#include <devicescontrol.h>


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
    void size_changed();                    // ���ڵ���ʵʱ������ʾ��widget�߶�

private slots:
    void setActionTriggered();			//����
    void startActionTriggered();          //��������ͣ��ť
    void quitActionTriggered();           //�˳�
    // GUI�������
    void toolBarControlTimerOutFcn();
    void UpdateHeightsValue();

    void updateVectorVelocityDisp(double *vectorVelocity);

private:
    Ui::MainWindow *ui;

    bool isWorking;

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

    QThread *workThread;
    DevicesControl *devicesControl;


    // ������ʾ
    wind_display *DisplaySpeed;
    void resizeEvent(QResizeEvent * event);

    //��������
    SOFTWARESETTINGS mysetting;
    SettingFile *m_setfile;
};

#endif // MAINWINDOW_H
