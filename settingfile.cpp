#include "settingfile.h"

settingfile::settingfile()
{

}

void settingfile::init_fsetting(const ACQSETTING &setting)
{
    fsetting = setting;
}

void settingfile::writeTo_file(const ACQSETTING &setting,const QString &a)      //写入文件
{
    fsetting = setting;
    QString path_a = a;

    QSettings settings(path_a,QSettings::IniFormat);
    settings.beginGroup("Laser_parameters");
    settings.setValue("laserMode",fsetting.isPulseMode);				//激光类型
    settings.setValue("laserPulseEnergy",fsetting.laserPulseEnergy);	//激光能量
    settings.setValue("laserPower",fsetting.laserPower);				//激光功率
    settings.setValue("laserRPF",fsetting.laserRPF);					//激光重频
    settings.setValue("laserPulseWidth",fsetting.laserPulseWidth);		//激光脉宽
    settings.setValue("laserWaveLength",fsetting.laserWaveLength);		//激光波长
    settings.setValue("AOM_Freq",fsetting.AOM_Freq);					//AOM移频量
    settings.endGroup();

    settings.beginGroup("Scan_parameters");
    settings.setValue("elevationAngle",fsetting.elevationAngle);		//俯仰角
    settings.setValue("start_azAngle",fsetting.start_azAngle);			//起始角
    settings.setValue("step_azAngle",fsetting.step_azAngle);			//步进角
    settings.setValue("angleNum",fsetting.angleNum);					//方向数
    settings.setValue("circleNum",fsetting.circleNum);					//圆周数
    settings.setValue("anglekey",fsetting.anglekey);					//方向数
    settings.setValue("circlekey",fsetting.circlekey);					//圆周数
    settings.setValue("detectMode",fsetting.detectMode);                //探测模式：0持续探测1单组探测2定时探测
    settings.setValue("SP",fsetting.SP);								//电机速度
    settings.setValue("IntervalTime",fsetting.IntervalTime);            //分组间隔
    settings.setValue("GroupTime",fsetting.GroupTime);                  //每组探测时间
    settings.endGroup();

    settings.beginGroup("Sample_parameters");
    settings.setValue("sampleFreq",fsetting.sampleFreq);				//采样频率
    settings.setValue("Trigger_Level",fsetting.Trigger_Level);          //触发电平
    settings.setValue("PreTrigger",fsetting.PreTrigger);                //预触发点数，保留，暂不提供设置
    settings.endGroup();

    settings.beginGroup("RealTime_Process");
    settings.setValue("plsAccNum",fsetting.plsAccNum);					//单方向累加脉冲数
    settings.setValue("nRangeBin",fsetting.nRangeBin);                  //距离门数
    settings.setValue("nPointsPerBin",fsetting.nPointsPerBin);          //距离门内点数
    settings.setValue("velocity_band",fsetting.velocity_band);          //径向风速范围
    settings.setValue("objFreqPoints",fsetting.objFreqPoints);          //径向风速点数范围
    settings.setValue("nDir_VectorCal",fsetting.nDir_VectorCal);        //矢量风速所需的径向风速数量
    settings.endGroup();

    settings.beginGroup("File_store");
    settings.setValue("DatafilePath",fsetting.DatafilePath);					//文件保存路径
    settings.setValue("autoCreate_DateDir",fsetting.autoCreate_DateDir);		//自动创建日期文件夹
    settings.setValue("nMaxDir_infile",fsetting.nMaxDir_inFile);
    settings.endGroup();

}

