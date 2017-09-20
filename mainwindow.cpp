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
    QString path = dirs.currentPath()+"/"+"214settings.ini";			// ��ȡ��ʼĬ��·���������Ĭ�������ļ�
    qDebug() << "initial path = " << path;
    m_setfile.test_create_file(path);									// ���settings.ini�Ƿ���ڣ����������򴴽�
    m_setfile.readFrom_file(path);										// ��ȡsettings.ini�ļ�
    mysetting = m_setfile.get_settings();								// mysetting��ȡ�ļ��еĲ���

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

    Compass.read();             // ��ȡһ����������ȷ����������״��
    checkMotor();               // ���������
    mysetting.step_azAngle = 60;

    //    adq.connectADQDevice();     // ���Ӳɼ���

    connect(&LaserSeed,&laserSeed::seedOpenReady, &LaserPulse,&laserPulse::beginPulseLaser);
    connect(&LaserPulse,&laserPulse::pulseCloseReady, &LaserSeed,&laserSeed::closeSeedLaser);
    connect(&LaserSeed,&laserSeed::laserSeedError, this,&MainWindow::laserErrorHint);
    connect(&LaserPulse,&laserPulse::laserPulseError, this,&MainWindow::laserErrorHint);


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


    connect(this, &MainWindow::size_changed,DisplaySpeed, &wind_display::setSubSize);
    TestTimer = new QTimer(this);
    //    timer->start(1000);
    connect(TestTimer, SIGNAL(timeout()), this, SLOT(changeData()));
    stopped = true;
    readyToCollect=false;

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
    toolBarControlTimer->stop();
    toolBarControlTimer->deleteLater();
}

