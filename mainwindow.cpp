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

    //�����ļ���ȡ���Զ���ȡ�ϴ�ʹ�õ������ļ�
    m_setfile = new SettingFile();
    mysetting = m_setfile->readSettings();
    qDebug() << "Settings already readed";
    qDebug() << "Settings_PathName"<<mysetting.dataFilePath;
    checkDataFilePath();

    //��������ӹ���������Ϊ�û��������͹���Ա������
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
    //��������ʾ���ƣ�˫��Alt��ʾ����Ա������������Ҽ�����ʾ�û�������
    toolBarControlTimer = new QTimer(this);
    toolBarControlTimer->setSingleShot(true);
    toolBarControlTimer->setInterval(2000);
    connect(toolBarControlTimer, &QTimer::timeout, this, &MainWindow::toolBarControlTimerOutFcn);
    doubleAltKeyPressedClassifyTimer = new QTimer(this);
    doubleAltKeyPressedClassifyTimer->setSingleShot(true);
    qDebug() << "tool bar created";

    //��ʾ����
    stGraph_HSpeed = new STGraph(this);
    DisplaySpeed = new wind_display(this);
    UpdateHeightsValue();
    setCentralWidget(DisplaySpeed);

    QDockWidget *dockWidget_1;
    dockWidget_1 = new QDockWidget;
    dockWidget_1->setWidget( stGraph_HSpeed);
    dockWidget_1->setFeatures(QDockWidget::NoDockWidgetFeatures);

    dockWidget_1->setWindowTitle(QString::fromLocal8Bit("ˮƽ����ʱ�շֲ�"));
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget_1);
    stGraph_HSpeed->initialShow(mysetting.nRangeBin, minDetectRange, rangeResol);

//    connect(devicesControl, &DevicesControl::hVelocityReady, stGraph_HSpeed, &STGraph::updateShow);
    connect(this, &MainWindow::size_changed,DisplaySpeed, &wind_display::setSubSize);
    devicesControl = new DevicesControl();
    connect(devicesControl, &DevicesControl::hVelocityReady, this, &MainWindow::updateHVelocityDisp);
    connect(devicesControl, &DevicesControl::hAngleReady, this, &MainWindow::updateHAngleDisp);
    connect(devicesControl, &DevicesControl::vVelocityReady, this, &MainWindow::updateVVelocityDisp);

    // ���Ʋ���
    workThread = new QThread;
    devicesControl->moveToThread(workThread);
    connect(workThread, &QThread::finished, devicesControl, &QObject::deleteLater);
    connect(devicesControl, &DevicesControl::detectionFinished, userToolBar, &UserToolBar::set_to_stopped);
    workThread->start();
    isWorking = false;

    // ��ʾ������
    for (int i = 0; i < Max_nLayers; ++i)
    {
        H_speed[i] = 0;
        V_speed[i] = 0;
        H_direction[i] = 0;
    }
    // ���춨ʱ�������ó�ʱΪ 1 ��
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
        UpdateHeightsValue();       //ˢ����ʾ
        UpdateHeightsValue();       //ˢ����ʾ
        stGraph_HSpeed->initialShow(mysetting.nRangeBin, minDetectRange, rangeResol);
    }
    delete parameterSetDialog;
}

void MainWindow::quitActionTriggered()
{
    //������
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

    //��ֱ�� ��С̽�����
    double resol = lightSpeed/mysetting.sampleFreq/1000000/2;        //��������ľ���ֱ���
    minDetectRange = resol*(mysetting.nPointsMirrorWidth+mysetting.nPointsPerBin/2);
    minDetectRange = minDetectRange*qSin(qDegreesToRadians(mysetting.elevationAngle));
    //��ֱ�� ����ֱ���
    rangeResol = resol*(mysetting.nPointsPerBin*(1-mysetting.overlapRatio));
    rangeResol = rangeResol*qSin(qDegreesToRadians(mysetting.elevationAngle));

    //�ص������������0��0.5��
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
            str += time.toString("yyyyMMdd");			//������ʾ��ʽ
            qDebug()<<"Dir not Match";
        }
        qDebug()<<str<<endl;
    }
    else												//ѡ����������·��ʱ�������ǰ����·�����ڣ���ɾ����
    {
        if( dirname == time.toString("yyyyMMdd"))
        {
            if (!mypath.exists())
            {
                str = mypath.absolutePath();
                int str_len = str.length();
                str.resize(str_len - 9);				//��ȥ/20xxxxxx
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
