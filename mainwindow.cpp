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

    QDir dirs;
    QString path = dirs.currentPath()+"/"+"214settings.ini";			// 获取初始默认路径，并添加默认配置文件
    qDebug() << "initial path = " << path;
    m_setfile.test_create_file(path);									// 检查settings.ini是否存在，若不存在则创建
    m_setfile.readFrom_file(path);										// 读取settings.ini文件
    mysetting = m_setfile.get_settings();								// mysetting获取文件中的参数

    userToolBar = new UserToolBar();
    connect(userToolBar->quitAction, &QAction::triggered, this, &MainWindow::quitActionTriggered);
    connect(userToolBar->startAction, &QAction::triggered, this, &MainWindow::action_start_Triggered);

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
    //    mouseEventClassifyTimer = new QTimer(this);
    //    mouseEventClassifyTimer->setSingleShot(true);
    //    mouseEventClassifyTimer->setInterval(25);
    //    connect(mouseEventClassifyTimer, SIGNAL(timeout()), this, SLOT(mouseEventClassifyTimerOutFcn()));
    doubleAltKeyPressedClassifyTimer = new QTimer(this);
    doubleAltKeyPressedClassifyTimer->setSingleShot(true);
    timeOclock = new QTimer(this);

    connect(&Compass, &compass::compassAngle, this, &MainWindow::showCompassAngle);
    connect(&Motor, &motor::motorPrepareOk, this, &MainWindow::readyToMove);
    connect(&Motor, &motor::beginMove, this, &MainWindow::timeStart);
    connect(timeOclock,SIGNAL(timeout()),this, SLOT(checkMove()));
    connect(&Motor, &motor::moveReady,this, &MainWindow::getPosition);
    connect(&Motor, &motor::motorError, this, &MainWindow::errorSolve);
    connect(&Motor, &motor::motorAngle, this, &MainWindow::checkMotorAngle);
    connect(&adq, &ADQ214::collectFinish, this, &MainWindow::getPosition);

    Compass.read();             // 读取一次罗盘数据确定罗盘连接状况
    checkMotor();               // 检查电机连接
    mysetting.step_azAngle = 60;

    //    adq.connectADQDevice();     // 连接采集卡

    // 显示部分
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
    TestTimer = new QTimer(this);
    //    timer->start(1000);
    connect(TestTimer, SIGNAL(timeout()), this, SLOT(changeData()));
    stopped = true;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete DisplaySpeed;
    delete userToolBar;
    delete adminToolBar;
    toolBarControlTimer->stop();
    toolBarControlTimer->deleteLater();
}

void MainWindow::action_set_triggered()
{
    qDebug() << " open setting ";
    ParaSetDlg = new paraDialog(this);
    ParaSetDlg->init_setting(mysetting,stopped);					// mysetting传递给设置窗口psetting
    ParaSetDlg->initial_para();										// 参数显示在设置窗口上，并连接槽
    ParaSetDlg->on_checkBox_autoCreate_DateDir_clicked();			// 更新文件存储路径
    if (ParaSetDlg->exec() == QDialog::Accepted)					// 确定键功能
    {
        mysetting =	ParaSetDlg->get_settings();						// mysetting获取修改后的参数
    }
    delete ParaSetDlg;
}

void MainWindow::on_startButton_clicked()
{
    Compass.read();
    moveNorth = true;
    Motor.prepare();            // 电机上电并设置速度，加速度参数
}

void MainWindow::showCompassAngle(const double &s)
{
    headAngle = s;                // 记录罗盘数值
}

