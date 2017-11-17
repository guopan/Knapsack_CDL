#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QWidget>
#include <QLabel>
#include <QFont>
#include <QString>
#include <QGridLayout>
#include <QDockWidget>
#include "display/rt_display.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //配置文件读取，自动读取上次使用的配置文件
    m_setfile = new SettingFile();
    mysetting = m_setfile->readSettings();
    qDebug() << "Settings already readed";
    qDebug() << "Settings_PathName"<<mysetting.dataFilePath;
    checkDataFilePath();

    //主界面添加工具栏，分为用户工具栏和管理员工具栏
    userToolBar = new UserToolBar();
    addToolBar(Qt::TopToolBarArea, userToolBar);
    adminToolBar = new AdminToolBar();
    addToolBar(Qt::TopToolBarArea, adminToolBar);
    isUserToolBarShowed = false;
    isAdminToolBarShowed = false;
    showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
    connect(userToolBar->quitAction, &QAction::triggered, this, &MainWindow::quitActionTriggered);
    connect(userToolBar->startAction, &QAction::triggered, this, &MainWindow::startActionTriggered);
    connect(adminToolBar->setAction,&QAction::triggered,this, &MainWindow::setActionTriggered);
    //工具栏显示控制，双击Alt显示管理员工具栏，点击右键，显示用户工具栏
    toolBarControlTimer = new QTimer(this);
    toolBarControlTimer->setSingleShot(true);
    toolBarControlTimer->setInterval(2000);
    connect(toolBarControlTimer, &QTimer::timeout, this, &MainWindow::toolBarControlTimerOutFcn);
    doubleAltKeyPressedClassifyTimer = new QTimer(this);
    doubleAltKeyPressedClassifyTimer->setSingleShot(true);
    qDebug() << "tool bar created";

    //显示部分
    stGraph_HSpeed = new STGraph(this);
    DisplaySpeed = new wind_display(this);
    UpdateHeightsValue();
    setCentralWidget(DisplaySpeed);

    QDockWidget *dockWidget_1;
    dockWidget_1 = new QDockWidget;
    dockWidget_1->setWidget( stGraph_HSpeed);
    dockWidget_1->setFeatures(QDockWidget::NoDockWidgetFeatures);

    dockWidget_1->setWindowTitle(QString::fromLocal8Bit("水平风速时空分布"));
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget_1);
    stGraph_HSpeed->initialShow(mysetting.nRangeBin, minDetectRange, rangeResol);

//    connect(devicesControl, &DevicesControl::hVelocityReady, stGraph_HSpeed, &STGraph::updateShow);
    connect(this, &MainWindow::size_changed,DisplaySpeed, &wind_display::setSubSize);
    devicesControl = new DevicesControl();
    connect(devicesControl, &DevicesControl::hVelocityReady, this, &MainWindow::updateHVelocityDisp);
    connect(devicesControl, &DevicesControl::hAngleReady, this, &MainWindow::updateHAngleDisp);
    connect(devicesControl, &DevicesControl::vVelocityReady, this, &MainWindow::updateVVelocityDisp);

    // 控制部分
    workThread = new QThread;
    devicesControl->moveToThread(workThread);
    connect(workThread, &QThread::finished, devicesControl, &QObject::deleteLater);
    workThread->start();
    isWorking = false;

    // 显示测试用
    for (int i = 0; i < Max_nLayers; ++i)
    {
        H_speed[i] = 0;
        V_speed[i] = 0;
        H_direction[i] = 0;
    }
    // 构造定时器，设置超时为 1 秒
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(changeData()));
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete DisplaySpeed;
    delete userToolBar;
    delete adminToolBar;
    delete m_setfile;
    toolBarControlTimer->stop();
    toolBarControlTimer->deleteLater();
    devicesControl->deleteLater();
}

void MainWindow::setActionTriggered()
{
    ParameterSetDialog *parameterSetDialog = new ParameterSetDialog(this);
    parameterSetDialog->setParaSettings(mysetting, isWorking);
    if (parameterSetDialog->exec() == QDialog::Accepted) {
        mysetting =  parameterSetDialog->getParaSettings();
        UpdateHeightsValue();       //刷新显示
        UpdateHeightsValue();       //刷新显示
        stGraph_HSpeed->initialShow(mysetting.nRangeBin, minDetectRange, rangeResol);
    }
    delete parameterSetDialog;
}

void MainWindow::quitActionTriggered()
{
    //待完善
    this->close();
}

void MainWindow::startActionTriggered()
{
    if (isWorking) {
        isWorking = false;
        devicesControl->stopAction();
        //        workThread->quit();
        //        workThread->wait();
    }
    else {
        isWorking = true;
        devicesControl->startAction(mysetting);
    }
}

void MainWindow::toolBarControlTimerOutFcn()
{
    isUserToolBarShowed = false;
    showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
}

