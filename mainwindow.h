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
    void size_changed();                    // 用于调整实时风速显示的widget高度

private slots:
    void setActionTriggered();			//设置
    void startActionTriggered();          //启动、暂停按钮
    void quitActionTriggered();           //退出
    // GUI界面相关
    void toolBarControlTimerOutFcn();
    void UpdateHeightsValue();

    void updateHVelocityDisp(double *hVelocity);
    void updateHAngleDisp(double *hAngle);
    void updateVVelocityDisp(double *vVelocity);

    void checkDataFilePath();
private:
    Ui::MainWindow *ui;

    bool isWorking;

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

    QThread *workThread;
    DevicesControl *devicesControl;

    // 数据显示
    wind_display *DisplaySpeed;
    void resizeEvent(QResizeEvent * event);
    STGraph *stGraph_HSpeed;

    double minDetectRange;      // 垂直向 最小探测距离
    double rangeResol;          // 垂直向 距离分辨率

    //参数配置
    SOFTWARESETTINGS mysetting;
    SettingFile *m_setfile;
};

#endif // MAINWINDOW_H
