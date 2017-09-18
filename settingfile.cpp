#include "settingfile.h"

settingfile::settingfile()
{

}

void settingfile::init_fsetting(const ACQSETTING &setting)
{
    fsetting = setting;
}

void settingfile::writeTo_file(const ACQSETTING &setting,const QString &a)      //д���ļ�
{
    fsetting = setting;
    QString path_a = a;

    QSettings settings(path_a,QSettings::IniFormat);
    settings.beginGroup("Laser_parameters");
    settings.setValue("laserMode",fsetting.isPulseMode);				//��������
    settings.setValue("laserPulseEnergy",fsetting.laserPulseEnergy);	//��������
    settings.setValue("laserPower",fsetting.laserPower);				//���⹦��
    settings.setValue("laserRPF",fsetting.laserRPF);					//������Ƶ
    settings.setValue("laserPulseWidth",fsetting.laserPulseWidth);		//��������
    settings.setValue("laserWaveLength",fsetting.laserWaveLength);		//���Ⲩ��
    settings.setValue("AOM_Freq",fsetting.AOM_Freq);					//AOM��Ƶ��
    settings.endGroup();

    settings.beginGroup("Scan_parameters");
    settings.setValue("elevationAngle",fsetting.elevationAngle);		//������
    settings.setValue("start_azAngle",fsetting.start_azAngle);			//��ʼ��
    settings.setValue("step_azAngle",fsetting.step_azAngle);			//������
    settings.setValue("angleNum",fsetting.angleNum);					//������
    settings.setValue("circleNum",fsetting.circleNum);					//Բ����
    settings.setValue("anglekey",fsetting.anglekey);					//������
    settings.setValue("circlekey",fsetting.circlekey);					//Բ����
    settings.setValue("detectMode",fsetting.detectMode);                //̽��ģʽ
    settings.setValue("SP",fsetting.SP);								//����ٶ�
    settings.setValue("IntervalTime",fsetting.IntervalTime);            //������
    settings.setValue("GroupTime",fsetting.GroupTime);                  //ÿ��̽��ʱ��
    settings.endGroup();

    settings.beginGroup("Sample_parameters");
    settings.setValue("sampleFreq",fsetting.sampleFreq);				//����Ƶ��
    settings.setValue("detRange",fsetting.detRange);					//̽�����
    settings.setValue("sampleNum",fsetting.sampleNum);                  //��������
    settings.setValue("plsAccNum",fsetting.plsAccNum);					//������
    settings.endGroup();

    settings.beginGroup("File_store");
    settings.setValue("DatafilePath",fsetting.DatafilePath);					//�ļ�����·��
    settings.setValue("autoCreate_DateDir",fsetting.autoCreate_DateDir);		//�Զ����������ļ���
    settings.endGroup();

}

