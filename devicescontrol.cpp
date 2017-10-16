#include "devicescontrol.h"

DevicesControl::DevicesControl(QObject *parent) : QObject(parent)
{
    qDebug() << "device control";
    timeOclock = new QTimer(this);
    ControlTimer = new QTimer(this);
    connect(&Compass, &compass::compassAngle, this, &DevicesControl::showCompassAngle);
    connect(&Motor, &motor::motorPrepareOk, this, &DevicesControl::readyToMove);
    connect(&Motor, &motor::motorAngle, this, &DevicesControl::checkMotorAngle);
    connect(&LaserSeed,&laserSeed::seedOpenReady, &LaserPulse,&laserPulse::beginPulseLaser);
    connect(&LaserPulse,&laserPulse::laserWorkRight,this,&DevicesControl::pulse_laser_opened_fcn);
    connect(ControlTimer,&QTimer::timeout, this,&DevicesControl::On_ControlTimer_TimeOut);
    connect(&Motor, &motor::beginMove, this, &DevicesControl::timeStart);
    connect(timeOclock,&QTimer::timeout,this, &DevicesControl::checkMove);
    connect(&Motor, &motor::moveReady,this, &DevicesControl::getPosition);
    connect(&LaserPulse,&laserPulse::pulseCloseReady, &LaserSeed,&laserSeed::closeSeedLaser);
    connect(&Motor, &motor::motorError, this, &DevicesControl::errorSolve);
    connect(&LaserSeed,&laserSeed::laserSeedError, this,&DevicesControl::laserErrorHint);
    connect(&LaserPulse,&laserPulse::laserPulseError, this,&DevicesControl::laserErrorHint);

    stopped = true;
    readyToCollect=false;
    isPulseLaserOpened = false;
}

void DevicesControl::startAction(SOFTWARESETTINGS settings)
{
    qDebug() << "start action";
    this->mysetting = settings;
    Compass.read();

}

void DevicesControl::stopAction()
{
    //待完善

    stop_now = true;
}


void DevicesControl::showCompassAngle(const double &s)
{
    qDebug() << "Angle" << s;
    headAngle = s;
    Motor.prepare();
}