void MainWindow::action_set_triggered()
{
    qDebug() << " open setting ";
    ParaSetDlg = new paraDialog(this);
    ParaSetDlg->init_setting(mysetting,stopped);					// mysetting���ݸ����ô���psetting
    ParaSetDlg->initial_para();										// ������ʾ�����ô����ϣ������Ӳ�
    ParaSetDlg->on_checkBox_autoCreate_DateDir_clicked();			// �����ļ��洢·��
    if (ParaSetDlg->exec() == QDialog::Accepted)					// ȷ��������
    {
        mysetting =	ParaSetDlg->get_settings();						// mysetting��ȡ�޸ĺ�Ĳ���
    }
    delete ParaSetDlg;
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
        if((s-motorPX0-mysetting.step_azAngle)<=0.5&&(s-motorPX0-mysetting.step_azAngle)>=-0.5)   //�ж��Ƿ񵽴�ָ��λ��,�������0.5��
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

void MainWindow::action_quit_triggered()
{
    timeOclock->stop();
    Motor.motorQuit();
    Sleep(100);
    this->close();
}


//���������вɼ���׼����������������ʱ��
//ֹͣ���趨ֹͣ��־�ź�
void MainWindow::action_start_Triggered()
{
    qDebug() << "Start Action Triggered!!!";

    if(stopped)                 //�����ǰΪֹͣ״̬������Կ�ʼ�ɼ�
    {
        //****�жϵ���Ƿ���ת����֮ǰ��̽���Ƿ��Ѿ�ֹͣ��ûֹͣ�͵ȣ�֮ǰ��̽��û�����������֪�������͵ȡ�


        if (mysetting.step_azAngle != 0)
        {

            moveNorth=true;
            Motor.position();        //-----���ת�� mysetting.start_azAngle;

        }
        Generate_freqAxis();        //����Ƶ��������
        Init_Buffers();             //�����������ռ�
        adq.Transfer_Settings(mysetting);   //�������ò�����adq
        SaveSpec_FileHead();        // �½��ļ���д���ļ�ͷ
        LaserSeed.beginSeedLaser(); //------�򿪼��������񣬴򿪼���Ŵ���
        capture_counter = 0;        // ̽�ⷽ�����������

        stop_now = false;
        if( mysetting.detectMode == 2)//��ʱ̽��
        {
            //****��¼��ǰʱ����Ϊ��ʼʱ��
        }
        State = waitMotor;
        ControlTimer->start(100);       // ��ʱ��ѯ���������������״̬���ɼ���������ʾ

        TestTimer->start(1000);
        stopped = false;
    }
    else
    {
        stop_now = true;
        TestTimer->stop();

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

    switch (State) {
    case waitMotor:    //****��ѯ���״̬��û��λ����ֱ�ӷ��أ����´ν��붨ʱ��
        if (mysetting.step_azAngle != 0)
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
        //****�ɼ�
        adq.Start_Capture();
        CaptureTime = QDateTime::currentDateTimeUtc();      //��¼��ǰʱ�䣬����д���ļ�
        Motor.moveRelative(mysetting.step_azAngle);    //-------���ת�������step_azAngleΪ0Ҳ���Ե��ú�������ת�Ϳ����ˣ�
        readyToCollect = false;
        adq.ConvertData2Spec();//ת��������
        //������ټ���
        LOSVelocityCal(mysetting.nRangeBin, nFFT_half,
                       20, mysetting.laserWaveLength,
                       freqAxis, adq.get_PSD_double());
        //****ʸ�����ٺϳ�
        //****������ʾ

        SaveSpec_AddData();        //�洢�����׵��ļ�

        //****�洢���ٵ��ļ�

        capture_counter++;
        //****�ж��Ƿ�Ӧ�ý��������½�����־stop_now
        switch (mysetting.detectMode) {
        case 0:                 //����̽��
            //�ж��Ƿ�Ӧ�ùر��ļ����������ļ�
            if(capture_counter == mysetting.nMaxDir_inFile)
            {
                SaveSpec_FileHead();        //�������ļ���д���ļ�ͷ
            }
            State = waitMotor;
            break;
        case 1:                 //����̽��
            //�ж�̽�ⷽ����
            if(capture_counter == mysetting.angleNum)
                State = Quit;
            else
                State = waitMotor;
            break;
        case 2:                 //��ʱ̽��
            // �жϷ�ﵽ��������
            dt = currentTime.msecsTo(Start_Time);
            if(dt > qint64(mysetting.GroupTime*60*1000))  // *60s? *1000ms?// ����ﵽ����ʱ��
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
        if(dt > qint64(mysetting.IntervalTime*60*1000))  // *60s? *1000ms?      //����ﵽ����ʱ��
        {
            Start_Time = Start_Time.addMSecs(mysetting.IntervalTime*60*1000);   //���¿�ʼʱ�䣬Ϊ���´μ�ʱ
            if (mysetting.step_azAngle != 0)
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
    if(!mypath.exists(mysetting.DatafilePath))		//����ļ��в����ڣ������ļ���
        mypath.mkpath(mysetting.DatafilePath);
}


// ������ټ������
void MainWindow::LOSVelocityCal(const int heightNum, const int totalSpecPoints,
                                const int objSpecPoints, const double lambda,
                                const double *freqAxis, const double *specData)
{
    for (int k = 0; k < totalSpecPoints; k++) {
        aomSpec[k] = specData[totalSpecPoints+k] - specData[k];
        for (int l = 0; l < heightNum - 2; l++){
            specArray[l*totalSpecPoints+k] = specData[totalSpecPoints*(l+2) + k] - specData[k];
        }
    }

    for (int i=0; i<totalSpecPoints; i++) {
        qDebug() << aomSpec[i];
    }
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

    for (int l = 0; l < heightNum -2; l++) {
        qDebug() << losVelocityIndex[l];
    }

    memset(losVelocity, 0, sizeof(double)*(heightNum-2));
    for(int i=0; i<heightNum-2; i++) {
        losVelocity[i] = (freqAxis[losVelocityIndex[i]] - freqAxis[aomIndex])*lambda/2;
    }
}

void MainWindow::SaveSpec_FileHead()
{
    Create_DataFolder();
    SpecFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss.zzz");
    //    KnapsackCDL(20170912_11_30_21).spec
    SpecFileName = mysetting.DatafilePath+"/"+"KnapsackCDL("+ SpecFileName +").spec";
    QFile outputSpec(SpecFileName);

    if(outputSpec.open(QFile::WriteOnly | QIODevice::Truncate))//QIODevice::Truncate��ʾ��ԭ�ļ��������
    {
        QDataStream specFile(&outputSpec);
        specFile << quint32(0xA1A0A1A8);                    // �ļ����ͱ�ʶ
        specFile << quint32(0x00000100);                    // �ļ��汾001.0
        specFile << "Knapsack Coherent Doppler Lidar Original Spectrum";

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        specFile << zero.toString("yyyy-MM-dd hh:mm:ss").toLatin1();          //1970-01-01 00:00:00


        //�������
        specFile << mysetting.isPulseMode;		//����̽�⣨true��or����̽�⣨false�� bool
        specFile << mysetting.laserPulseEnergy;	//������������λ��J������ģʽ��Ϊ0     float
        specFile << mysetting.laserPower;		//���⹦�ʣ���λmW������ģʽ��Ϊ0     float
        specFile << mysetting.laserRPF;			//����Ƶ��                         quint16
        specFile << mysetting.laserPulseWidth;	//������                         quint16
        specFile << mysetting.laserWaveLength;	//���Ⲩ��                         quint16
        specFile << mysetting.AOM_Freq;			//AOM��Ƶ��                        quint16

        //ɨ�����
        specFile << mysetting.detectMode;		//̽�ⷽʽ��0����̽��1����̽��2��ʱ̽��  int
        specFile << mysetting.elevationAngle;	//������                             quint16
        specFile << mysetting.start_azAngle;	//��ʼ��                             quint16
        specFile << mysetting.step_azAngle;		//������                             quint16
        specFile << mysetting.angleNum;			//������                             quint32
        specFile << mysetting.IntervalTime;		//��ʱ̽��������λ������              float
        specFile << mysetting.GroupTime;		//��ʱ̽�ⵥ��ʱ�䣬��λ������           float

        //��������
        specFile << mysetting.sampleFreq;		//����Ƶ��                         quint16
        specFile << mysetting.Trigger_Level;    //������ƽ                         quint16
        specFile << mysetting.PreTrigger;       //Ԥ�����������������ݲ��ṩ����       int

        //ʵʱ�������
        specFile << mysetting.plsAccNum;        //�������ۼ�������         quint16
        specFile << mysetting.nRangeBin;        //��������                quint16
        specFile << mysetting.nPointsPerBin;    //�������ڵ���             quint16

        //        Frequency Axis: 1.00 2.00 3.00 4.00 ...							#�ַ�����ʽ����Ƶ�����������Ƶ�ʣ���ȷ��С�������λ����λ��MHz
        //        Height Axis: 100.00 200.00 300.00 400.00 ...						#�ַ�����ʽ����߶����������߶ȣ���ȷ��С�������λ����λ��m
        //        Spectrum Point Size: 8 Bytes int

        outputSpec.close();
        qDebug() << "Specfile Header added!";
    }
}

void MainWindow::SaveSpec_AddData()
{
    QFile outputSpec(SpecFileName);
    if(outputSpec.open(QFile::WriteOnly))       //׷��
    {
        QDataStream specFile(&outputSpec);
        specFile << CaptureTime.toMSecsSinceEpoch();
        specFile << currentMotorAngle;
        specFile.writeRawData((char*)adq.get_PSD_Union(), mysetting.nRangeBin *nFFT_half*8);      // 8ΪUint64��ռ�ֽ���

        //		int ret;
        //		ret = specFile.writeRawData((char*)data_a,mysetting.sampleNum*mysetting.plsAccNum*2);//����ֵΪд������ݵ��ֽ���
        //�������ݵ�д��
        outputSpec.close();
        qDebug() << "Specfile 1 Dir added!";
    }
}

void MainWindow::on_pushButton_test_clicked()
{
    SaveSpec_FileHead();
}

void MainWindow::Generate_freqAxis()
{
    for (int i = 0; i < nFFT_half; i++)
        freqAxis[i] = mysetting.sampleFreq*(i+1)/nFFT_half;
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