void settingfile::readFrom_file(const QString &b)
{
    QString path_b = b;
    QSettings settings(path_b,QSettings::IniFormat);
    settings.beginGroup("Laser_parameters");
    fsetting.laserRPF = settings.value("laserRPF").toInt();
    fsetting.laserPulseWidth = settings.value("laserPulseWidth").toInt();
    fsetting.laserWaveLength = settings.value("laserWaveLength").toInt();
    fsetting.AOM_Freq = settings.value("AOM_Freq").toInt();
    settings.endGroup();

    settings.beginGroup("Scan_parameters");
    fsetting.detectMode = settings.value("detectMode").toBool();            //̽��ģʽ
    fsetting.elevationAngle = settings.value("elevationAngle").toInt();		//������
    fsetting.start_azAngle = settings.value("start_azAngle").toInt();		//��ʼ��
    fsetting.step_azAngle = settings.value("step_azAngle").toInt();			//������
    fsetting.angleNum = settings.value("angleNum").toInt();					//������
    fsetting.circleNum = settings.value("circleNum").toFloat();				//Բ����
    fsetting.anglekey = settings.value("anglekey").toBool();				//�����
    fsetting.circlekey = settings.value("circlekey").toBool();				//Բ�ܼ�
    fsetting.SP = settings.value("SP").toInt();								//����ٶ�
    fsetting.IntervalTime = settings.value("IntervalTime").toFloat();       //�������
    fsetting.GroupTime = settings.value("GroupTime").toFloat();             //Բ�ܼ���
    settings.endGroup();

    settings.beginGroup("Sample_parameters");
    fsetting.sampleFreq = settings.value("sampleFreq").toInt();				//����Ƶ��
    fsetting.detRange = settings.value("detRange").toFloat();				//̽�����
    fsetting.sampleNum = settings.value("sampleNum").toInt();				//��������
    fsetting.plsAccNum = settings.value("plsAccNum").toInt();				//������
    settings.endGroup();

    settings.beginGroup("File_store");
    fsetting.DatafilePath = settings.value("DatafilePath").toString();					//�ļ�����·��
    fsetting.autoCreate_DateDir = settings.value("autoCreate_DateDir").toBool();		//�Զ�������С�ļ���
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
    QString prefix_str = QDateTime::currentDateTime().toString("yyyyMMdd");				//��ȡ��������
    QFileInfo file(path_c);                                               //2?
    QSettings settings(path_c,QSettings::IniFormat);
    if(file.exists() == false)                              //����Ϊ�����ļ�������ʱ��Ĭ������
    {
        settings.beginGroup("Laser_parameters");
        settings.setValue("isPulseMode",true);				//����ģʽ
        settings.setValue("laserPulseEnergy",10);			//������������λ��J
        settings.setValue("laserPower",100);				//���⹦�ʣ���λmW
        settings.setValue("laserRPF",10000);				//������Ƶ
        settings.setValue("laserPulseWidth",500);			//��������
        settings.setValue("laserWaveLength",1540);			//���Ⲩ��
        settings.setValue("AOM_Freq",120);					//AOM��Ƶ��
        settings.endGroup();

        settings.beginGroup("Scan_parameters");
        settings.setValue("detectMode",1);                  //̽�ⷽʽ��0����̽��1����̽��2��ʱ̽��
        settings.setValue("elevationAngle",70);				//������
        settings.setValue("start_azAngle",0);				//��ʼ��
        settings.setValue("step_azAngle",90);				//������
        settings.setValue("angleNum",80);					//������
        settings.setValue("circleNum",20);					//Բ����
        settings.setValue("anglekey",false);			    //�����
        settings.setValue("circlekey",false);				//Բ�ܼ�
        settings.setValue("continusdete",true);             //����̽��
        settings.setValue("SP",90);							//����ٶ�
        settings.setValue("IntervalTime",15);               //��ʱ̽��������λ������
        settings.setValue("GroupTime",3);                   //��ʱ̽�ⵥ��ʱ�䣬��λ������
        settings.endGroup();

        settings.beginGroup("Sample_parameters");
        settings.setValue("sampleFreq",400);				//����Ƶ��
        settings.setValue("Trigger_Level",2000);             //������ƽ
        settings.setValue("PreTrigger",500);                //Ԥ�����������������ݲ��ṩ����
        settings.endGroup();

        settings.beginGroup("RealTime_Process");

        settings.setValue("plsAccNum",5000);				//�������ۼ�������
        settings.setValue("nRangeBin",13);                  //��������
        settings.setValue("nPointsPerBin",250);             //�������ڵ���
        settings.endGroup();

        settings.beginGroup("File_store");
        path_c.chop(16);									//�ص�ĩβ�����ļ���
        path_c.append("/").append(prefix_str);              //·��ĩβ���������ļ��У�������
        settings.setValue("DatafilePath",path_c);			//�ļ�����·��
        settings.setValue("autoCreate_DateDir",true);		//�Զ����������ļ���
        settings.endGroup();
    }
    else
    {
        path_c.chop(16);									//�ص�ĩβ�����ļ���
        path_c.append("/").append(prefix_str);				//·��ĩβ���������ļ���
        settings.beginGroup("File_store");
        settings.setValue("dataFileName_Prefix",prefix_str);//ǰ׺�ļ���
        settings.endGroup();
        qDebug() <<"Settings file exist";
    }

    LF_path = path_c;
    LF_path.append(".log");
    QFile LogFile(LF_path);
    if(LogFile.exists() == false)
    {
        QString cteate_time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        cteate_time.append(QString::fromLocal8Bit("������¼�ļ�"));
        LogFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);   //ʹ��QTextStream���ļ�������У���Ҫʹ��QIODevice::Text��־
        QTextStream record_str(&LogFile);
        record_str << cteate_time << endl;
        LogFile.close();
    }

}

bool settingfile::isSettingsChanged(const ACQSETTING &setting)
{
    ACQSETTING dlgsetting = setting;
    if(fsetting.laserRPF != dlgsetting.laserRPF)					//������Ƶ
        return true;
    if(fsetting.laserPulseWidth != dlgsetting.laserPulseWidth)		//������
        return true;
    if(fsetting.laserWaveLength != dlgsetting.laserWaveLength)		//���Ⲩ��
        return true;
    if(fsetting.AOM_Freq != dlgsetting.AOM_Freq)					//AOM��Ƶ��
        return true;

    if(fsetting.elevationAngle != dlgsetting.elevationAngle)		//������
        return true;
    if(fsetting.start_azAngle != dlgsetting.start_azAngle)			//��ʼ��
        return true;
    if(fsetting.step_azAngle != dlgsetting.step_azAngle)			//������
        return true;
    if(fsetting.angleNum != dlgsetting.angleNum)					//������
        return true;
    if(fsetting.circleNum != dlgsetting.circleNum)					//Բ����
        return true;
    if(fsetting.anglekey != dlgsetting.anglekey)
        return true;
    if(fsetting.circlekey != dlgsetting.circlekey)
        return true;
    //    if(fsetting.continusdete != dlgsetting.continusdete)            //����̽��
    //        return true;
    if(fsetting.SP != dlgsetting.SP)								//����ٶ�
        return true;
    if(fsetting.IntervalTime != dlgsetting.IntervalTime)
        return true;
    if(fsetting.GroupTime != dlgsetting.GroupTime)
        return true;

    if(fsetting.sampleFreq != dlgsetting.sampleFreq)				//����Ƶ��
        return true;
    if(fsetting.detRange != dlgsetting.detRange)					//̽�����
        return true;
    if(fsetting.sampleNum != dlgsetting.sampleNum)
        return true;
    if(fsetting.plsAccNum != dlgsetting.plsAccNum)
        return true;

    if(fsetting.DatafilePath != dlgsetting.DatafilePath)			  //�ļ�����·��
        return true;
    if(fsetting.autoCreate_DateDir != dlgsetting.autoCreate_DateDir)//�Զ����������ļ���
        return true;

    return false;

}

void settingfile::updatelogFile(const QString &addInstruct)
{
    instruct_str = addInstruct;
    qDebug() << "instruct_str = " << instruct_str;
    //�ɼ�����˵��
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


