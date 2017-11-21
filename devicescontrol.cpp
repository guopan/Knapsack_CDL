#include "devicescontrol.h"
#include <QtMath>

# define method 1

DevicesControl::DevicesControl(QObject *parent) : QObject(parent)
{
    motorCheckTimer = new QTimer(this);
    ControlTimer = new QTimer(this);
    connect(&Compass, &compass::compassAngle, this, &DevicesControl::showCompassAngle);
    connect(&Motor, &motor::motorPrepareOk, this, &DevicesControl::readyToMove);
    connect(&Motor, &motor::motorAngle, this, &DevicesControl::checkMotorAngle);
    connect(&LaserSeed,&laserSeed::seedOpenReady, &LaserPulse,&laserPulse::beginPulseLaser);
    connect(&LaserPulse,&laserPulse::laserWorkRight,this,&DevicesControl::pulse_laser_opened_fcn);
    connect(ControlTimer,&QTimer::timeout, this,&DevicesControl::On_ControlTimer_TimeOut);
    connect(&Motor, &motor::beginMove, this, &DevicesControl::timeStart);
    connect(motorCheckTimer,&QTimer::timeout,this, &DevicesControl::checkMove);
    connect(&Motor, &motor::moveReady,this, &DevicesControl::getPosition);
    connect(&LaserPulse,&laserPulse::pulseCloseReady, &LaserSeed,&laserSeed::closeSeedLaser);
    connect(&Motor, &motor::motorError, this, &DevicesControl::errorSolve);
    connect(&LaserSeed,&laserSeed::laserSeedError, this,&DevicesControl::laserErrorHint);
    connect(&LaserPulse,&laserPulse::laserPulseError, this,&DevicesControl::laserErrorHint);
//    connect(&LaserSeed,&laserSeed::laserColseRight, this,&DevicesControl::quitControlTimer);
    connect(&LaserSeed,&laserSeed::laserColseRight, ControlTimer,&QTimer::stop);
    connect(&LaserSeed,&laserSeed::laserColseRight, this,&DevicesControl::detectionFinished);

//    connect(&Motor, &motor::motorClosed, this, &DevicesControl::quitLaser);
    connect(&Motor, &motor::motorClosed, &LaserPulse, &laserPulse::closePulseLaser);

    readyToCollect = false;
}

void DevicesControl::startAction(SOFTWARESETTINGS settings)
{
    qDebug() << "start action";
    this->mysetting = settings;
    if(mysetting.overlapRatio>0)
        nRB_ovlp = mysetting.nRangeBin*2-1;
    else
        nRB_ovlp = mysetting.nRangeBin;
    Compass.read();

}

void DevicesControl::stopAction()
{
    //待完善
    qDebug() << "stop action!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    stop_now = true;
}


void DevicesControl::showCompassAngle(const double &s)
{
    headAngle = s;
    Motor.prepare();
}

