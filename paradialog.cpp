#include "paradialog.h"
#include "ui_paradialog.h"
#include "mainwindow.h"

paraDialog::paraDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::paraDialog)
{
    ui->setupUi(this);
}

paraDialog::~paraDialog()
{
    delete ui;
}

void paraDialog::init_setting(const ACQSETTING &setting, bool sop)
{
    psetting = setting;
    defaulsetting = setting;
    dlg_setfile.init_fsetting(psetting);						//把psetting传递给fsetting
    nocollecting = sop;											//程序未进行采集
}

void paraDialog::initial_para()
{
    update_show();
    //激光参数
    connect(ui->lineEdit_laserRPF,&QLineEdit::textChanged,this,&paraDialog::set_laserRPF);						//激光重频
    connect(ui->lineEdit_laserPulseWidth,&QLineEdit::textChanged,this,&paraDialog::set_laserPulseWidth);		//激光脉宽
    connect(ui->lineEdit_laserWaveLength,&QLineEdit::textChanged,this,&paraDialog::set_laserWaveLength);		//激光波长
    connect(ui->lineEdit_AOM_Freq,&QLineEdit::textChanged,this,&paraDialog::set_AOM_Freq);						//AOM移频量

    //扫描参数
    connect(ui->lineEdit_elevationAngle,&QLineEdit::textChanged,this,&paraDialog::set_elevationAngle);			//俯仰角 -> 探测方式
    connect(ui->lineEdit_start_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_start_azAngle);			//起始角
    connect(ui->lineEdit_step_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_step_azAngle);				//步进角 -> 探测方式
    connect(ui->lineEdit_step_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_SP_Interval);				//步进角 -> 速度和圆周间间隔使能状态
    connect(ui->lineEdit_step_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_circleNum);					//步进角 -> 圆周数
    connect(ui->lineEdit_step_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_angleNum);					//步进角 -> 方向数
    connect(ui->lineEdit_angleNum,&QLineEdit::textChanged,this,&paraDialog::set_circleNum);						//方向数 -> 圆周数
    connect(ui->lineEdit_circleNum, &QLineEdit::textChanged,this,&paraDialog::set_angleNum);					//圆周数 -> 方向数
    connect(ui->radioButton_anglekey,&QRadioButton::clicked,this,&paraDialog::set_anglekey);					//方向键
    connect(ui->radioButton_circlekey,&QRadioButton::clicked,this,&paraDialog::set_circlekey);					//圆周键
//    connect(ui->radioButton_continousdet,&QRadioButton::clicked,this,&paraDialog::set_continusdetekey);        //连续探测
    connect(ui->lineEdit_SP,&QLineEdit::textChanged,this,&paraDialog::set_motorSP);								//最高转速 -> 探测时间
    connect(ui->lineEdit_direct_interval,&QLineEdit::textChanged,this,&paraDialog::set_time_direct_interval);
    connect(ui->lineEdit_circle_interval,&QLineEdit::textChanged,this,&paraDialog::set_time_circle_interval);

    //采样设置
    connect(ui->comboBox_sampleFreq,&QComboBox::currentTextChanged,this,&paraDialog::set_sampleFreq);			//采样频率 -> 采样点数
    connect(ui->lineEdit_detRange,&QLineEdit::textChanged,this,&paraDialog::set_detRange);						//探测距离 -> 采样点数
    connect(ui->lineEdit_sampleNum,&QLineEdit::textChanged,this,&paraDialog::set_filesize);						//采样点数 -> 数据量
    connect(ui->lineEdit_plsAccNum,&QLineEdit::textChanged,this,&paraDialog::set_plsAccNum);					//脉冲数	  -> 数据量

    ui->lineEdit_DatafilePath->setReadOnly(true);
}

