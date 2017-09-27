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

    m_setfile = new SettingFile();
    mysetting = m_setfile->readSettings();								// mysetting��ȡ�ļ��еĲ���

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

    Compass.read();             // ��ȡһ����������ȷ����������״��
    checkMotor();               // ���������
    //    mysetting.step_azAngle = 0;

    //    adq.connectADQDevice();     // ���Ӳɼ���

    connect(&LaserSeed,&laserSeed::seedOpenReady, &LaserPulse,&laserPulse::beginPulseLaser);
    //    connect(&LaserSeed,&laserSeed::seedOpenReady, &MainWindow::beginLaser2);
    connect(&LaserPulse,&laserPulse::pulseCloseReady, &LaserSeed,&laserSeed::closeSeedLaser);
    connect(&LaserSeed,&laserSeed::laserSeedError, this,&MainWindow::laserErrorHint);
    connect(&LaserPulse,&laserPulse::laserPulseError, this,&MainWindow::laserErrorHint);
    connect(&LaserPulse, SIGNAL(laserWorkRight()), this, SLOT(pulse_laser_opened_fcn()));

    //��ʾ����

    DisplaySpeed = new wind_display(this);
    DisplaySpeed->set_nLayers(mysetting.nRangeBin);
    UpdateHeightsValue();

    ui->gridLayout->addWidget(DisplaySpeed);

    connect(this, &MainWindow::size_changed,DisplaySpeed, &wind_display::setSubSize);
    TestTimer = new QTimer(this);
    //    timer->start(1000);
    connect(TestTimer, SIGNAL(timeout()), this, SLOT(changeData()));

    // ״̬������ʼ��
    stopped = true;
    readyToCollect=false;
    isPulseLaserOpened = false;

    // ������ָ���ÿ�
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
    delete m_setfile;
    toolBarControlTimer->stop();
    toolBarControlTimer->deleteLater();
}

void MainWindow::action_set_triggered()
{
    ParameterSetDialog *parameterSetDialog = new ParameterSetDialog(this);
    parameterSetDialog->setParaSettings(mysetting);
    if (parameterSetDialog->exec() == QDialog::Accepted) {
        mysetting =  parameterSetDialog->getParaSettings();
        DisplaySpeed->set_nLayers(mysetting.nRangeBin);
        UpdateHeightsValue();
    }
    delete parameterSetDialog;
}

void MainWindow::on_startButton_clicked()
{
    Compass.read();
    moveNorth = true;
    Motor.prepare();            // ����ϵ粢�����ٶȣ����ٶȲ���
}

void MainWindow::showCompassAngle(const double &s)
{
    headAngle = s;                // ��¼������ֵ
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
            //            adq.Start_Capture();        //ָ����ʼ�ɼ�������
            readyToCollect=true;
            LaserPulse.checkLaser();
            LaserSeed.checkLaser();
            checkReady = false;
            motorPX0 = s;
            if(motorPX0>360-mysetting.azAngleStep)
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
        if((s-motorPX0-mysetting.azAngleStep)<=0.5&&(s-motorPX0-mysetting.azAngleStep)>=-0.5)   //�ж��Ƿ񵽴�ָ��λ��,�������0.5��
        {
            //adq.Start_Capture();
            readyToCollect=true;
            LaserPulse.checkLaser();
            LaserSeed.checkLaser();
            checkReady = false;
            motorPX0 = s;
            if(motorPX0>360-mysetting.azAngleStep)
                motorPX0 = motorPX0-360;
        }
        else
        {
            Motor.moveRelative(motorPX0+mysetting.azAngleStep-s);
        }

    }
}

void MainWindow::readyToMove()
{
    Motor.position();
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
    capture_counter = 0;        // ̽�ⷽ�����������

    stop_now = false;
    if( mysetting.detectMode == 2)//��ʱ̽��
    {
        Start_Time = QDateTime::currentDateTimeUtc();   //��¼��ǰʱ����Ϊ��ʼʱ��

    }
    State = waitMotor;
    ControlTimer->start(CheckPeriod);       // ��ʱ��ѯ���������������״̬���ɼ���������ʾ

    //        TestTimer->start(1000);
    stopped = false;
}