void DevicesControl::checkMotorAngle(const double &s)
{
    currentMotorAngle = s;
    if(moveNorth)       // 采集之初，驱动电机指向正北时
    {
        if(headAngle-s>=-0.5&&headAngle-s<=0.5)
        {
            moveNorth = false;
            readyToCollect = true;
            checkReady = false;
            motorPX0 = s;
            if(motorPX0>360-mysetting.azAngleStep)
                motorPX0 = motorPX0-360;
            adq.Transfer_Settings(mysetting);
            LaserSeed.beginSeedLaser(mysetting.laserLocalPower,mysetting.laserPulseEnergy);
        }
        else
        {
            Motor.moveRelative(headAngle-s);
        }
    }
    else                // 每次采集结束，转到下一方向时
    {
        if((s-motorPX0-mysetting.azAngleStep)<=0.01&&(s-motorPX0-mysetting.azAngleStep)>=-0.01)   //判断是否到达指定位置,误差暂设0.5°
        {
            readyToCollect = true;
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

void DevicesControl::getPosition()
{

    if(motorCheckTimer->isActive())
    {
        motorCheckTimer->stop();
    }
    Motor.position();
}

void DevicesControl::checkMotor()
{
    QSerialPort my_serial;
    my_serial.setPortName(MotorComPort);
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
    motorCheckTimer->stop();
    qDebug()<<"motor already stop";
}

void DevicesControl::laserErrorHint(const QString &s)
{
    qDebug() << s;
}

void DevicesControl::readyToMove()
{
    if(mysetting.azAngleStep != 0) {
        moveNorth = true;
        Motor.position();
    }
}

void DevicesControl::openLaser()
{
    LaserSeed.beginSeedLaser(mysetting.laserLocalPower,mysetting.laserPulseEnergy);
}

void DevicesControl::closeLaser()
{
    LaserPulse.closePulseLaser();
}

void DevicesControl::checkMove()
{
    Motor.checkMove();      //检查电机转动状态
}

// 罗盘读好了，电机转到位了，激光器也都打开了
// 在这里，完成所有的准备工作
void DevicesControl::pulse_laser_opened_fcn()
{
    qDebug() << "pulse laser open success!!";
    capture_counter = 0;        // 探测方向计数器置零

    stop_now = false;
    if( mysetting.detectMode == 2) {
        Start_Time = QDateTime::currentDateTimeUtc();
    }
    State = WaitMotor;          //貌似可以直接开始采集了吧？？不用wait了吧？
    Generate_freqAxis();
    SaveSpec_FileHead();        //建立新文件，写入文件头
    SaveVelo_FileHead();        //建立新文件，写入文件头

    Init_Buffers();
    azimuthAngle = VectorXd::Zero(mysetting.nDirsVectorCal);
    losVelocityMat = MatrixXd::Zero(nRB_ovlp, mysetting.nDirsVectorCal);
    ControlTimer->start(stateCheckPeriod);
}

void DevicesControl::On_ControlTimer_TimeOut()
{
    QDateTime currentTime;
    qint64 dt;
    if (stop_now) {
        State = Stop;
        stop_now = false;
    }
    switch (State) {
    case WaitMotor:    //****查询电机状态，没到位，则直接返回，等下次进入定时器
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
            Motor.moveRelative(mysetting.azAngleStep);      //-------相对转动电机（step_azAngle为0也可以调用函数，不转就可以了）
        readyToCollect = false;
        LOSVelocityCal(nRB_ovlp+2, nFFT_half,
                       20, mysetting.laserWaveLength,
                       freqAxis, adq.get_PSD_double());     //径向风速计算

        //****矢量风速合成
        qDebug() << "start vector velocity";
        VectorXd tempAzAngle = azimuthAngle.tail(mysetting.nDirsVectorCal-1);
        azimuthAngle.head(mysetting.nDirsVectorCal-1) = tempAzAngle;
        azimuthAngle(mysetting.nDirsVectorCal-1) = currentMotorAngle;

        for (int i=0; i<nRB_ovlp; i++) {
            for (uint j=1; j<mysetting.nDirsVectorCal; j++) {
                losVelocityMat(i,j-1) = losVelocityMat(i,j);
            }
        }
        for (int i=0; i<nRB_ovlp; i++) {
            losVelocityMat(i,mysetting.nDirsVectorCal-1) = losVelocity[i];
        }

        if (capture_counter > mysetting.nDirsVectorCal) {
            if(method==1){
                //滤波正弦波拟合
                double *azAngle = new double[azimuthAngle.rows()];
                for (int i = 0; i < azimuthAngle.rows(); i++) {
                    azAngle[i] = azimuthAngle(i);
                }
                double sigama2 = 4;

                if (hVelocity != NULL) {
                    delete [] hVelocity;
                }
                if (hAngle != NULL) {
                    delete [] hAngle;
                }
                if (vVelocity != NULL) {
                    delete [] vVelocity;
                }
                hVelocity = new double [nRB_ovlp];
                hAngle = new double [nRB_ovlp];
                vVelocity = new double [nRB_ovlp];

                for(int i=0;i<nRB_ovlp;i++)
                {
                    double *losVelocityPerHeight=new double [mysetting.nDirsVectorCal];
                    for(int j=0;j<mysetting.nDirsVectorCal;j++)
                    {
                        losVelocityPerHeight[j]=losVelocityMat(i,j);
                    }
                    vectorVelocity = fswf.getVelocity(mysetting.nDirsVectorCal,sigama2,mysetting.elevationAngle,azAngle,losVelocityPerHeight);
                    hVelocity[i] = qSqrt(*(vectorVelocity+1)**(vectorVelocity+1) +
                                       *(vectorVelocity+2)**(vectorVelocity+2));
                    hAngle[i] = 0.0-qRadiansToDegrees(qAtan2(*(vectorVelocity+2), *(vectorVelocity+1)));
                    if(hAngle[i] < 0) {
                        hAngle[i] = hAngle[i] + 360.0;
                    }
                    vVelocity[i] = *vectorVelocity;
//                    delete []vectorVelocity;
                    delete []losVelocityPerHeight;
                }
                delete []azAngle;
            }
            else{
                std::cout << "azimuthAngle = " << azimuthAngle << std::endl;
                DSWF dswf(mysetting.elevationAngle,azimuthAngle,mysetting.nDirsVectorCal,nRB_ovlp,losVelocityMat);
                hVelocity = dswf.getHVelocity();
                hAngle = dswf.getHAngle();
                vVelocity = dswf.getVVelocity();
            }

            SaveVelo_AddData();     // 存储矢量风速到文件
            //更新显示
            emit hVelocityReady(hVelocity);
            emit hAngleReady(hAngle);
            emit vVelocityReady(vVelocity);
        }


        SaveSpec_AddData();         //存储功率谱到文件

        capture_counter++;
        qDebug() << "capture_counter = " << capture_counter;
        //****判断是否应该结束，更新结束标志stop_now
        switch (mysetting.detectMode) {
        case 0:                 //单组探测
            //判断探测方向数
            if(capture_counter == mysetting.angleNum)
                State = Stop;
            else
                State = WaitMotor;
            break;
        case 1:                 //持续探测
            //判断是否应该关闭文件，建立新文件
            if(capture_counter == mysetting.nDirsPerFile)
            {
                SaveSpec_FileHead();        //建立新文件，写入文件头
            }
            State = WaitMotor;
            break;

        case 2:                 //定时探测
            // 判断否达到待机条件
            dt = currentTime.msecsTo(Start_Time);
            if(dt > qint64(mysetting.groupTime*60*1000))  // *60s*1000ms 如果达到待机时间
                State = Standby;
            else
                State = WaitMotor;
            break;
        default:
            break;
        }
        break;
    }

    case Stop:
//        ControlTimer->stop();
//        LaserPulse.closePulseLaser();  //-------关闭激光放大器,关闭激光器本振
        Motor.motorQuit();
//        State = Quit;
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
                State = WaitMotor;
            }
            else
                State = Capture;
        }
        break;

    case Quit:
        break;

    default:
        break;
    }
}