void paraDialog::update_show()
{
    //激光参数
    ui->lineEdit_laserRPF->setText(QString::number(psetting.laserRPF));
    ui->lineEdit_laserPulseWidth->setText(QString::number(psetting.laserPulseWidth));
    ui->lineEdit_laserWaveLength->setText(QString::number(psetting.laserWaveLength));
    ui->lineEdit_AOM_Freq->setText(QString::number(psetting.AOM_Freq));

    //采样设置
    ui->comboBox_sampleFreq->setCurrentText((QString::number(psetting.sampleFreq)));
    ui->lineEdit_detRange->setText(QString::number(psetting.detRange));
    ui->lineEdit_sampleNum->setText(QString::number(psetting.sampleNum));
    ui->lineEdit_plsAccNum->setText(QString::number(psetting.plsAccNum));

    //探测方式
    ui->comboBox_DetetectMoMode->setCurrentIndex(psetting.detectMode);
    //扫描参数——俯仰角
    ui->lineEdit_elevationAngle->setText(QString::number(psetting.elevationAngle));
    //方位角
    ui->lineEdit_start_azAngle->setText(QString::number(psetting.start_azAngle));
    ui->lineEdit_step_azAngle->setText(QString::number(psetting.step_azAngle));
    //扫描探测
    ui->groupBox_6->setEnabled(true);
//    ui->radioButton_continousdet->setChecked(psetting.continusdete);
    ui->radioButton_anglekey->setChecked(psetting.anglekey);
    ui->radioButton_circlekey->setChecked(psetting.circlekey);
    if(psetting.anglekey)
    {
        ui->lineEdit_circleNum->setEnabled(false);
        ui->lineEdit_angleNum->setEnabled(true);
    }
    else
    {
        ui->lineEdit_circleNum->setEnabled(true);
        ui->lineEdit_angleNum->setEnabled(false);
    }
    ui->lineEdit_angleNum->setText(QString::number(psetting.angleNum));
    ui->lineEdit_circleNum->setText(QString::number(psetting.circleNum));
    ui->lineEdit_SP->setText(QString::number(psetting.SP));

    //扫描参数——定时设置
    ui->lineEdit_direct_interval->setText(QString::number(psetting.IntervalTime,'f',2));
    ui->lineEdit_circle_interval->setText(QString::number(psetting.GroupTime,'f',2));

    //文件存储

    ui->lineEdit_DatafilePath->setText(psetting.DatafilePath);
    ui->checkBox_autocreate_dateDir->setChecked(psetting.autoCreate_DateDir);
    //更新通道文件名称

    //以下是参考信息
    //探测方式
    show_detect_mode();

    //预估探测时间
    set_dect_time();

    //更新文件数据量
    direct_size = SIZE_OF_FILE_HEADER + psetting.plsAccNum*psetting.sampleNum;								//单位B
    //双通道数据量
    set_filesize();
}

//探测方式
void paraDialog::show_detect_mode()
{
    if(psetting.elevationAngle == 0)
    {
        if(psetting.step_azAngle == 0)
            ui->lineEdit_detectDir->setText(QString::fromLocal8Bit("水平单向探测"));        //Horizontal single
        else
            ui->lineEdit_detectDir->setText(QString::fromLocal8Bit("水平扫描探测，每周方向数")+QString::number(360/psetting.step_azAngle));//Horizontal scanning，directions
    }
    else
        if(psetting.step_azAngle == 0)
            ui->lineEdit_detectDir->setText(QString::fromLocal8Bit("单向径向探测"));//Single radial
        else
            ui->lineEdit_detectDir->setText(QString::fromLocal8Bit("圆锥扫描探测，每周方向数")+QString::number(360/psetting.step_azAngle));
}

