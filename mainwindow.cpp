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
    connect(userToolBar->quitAction, &QAction::triggered, this, &MainWindow::action_quit_triggered);
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

    ControlTimer = new QTimer(this);
    connect(ControlTimer,&QTimer::timeout, this,&MainWindow::On_ControlTimer_TimeOut);

    Compass.read();             // 读取一次罗盘数据确定罗盘连接状况
    checkMotor();               // 检查电机连接
    //    mysetting.step_azAngle = 0;

    //    adq.connectADQDevice();     // 连接采集卡

    connect(&LaserSeed,&laserSeed::seedOpenReady, &LaserPulse,&laserPulse::beginPulseLaser);
    //    connect(&LaserSeed,&laserSeed::seedOpenReady, &MainWindow::beginLaser2);
    connect(&LaserPulse,&laserPulse::pulseCloseReady, &LaserSeed,&laserSeed::closeSeedLaser);
    connect(&LaserSeed,&laserSeed::laserSeedError, this,&MainWindow::laserErrorHint);
    connect(&LaserPulse,&laserPulse::laserPulseError, this,&MainWindow::laserErrorHint);
    connect(&LaserPulse, SIGNAL(laserWorkRight()), this, SLOT(pulse_laser_opened_fcn()));

    //显示部分

    DisplaySpeed = new wind_display(this);
    DisplaySpeed->set_nLayers(mysetting.nRangeBin);
    UpdateHeightsValue();

    ui->gridLayout->addWidget(DisplaySpeed);

    connect(this, &MainWindow::size_changed,DisplaySpeed, &wind_display::setSubSize);
    TestTimer = new QTimer(this);
    //    timer->start(1000);
    connect(TestTimer, SIGNAL(timeout()), this, SLOT(changeData()));

    // 状态变量初始化
    stopped = true;
    readyToCollect=false;
    isPulseLaserOpened = false;

    // 缓冲区指针置空
    losVelocity = nullptr;
    aomSpec = nullptr;
    specArray = nullptr;
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
        DisplaySpeed->set_nLayers(mysetting.nRangeBin);
        UpdateHeightsValue();
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
            //            adq.Start_Capture();        //指北后开始采集卡工作
            readyToCollect=true;
            LaserPulse.checkLaser();
            LaserSeed.checkLaser();
            checkReady = false;
            motorPX0 = s;
            if(motorPX0>360-mysetting.step_azAngle)
                motorPX0 = motorPX0-360;
        }
        else
        {
            qDebug()<<"moveNorth gap="<<headAngle-s;
            Motor.moveRelative(headAngle-s);
        }
    }
    else
    {
        if((s-motorPX0-mysetting.step_azAngle)<=0.5&&(s-motorPX0-mysetting.step_azAngle)>=-0.5)   //判断是否到达指定位置,误差暂设0.5°
        {
            //adq.Start_Capture();
            readyToCollect=true;
            LaserPulse.checkLaser();
            LaserSeed.checkLaser();
            checkReady = false;
            motorPX0 = s;
            if(motorPX0>360-mysetting.step_azAngle)
                motorPX0 = motorPX0-360;
        }
        else
        {
            Motor.moveRelative(motorPX0+mysetting.step_azAngle-s);
        }

    }
}