void MainWindow::showToolBar(bool isUserToolBarShowed, bool isAdminToolBarShowed)
{
    userToolBar->setVisible(isUserToolBarShowed);
    adminToolBar->setVisible(isAdminToolBarShowed);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!isAdminToolBarShowed) {
        if (event->button() == Qt::RightButton){
            if (isUserToolBarShowed) {
                isUserToolBarShowed = false;
                showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
                toolBarControlTimer->stop();
            }
            else {
                isUserToolBarShowed = true;
                showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
                toolBarControlTimer->stop();
                toolBarControlTimer->start(3000);
            }
        }
        else if(event->button() == Qt::LeftButton) {
            isUserToolBarShowed = false;
            showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
            toolBarControlTimer->stop();
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt) {
        if (doubleAltKeyPressedClassifyTimer->isActive()) {
            if (isAdminToolBarShowed) {
                isAdminToolBarShowed = false;
                isUserToolBarShowed = false;
                showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
            }
            else {
                toolBarControlTimer->stop();
                isAdminToolBarShowed = true;
                isUserToolBarShowed = true;
                showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
            }
        }
        else {
            doubleAltKeyPressedClassifyTimer->start(400);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
    emit size_changed();
}

void MainWindow::UpdateHeightsValue()
{

    //垂直向 最小探测距离
    double resol = lightSpeed/mysetting.sampleFreq/1000000/2;        //单采样点的径向分辨率
    minDetectRange = resol*(mysetting.nPointsMirrorWidth+mysetting.nPointsPerBin/2);
    minDetectRange = minDetectRange*qSin(qDegreesToRadians(mysetting.elevationAngle));
    //垂直向 距离分辨率
    rangeResol = resol*(mysetting.nPointsPerBin*(1-mysetting.overlapRatio));
    rangeResol = rangeResol*qSin(qDegreesToRadians(mysetting.elevationAngle));

    //重叠后距离门数（0或0.5）
    int nRB_ovlp;
    if( mysetting.overlapRatio > 0)
        nRB_ovlp = mysetting.nRangeBin*2 - 1;
    else
        nRB_ovlp = mysetting.nRangeBin;

    for(int i=0;i<nRB_ovlp;i++)
    {
        Height_values[i] = minDetectRange + i*rangeResol;
        qDebug()<<Height_values[i];
    }
    DisplaySpeed->set_nLayers(nRB_ovlp);
    DisplaySpeed->setHeights(Height_values);
}

void MainWindow::updateHVelocityDisp(double *hVelocity)
{
    DisplaySpeed->setHSpeed(hVelocity);
    qDebug() << "HSpeed update show";
    stGraph_HSpeed->updateShow(hVelocity);
}

void MainWindow::updateHAngleDisp(double *hAngle)
{
    DisplaySpeed->setHDirection(hAngle);
    qDebug() << "HAngle update show";
}

void MainWindow::updateVVelocityDisp(double *vVelocity)
{
    DisplaySpeed->setVSpeed(vVelocity);
    qDebug() << "VSpeed update show";
}

void MainWindow::checkDataFilePath()
{
    QString str = mysetting.dataFilePath;

    QDir mypath(str);
    QString dirname = mypath.dirName();
    QDateTime time = QDateTime::currentDateTime();

    int num = dirname.toInt();
    int len = dirname.length();
    if(mysetting.autoCreateDateDir)
    {
        QString today_str = time.toString("yyyyMMdd");
        int today_int = today_str.toInt();
        if(len == 8 && (num != today_int) && qAbs(num - today_int)<10000)
        {
            str = mypath.absolutePath();
            int str_len = str.length();
            str.resize(str_len - 8);
            str += today_str;
        }
        else if( dirname != time.toString("yyyyMMdd"))
        {
            str = mypath.absolutePath();
            str += QString("/");
            str += time.toString("yyyyMMdd");			//设置显示格式
            qDebug()<<"Dir not Match";
        }
        qDebug()<<str<<endl;
    }
    else												//选择不生成日期路径时，如果当前日期路径存在，则删除。
    {
        if( dirname == time.toString("yyyyMMdd"))
        {
            if (!mypath.exists())
            {
                str = mypath.absolutePath();
                int str_len = str.length();
                str.resize(str_len - 9);				//减去/20xxxxxx
            }
            qDebug()<<"Dir Match"<<str<<endl;
        }
    }
    mysetting.dataFilePath = str;
}

void MainWindow::changeData()
{
    for (int i = 0; i < Max_nLayers; ++i)
    {
        H_speed[i] += (1.3 - H_speed[i]*0.01*i);
        if(H_speed[i]>15)
            H_speed[i] -= 14.3;

        H_direction[i] += 1.3 + H_direction[i]*0.2 + i/18;
        if(H_direction[i] >= 360)
            H_direction[i] -= 360;

        V_speed[i] += 0.7-0.2*i;
        if(V_speed[i]>6)
            V_speed[i] -= 12 + i/12;
        if(V_speed[i]<-6)
            V_speed[i] += 8 + i/7;
    }
    DisplaySpeed->setHSpeed(H_speed);
    DisplaySpeed->setVSpeed(V_speed);
    DisplaySpeed->setHDirection(H_direction);

    stGraph_HSpeed->updateShow(H_speed);
//    stGraph_VSpeed->updateShow(V_speed);
}