//探测时间=电机转动时间+触发时间+上传时间+方向间间隔时间+圆周间间隔时间
void paraDialog::set_dect_time()
{
    int time_need = psetting.angleNum*psetting.step_azAngle/psetting.SP +
            psetting.angleNum*psetting.sampleNum/(psetting.sampleFreq*1000000) +
            psetting.angleNum*ui->lineEdit_sglfilesize->text().toFloat()/UPLOAD_SPEED;
    if(psetting.step_azAngle == 0)
        time_need = time_need + (psetting.angleNum-1)*psetting.IntervalTime;
    else
        time_need = time_need + (psetting.angleNum-1)*psetting.IntervalTime
                + psetting.GroupTime*60*psetting.angleNum/(360/psetting.step_azAngle);
    if(time_need < 1)
        ui->lineEdit_totalTime->setText("<1s");									//在1s以下
    else
        if(time_need < 60)														//在1min以下
            ui->lineEdit_totalTime->setText(QString::number(time_need)+"s");
        else
            if(time_need < 3600)												//在1h以下
            {
                int m = time_need/60;
                int s = time_need%60;
                if(s == 0)
                    ui->lineEdit_totalTime->setText(QString::number(m)+"min");	//无秒
                else
                    ui->lineEdit_totalTime->setText(QString::number(m)+"min"+QString::number(s)+"s");
            }
            else																//在1h以上
            {
                int h = time_need/3600;
                int remain = time_need%3600;
                if(remain == 0)
                    ui->lineEdit_totalTime->setText(QString::number(h)+"h");//无分无秒
                else
                {
                    int m = remain/60;
                    int s = remain%60;
                    if(s == 0)
                        ui->lineEdit_totalTime->setText(QString::number(h)+"h"+QString::number(m)+"m");//无秒
                    else
                        ui->lineEdit_totalTime->setText(QString::number(h)+"h"+QString::number(m)+"m"+QString::number(s)+"s");
                }
            }
}


void paraDialog::set_laserRPF()														//psetting获取编辑框值
{
    psetting.laserRPF = ui->lineEdit_laserRPF->text().toInt();
}

void paraDialog::set_laserPulseWidth()												//psetting获取编辑框值
{
    psetting.laserPulseWidth = ui->lineEdit_laserPulseWidth->text().toInt();
}

void paraDialog::set_laserWaveLength()												//psetting获取编辑框值
{
    psetting.laserWaveLength = ui->lineEdit_laserWaveLength->text().toInt();
}

void paraDialog::set_AOM_Freq()														//psetting获取编辑框值
{
    psetting.AOM_Freq = ui->lineEdit_AOM_Freq->text().toInt();
}

void paraDialog::set_elevationAngle()												//俯仰角 决定探测方向是水平还是径向
{
    psetting.elevationAngle = ui->lineEdit_elevationAngle->text().toInt();
    show_detect_mode();																//参考信息中的探测方式
}

void paraDialog::set_start_azAngle()												//psetting获取编辑框值
{
    psetting.start_azAngle = ui->lineEdit_start_azAngle->text().toInt();
}

void paraDialog::set_step_azAngle()													//步进角 是否为0决定是不是圆周扫描，大小决定每周扫描数//psetting获取编辑框值
{
    psetting.step_azAngle = ui->lineEdit_step_azAngle->text().toInt();
    show_detect_mode();
    set_dect_time();
}

void paraDialog::set_SP_Interval()
{
    if(ui->lineEdit_step_azAngle->text().toInt() == 0)
    {
        ui->lineEdit_SP->setEnabled(false);
        ui->radioButton_anglekey->setChecked(true);
        ui->radioButton_circlekey->setEnabled(false);
        ui->radioButton_circlekey->setChecked(false);
        set_anglekey();
        ui->lineEdit_circle_interval->setEnabled(false);
    }
    else
    {
        ui->lineEdit_SP->setEnabled(true);
        ui->radioButton_circlekey->setEnabled(true);
        ui->lineEdit_circle_interval->setEnabled(true);
    }
}

//设置圆周数输入框的显示数值
void paraDialog::set_circleNum()													//圆周数 影响方向数,psetting获取编辑框值
{
    if(psetting.circlekey == true)
        psetting.circleNum = ui->lineEdit_circleNum->text().toFloat();
    else
    {
        psetting.angleNum = ui->lineEdit_angleNum->text().toInt();
        psetting.step_azAngle = ui->lineEdit_step_azAngle->text().toInt();
        psetting.circleNum = (float)psetting.angleNum/360*psetting.step_azAngle;
        ui->lineEdit_circleNum->setText(QString::number(psetting.circleNum,'f',2));
    }

    //    check_update_SN();
    set_dect_time();																//预估时间
    //双通道数据量
    ui->lineEdit_totalsize->setText(QString::number(4*psetting.angleNum*direct_size/(1024*1024),'f',2));
}