void settingfile::readFrom_file(const QString &b)
{
    QString path_b = b;
    QSettings settings(path_b,QSettings::IniFormat);
    settings.beginGroup("Laser_parameters");
    fsetting.isPulseMode = settings.value("isPulseMode").toInt();             //激光类型
    fsetting.laserPulseEnergy = settings.value("laserPulseEnergy").toDouble();   //激光能量
    fsetting.laserPower = settings.value("laserPower").toDouble();               //激光功率
    fsetting.laserRPF = settings.value("laserRPF").toDouble();                   //激光重频
    fsetting.laserPulseWidth = settings.value("laserPulseWidth").toDouble();     //激光脉宽
    fsetting.laserWaveLength = settings.value("laserWaveLength").toDouble();     //激光波长
    fsetting.AOM_Freq = settings.value("AOM_Freq").toDouble();                   //AOM移频量
    settings.endGroup();

    settings.beginGroup("Scan_parameters");
    fsetting.detectMode = settings.value("detectMode").toInt();                 //探测模式
    fsetting.elevationAngle = settings.value("elevationAngle").toDouble();		//俯仰角
    fsetting.start_azAngle = settings.value("start_azAngle").toDouble();		//起始角
    fsetting.step_azAngle = settings.value("step_azAngle").toDouble();			//步进角
    fsetting.angleNum = settings.value("angleNum").toDouble();					//方向数
    fsetting.circleNum = settings.value("circleNum").toDouble();				//圆周数
    fsetting.anglekey = settings.value("anglekey").toBool();				    //方向键
    fsetting.circlekey = settings.value("circlekey").toBool();				    //圆周键
    fsetting.SP = settings.value("SP").toInt();								    //电机速度
    fsetting.IntervalTime = settings.value("IntervalTime").toDouble();          //方向间间隔
    fsetting.GroupTime = settings.value("GroupTime").toDouble();                //圆周间间隔
    settings.endGroup();

    settings.beginGroup("Sample_parameters");
    fsetting.sampleFreq = settings.value("sampleFreq").toDouble();				//采样频率
    fsetting.Trigger_Level = settings.value("Trigger_Level").toDouble();        //触发电平
    fsetting.PreTrigger = settings.value("PreTrigger").toInt();                 //预触发点数，保留，暂不提供设置
    settings.endGroup();

    settings.beginGroup("RealTime_Process");
    fsetting.plsAccNum = settings.value("plsAccNum").toInt();				//单方向累加脉冲数
    fsetting.nRangeBin = settings.value("nRangeBin").toInt();               //距离门数
    fsetting.nPointsPerBin = settings.value("nPointsPerBin").toInt();       //距离门内点数
    fsetting.velocity_band = settings.value("velocity_band").toDouble();    //径向风速范围
    fsetting.objFreqPoints = settings.value("objFreqPoints").toInt();       //径向风速点数范围
    fsetting.nDir_VectorCal = settings.value("nDir_VectorCal").toInt();     //矢量风速所需的径向风速数量
    settings.endGroup();

    settings.beginGroup("File_store");
    fsetting.DatafilePath = settings.value("DatafilePath").toString();					//文件保存路径
    fsetting.autoCreate_DateDir = settings.value("autoCreate_DateDir").toBool();		//自动创建最小文件夹
    fsetting.nMaxDir_inFile = settings.value("nMaxDir_inFile").toInt();                 //单文件方向数
    settings.endGroup();
}

ACQSETTING settingfile::get_settings()
{
    return fsetting;
}

void settingfile::checkValid()
{

}

void settingfile::test_create_file(const QString &iniFilePath)
{
    QString path_c = iniFilePath;
    QString prefix_str = QDateTime::currentDateTime().toString("yyyyMMdd");				//获取最新日期
    QFileInfo file(path_c);
    QSettings settings(path_c,QSettings::IniFormat);
    if(file.exists() == false)                              //以下为配置文件不存在时的默认配置
    {
        settings.beginGroup("Laser_parameters");
        settings.setValue("isPulseMode",0);				    //激光模式:0脉冲模式1连续模式
        settings.setValue("laserPulseEnergy",10);			//激光能量，单位μJ
        settings.setValue("laserPower",100);				//激光功率，单位mW
        settings.setValue("laserRPF",10000);				//激光重频
        settings.setValue("laserPulseWidth",500);			//激光脉宽
        settings.setValue("laserWaveLength",1.55);			//激光波长
        settings.setValue("AOM_Freq",120);					//AOM移频量
        settings.endGroup();

        settings.beginGroup("Scan_parameters");
        settings.setValue("detectMode",1);                  //探测方式：0持续探测1单组探测2定时探测
        settings.setValue("elevationAngle",70);				//俯仰角
        settings.setValue("start_azAngle",0);				//起始角
        settings.setValue("step_azAngle",0);				//步进角
        settings.setValue("angleNum",80);					//方向数
        settings.setValue("circleNum",20);					//圆周数
        settings.setValue("anglekey",true);			        //方向键
        settings.setValue("circlekey",false);				//圆周键
        settings.setValue("SP",90);							//电机速度
        settings.setValue("IntervalTime",15);               //定时探测间隔，单位：分钟
        settings.setValue("GroupTime",3);                   //定时探测单组时间，单位：分钟
        settings.endGroup();

        settings.beginGroup("Sample_parameters");
        settings.setValue("sampleFreq",400);				//采样频率
        settings.setValue("Trigger_Level",2000);            //触发电平
        settings.setValue("PreTrigger",500);                //预触发点数，保留，暂不提供设置
        settings.endGroup();

        settings.beginGroup("RealTime_Process");
        settings.setValue("plsAccNum",5000);				//单方向累加脉冲数
        settings.setValue("nRangeBin",13);                  //距离门数
        settings.setValue("nPointsPerBin",250);             //距离门内点数
        settings.setValue("velocity_band",20);
        settings.setValue("objFreqPoints",40);
        settings.setValue("nDir_VectorCal",4);
        settings.endGroup();

        settings.beginGroup("File_store");
        path_c.chop(16);									//截掉末尾配置文件名
        path_c.append("/").append(prefix_str);              //路径末尾加上日期文件夹？？？？
        settings.setValue("DatafilePath",path_c);			//文件保存路径
        settings.setValue("autoCreate_DateDir",true);		//自动创建日期文件夹
        settings.setValue("nMaxDir_inFile",1000);           //单文件方向数
        settings.endGroup();
    }
    else
    {
        path_c.chop(16);									//截掉末尾配置文件名
        path_c.append("/").append(prefix_str);				//路径末尾加上日期文件夹
        settings.beginGroup("File_store");
        settings.setValue("dataFileName_Prefix",prefix_str);//前缀文件名
        settings.endGroup();
        qDebug() <<"Settings file exist";
    }

    LF_path = path_c;
    LF_path.append(".log");
    QFile LogFile(LF_path);
    if(LogFile.exists() == false)
    {
        QString cteate_time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        cteate_time.append(QString::fromLocal8Bit("创建记录文件"));
        LogFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);   //使用QTextStream向文件输出换行，需要使用QIODevice::Text标志
        QTextStream record_str(&LogFile);
        record_str << cteate_time << endl;
        LogFile.close();
    }

}