void MainWindow::checkMotorAngle(const double &s)
{
    qDebug()<<"s="<<s;
    if(moveNorth)
    {
        if(headAngle-s>=-0.5&&headAngle-s<=0.5)
        {
            qDebug()<<"success ";
            moveNorth = false;
            adq.Start_Capture();        //指北后开始采集卡工作
            checkReady = false;
        }
        else
        {
            qDebug()<<"moveNorth gap="<<headAngle-s;
            Motor.moveRelative(headAngle-s);
        }
    }
    else
    {
        if(!checkReady)           //先确定每次转动前的初始位置
        {
            motorPX0 = s;
            Motor.moveRelative(mysetting.step_azAngle);
            if(motorPX0>360-mysetting.step_azAngle)
                motorPX0 = motorPX0-360;
            checkReady = true;
        }
        else
        {
            if((s-motorPX0-mysetting.step_azAngle)<=0.5&&(s-motorPX0-mysetting.step_azAngle)>=-0.5)   //判断是否到达指定位置,误差暂设0.5°
            {
                adq.Start_Capture();
                checkReady = false;
            }
            else
            {
                Motor.moveRelative(motorPX0+mysetting.step_azAngle-s);
            }
        }
    }
}

void MainWindow::readyToMove()
{
    Motor.position();    //电机转动前先读取一次电机位置
}

void MainWindow::checkMove()
{
    Motor.checkMove();      //检查电机转动状态
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
        //        QMessageBox::critical(this, QString::fromStdString("提示"), QString::fromStdString("电机连接异常!"));
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

void MainWindow::errorSolve()
{
    timeOclock->stop();
    if(moveNorth)
    {
        Compass.read();
        Motor.prepare();
    }
    else
    {
        Motor.prepare();
    }
}

void MainWindow::changeData()
{
    for (int i = 0; i < nLayers; ++i)
    {
        // 水平风速
        H_speed[i] += (1.9 - H_speed[i]*0.02*i);
        if(H_speed[i]>20)
            H_speed[i] -= 19.8;

        // 水平风向
        H_direction[i] += 100.3 + H_direction[i]*0.02*i;
        if(H_direction[i]>=360)
            H_direction[i] -= 360;

        // 垂直风速
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
    timeOclock->stop();
    Motor.motorQuit();
    Sleep(100);
    this->close();
}

void MainWindow::action_start_Triggered()
{
    qDebug() << "Start Action Triggered!!!";
    if(stopped)                 //如果当前为停止状态，则开始采集
    {
        if (mysetting.step_azAngle != 0)
        {
            //****电机转到 mysetting.start_azAngle;
        }

        //****计算频率坐标轴

        switch (mysetting.detectMode) {
        case 0:                 //持续探测

            break;
        case 1:                 //单组探测
            //****计数器置零direction
            break;
        case 2:                 //定时探测
            //****启动定时器

            break;
        default:
            break;
        }

        //****新建文件
        //****写入文件头
        //****打开激光器本振
        //****打开激光放大器

        bool stop_now = false;
        int capture_counter = 0;
        do
        {
            //****采集
            adq.Start_Capture();
            //****相对转动电机（step_azAngle为0也可以调用函数，不转就可以了）
            //****转换
            //****径向风速计算
            //****矢量风速合成
            //****更新显示
            //****存储功率谱到文件
            //****存储风速到文件
            if (mysetting.step_azAngle != 0)
            {
                //****开启查询电机定时器
                //****判断电机转动到位
                //****关闭查询电机定时器
            }

            capture_counter++;
            //****判断是否应该结束，更新结束标志stop_now
            switch (mysetting.detectMode) {
            case 0:                 //持续探测
                //****判断是否应该关闭文件，建立新文件
//                if(capture_counter = mysetting.文件内的方向数量)
                {
                    //****
                }
                break;
            case 1:                 //单组探测
                //****判断探测方向数
                if(capture_counter = mysetting.angleNum)
                    stop_now = true;
                break;
            case 2:                 //定时探测
                //****判断定时器是否达到条件

                break;
            default:
                break;
            }
        }while(!stop_now);


        TestTimer->start(1000);
        stopped = false;
    }
    else
    {
        TestTimer->stop();
        stopped = true;
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
            doubleAltKeyPressedClassifyTimer->start(500);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
    emit size_changed();
}