void DevicesControl::timeStart()
{
    motorCheckTimer->start(motorCheckPeriod);
}

// 生成频率坐标轴，用于计算径向风速，并会被写入到功率谱的记录文件
void DevicesControl::Generate_freqAxis()
{
    for (int i = 0; i < nFFT_half; i++)  {
        freqAxis[i] = mysetting.sampleFreq/2.0*(i+1)/nFFT_half;
        //        qDebug() << freqAxis[i];
    }
}

// 计算径向风速
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

    int startIndex = aomIndex - objSpecPoints;  // 目标风速区间起点
    int endIndex = aomIndex + objSpecPoints;    // 目标风速区间终点

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
        losVelocity[i] = -(freqAxis[losVelocityIndex[i]] - freqAxis[aomIndex])*lambda/2;
    }
}

// 初始化各个数据缓冲区，申请内存空间
void DevicesControl::Init_Buffers()
{
    losVelocity = new double[nRB_ovlp];
    aomSpec = new double[nFFT_half];
    specArray = new double[nRB_ovlp * nFFT_half];

    adq.Init_Buffers();
}

// 保存矢量风速反演结果——文件头
void DevicesControl::SaveVelo_FileHead()
{
    Create_DataFolder();
    VeloFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    //    VeloFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").Velo";
    VeloFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+ VeloFileName +").Velo";
    //    qDebug()<<"VeloFileName = "<<VeloFileName;
    QFile outputVelo(VeloFileName);

    if(outputVelo.open(QFile::WriteOnly | QIODevice::Truncate|QIODevice::Text))//QIODevice::Truncate表示将原文件内容清空
    {
        QTextStream VeloFile(&outputVelo);
        VeloFile << "Knapsack Coherent Doppler Lidar Velocity Vector" << endl;
        for(int i=0;i<60;i++)
            VeloFile << "=";                                // =分隔符
        VeloFile << endl<< "Data File Version: 1.0" << endl;          // 文件类型版本
        VeloFile << "Global Definitions:" << endl;

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        //        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        VeloFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //激光参数
        VeloFile << "LaserMode:        ";
        if(1)
        {
            VeloFile << "Pulse" << endl;		//脉冲探测（true）or连续探测（false） bool
            VeloFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;    //激光能量，单位μJ，连续模式下为0
        }
        else
        {
            VeloFile << "Continuous" << endl;	//脉冲探测（true）or连续探测（false） bool
            VeloFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl; //激光功率，单位mW，脉冲模式下为0
        }
        VeloFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;			//激光频率
        VeloFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;     //脉冲宽度
        VeloFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;     //激光波长
        VeloFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;        //AOM移频量

        //扫描参数
        VeloFile << "detectMode:       " ;      //探测方式：0持续探测1单组探测2定时探测
        switch (mysetting.detectMode) {
        case 1:                 //持续探测
            VeloFile << "NonStop" << endl;
            break;
        case 0:                 //单组探测
            VeloFile << "SingleGroup" << endl;
            break;
        case 2:                 //定时探测
            VeloFile << "scheduled" << endl;
            break;
        }

        VeloFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//俯仰角
        VeloFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//起始角
        VeloFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;	    //步进角
        VeloFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//方向数
        VeloFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//定时探测间隔，单位：分钟
        VeloFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//定时探测单组时间，单位：分钟

        //采样参数
        VeloFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;          //采样频率
        VeloFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;        //触发电平
        VeloFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;   //预触发点数，保留，暂不提供设置

        //实时处理参数
        VeloFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;          //单方向累加脉冲数
        VeloFile << "nRangeBin:        " << QString::number(nRB_ovlp) << endl;                      //距离门数
        VeloFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;       //距离门内点数

        VeloFile << "Height Axis:      ";
        QString str= "";
        CalHeightsValues();
        for(int i=0;i<nRB_ovlp;i++)
            str = str + QString::number(Height_values[i],'f', 2) + " ";
        VeloFile << str << endl;
        for(int i=0;i<60;i++)
            VeloFile << "=";                    // =分隔符
        VeloFile << endl;
        outputVelo.close();
        //        qDebug() << "Velofile Header added!";
    }
}