//���������вɼ���׼����������������ʱ��
//ֹͣ���趨ֹͣ��־�ź�
void MainWindow::action_start_Triggered()
{
    qDebug() << "Start Action Triggered!!!";

    if(stopped)                 //�����ǰΪֹͣ״̬������Կ�ʼ�ɼ�
    {
        //****�жϵ���Ƿ���ת����֮ǰ��̽���Ƿ��Ѿ�ֹͣ��ûֹͣ�͵ȣ�֮ǰ��̽��û�����������֪�������͵ȡ�


        if (mysetting.azAngleStep != 0)
        {

            moveNorth=true;
            Motor.position();        //-----���ת�� mysetting.start_azAngle;

        }
        Generate_freqAxis();        //����Ƶ��������
        adq.Transfer_Settings(mysetting);   //�������ò�����adq
        Init_Buffers();             //�����������ռ�
        SaveSpec_FileHead();        // �½��ļ���д���ļ�ͷ
        LaserSeed.beginSeedLaser(); //------�򿪼��������񣬴򿪼���Ŵ���

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
    case waitMotor:    //****��ѯ���״̬��û��λ����ֱ�ӷ��أ����´ν��붨ʱ��
        if (mysetting.azAngleStep != 0)
        {
            if(readyToCollect)
            {
                State = Capture;
            }
            //---------�жϵ��ת����λ
        }
        else
            State = Capture;
        break;

    case Capture:
        qDebug() << "capture start!!!";

        adq.Start_Capture();                                //�ɼ�
        qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaa!!!!!!!!!!!!!!";
        CaptureTime = QDateTime::currentDateTimeUtc();      //��¼��ǰʱ�䣬����д���ļ�
        if(mysetting.azAngleStep != 0)
            Motor.moveRelative(mysetting.azAngleStep);         //-------���ת�������step_azAngleΪ0Ҳ���Ե��ú�������ת�Ϳ����ˣ�
        readyToCollect = false;
        LOSVelocityCal(mysetting.nRangeBin+2, nFFT_half,
                       20, mysetting.laserWaveLength,
                       freqAxis, adq.get_PSD_double());     //������ټ���
        //****ʸ�����ٺϳ�
        //****������ʾ
        for(int i = 0;i<mysetting.nRangeBin;i++)
            H_speed[i] = qAbs(losVelocity[i]);

        DisplaySpeed->setHSpeed(H_speed);
        for(int i = 0;i<mysetting.nRangeBin;i++)
            H_direction[i] = losVelocity[i]>0?0:(180);
        DisplaySpeed->setHDirection(H_direction);

        SaveSpec_AddData();        //�洢�����׵��ļ�

        //****�洢���ٵ��ļ�

        capture_counter++;
        //****�ж��Ƿ�Ӧ�ý��������½�����־stop_now
        switch (mysetting.detectMode) {
        case 1:                 //����̽��
            //�ж��Ƿ�Ӧ�ùر��ļ����������ļ�
            if(capture_counter == mysetting.nDirsPerFile)
            {
                SaveSpec_FileHead();        //�������ļ���д���ļ�ͷ
            }
            State = waitMotor;
            break;
        case 0:                 //����̽��
            //�ж�̽�ⷽ����
            if(capture_counter == mysetting.angleNum)
                State = Quit;
            else
                State = waitMotor;
            break;
        case 2:                 //��ʱ̽��
            // �жϷ�ﵽ��������
            dt = currentTime.msecsTo(Start_Time);
            if(dt > qint64(mysetting.groupTime*60*1000))  // *60s? *1000ms?// ����ﵽ����ʱ��
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
        LaserPulse.closePulseLaser();  //-------�رռ���Ŵ���,�رռ���������
        stopped = true;
        userToolBar->set_to_stopped();
        break;

    case Standby:       //Ҳ��֮ǰ��Ҫһ��ֹͣ״̬
        currentTime = QDateTime::currentDateTimeUtc();
        dt = currentTime.msecsTo(Start_Time);
        if(dt > qint64(mysetting.intervalTime*60*1000))  // *60s? *1000ms?      //����ﵽ����ʱ��
        {
            Start_Time = Start_Time.addMSecs(mysetting.intervalTime*60*1000);   //���¿�ʼʱ�䣬Ϊ���´μ�ʱ
            if (mysetting.azAngleStep != 0)
            {
                moveNorth = true;
                Motor.position();
                //---------���ת�� mysetting.start_azAngle;
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

//���ݴ洢�ļ��еĴ���
void MainWindow::Create_DataFolder()
{
    QDir mypath;
    if(!mypath.exists(mysetting.dataFilePath))		//����ļ��в����ڣ������ļ���
        mypath.mkpath(mysetting.dataFilePath);
}



// ������ټ������
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
    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").spec";
    //    SpecFileName = mysetting.DatafilePath+"/"+"KnapsackCDL("+ SpecFileName +").spec";
    qDebug()<<"SpecFileName = "<<SpecFileName;
    QFile outputSpec(SpecFileName);

    if(outputSpec.open(QFile::WriteOnly | QIODevice::Truncate|QIODevice::Text))//QIODevice::Truncate��ʾ��ԭ�ļ��������
    {
        QTextStream specFile(&outputSpec);
        specFile << "Knapsack Coherent Doppler Lidar Original Spectrum" << endl;
        for(int i=0;i<60;i++)
            specFile << "=";                                // =�ָ���
        specFile << endl<< "Data File Version: 1.0" << endl;          // �ļ����Ͱ汾
        specFile << "Global Definitions:" << endl;

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        specFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //�������
        specFile << "LaserMode:        ";
        if(1)
        {
            specFile << "Pulse" << endl;		//����̽�⣨true��or����̽�⣨false�� bool
            specFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;	//������������λ��J������ģʽ��Ϊ0
        }
        else
        {
            specFile << "Continuous" << endl;	//����̽�⣨true��or����̽�⣨false�� bool
            specFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl;		//���⹦�ʣ���λmW������ģʽ��Ϊ0
        }
        specFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;			//����Ƶ��
        specFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;	//������
        specFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;	//���Ⲩ��
        specFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;			//AOM��Ƶ��

        //ɨ�����
        specFile << "detectMode:       " ;      //̽�ⷽʽ��0����̽��1����̽��2��ʱ̽��
        switch (mysetting.detectMode) {
        case 1:                 //����̽��
            specFile << "NonStop" << endl;
            break;
        case 0:                 //����̽��
            specFile << "SingleGroup" << endl;
            break;
        case 2:                 //��ʱ̽��
            specFile << "scheduled" << endl;
            break;
        }

        specFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//������
        specFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//��ʼ��
        specFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;	//������
        specFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//������
        specFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//��ʱ̽��������λ������
        specFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//��ʱ̽�ⵥ��ʱ�䣬��λ������

        //��������
        specFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;		//����Ƶ��
        specFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;    //������ƽ
        specFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;       //Ԥ�����������������ݲ��ṩ����

        //ʵʱ�������
        specFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;        //�������ۼ�������
        specFile << "nRangeBin:        " << QString::number(mysetting.nRangeBin) << endl;        //��������
        specFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;    //�������ڵ���

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
            specFile << "=";                    // =�ָ���
        specFile << endl;
        outputSpec.close();
        qDebug() << "Specfile Header added!";
    }
}

// ���Ѿ�д���ļ�ͷ���ļ��У����һ�����ݼ�¼
void MainWindow::SaveSpec_AddData()
{
    QFile outputSpec(SpecFileName);
    if(outputSpec.open(QFile::WriteOnly|QIODevice::Append))       //׷��
    {
        QDataStream specFile(&outputSpec);
        specFile << CaptureTime.toMSecsSinceEpoch();
        specFile << currentMotorAngle;
        specFile.writeRawData((char*)adq.get_PSD_Union(), mysetting.nRangeBin * nFFT_half*8);      // 8ΪUint64��ռ�ֽ���

        //		int ret;
        //		ret = specFile.writeRawData((char*)data_a,mysetting.sampleNum*mysetting.plsAccNum*2);//����ֵΪд������ݵ��ֽ���
        //�������ݵ�д��
        outputSpec.close();
        qDebug() << "Specfile 1 Dir added!";
    }
}

void MainWindow::on_pushButton_test_clicked()
{
    Generate_freqAxis();
    Init_Buffers();             //�����������ռ�
    SaveSpec_FileHead();
    SaveSpec_AddData();
}

// ����õ�Ƶ��������
void MainWindow::Generate_freqAxis()
{
    qDebug() << "mysetting.sampleFreq = " << mysetting.sampleFreq;
    for (int i = 0; i < nFFT_half; i++)  {
        freqAxis[i] = mysetting.sampleFreq/2.0*(i+1)/nFFT_half;
        qDebug() << freqAxis[i];
    }
}

void MainWindow::Init_Buffers()           // ��ʼ���������ݴ洢������
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
