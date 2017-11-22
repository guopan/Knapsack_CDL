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
    //������
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
    if(moveNorth)       // �ɼ�֮�����������ָ������ʱ
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
    else                // ÿ�βɼ�������ת����һ����ʱ
    {
        if((s-motorPX0-mysetting.azAngleStep)<=0.01&&(s-motorPX0-mysetting.azAngleStep)>=-0.01)   //�ж��Ƿ񵽴�ָ��λ��,�������0.5��
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
    Motor.checkMove();      //�����ת��״̬
}

// ���̶����ˣ����ת��λ�ˣ�������Ҳ������
// �����������е�׼������
void DevicesControl::pulse_laser_opened_fcn()
{
    qDebug() << "pulse laser open success!!";
    capture_counter = 0;        // ̽�ⷽ�����������

    stop_now = false;
    if( mysetting.detectMode == 2) {
        Start_Time = QDateTime::currentDateTimeUtc();
    }
    State = WaitMotor;          //ò�ƿ���ֱ�ӿ�ʼ�ɼ��˰ɣ�������wait�˰ɣ�
    Generate_freqAxis();
    SaveSpec_FileHead();        //�������ļ���д���ļ�ͷ
    SaveVelo_FileHead();        //�������ļ���д���ļ�ͷ

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
    case WaitMotor:    //****��ѯ���״̬��û��λ����ֱ�ӷ��أ����´ν��붨ʱ��
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
        CaptureTime = QDateTime::currentDateTimeUtc();      //��¼��ǰʱ�䣬����д���ļ�
        if(mysetting.azAngleStep != 0)
            Motor.moveRelative(mysetting.azAngleStep);      //-------���ת�������step_azAngleΪ0Ҳ���Ե��ú�������ת�Ϳ����ˣ�
        readyToCollect = false;
        LOSVelocityCal(nRB_ovlp+2, nFFT_half,
                       20, mysetting.laserWaveLength,
                       freqAxis, adq.get_PSD_double());     //������ټ���

        //****ʸ�����ٺϳ�
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
                //�˲����Ҳ����
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
                        losVelocityPerHeight[j]=losVelocityMat(i,j); //ÿ���߶Ȳ��������ľ������
                    }
                    vectorVelocity = fswf.getVelocity(mysetting.nDirsVectorCal,sigama2,mysetting.elevationAngle,azAngle,losVelocityPerHeight);
                    //���ݷ��صľ���������εõ�ˮƽ���١�ˮƽ���򡢴�ֱ����
                    hVelocity[i] = qSqrt(*(vectorVelocity+1)**(vectorVelocity+1) +
                                       *(vectorVelocity+2)**(vectorVelocity+2));
                    hAngle[i] = 0.0-qRadiansToDegrees(qAtan2(*(vectorVelocity+2), *(vectorVelocity+1)));
                    if(hAngle[i] < 0) {
                        hAngle[i] = hAngle[i] + 360.0;
                    }
                    vVelocity[i] = *vectorVelocity;
                    delete []losVelocityPerHeight;
                }
                delete []azAngle;
            }
            else{
                //ֱ�����Ҳ����
                std::cout << "azimuthAngle = " << azimuthAngle << std::endl;
                DSWF dswf(mysetting.elevationAngle,azimuthAngle,mysetting.nDirsVectorCal,nRB_ovlp,losVelocityMat);
                hVelocity = dswf.getHVelocity();
                hAngle = dswf.getHAngle();
                vVelocity = dswf.getVVelocity();
            }

            SaveVelo_AddData();     // �洢ʸ�����ٵ��ļ�
            //������ʾ
            emit hVelocityReady(hVelocity);
            emit hAngleReady(hAngle);
            emit vVelocityReady(vVelocity);
        }


        SaveSpec_AddData();         //�洢�����׵��ļ�

        capture_counter++;
        qDebug() << "capture_counter = " << capture_counter;
        //****�ж��Ƿ�Ӧ�ý��������½�����־stop_now
        switch (mysetting.detectMode) {
        case 0:                 //����̽��
            //�ж�̽�ⷽ����
            if(capture_counter == mysetting.angleNum)
                State = Stop;
            else
                State = WaitMotor;
            break;
        case 1:                 //����̽��
            //�ж��Ƿ�Ӧ�ùر��ļ����������ļ�
            if(capture_counter == mysetting.nDirsPerFile)
            {
                SaveSpec_FileHead();        //�������ļ���д���ļ�ͷ
            }
            State = WaitMotor;
            break;

        case 2:                 //��ʱ̽��
            // �жϷ�ﵽ��������
            dt = currentTime.msecsTo(Start_Time);
            if(dt > qint64(mysetting.groupTime*60*1000))  // *60s*1000ms ����ﵽ����ʱ��
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
//        LaserPulse.closePulseLaser();  //-------�رռ���Ŵ���,�رռ���������
        Motor.motorQuit();
//        State = Quit;
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

// ����Ƶ�������ᣬ���ڼ��㾶����٣����ᱻд�뵽�����׵ļ�¼�ļ�
void DevicesControl::Generate_freqAxis()
{
    for (int i = 0; i < nFFT_half; i++)  {
        freqAxis[i] = mysetting.sampleFreq/2.0*(i+1)/nFFT_half;
        //        qDebug() << freqAxis[i];
    }
}

// ���㾶�����
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

    int startIndex = aomIndex - objSpecPoints;  // Ŀ������������
    int endIndex = aomIndex + objSpecPoints;    // Ŀ����������յ�

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

// ��ʼ���������ݻ������������ڴ�ռ�
void DevicesControl::Init_Buffers()
{
    losVelocity = new double[nRB_ovlp];
    aomSpec = new double[nFFT_half];
    specArray = new double[nRB_ovlp * nFFT_half];

    adq.Init_Buffers();
}

// ����ʸ�����ٷ��ݽ�������ļ�ͷ
void DevicesControl::SaveVelo_FileHead()
{
    Create_DataFolder();
    VeloFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    //    VeloFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").Velo";
    VeloFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+ VeloFileName +").Velo";
    //    qDebug()<<"VeloFileName = "<<VeloFileName;
    QFile outputVelo(VeloFileName);

    if(outputVelo.open(QFile::WriteOnly | QIODevice::Truncate|QIODevice::Text))//QIODevice::Truncate��ʾ��ԭ�ļ��������
    {
        QTextStream VeloFile(&outputVelo);
        VeloFile << "Knapsack Coherent Doppler Lidar Velocity Vector" << endl;
        for(int i=0;i<60;i++)
            VeloFile << "=";                                // =�ָ���
        VeloFile << endl<< "Data File Version: 1.0" << endl;          // �ļ����Ͱ汾
        VeloFile << "Global Definitions:" << endl;

        QDateTime zero = QDateTime::fromSecsSinceEpoch(0,Qt::UTC);
        //        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        VeloFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //�������
        VeloFile << "LaserMode:        ";
        if(1)
        {
            VeloFile << "Pulse" << endl;		//����̽�⣨true��or����̽�⣨false�� bool
            VeloFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;    //������������λ��J������ģʽ��Ϊ0
        }
        else
        {
            VeloFile << "Continuous" << endl;	//����̽�⣨true��or����̽�⣨false�� bool
            VeloFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl; //���⹦�ʣ���λmW������ģʽ��Ϊ0
        }
        VeloFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;			//����Ƶ��
        VeloFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;     //������
        VeloFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;     //���Ⲩ��
        VeloFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;        //AOM��Ƶ��

        //ɨ�����
        VeloFile << "detectMode:       " ;      //̽�ⷽʽ��0����̽��1����̽��2��ʱ̽��
        switch (mysetting.detectMode) {
        case 1:                 //����̽��
            VeloFile << "NonStop" << endl;
            break;
        case 0:                 //����̽��
            VeloFile << "SingleGroup" << endl;
            break;
        case 2:                 //��ʱ̽��
            VeloFile << "scheduled" << endl;
            break;
        }

        VeloFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//������
        VeloFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//��ʼ��
        VeloFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;	    //������
        VeloFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//������
        VeloFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//��ʱ̽��������λ������
        VeloFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//��ʱ̽�ⵥ��ʱ�䣬��λ������

        //��������
        VeloFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;          //����Ƶ��
        VeloFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;        //������ƽ
        VeloFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;   //Ԥ�����������������ݲ��ṩ����

        //ʵʱ�������
        VeloFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;          //�������ۼ�������
        VeloFile << "nRangeBin:        " << QString::number(nRB_ovlp) << endl;                      //��������
        VeloFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;       //�������ڵ���

        VeloFile << "Height Axis:      ";
        QString str= "";
        CalHeightsValues();
        for(int i=0;i<nRB_ovlp;i++)
            str = str + QString::number(Height_values[i],'f', 2) + " ";
        VeloFile << str << endl;
        for(int i=0;i<60;i++)
            VeloFile << "=";                    // =�ָ���
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
//    LaserPulse.closePulseLaser();  // �رռ���Ŵ��������رռ���������
//}

// ���湦���׼����������ļ�ͷ
void DevicesControl::SaveSpec_FileHead()
{
    Create_DataFolder();
    SpecFileName = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    //    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+").spec";
    SpecFileName = mysetting.dataFilePath+"/"+"KnapsackCDL("+ SpecFileName +").spec";
    //    qDebug()<<"SpecFileName = "<<SpecFileName;
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
        //        qDebug()<< zero.toString("yyyy-MM-dd hh:mm:ss");
        specFile << "Base Time:        " << zero.toString("yyyy-MM-dd hh:mm:ss") << endl;          //1970-01-01 00:00:00

        //�������
        specFile << "LaserMode:        ";
        if(1)
        {
            specFile << "Pulse" << endl;		//����̽�⣨true��or����̽�⣨false�� bool
            specFile << "laserPulseEnergy: " << QString::number(mysetting.laserPulseEnergy) << endl;    //������������λ��J������ģʽ��Ϊ0
        }
        else
        {
            specFile << "Continuous" << endl;	//����̽�⣨true��or����̽�⣨false�� bool
            specFile << "laserPower:       " << QString::number(mysetting.laserLocalPower) << endl;     //���⹦�ʣ���λmW������ģʽ��Ϊ0
        }
        specFile << "laserRPF:         " << QString::number(mysetting.laserRPF) << endl;                //����Ƶ��
        specFile << "laserPulseWidth:  " << QString::number(mysetting.laserPulseWidth) << endl;         //������
        specFile << "laserWaveLength:  " << QString::number(mysetting.laserWaveLength) << endl;         //���Ⲩ��
        specFile << "AOM_Freq:         " << QString::number(mysetting.laserAOMFreq) << endl;            //AOM��Ƶ��

        //ɨ�����
        specFile << "detectMode:       " ;      //̽�ⷽʽ��0����̽��1����̽��2��ʱ̽��
        switch (mysetting.detectMode) {
        case 0:                 //����̽��
            specFile << "SingleGroup" << endl;
            break;
        case 1:                 //����̽��
            specFile << "NonStop" << endl;
            break;
        case 2:                 //��ʱ̽��
            specFile << "scheduled" << endl;
            break;
        }

        specFile << "elevationAngle:   " << QString::number(mysetting.elevationAngle) << endl;	//������
        specFile << "start_azAngle:    " << QString::number(mysetting.azAngleStart) << endl;	//��ʼ��
        specFile << "step_azAngle:     " << QString::number(mysetting.azAngleStep) << endl;     //������
        specFile << "angleNum:         " << QString::number(mysetting.angleNum) << endl;		//������
        specFile << "IntervalTime:     " << QString::number(mysetting.intervalTime) << endl;	//��ʱ̽��������λ������
        specFile << "GroupTime:        " << QString::number(mysetting.groupTime) << endl;		//��ʱ̽�ⵥ��ʱ�䣬��λ������

        //��������
        specFile << "sampleFreq:       " << QString::number(mysetting.sampleFreq) << endl;          //����Ƶ��
        specFile << "Trigger_Level:    " << QString::number(mysetting.triggerLevel) << endl;        //������ƽ
        specFile << "PreTrigger:       " << QString::number(mysetting.nPointsPreTrigger) << endl;   //Ԥ�����������������ݲ��ṩ����

        //ʵʱ�������
        specFile << "plsAccNum:        " << QString::number(mysetting.nPulsesAcc) << endl;          //�������ۼ�������
        specFile << "nRangeBin:        " << QString::number(nRB_ovlp) << endl;                      //��������
        specFile << "nPointsPerBin:    " << QString::number(mysetting.nPointsPerBin) << endl;       //�������ڵ���

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
            specFile << "=";                    // =�ָ���
        specFile << endl;
        outputSpec.close();
        //        qDebug() << "Specfile Header added!";
    }
}

void DevicesControl::SaveSpec_AddData()
{
    QFile outputSpec(SpecFileName);
    if(outputSpec.open(QFile::WriteOnly|QIODevice::Append))       //׷��
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
    if(outputVelo.open(QFile::WriteOnly|QIODevice::Append))       //׷��
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
    if(!mypath.exists(mysetting.dataFilePath))		//����ļ��в����ڣ������ļ���
        mypath.mkpath(mysetting.dataFilePath);
}

// ��������߶Ȳ�ĸ߶�ֵ������д�뵽����ļ�
void DevicesControl::CalHeightsValues()
{
    //��ֱ�� ��С̽����� minDetectRange
    double resol = lightSpeed/mysetting.sampleFreq/1000000/2;        //��������ľ���ֱ���
    double minDetectRange = resol*(mysetting.nPointsMirrorWidth+mysetting.nPointsPerBin/2);
    minDetectRange = minDetectRange*qSin(qDegreesToRadians(mysetting.elevationAngle));

    //��ֱ�� ����ֱ��� rangeResol
    double rangeResol = resol*(mysetting.nPointsPerBin*(1-mysetting.overlapRatio));
    rangeResol = rangeResol*qSin(qDegreesToRadians(mysetting.elevationAngle));

    for(int i=0;i<nRB_ovlp;i++)
        Height_values[i] = minDetectRange + i*rangeResol;
}
