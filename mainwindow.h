#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "global_defines.h"
#include "settingfile.h"
#include "parametersetdialog.h"

#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include "usertoolbar.h"
#include "admintoolbar.h"
#include "devicescontrol.h"
#include "display/wind_display.h"
#include "display/stgraph.h"


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

public slots:
    void changeData();
signals:
    void size_changed();                    // ���ڵ���ʵʱ������ʾ��widget�߶�

private slots:
    void setActionTriggered();			//����
    void startActionTriggered();          //��������ͣ��ť
    void quitActionTriggered();           //�˳�
    // GUI�������
    void toolBarControlTimerOutFcn();
    void UpdateHeightsValue();

    void updateHVelocityDisp(double *hVelocity);
    void updateHAngleDisp(double *hAngle);
    void updateVVelocityDisp(double *vVelocity);

    void checkDataFilePath();
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
    STGraph *stGraph_HSpeed;

    double minDetectRange;      // ��ֱ�� ��С̽�����
    double rangeResol;          // ��ֱ�� ����ֱ���

    //��������
    SOFTWARESETTINGS mysetting;
    SettingFile *m_setfile;
};

#endif // MAINWINDOW_H