//void DevicesControl::quitControlTimer()
//{
//    ControlTimer->stop();
//}

//void DevicesControl::quitLaser()
//{
//    LaserPulse.closePulseLaser();  // 关闭激光放大器，随后关闭激光器本振
//}

// 保存功率谱计算结果——文件头
void DevicesControl::SaveSpec_FileHead()
{
    Create_DataFolder();
    SpecFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    //    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").spec";
    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+ SpecFileName +").spec";
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
            specFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;    //激光能量，单位μJ，连续模式下为0
        }
        else
        {
            specFile << "Continuous" << endl;	//脉冲探测（true）or连续探测（false） bool
            specFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl;     //激光功率，单位mW，脉冲模式下为0
        }
        specFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;                //激光频率
        specFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;         //脉冲宽度
        specFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;         //激光波长
        specFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;            //AOM移频量

        //扫描参数
        specFile << "detectMode:       " ;      //探测方式：0持续探测1单组探测2定时探测
        switch (mysetting.detectMode) {
        case 0:                 //单组探测
            specFile << "SingleGroup" << endl;
            break;
        case 1:                 //持续探测
            specFile << "NonStop" << endl;
            break;
        case 2:                 //定时探测
            specFile << "scheduled" << endl;
            break;
        }

        specFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//俯仰角
        specFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//起始角
        specFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;     //步进角
        specFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//方向数
        specFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//定时探测间隔，单位：分钟
        specFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//定时探测单组时间，单位：分钟

        //采样参数
        specFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;          //采样频率
        specFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;        //触发电平
        specFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;   //预触发点数，保留，暂不提供设置

        //实时处理参数
        specFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;          //单方向累加脉冲数
        specFile << "nRangeBin:        " << QString::number(nRB_ovlp) << endl;                      //距离门数
        specFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;       //距离门内点数

        specFile << "Frequency Axis:   ";
        QString str = "";
        for(int i=0;i<nFFT_half;i++)
            str = str + QString::number(freqAxis[i],'f', 2) + " ";
        specFile << str << endl;

        specFile << "Height Axis:      ";
        CalHeightsValues();
        str = "";
        for(int i=0;i<nRB_ovlp;i++)
            str = str + QString::number(Height_values[i],'f', 2) + " ";
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
        quint64 CTms = CaptureTime.toMSecsSinceEpoch();
        specFile.writeRawData((char*)&CTms,sizeof(quint64));
        specFile.writeRawData((char*)&currentMotorAngle,sizeof(double));
        specFile.writeRawData((char*)adq.get_PSD_Union(), (nRB_ovlp+2) * nFFT_half*sizeof(quint64));
        outputSpec.close();
    }
}