//设置方向数输入框的显示数值
void paraDialog::set_angleNum()														//方向数 决定圆周数，影响总数据量（双通道乘2），探测总时间//psetting获取编辑框值
{
    if(psetting.anglekey == true)
        psetting.angleNum = ui->lineEdit_angleNum->text().toInt();
    else
    {
        psetting.circleNum = ui->lineEdit_circleNum->text().toFloat();
        psetting.step_azAngle = ui->lineEdit_step_azAngle->text().toInt();
        psetting.angleNum = (int)(psetting.circleNum*360/psetting.step_azAngle);
        ui->lineEdit_angleNum->setText(QString::number(psetting.angleNum));
    }

    set_dect_time();																//预估时间
    //总数据量  双通道
    ui->lineEdit_totalsize->setText(QString::number(4*psetting.angleNum*direct_size/(1024*1024),'f',2));
}

void paraDialog::set_anglekey()														//方向键
{
    psetting.anglekey = true;
    psetting.circlekey = false;
    ui->lineEdit_angleNum->setEnabled(true);
    ui->lineEdit_circleNum->setEnabled(false);
}

void paraDialog::set_circlekey()													//圆周键
{
    psetting.anglekey = false;
    psetting.circlekey = true;
    ui->lineEdit_angleNum->setEnabled(false);
    ui->lineEdit_circleNum->setEnabled(true);
}

void paraDialog::set_continusdetekey()
{
    psetting.anglekey = false;
    psetting.circlekey = false;
    psetting.detectMode = 2;
    ui->lineEdit_angleNum->setEnabled(true);
    ui->lineEdit_circleNum->setEnabled(false);
}

void paraDialog::set_motorSP()														//电机转速
{
    if(ui->lineEdit_SP->text().toInt() > 90)
    {
        QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("最高转速不能超过90°/s"));
        ui->lineEdit_SP->setText(NULL);
    }
    else
    {
        psetting.SP = ui->lineEdit_SP->text().toInt();
        set_dect_time();
    }
}

void paraDialog::set_time_direct_interval()
{
    psetting.IntervalTime = ui->lineEdit_direct_interval->text().toFloat();
    set_dect_time();
}

void paraDialog::set_time_circle_interval()
{
    psetting.GroupTime = ui->lineEdit_circle_interval->text().toFloat();
    set_dect_time();
}


void paraDialog::set_sampleFreq()													//采样频率 影响采样点数、单文件量、总数据量//psetting获取编辑框值
{
    psetting.sampleFreq = ui->comboBox_sampleFreq->currentText().toInt();
    int NumMax_s = psetting.sampleFreq*psetting.detRange/FACTOR;
    int judge_page_s = NumMax_s%256;
    pagePerPls = NumMax_s/256;
    if(judge_page_s != 0)
        pagePerPls++;
    psetting.sampleNum = 256*pagePerPls;
    direct_size = SIZE_OF_FILE_HEADER + psetting.plsAccNum*psetting.sampleNum;		//单个方向上的数据量
    ui->lineEdit_sampleNum->setText(QString::number(psetting.sampleNum));			//采样点数
}

void paraDialog::set_detRange()														//探测距离 影响采样点数、单文件量、总数据量//psetting获取编辑框值
{
    psetting.detRange = 1000*(ui->lineEdit_detRange->text().toFloat());
    int NumMax_d = psetting.sampleFreq*psetting.detRange/FACTOR;
    int judge_page_d = NumMax_d%256;
    pagePerPls = NumMax_d/256;
    if(judge_page_d != 0)
        pagePerPls++;
    psetting.sampleNum = 256*pagePerPls;
    direct_size = SIZE_OF_FILE_HEADER + psetting.plsAccNum*psetting.sampleNum;		//单个方向上的数据量
    ui->lineEdit_sampleNum->setText(QString::number(psetting.sampleNum));			//采样点数
}