void MainWindow::readyToMove()
{
    Motor.position();
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
    if(timeOclock->isActive())
    {
        timeOclock->stop();
    }
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

void MainWindow::laserErrorHint(const QString &s)
{
    qDebug()<<s;
    //    LaserPulse.closePulseLaser();
}

void MainWindow::changeData()
{
    for (int i = 0; i < mysetting.nRangeBin ; ++i)
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

void MainWindow::action_quit_triggered()
{
    timeOclock->stop();
    Motor.motorQuit();
    Sleep(100);
    this->close();
}

void MainWindow::pulse_laser_opened_fcn()
{
    qDebug() << "pulse laser open success!!";
    isPulseLaserOpened = true;
    capture_counter = 0;        // 探测方向计数器置零

    stop_now = false;
    if( mysetting.detectMode == 2)//定时探测
    {
        Start_Time = QDateTime::currentDateTimeUtc();   //记录当前时间作为开始时间

    }
    State = waitMotor;
    ControlTimer->start(CheckPeriod);       // 定时查询电机（激光器？）状态、采集、处理、显示

    //        TestTimer->start(1000);
    stopped = false;
}

//启动：进行采集的准备工作，并开启定时器
//停止：设定停止标志信号
void MainWindow::action_start_Triggered()
{
    qDebug() << "Start Action Triggered!!!";

    if(stopped)                 //如果当前为停止状态，则可以开始采集
    {
        //****判断电机是否在转动，之前的探测是否已经停止，没停止就等，之前的探测没结束（如何能知道？）就等。


        if (mysetting.step_azAngle != 0)
        {

            moveNorth=true;
            Motor.position();        //-----电机转到 mysetting.start_azAngle;

        }
        Generate_freqAxis();        //计算频率坐标轴
        adq.Transfer_Settings(mysetting);   //传递配置参数到adq
        Init_Buffers();             //申请各个缓存空间
        SaveSpec_FileHead();        // 新建文件、写入文件头
        LaserSeed.beginSeedLaser(); //------打开激光器本振，打开激光放大器

    }
    else
    {
        stop_now = true;
        //        TestTimer->stop();

    }
}

//
void MainWindow::On_ControlTimer_TimeOut()
{
    QDateTime currentTime;
    qint64 dt;
    if (stop_now)
    {
        State = Quit;
        stop_now = false;
    }

    //    qDebug() << State;
    switch (State) {
    case waitMotor:    //****查询电机状态，没到位，则直接返回，等下次进入定时器
        if (mysetting.step_azAngle != 0)
        {
            if(readyToCollect)
            {
                State = Capture;
            }
            //---------判断电机转动到位
        }
        else
            State = Capture;
        break;

    case Capture:
        qDebug() << "capture start!!!";

        adq.Start_Capture();                                //采集
        qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaa!!!!!!!!!!!!!!";
        CaptureTime = QDateTime::currentDateTimeUtc();      //记录当前时间，将来写入文件
        if(mysetting.step_azAngle != 0)
            Motor.moveRelative(mysetting.step_azAngle);         //-------相对转动电机（step_azAngle为0也可以调用函数，不转就可以了）
        readyToCollect = false;
        LOSVelocityCal(mysetting.nRangeBin+2, nFFT_half,
                       20, mysetting.laserWaveLength,
                       freqAxis, adq.get_PSD_double());     //径向风速计算
        //****矢量风速合成
        //****更新显示
        for(int i = 0;i<mysetting.nRangeBin;i++)
            H_speed[i] = qAbs(losVelocity[i]);

        DisplaySpeed->setHSpeed(H_speed);
        for(int i = 0;i<mysetting.nRangeBin;i++)
            H_direction[i] = losVelocity[i]>0?0:(180);
        DisplaySpeed->setHDirection(H_direction);

        SaveSpec_AddData();        //存储功率谱到文件

        //****存储风速到文件

        capture_counter++;
        //****判断是否应该结束，更新结束标志stop_now
        switch (mysetting.detectMode) {
        case 0:                 //持续探测
            //判断是否应该关闭文件，建立新文件
            if(capture_counter == mysetting.nMaxDir_inFile)
            {
                SaveSpec_FileHead();        //建立新文件，写入文件头
            }
            State = waitMotor;
            break;
        case 1:                 //单组探测
            //判断探测方向数
            if(capture_counter == mysetting.angleNum)
                State = Quit;
            else
                State = waitMotor;
            break;
        case 2:                 //定时探测
            // 判断否达到待机条件
            dt = currentTime.msecsTo(Start_Time);
            if(dt > qint64(mysetting.GroupTime*60*1000))  // *60s? *1000ms?// 如果达到待机时间
            {
                State = Standby;
            }
            break;
        default:
            break;
        }
        break;

    case Quit:
        ControlTimer->stop();
        LaserPulse.closePulseLaser();  //-------关闭激光放大器,关闭激光器本振
        stopped = true;
        userToolBar->set_to_stopped();
        break;

    case Standby:       //也许之前需要一个停止状态
        currentTime = QDateTime::currentDateTimeUtc();
        dt = currentTime.msecsTo(Start_Time);
        if(dt > qint64(mysetting.IntervalTime*60*1000))  // *60s? *1000ms?      //如果达到启动时间
        {
            Start_Time = Start_Time.addMSecs(mysetting.IntervalTime*60*1000);   //更新开始时间，为了下次计时
            if (mysetting.step_azAngle != 0)
            {
                moveNorth = true;
                Motor.position();
                //---------电机转到 mysetting.start_azAngle;
                State = waitMotor;
            }
            else
                State = Capture;
        }
        break;

    default:
        break;
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

void MainWindow::openLaser()
{
    LaserSeed.beginSeedLaser();
}

void MainWindow::closeLaser()
{
    LaserPulse.closePulseLaser();
}

void MainWindow::on_pushButton_2_clicked()
{
    LaserSeed.beginSeedLaser();
}

void MainWindow::on_pushButton_clicked()
{
    LaserPulse.closePulseLaser();
}

//数据存储文件夹的创建
void MainWindow::Create_DataFolder()
{
    QDir mypath;
    if(!mypath.exists(mysetting.DatafilePath))		//如果文件夹不存在，创建文件夹
        mypath.mkpath(mysetting.DatafilePath);
}



// 径向风速计算程序
void MainWindow::LOSVelocityCal(const int heightNum, const int totalSpecPoints,
                                const int objSpecPoints, const double lambda,
                                const double *freqAxis, const double *specData)
{
    qDebug() << "heightNum" << heightNum;
    qDebug() << "totalSpecPoints" << totalSpecPoints;
    qDebug() << "objSpecPoints" << objSpecPoints;
    qDebug() << "lambda" << lambda;

    for (int i=0; i<totalSpecPoints; i++) {
        qDebug() << freqAxis[i];
    }

    for (int k = 0; k < totalSpecPoints; k++) {
        aomSpec[k] = specData[totalSpecPoints+k] - specData[k];
        for (int l = 0; l < heightNum - 2; l++){
            specArray[l*totalSpecPoints+k] = specData[totalSpecPoints*(l+2) + k] - specData[k];
        }
    }

    //    for (int i=0; i<totalSpecPoints; i++) {
    //        qDebug() << aomSpec[i];
    //    }
    int aomIndex = 0;
    double temp = aomSpec[0];
    for (int k = 1; k < totalSpecPoints; k++) {
        if (aomSpec[k] > temp) {
            temp = aomSpec[k];
            aomIndex = k;
        }
    }

    qDebug() << aomIndex;
    int startIndex = aomIndex - objSpecPoints;
    int endIndex = aomIndex + objSpecPoints;

    int *losVelocityIndex = new int[heightNum - 2];
    temp = 0;
    for (int l = 0; l < heightNum -2; l++) {
        losVelocityIndex[l] = startIndex;
        temp = specArray[l*totalSpecPoints+ startIndex];
        for (int k = startIndex + 1; k <= endIndex; k++) {
            if (specArray[l*totalSpecPoints+ k] >temp) {
                temp = specArray[l*totalSpecPoints+ k];
                losVelocityIndex[l] = k;
            }
        }
    }

    //    for (int l = 0; l < heightNum -2; l++) {
    //        qDebug() << losVelocityIndex[l];
    //    }

    memset(losVelocity, 0, sizeof(double)*(heightNum-2));
    for(int i=0; i<heightNum-2; i++) {
        losVelocity[i] = (freqAxis[losVelocityIndex[i]] - freqAxis[aomIndex])*lambda/2;
        qDebug() << "losVelocity" << losVelocity[i];
    }
}

void MainWindow::SaveSpec_FileHead()
{
    Create_DataFolder();
    SpecFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    SpecFileName = mysetting.DatafilePath+"/"+"KnapsackCDL("+").spec";
    //    SpecFileName = mysetting.DatafilePath+"/"+"KnapsackCDL("+ SpecFileName +").spec";
    qDebug()<<"SpecFileName = "<<SpecFileName;
    QFile outputSpec(SpecFileName);

    if(outputSpec.open(QFile::WriteOnly | QIODevice::Truncate|QIODevice::Text))//QIODevice::Truncate表示将原文件内容清空
    {
        QTextStream specFile(&outputSpec);
        specFile << "Knapsack Coherent Doppler Lidar Original Spectrum" << endl;
        for(int i=0;i<60;i++)
            specFile << "=";                                // =分隔符
        specFile << endl<< "Data File Version: 1.0" << endl;          // 文件类型版本
        specFile << "Global Definitions:" << endl;

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        specFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //激光参数
        specFile << "LaserMode:        ";
        if(mysetting.isPulseMode)
        {
            specFile << "Pulse" << endl;		//脉冲探测（true）or连续探测（false） bool
            specFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;	//激光能量，单位μJ，连续模式下为0
        }
        else
        {
            specFile << "Continuous" << endl;	//脉冲探测（true）or连续探测（false） bool
            specFile << "laserPower:       " << QString::number(mysetting.laserPower) << endl;		//激光功率，单位mW，脉冲模式下为0
        }
        specFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;			//激光频率
        specFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;	//脉冲宽度
        specFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;	//激光波长
        specFile << "AOM_Freq:         " << QString::number(mysetting.AOM_Freq) << endl;			//AOM移频量

        //扫描参数
        specFile << "detectMode:       " ;      //探测方式：0持续探测1单组探测2定时探测
        switch (mysetting.detectMode) {
        case 0:                 //持续探测
            specFile << "NonStop" << endl;
            break;
        case 1:                 //单组探测
            specFile << "SingleGroup" << endl;
            break;
        case 2:                 //定时探测
            specFile << "scheduled" << endl;
            break;
        }

        specFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//俯仰角
        specFile << "start_azAngle:    " << QString::number(mysetting.start_azAngle) << endl;	//起始角
        specFile << "step_azAngle:     " << QString::number(mysetting.step_azAngle) << endl;	//步进角
        specFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//方向数
        specFile << "IntervalTime:     " << QString::number(mysetting.IntervalTime) << endl;	//定时探测间隔，单位：分钟
        specFile << "GroupTime:        " << QString::number(mysetting.GroupTime) << endl;		//定时探测单组时间，单位：分钟

        //采样参数
        specFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;		//采样频率
        specFile << "Trigger_Level:    " << QString::number(mysetting.Trigger_Level) << endl;    //触发电平
        specFile << "PreTrigger:       " << QString::number(mysetting.PreTrigger) << endl;       //预触发点数，保留，暂不提供设置

        //实时处理参数
        specFile << "plsAccNum:        " << QString::number(mysetting.plsAccNum) << endl;        //单方向累加脉冲数
        specFile << "nRangeBin:        " << QString::number(mysetting.nRangeBin) << endl;        //距离门数
        specFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;    //距离门内点数

        specFile << "Frequency Axis:   ";
        QString str = "";
        for(int i=0;i<nFFT_half;i++)
            str = str + QString::number(freqAxis[i],'f', 2) + " ";
        specFile << str << endl;

        specFile << "Height Axis:      ";
        str = "";
        for(int i=0;i<mysetting.nRangeBin;i++)
            str = str + QString::number(Height_values[i],'f', 2) + " ";
        specFile << str << endl;

        specFile << "Spectrum Point Size: "<< sizeof(quint64) <<  " Bytes int" << endl;

        for(int i=0;i<60;i++)
            specFile << "=";                    // =分隔符
        specFile << endl;
        outputSpec.close();
        qDebug() << "Specfile Header added!";
    }
}

// 在已经写好文件头的文件中，添加一条数据记录
void MainWindow::SaveSpec_AddData()
{
    QFile outputSpec(SpecFileName);
    if(outputSpec.open(QFile::WriteOnly|QIODevice::Append))       //追加
    {
        QDataStream specFile(&outputSpec);
        specFile << CaptureTime.toMSecsSinceEpoch();
        specFile << currentMotorAngle;
        specFile.writeRawData((char*)adq.get_PSD_Union(), mysetting.nRangeBin * nFFT_half*8);      // 8为Uint64所占字节数

        //		int ret;
        //		ret = specFile.writeRawData((char*)data_a,mysetting.sampleNum*mysetting.plsAccNum*2);//返回值为写入的数据的字节数
        //采样数据的写入
        outputSpec.close();
        qDebug() << "Specfile 1 Dir added!";
    }
}

void MainWindow::on_pushButton_test_clicked()
{
    Generate_freqAxis();
    Init_Buffers();             //申请各个缓存空间
    SaveSpec_FileHead();
    SaveSpec_AddData();
}

// 计算得到频率坐标轴
void MainWindow::Generate_freqAxis()
{
    qDebug() << "mysetting.sampleFreq = " << mysetting.sampleFreq;
    for (int i = 0; i < nFFT_half; i++)  {
        freqAxis[i] = mysetting.sampleFreq/2.0*(i+1)/nFFT_half;
        qDebug() << freqAxis[i];
    }
}

void MainWindow::Init_Buffers()           // 初始化各个数据存储缓冲区
{
    if (losVelocity != nullptr)
        delete losVelocity;
    losVelocity = new double[mysetting.nRangeBin];

    if (aomSpec != nullptr)
        delete aomSpec;
    aomSpec = new double[nFFT_half];

    if (specArray != nullptr)
        delete specArray;
    specArray = new double[mysetting.nRangeBin * nFFT_half];

    adq.Init_Buffers();

}

void MainWindow::UpdateHeightsValue()           //
{
    double range_resolution = 300/mysetting.sampleFreq/2*mysetting.nPointsPerBin;
    for(int i=0;i<mysetting.nRangeBin;i++)
    {
        Height_values[i] = (i+1.5)*range_resolution;
        qDebug() << "Height_values["<<i<<"] = "<<Height_values[i];
    }
    DisplaySpeed->setHeights(Height_values);
}
