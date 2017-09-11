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
#include "wind_display.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDir dirs;
    QString path = dirs.currentPath()+"/"+"214settings.ini";				//??????????????????
    qDebug() << "initial path = " << path;
    m_setfile.test_create_file(path);									//??settings.ini????????????
    m_setfile.readFrom_file(path);										//??settings.ini??
    mysetting = m_setfile.get_setting();								//mysetting????????

    userToolBar = new UserToolBar();
    connect(userToolBar->quitAction, SIGNAL(triggered(bool)), this, SLOT(quitActionTriggered()));
    addToolBar(Qt::TopToolBarArea, userToolBar);
    adminToolBar = new AdminToolBar();
    addToolBar(Qt::TopToolBarArea, adminToolBar);
    isUserToolBarShowed = false;
    isAdminToolBarShowed = false;
    showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
    connect(adminToolBar,SIGNAL(actionTriggered(QAction*)),this, SLOT(action_set_triggered()));

    toolBarControlTimer = new QTimer(this);
    toolBarControlTimer->setSingleShot(true);
    toolBarControlTimer->setInterval(10000);
    connect(toolBarControlTimer, SIGNAL(timeout()), this, SLOT(toolBarControlTimerOutFcn()));
    mouseEventClassifyTimer = new QTimer(this);
    mouseEventClassifyTimer->setSingleShot(true);
    mouseEventClassifyTimer->setInterval(25);
    connect(mouseEventClassifyTimer, SIGNAL(timeout()), this, SLOT(mouseEventClassifyTimerOutFcn()));
    doubleAltKeyPressedClassifyTimer = new QTimer(this);
    doubleAltKeyPressedClassifyTimer->setSingleShot(true);
    timeOclock = new QTimer(this);

    connect(&Compass, &compass::compassAngle, this, &MainWindow::showCompassAngle);
    connect(&Motor, &motor::motorPrepareOk, this, &MainWindow::readyToMove);
    connect(&Motor, &motor::beginMove, this, &MainWindow::timeStart);
    connect(timeOclock,SIGNAL(timeout()),this, SLOT(checkMove()));
    connect(&Motor, &motor::moveReady,this, &MainWindow::getPosition);
    connect(&Motor, &motor::motorAngle, this, &MainWindow::checkMotorAngle);
    connect(&adq, &ADQ214::collectFinish, this, &MainWindow::getPosition);

    Compass.read();             // ��ȡһ����������ȷ����������״��
    checkMotor();               // ���������
    perTime = 60;
//    adq.connectADQDevice();     // ���Ӳɼ���

    //��ʾ����
    H_low = 100;
    H_high = 1000;
    for(int i=0;i<nLayers;i++)
    {
        H_speed[i] = 0;
        H_direction[i] = 0;
        V_speed[i] = 0;
        Height_values[i] = H_high - i*100;
    }

    DisplaySpeed = new wind_display(this);
    DisplaySpeed->setHeights(Height_values);

    ui->gridLayout->addWidget(DisplaySpeed);
//    connect(&Motor, &motor::moveReady,this, &MainWindow::getPosition);
//    connect(&Motor, &motor::motorAngle, this, &MainWindow::checkMotorAngle);
//    connect(&adq, &ADQ214::collectFinish, this, &MainWindow::getPosition);

    connect(this, &MainWindow::size_changed,DisplaySpeed, &wind_display::setSubSize);
    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(changeData()));
}

MainWindow::~MainWindow()
{

    delete ui;
    delete DisplaySpeed;
    delete userToolBar;
    delete adminToolBar;
    toolBarControlTimer->stop();
    toolBarControlTimer->deleteLater();
    mouseEventClassifyTimer->stop();
    mouseEventClassifyTimer->deleteLater();
}

void MainWindow::action_set_triggered()
{
    qDebug() << " open setting ";
    ParaSetDlg = new paraDialog(this);
    ParaSetDlg->init_setting(mysetting,stopped);					//mysetting???????psetting
    ParaSetDlg->initial_para();										//???????????????
    ParaSetDlg->on_checkBox_autocreate_datafile_clicked();			//????????
    if (ParaSetDlg->exec() == QDialog::Accepted)					// ?????
    {
        mysetting =	ParaSetDlg->get_settings();						//mysetting????????

    }
    delete ParaSetDlg;
}

//void MainWindow::on_readCompassButton_clicked()  //(������ť���ɲ�ִ��)
//{
//    Compass.read();           //��ȡ��������
//}

void MainWindow::on_startButton_clicked()
{
    Compass.read();
    moveNorth=true;
    Motor.prepare();            //����ϵ粢�����ٶȣ����ٶȲ���

}

void MainWindow::showCompassAngle(const double &s)
{
    //    ui->compassText->setText(QString::number(s,'f2',2));
    headAngle=s;           // ��¼������ֵ
}