void paraDialog::set_plsAccNum()													//脉冲数影响单文件量、总数据量（双通道乘2）//psetting获取编辑框值
{
    psetting.plsAccNum = ui->lineEdit_plsAccNum->text().toInt();
    direct_size = SIZE_OF_FILE_HEADER + psetting.plsAccNum*psetting.sampleNum;       //单方向上的数据量 双通道
    set_filesize();
}

//双通道数据大小量
void paraDialog::set_filesize()
{
    filesize_over();
    ui->lineEdit_sglfilesize->setText(QString::number(4*direct_size/(1024*1024),'f',2));
    ui->lineEdit_totalsize->setText(QString::number(4*psetting.angleNum*direct_size/(1024*1024),'f',2));
    set_dect_time();
}


void paraDialog::filesize_over()
{
    if(direct_size > DATA_MEMORY)
    {
        ui->pushButton_save->setEnabled(false);
        ui->pushButton_sure->setEnabled(false);
        ui->comboBox_sampleFreq->setStyleSheet("color: red;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
        ui->lineEdit_detRange->setStyleSheet("color: red;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
        ui->lineEdit_plsAccNum->setStyleSheet("color: red;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
    }
    else
    {
        ui->pushButton_save->setEnabled(true);
        if(nocollecting == false)																		//若程序采集，确定键为非使能状态
            ui->pushButton_sure->setEnabled(false);
        else
            ui->pushButton_sure->setEnabled(true);
        ui->comboBox_sampleFreq->setStyleSheet("color: black;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
        ui->lineEdit_detRange->setStyleSheet("color: black;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
        ui->lineEdit_plsAccNum->setStyleSheet("color: black;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
    }
}

//路径显示设置
void paraDialog::show_DatafilePath(QString str)
{
    QDir mypath(str);
    if(!mypath.exists()) {
        ui->lineEdit_DatafilePath->setStyleSheet("color: red;""font-size:10pt;""font-family:'Microsoft YaHei UI';");     //路径不存在，红色
    }
    else {
        ui->lineEdit_DatafilePath->setStyleSheet("color: black;""font-size:10pt;""font-family:'Microsoft YaHei UI';");   //存在，黑色
    }
    psetting.DatafilePath = str;
    ui->lineEdit_DatafilePath->setText(str);
}

void paraDialog::on_checkBox_autoCreate_DateDir_clicked()
{
    psetting.autoCreate_DateDir = ui->checkBox_autocreate_dateDir->isChecked();
    QString str = psetting.DatafilePath;

    QDir mypath(str);
    QString dirname = mypath.dirName();
    QDateTime time = QDateTime::currentDateTime();

    if(psetting.autoCreate_DateDir)
    {
        int num = dirname.toInt();
        int len = dirname.length();
        QString today_str = time.toString("yyyyMMdd");
        int today_int = today_str.toInt();
        if(len == 8 && (num != today_int) && qAbs(num - today_int)<10000)
        {
            str = mypath.absolutePath();
            int str_len = str.length();
            str.resize(str_len - 8);
            str += today_str;
            qDebug()<<str<<endl;
        }

        else if( dirname != time.toString("yyyyMMdd"))
        {
            str = mypath.absolutePath();
            str += QString("/");
            str += time.toString("yyyyMMdd");			//设置显示格式
            qDebug()<<"Dir not Match";
        }
        qDebug()<<str<<endl;
    }
    else												//取消选择时，如果当前日期路径不存在，则取消，如存在，则不变。
    {
        if( dirname == time.toString("yyyyMMdd"))
        {
            if (!mypath.exists())
            {
                str = mypath.absolutePath();
                int str_len = str.length();
                str.resize(str_len - 9);				//减去/20xxxxxx
            }
            qDebug()<<"Dir Match"<<str<<endl;
        }
    }
    show_DatafilePath(str);
}

void paraDialog::on_pushButton_pathModify_clicked()
{
    QFileDialog *fd = new QFileDialog(this,QString::fromLocal8Bit("Select the folder"),psetting.DatafilePath);
    fd->setFileMode(QFileDialog::Directory);
    fd->setOption(QFileDialog::ShowDirsOnly, true);
    if(fd->exec() == QFileDialog::Accepted)
    {
        QStringList file = fd->selectedFiles();
        QString str = static_cast<QString>(file.at(0));
        if (str.length() == 3)
            str.resize(2);
        show_DatafilePath(str);
    }
    on_checkBox_autoCreate_DateDir_clicked();
}


//检查更新文件编号位数
//void paraDialog::check_update_SN()
//{
//    int suffix_Num = psetting.dataFileName_Suffix.toInt();				//后缀序号
//    int lenNum = suffix_Num + psetting.angleNum - 1;					//采集所需最大后缀序号
//    lenStr = QString::number(lenNum);									//最大序号String型
//    if(psetting.dataFileName_Suffix.length() < lenStr.length())
//    {
//        psetting.dataFileName_Suffix.sprintf("%08d",suffix_Num);
//        psetting.dataFileName_Suffix = psetting.dataFileName_Suffix.right(lenStr.length());
//    }
//}



ACQSETTING paraDialog::get_settings()
{
    return psetting;
}

void paraDialog::on_pushButton_save_clicked()
{
    //    if(psetting.dataFileName_Suffix.length() < lenStr.length())
    //    {
    //        QMessageBox::information(this,QString::fromLocal8Bit("提示"),
    //                                 QString::fromLocal8Bit("请重新设置序号，最小长度为") + QString::number(lenStr.length()));
    //        return;
    //    }
    profile_path = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("保存"),".","*.ini");
    if(!profile_path.isEmpty())
    {
        if(QFileInfo(profile_path).suffix().isEmpty())
            profile_path.append(".ini");											//如果无后缀，自动补上.ini
        dlg_setfile.writeTo_file(psetting,profile_path);
    }
}


void paraDialog::on_pushButton_cancel_clicked()
{
    reject();
}

void paraDialog::on_pushButton_load_clicked()
{
    profile_path = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开"),".","*.ini");
    if(!profile_path.isEmpty())
    {
        dlg_setfile.readFrom_file(profile_path);
        psetting = dlg_setfile.get_settings();
        defaulsetting = dlg_setfile.get_settings();
        update_show();
    }
}

void paraDialog::on_pushButton_reset_clicked()
{
    psetting = defaulsetting;
    update_show();
}


void paraDialog::on_pushButton_sure_clicked()
{
    //    if(psetting.dataFileName_Suffix.length() < lenStr.length())
    //    {
    //        QMessageBox::information(this,QString::fromLocal8Bit("提示"),
    //                                 QString::fromLocal8Bit("请重新设置序号，最小长度为") + QString::number(lenStr.length()));
    //        return;
    //    }

    QString Disk_Name = psetting.DatafilePath.left(3);								//路径对应硬盘分区名
    quint64 freeSpace = getDiskFreeSpace(Disk_Name);								//获取路径对应硬盘分区的空间大小MB
    float totalfile_Space = ui->lineEdit_totalsize->text().toFloat();
    if(freeSpace > totalfile_Space+100)
        accept();
    else
        QMessageBox::warning(this,QString::fromLocal8Bit("提示"),
                             Disk_Name.left(1)+QString::fromLocal8Bit("The remaining space of the disk")
                             + QString::number(freeSpace) + QString::fromLocal8Bit("MB"));
}

quint64 paraDialog::getDiskFreeSpace(QString driver)
{
    LPCWSTR lpcwstrDriver = (LPCWSTR)driver.utf16();
    ULARGE_INTEGER liFreeBytesAvailable,liTotalBytes,liTotalFreeBytes;
    if(!GetDiskFreeSpaceEx(lpcwstrDriver,&liFreeBytesAvailable,&liTotalBytes,&liTotalFreeBytes))
    {
        qDebug() << "Error: Call to GetDiskFreeSpaceEx failed";
        return 0;
    }
    return (quint64)liTotalFreeBytes.QuadPart/1024/1024;			//单位为MB
}