void DevicesControl::SaveVelo_AddData()
{
    QFile outputVelo(VeloFileName);
    if(outputVelo.open(QFile::WriteOnly|QIODevice::Append))       //追加
    {
        QDataStream veloFile(&outputVelo);
        quint64 CTms = CaptureTime.toMSecsSinceEpoch();
        veloFile.writeRawData((char*)&CTms,sizeof(quint64));
        veloFile.writeRawData((char*)&currentMotorAngle,sizeof(double));

        veloFile.writeRawData((char*)hVelocity, nRB_ovlp * sizeof(double));
        veloFile.writeRawData((char*)hAngle, nRB_ovlp * sizeof(double));
        veloFile.writeRawData((char*)vVelocity, nRB_ovlp * sizeof(double));
        outputVelo.close();
    }
}

void DevicesControl::Create_DataFolder()
{
    QDir mypath;
    if(!mypath.exists(mysetting.dataFilePath))		//如果文件夹不存在，创建文件夹
        mypath.mkpath(mysetting.dataFilePath);
}

// 计算各个高度层的高度值，用于写入到结果文件
void DevicesControl::CalHeightsValues()
{
    //垂直向 最小探测距离 minDetectRange
    double resol = lightSpeed/mysetting.sampleFreq/1000000/2;        //单采样点的径向分辨率
    double minDetectRange = resol*(mysetting.nPointsMirrorWidth+mysetting.nPointsPerBin/2);
    minDetectRange = minDetectRange*qSin(qDegreesToRadians(mysetting.elevationAngle));

    //垂直向 距离分辨率 rangeResol
    double rangeResol = resol*(mysetting.nPointsPerBin*(1-mysetting.overlapRatio));
    rangeResol = rangeResol*qSin(qDegreesToRadians(mysetting.elevationAngle));

    for(int i=0;i<nRB_ovlp;i++)
        Height_values[i] = minDetectRange + i*rangeResol;
}
