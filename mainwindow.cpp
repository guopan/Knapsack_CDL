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
#include "display/wind_display.h"
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
    DisplaySpeed = new wind_display(this);
    DisplaySpeed->set_nLayers(mysetting.nRangeBin);
    UpdateHeightsValue();
    ui->gridLayout->addWidget(DisplaySpeed);

    connect(this, &MainWindow::size_changed,DisplaySpeed, &wind_display::setSubSize);
//    qDebug() << "aaaaaaaaaaaaaaa";
    devicesControl = new DevicesControl();
    connect(devicesControl, &DevicesControl::vectorVelocityReady, this, &MainWindow::updateVectorVelocityDisp);
    workThread = new QThread;
    devicesControl->moveToThread(workThread);
    workThread->start();
    isWorking = false;
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
        DisplaySpeed->set_nLayers(mysetting.nRangeBin);
        UpdateHeightsValue();
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
    double range_resolution = 300/mysetting.sampleFreq/2*mysetting.nPointsPerBin;
    for(int i=0;i<mysetting.nRangeBin;i++) {
        Height_values[i] = (i+1.5)*range_resolution;
    }
    DisplaySpeed->setHeights(Height_values);
}

void MainWindow::updateVectorVelocityDisp(double *vectorVelocity)
{
    for (int i=0; i<mysetting.nRangeBin; i++) {
        qDebug() << vectorVelocity[i];
    }
}