void DevicesControl::checkMotorAngle(const double &s)
{
    qDebug()<<"check motor angle = " << s;
    if(moveNorth)
    {
        if(headAngle-s>=-0.5&&headAngle-s<=0.5)
        {
            qDebug()<<"success ";
            moveNorth = false;
            readyToCollect=true;
            checkReady = false;
            motorPX0 = s;
            if(motorPX0>360-mysetting.azAngleStep)
                motorPX0 = motorPX0-360;
            adq.Transfer_Settings(mysetting);
            LaserSeed.beginSeedLaser();
        }
        else
        {
            Motor.moveRelative(headAngle-s);
        }
    }
    else
    {
        if((s-motorPX0-mysetting.azAngleStep)<=0.01&&(s-motorPX0-mysetting.azAngleStep)>=-0.01)   //判断是否到达指定位置,误差暂设0.5°
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
   /* if(moveNorth)
    {
        adq.Transfer_Settings(mysetting);
        LaserSeed.beginSeedLaser();
        moveNorth=false;
        readyToCollect=true;
    }
   else
    {
        readyToCollect=true;
        LaserPulse.checkLaser();
        LaserSeed.checkLaser();
    }*/
}

void DevicesControl::getPosition()
{

    if(timeOclock->isActive())
    {
        timeOclock->stop();
    }
    Motor.position();
}

void DevicesControl::checkMotor()
{
    QSerialPort my_serial;
    my_serial.setPortName("COM8");
    if(!my_serial.open(QIODevice::ReadWrite))
    {
        qDebug()<<"motor not open";
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

void DevicesControl::errorSolve()
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

void DevicesControl::laserErrorHint(const QString &s)
{
    qDebug() << s;
}

void DevicesControl::readyToMove()
{
    qDebug()<< "motor prepare right";
    if(stopped) {
        if(mysetting.azAngleStep != 0) {
            moveNorth = true;
            Motor.position();
        }
    }
}

void DevicesControl::openLaser()
{
    LaserSeed.beginSeedLaser();
}

void DevicesControl::closeLaser()
{
    LaserPulse.closePulseLaser();
}

void DevicesControl::checkMove()
{
    Motor.checkMove();      //检查电机转动状态
}

void DevicesControl::pulse_laser_opened_fcn()
{
    qDebug() << "pulse laser open success!!";
    isPulseLaserOpened = true;
    capture_counter = 0;        // 探测方向计数器置零

    stop_now = false;
    if( mysetting.detectMode == 2) {
        Start_Time = QDateTime::currentDateTimeUtc();
    }
    State = waitMotor;
    ControlTimer->start(CheckPeriod);
    stopped = false;
    Init_Buffers();
    Generate_freqAxis();
    azimuthAngle = VectorXd::Zero(mysetting.nDirsVectorCal);
    losVelocityMat = MatrixXd::Zero(mysetting.nRangeBin, mysetting.nDirsVectorCal);
}

void DevicesControl::On_ControlTimer_TimeOut()
{
    QDateTime currentTime;
    qint64 dt;
    if (stop_now) {
        State = Quit;
        stop_now = false;
    }
    switch (State) {
    case waitMotor:    //****查询电机状态，没到位，则直接返回，等下次进入定时器
        if (mysetting.azAngleStep != 0) {
            if(readyToCollect) {
                State = Capture;
            }
        }
        else
            State = Capture;
        break;

    case Capture: {
        qDebug() << "capture start!!!";

        bool status = adq.Start_Capture();
        if(status == false) {
            qDebug() << "ADQ failed";
            ControlTimer->stop();
            return;
        }
        CaptureTime = QDateTime::currentDateTimeUtc();      //记录当前时间，将来写入文件
        if(mysetting.azAngleStep != 0)
            Motor.moveRelative(mysetting.azAngleStep);         //-------相对转动电机（step_azAngle为0也可以调用函数，不转就可以了）
        readyToCollect = false;
        LOSVelocityCal(mysetting.nRangeBin+2, nFFT_half,
                       20, mysetting.laserWaveLength,
                       freqAxis, adq.get_PSD_double());     //径向风速计算

        //****矢量风速合成
        qDebug() << "start vector velocity";
        VectorXd tempAzAngle = azimuthAngle.tail(mysetting.nDirsVectorCal-1);
        azimuthAngle.head(mysetting.nDirsVectorCal-1) = tempAzAngle;
        azimuthAngle(mysetting.nDirsVectorCal-1) = currentMotorAngle;

        for (int i=0; i<mysetting.nRangeBin; i++) {
            for (int j=1; j<mysetting.nDirsVectorCal; j++) {
                losVelocityMat(i,j-1) = losVelocityMat(i,j);
            }
        }
        for (int i=0; i<mysetting.nRangeBin; i++) {
            losVelocityMat(i,mysetting.nDirsVectorCal-1) = losVelocity[i];
        }

        if (capture_counter > mysetting.nDirsVectorCal) {
            qDebug() << "Starting vector velocity cal!!!!";
            std::cout << "azimuthAngle = " << azimuthAngle << std::endl;
            DSWF dswf(mysetting.elevationAngle,azimuthAngle,mysetting.nDirsVectorCal,mysetting.nRangeBin,losVelocityMat);
            qDebug() << "End vector velocity cal";
            hVelocity = dswf.getHVelocity();
            hAngle = dswf.getHAngle();
            vVelocity = dswf.getVVelocity();

            //****更新显示
            emit hVelocityReady(hVelocity);
            emit hAngleReady(hAngle);
            emit vVelocityReady(vVelocity);
            qDebug() << "update show signal emited";
        }
        qDebug() << "stop vector velocity";

        //存储功率谱到文件
        SaveSpec_AddData();

        //****存储风速到文件

        capture_counter++;
        qDebug() << "capture_counter = " << capture_counter;
        //****判断是否应该结束，更新结束标志stop_now
        switch (mysetting.detectMode) {
        case 1:                 //持续探测
            //判断是否应该关闭文件，建立新文件
            if(capture_counter == mysetting.nDirsPerFile)
            {
                SaveSpec_FileHead();        //建立新文件，写入文件头
            }
            State = waitMotor;
            break;
        case 0:                 //单组探测
            //判断探测方向数
            if(capture_counter == mysetting.angleNum)
                State = Quit;
            else
                State = waitMotor;
            break;
        case 2:                 //定时探测
            // 判断否达到待机条件
            dt = currentTime.msecsTo(Start_Time);
            if(dt > qint64(mysetting.groupTime*60*1000))  // *60s? *1000ms?// 如果达到待机时间
            {
                State = Standby;
            }
            break;
        default:
            break;
        }
        break;
    }

    case Quit:
        ControlTimer->stop();
        LaserPulse.closePulseLaser();  //-------关闭激光放大器,关闭激光器本振
        stopped = true;
        break;

    case Standby:       //也许之前需要一个停止状态
        currentTime = QDateTime::currentDateTimeUtc();
        dt = currentTime.msecsTo(Start_Time);
        if(dt > qint64(mysetting.intervalTime*60*1000))  // *60s? *1000ms?      //如果达到启动时间
        {
            Start_Time = Start_Time.addMSecs(mysetting.intervalTime*60*1000);   //更新开始时间，为了下次计时
            if (mysetting.azAngleStep != 0)
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

void DevicesControl::timeStart()
{
    timeOclock->start(1000);
}

void DevicesControl::Generate_freqAxis()
{
    for (int i = 0; i < nFFT_half; i++)  {
        freqAxis[i] = mysetting.sampleFreq/2.0*(i+1)/nFFT_half;
        //        qDebug() << freqAxis[i];
    }
}

void DevicesControl::LOSVelocityCal(const int heightNum, const int totalSpecPoints, const int objSpecPoints, const double lambda, const double *freqAxis, const double *specData)
{
    for (int k = 0; k < totalSpecPoints; k++) {
        aomSpec[k] = specData[totalSpecPoints+k] - specData[k];
        for (int l = 0; l < heightNum - 2; l++){
            specArray[l*totalSpecPoints+k] = specData[totalSpecPoints*(l+2) + k] - specData[k];
        }
    }
    int aomIndex = 0;
    double temp = aomSpec[0];
    for (int k = 1; k < totalSpecPoints; k++) {
        if (aomSpec[k] > temp) {
            temp = aomSpec[k];
            aomIndex = k;
        }
    }

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
    memset(losVelocity, 0, sizeof(double)*(heightNum-2));
    for(int i=0; i<heightNum-2; i++) {
        losVelocity[i] = (freqAxis[losVelocityIndex[i]] - freqAxis[aomIndex])*lambda/2;
        //        qDebug() << "losVelocity" << losVelocity[i];
    }
}

void DevicesControl::Init_Buffers()
{
    losVelocity = new double[mysetting.nRangeBin];
    aomSpec = new double[nFFT_half];
    specArray = new double[mysetting.nRangeBin * nFFT_half];

    adq.Init_Buffers();
}

void DevicesControl::SaveSpec_FileHead()
{
    Create_DataFolder();
    SpecFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").spec";
    //    SpecFileName = mysetting.DatafilePath+"/"+"KnapsackCDL("+ SpecFileName +").spec";
    //    qDebug()<<"SpecFileName = "<<SpecFileName;
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
        //        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        specFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //激光参数
        specFile << "LaserMode:        ";
        if(1)
        {
            specFile << "Pulse" << endl;		//脉冲探测（true）or连续探测（false） bool
            specFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;	//激光能量，单位μJ，连续模式下为0
        }
        else
        {
            specFile << "Continuous" << endl;	//脉冲探测（true）or连续探测（false） bool
            specFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl;		//激光功率，单位mW，脉冲模式下为0
        }
        specFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;			//激光频率
        specFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;	//脉冲宽度
        specFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;	//激光波长
        specFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;			//AOM移频量

        //扫描参数
        specFile << "detectMode:       " ;      //探测方式：0持续探测1单组探测2定时探测
        switch (mysetting.detectMode) {
        case 1:                 //持续探测
            specFile << "NonStop" << endl;
            break;
        case 0:                 //单组探测
            specFile << "SingleGroup" << endl;
            break;
        case 2:                 //定时探测
            specFile << "scheduled" << endl;
            break;
        }

        specFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//俯仰角
        specFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//起始角
        specFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;	//步进角
        specFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//方向数
        specFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//定时探测间隔，单位：分钟
        specFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//定时探测单组时间，单位：分钟

        //采样参数
        specFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;		//采样频率
        specFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;    //触发电平
        specFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;       //预触发点数，保留，暂不提供设置

        //实时处理参数
        specFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;        //单方向累加脉冲数
        specFile << "nRangeBin:        " << QString::number(mysetting.nRangeBin) << endl;        //距离门数
        specFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;    //距离门内点数

        specFile << "Frequency Axis:   ";
        QString str = "";
        for(int i=0;i<nFFT_half;i++)
            str = str + QString::number(freqAxis[i],'f', 2) + " ";
        specFile << str << endl;

        specFile << "Height Axis:      ";
        str = "";
        //        for(int i=0;i<mysetting.nRangeBin;i++)
        //            str = str + QString::number(Height_values[i],'f', 2) + " ";
        specFile << str << endl;

        specFile << "Spectrum Point Size: "<< sizeof(quint64) <<  " Bytes int" << endl;

        for(int i=0;i<60;i++)
            specFile << "=";                    // =分隔符
        specFile << endl;
        outputSpec.close();
        //        qDebug() << "Specfile Header added!";
    }
}

void DevicesControl::SaveSpec_AddData()
{
    QFile outputSpec(SpecFileName);
    if(outputSpec.open(QFile::WriteOnly|QIODevice::Append))       //追加
    {
        QDataStream specFile(&outputSpec);
        specFile << CaptureTime.toMSecsSinceEpoch();
        specFile << currentMotorAngle;
        specFile.writeRawData((char*)adq.get_PSD_Union(), mysetting.nRangeBin * nFFT_half*8);      // 8为Uint64所占字节数
        outputSpec.close();
        //        qDebug() << "Specfile 1 Dir added!";
    }
}

void DevicesControl::Create_DataFolder()
{
    QDir mypath;
    if(!mypath.exists(mysetting.dataFilePath))		//如果文件夹不存在，创建文件夹
        mypath.mkpath(mysetting.dataFilePath);
}