void MainWindow::checkMotorAngle(const double &s)
{
    qDebug()<<"s="<<s;
    if(moveNorth)
    {
        if(headAngle-s>=-0.5&&headAngle-s<=0.5)
        {
            qDebug()<<"success ";
            moveNorth=false;
            adq.Start_Capture();        //ָ����ʼ�ɼ�������
            checkReady=false;
        }
        else
        {
            qDebug()<<"moveNorth gap="<<headAngle-s;
            Motor.moveRelative(headAngle-s);
        }
    }
    else
    {
        if(!checkReady)           //��ȷ��ÿ��ת��ǰ�ĳ�ʼλ��
        {
            motorPX0=s;
            Motor.moveRelative(perTime);
            if(motorPX0>360-perTime)
            {motorPX0=motorPX0-360;}
            checkReady=true;
        }
        else
        {
            if((s-motorPX0-perTime)<=0.5&&(s-motorPX0-perTime)>=-0.5)   //�ж��Ƿ񵽴�ָ��λ��,�������0.5��
            {
                adq.Start_Capture();
                checkReady=false;
            }
            else
            {
                Motor.moveRelative(motorPX0+perTime-s);
            }
        }
    }
}

void MainWindow::readyToMove()
{
    Motor.position();    //���ת��ǰ�ȶ�ȡһ�ε��λ��
}

void MainWindow::checkMove()
{
    Motor.checkMove();      //�����ת��״̬
}

void MainWindow::timeStart()
{
    timeOclock->start(1000);
}

void MainWindow::getPosition()
{
    timeOclock->stop();
    Motor.position();
}

void MainWindow::checkMotor()
{
    QSerialPort my_serial;
    my_serial.setPortName("COM8");
    if(!my_serial.open(QIODevice::ReadWrite))
    {
        qDebug()<<"motor not open";
        //        QMessageBox::critical(this, QString::fromStdString("��ʾ"), QString::fromStdString("��������쳣!"));
        // return;
    }

    my_serial.setBaudRate(QSerialPort::Baud19200);
    my_serial.setDataBits(QSerialPort::Data8);
    my_serial.setStopBits(QSerialPort::OneStop);
    my_serial.setFlowControl(QSerialPort::NoFlowControl);
    QString test("VR;");
    QByteArray testData = test.toLocal8Bit();
    my_serial.write(testData);
    if(my_serial.waitForBytesWritten(15))
    {
        if(my_serial.waitForReadyRead(30))
        {
            QByteArray testResponse = my_serial.readAll();
            while(my_serial.waitForReadyRead(10))
                testResponse += my_serial.readAll();
            QString response(testResponse);
            if(response.left(10) == "VR;Whistle")
            {
                qDebug()<<"motor ok";
            }
            else
            {qDebug()<<"motor error";}
        }
    }
}

void MainWindow::changeData()
{
    for (int i = 0; i < nLayers; ++i)
    {
        // ˮƽ����
        H_speed[i] += (1.9 - H_speed[i]*0.02*i);
        if(H_speed[i]>20)
            H_speed[i] -= 19.8;

        // ˮƽ����
        H_direction[i] += 100.3 + H_direction[i]*0.02*i;
        if(H_direction[i]>=360)
            H_direction[i] -= 360;

        // ��ֱ����
        V_speed[i] += (0.7+i*0.2+V_speed[i]*0.51);
        if(V_speed[i]>6)
            V_speed[i] -= 5.5;
        if(V_speed[i]<0)
            V_speed[i] += 5.5;
        V_speed[i] -= 3;

    }
        DisplaySpeed->setHSpeed(H_speed);
        DisplaySpeed->setHDirection(H_direction);
        DisplaySpeed->setVSpeed(V_speed);
}

void MainWindow::quitActionTriggered()
{
    this->close();
}

void MainWindow::startActionTriggered()
{
    qDebug() << "Start Action Triggered!!!";
    timer->start(1000);
}

void MainWindow::toolBarControlTimerOutFcn()
{
    qDebug() << "1111111111111";
    isUserToolBarShowed = false;
    showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
}

void MainWindow::mouseEventClassifyTimerOutFcn()
{
    if (isUserToolBarShowed) {
        isUserToolBarShowed = false;
        showToolBar(isUserToolBarShowed, isAdminToolBarShowed);
        toolBarControlTimer->stop();
    }
}

void MainWindow::showToolBar(bool isUserToolBarShowed, bool isAdminToolBarShowed)
{
    userToolBar->setVisible(isUserToolBarShowed);
    adminToolBar->setVisible(isAdminToolBarShowed);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!isAdminToolBarShowed) {
        mouseEventClassifyTimer->stop();
        if (event->button() == Qt::LeftButton) {
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
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!isAdminToolBarShowed) {
        if (event->button() == Qt::LeftButton)
            mouseEventClassifyTimer->start(200);
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
            doubleAltKeyPressedClassifyTimer->start(500);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
    emit size_changed();
}