bool settingfile::isSettingsChanged(const ACQSETTING &setting)
{
    ACQSETTING dlgsetting = setting;
    if(fsetting.isPulseMode != dlgsetting.isPulseMode)              //激光类型
        return true;
    if(fsetting.laserPulseEnergy != dlgsetting.laserPulseEnergy)    //激光能量
        return true;
    if(fsetting.laserPower != dlgsetting.laserPower)                //激光功率
        return true;
    if(fsetting.laserRPF != dlgsetting.laserRPF)					//激光重频
        return true;
    if(fsetting.laserPulseWidth != dlgsetting.laserPulseWidth)		//脉冲宽度
        return true;
    if(fsetting.laserWaveLength != dlgsetting.laserWaveLength)		//激光波长
        return true;
    if(fsetting.AOM_Freq != dlgsetting.AOM_Freq)					//AOM移频量
        return true;

    if(fsetting.detectMode != dlgsetting.detectMode)
        return true;                                                //探测方式
    if(fsetting.elevationAngle != dlgsetting.elevationAngle)		//俯仰角
        return true;
    if(fsetting.start_azAngle != dlgsetting.start_azAngle)			//起始角
        return true;
    if(fsetting.step_azAngle != dlgsetting.step_azAngle)			//步进角
        return true;
    if(fsetting.angleNum != dlgsetting.angleNum)					//方向数
        return true;
    if(fsetting.circleNum != dlgsetting.circleNum)					//圆周数
        return true;
    if(fsetting.anglekey != dlgsetting.anglekey)
        return true;
    if(fsetting.circlekey != dlgsetting.circlekey)
        return true;
    if(fsetting.SP != dlgsetting.SP)								//电机速度
        return true;
    if(fsetting.IntervalTime != dlgsetting.IntervalTime)
        return true;
    if(fsetting.GroupTime != dlgsetting.GroupTime)
        return true;

    if(fsetting.sampleFreq != dlgsetting.sampleFreq)				//采样频率
        return true;
    if(fsetting.Trigger_Level != dlgsetting.Trigger_Level)          //触发电平
        return true;
    if(fsetting.PreTrigger !=dlgsetting.PreTrigger)                 //预触发点数
        return true;

    if(fsetting.plsAccNum != dlgsetting.plsAccNum)                  //单方向累加脉冲数
        return true;
    if(fsetting.nRangeBin !=dlgsetting.nRangeBin)                   //距离门数
        return true;
    if(fsetting.nPointsPerBin != dlgsetting.nPointsPerBin)          //距离门内点数
        return true;
    if(fsetting.velocity_band != dlgsetting.velocity_band)          //径向风速范围
        return true;
    if(fsetting.objFreqPoints != dlgsetting.objFreqPoints)
        return true;
    if(fsetting.nDir_VectorCal != dlgsetting.nDir_VectorCal)        //矢量风速所需的径向风速数量
        return true;

    if(fsetting.DatafilePath != dlgsetting.DatafilePath)			  //文件保存路径
        return true;
    if(fsetting.autoCreate_DateDir != dlgsetting.autoCreate_DateDir)  //自动创建日期文件夹
        return true;
    if(fsetting.nMaxDir_inFile != dlgsetting.nMaxDir_inFile)          //单文件方向数
        return true;

    return false;

}

void settingfile::updatelogFile(const QString &addInstruct)
{
    instruct_str = addInstruct;
    qDebug() << "instruct_str = " << instruct_str;
    //采集文字说明
    QFile last_LF(LF_path);
    QString last_time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    last_time.append(QString::fromLocal8Bit("start collecting"));
    last_LF.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream record_str(&last_LF);
    record_str << last_time << endl;

    if(instruct_str != NULL)
        record_str << instruct_str << endl;
    last_LF.close();

}


