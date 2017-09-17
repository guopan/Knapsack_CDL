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
    dlg_setfile.init_fsetting(psetting);						//��psetting���ݸ�fsetting
    nocollecting = sop;											//����δ���вɼ�
}

void paraDialog::initial_para()
{
    update_show();
    //�������
    connect(ui->lineEdit_laserRPF,&QLineEdit::textChanged,this,&paraDialog::set_laserRPF);						//������Ƶ
    connect(ui->lineEdit_laserPulseWidth,&QLineEdit::textChanged,this,&paraDialog::set_laserPulseWidth);		//��������
    connect(ui->lineEdit_laserWaveLength,&QLineEdit::textChanged,this,&paraDialog::set_laserWaveLength);		//���Ⲩ��
    connect(ui->lineEdit_AOM_Freq,&QLineEdit::textChanged,this,&paraDialog::set_AOM_Freq);						//AOM��Ƶ��

    //ɨ�����
    connect(ui->lineEdit_elevationAngle,&QLineEdit::textChanged,this,&paraDialog::set_elevationAngle);			//������ -> ̽�ⷽʽ
    connect(ui->lineEdit_start_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_start_azAngle);			//��ʼ��
    connect(ui->lineEdit_step_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_step_azAngle);				//������ -> ̽�ⷽʽ
    connect(ui->lineEdit_step_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_SP_Interval);				//������ -> �ٶȺ�Բ�ܼ���ʹ��״̬
    connect(ui->lineEdit_step_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_circleNum);					//������ -> Բ����
    connect(ui->lineEdit_step_azAngle,&QLineEdit::textChanged,this,&paraDialog::set_angleNum);					//������ -> ������
    connect(ui->lineEdit_angleNum,&QLineEdit::textChanged,this,&paraDialog::set_circleNum);						//������ -> Բ����
    connect(ui->lineEdit_circleNum, &QLineEdit::textChanged,this,&paraDialog::set_angleNum);					//Բ���� -> ������
    connect(ui->radioButton_anglekey,&QRadioButton::clicked,this,&paraDialog::set_anglekey);					//�����
    connect(ui->radioButton_circlekey,&QRadioButton::clicked,this,&paraDialog::set_circlekey);					//Բ�ܼ�
//    connect(ui->radioButton_continousdet,&QRadioButton::clicked,this,&paraDialog::set_continusdetekey);        //����̽��
    connect(ui->lineEdit_SP,&QLineEdit::textChanged,this,&paraDialog::set_motorSP);								//���ת�� -> ̽��ʱ��
    connect(ui->lineEdit_direct_interval,&QLineEdit::textChanged,this,&paraDialog::set_time_direct_interval);
    connect(ui->lineEdit_circle_interval,&QLineEdit::textChanged,this,&paraDialog::set_time_circle_interval);

    //��������
    connect(ui->comboBox_sampleFreq,&QComboBox::currentTextChanged,this,&paraDialog::set_sampleFreq);			//����Ƶ�� -> ��������
    connect(ui->lineEdit_detRange,&QLineEdit::textChanged,this,&paraDialog::set_detRange);						//̽����� -> ��������
    connect(ui->lineEdit_sampleNum,&QLineEdit::textChanged,this,&paraDialog::set_filesize);						//�������� -> ������
    connect(ui->lineEdit_plsAccNum,&QLineEdit::textChanged,this,&paraDialog::set_plsAccNum);					//������	  -> ������

    ui->lineEdit_DatafilePath->setReadOnly(true);
}

void paraDialog::update_show()
{
    //�������
    ui->lineEdit_laserRPF->setText(QString::number(psetting.laserRPF));
    ui->lineEdit_laserPulseWidth->setText(QString::number(psetting.laserPulseWidth));
    ui->lineEdit_laserWaveLength->setText(QString::number(psetting.laserWaveLength));
    ui->lineEdit_AOM_Freq->setText(QString::number(psetting.AOM_Freq));

    //��������
    ui->comboBox_sampleFreq->setCurrentText((QString::number(psetting.sampleFreq)));
    ui->lineEdit_detRange->setText(QString::number(psetting.detRange));
    ui->lineEdit_sampleNum->setText(QString::number(psetting.sampleNum));
    ui->lineEdit_plsAccNum->setText(QString::number(psetting.plsAccNum));

    //ɨ���������������
    ui->lineEdit_elevationAngle->setText(QString::number(psetting.elevationAngle));
    //��λ��
    ui->lineEdit_start_azAngle->setText(QString::number(psetting.start_azAngle));
    ui->lineEdit_step_azAngle->setText(QString::number(psetting.step_azAngle));
    //ɨ��̽��
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

    //ɨ�����������ʱ����
    ui->lineEdit_direct_interval->setText(QString::number(psetting.IntervalTime,'f',2));
    ui->lineEdit_circle_interval->setText(QString::number(psetting.GroupTime,'f',2));

    //�ļ��洢

    ui->lineEdit_DatafilePath->setText(psetting.DatafilePath);
    ui->checkBox_autocreate_datafile->setChecked(psetting.autocreate_datafile);
    //����ͨ���ļ�����

    //�����ǲο���Ϣ
    //̽�ⷽʽ
    show_detect_mode();

    //Ԥ��̽��ʱ��
    set_dect_time();

    //�����ļ�������
    direct_size = SIZE_OF_FILE_HEADER + psetting.plsAccNum*psetting.sampleNum;								//��λB
    //˫ͨ��������
    set_filesize();
}

//̽�ⷽʽ
void paraDialog::show_detect_mode()
{
    if(psetting.elevationAngle == 0)
    {
        if(psetting.step_azAngle == 0)
            ui->lineEdit_detectDir->setText(QString::fromLocal8Bit("ˮƽ����̽��"));        //Horizontal single
        else
            ui->lineEdit_detectDir->setText(QString::fromLocal8Bit("ˮƽɨ��̽�⣬ÿ�ܷ�����")+QString::number(360/psetting.step_azAngle));//Horizontal scanning��directions
    }
    else
        if(psetting.step_azAngle == 0)
            ui->lineEdit_detectDir->setText(QString::fromLocal8Bit("������̽��"));//Single radial
        else
            ui->lineEdit_detectDir->setText(QString::fromLocal8Bit("Բ׶ɨ��̽�⣬ÿ�ܷ�����")+QString::number(360/psetting.step_azAngle));
}

//̽��ʱ��=���ת��ʱ��+����ʱ��+�ϴ�ʱ��+�������ʱ��+Բ�ܼ���ʱ��
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
        ui->lineEdit_totalTime->setText("<1s");									//��1s����
    else
        if(time_need < 60)														//��1min����
            ui->lineEdit_totalTime->setText(QString::number(time_need)+"s");
        else
            if(time_need < 3600)												//��1h����
            {
                int m = time_need/60;
                int s = time_need%60;
                if(s == 0)
                    ui->lineEdit_totalTime->setText(QString::number(m)+"min");	//����
                else
                    ui->lineEdit_totalTime->setText(QString::number(m)+"min"+QString::number(s)+"s");
            }
            else																//��1h����
            {
                int h = time_need/3600;
                int remain = time_need%3600;
                if(remain == 0)
                    ui->lineEdit_totalTime->setText(QString::number(h)+"h");//�޷�����
                else
                {
                    int m = remain/60;
                    int s = remain%60;
                    if(s == 0)
                        ui->lineEdit_totalTime->setText(QString::number(h)+"h"+QString::number(m)+"m");//����
                    else
                        ui->lineEdit_totalTime->setText(QString::number(h)+"h"+QString::number(m)+"m"+QString::number(s)+"s");
                }
            }
}


void paraDialog::set_laserRPF()														//psetting��ȡ�༭��ֵ
{
    psetting.laserRPF = ui->lineEdit_laserRPF->text().toInt();
}

void paraDialog::set_laserPulseWidth()												//psetting��ȡ�༭��ֵ
{
    psetting.laserPulseWidth = ui->lineEdit_laserPulseWidth->text().toInt();
}

void paraDialog::set_laserWaveLength()												//psetting��ȡ�༭��ֵ
{
    psetting.laserWaveLength = ui->lineEdit_laserWaveLength->text().toInt();
}

void paraDialog::set_AOM_Freq()														//psetting��ȡ�༭��ֵ
{
    psetting.AOM_Freq = ui->lineEdit_AOM_Freq->text().toInt();
}

void paraDialog::set_elevationAngle()												//������ ����̽�ⷽ����ˮƽ���Ǿ���
{
    psetting.elevationAngle = ui->lineEdit_elevationAngle->text().toInt();
    show_detect_mode();																//�ο���Ϣ�е�̽�ⷽʽ
}

void paraDialog::set_start_azAngle()												//psetting��ȡ�༭��ֵ
{
    psetting.start_azAngle = ui->lineEdit_start_azAngle->text().toInt();
}

void paraDialog::set_step_azAngle()													//������ �Ƿ�Ϊ0�����ǲ���Բ��ɨ�裬��С����ÿ��ɨ����//psetting��ȡ�༭��ֵ
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

//����Բ������������ʾ��ֵ
void paraDialog::set_circleNum()													//Բ���� Ӱ�췽����,psetting��ȡ�༭��ֵ
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
    set_dect_time();																//Ԥ��ʱ��
    //˫ͨ��������
    ui->lineEdit_totalsize->setText(QString::number(4*psetting.angleNum*direct_size/(1024*1024),'f',2));
}

//���÷�������������ʾ��ֵ
void paraDialog::set_angleNum()														//������ ����Բ������Ӱ������������˫ͨ����2����̽����ʱ��//psetting��ȡ�༭��ֵ
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

    set_dect_time();																//Ԥ��ʱ��
    //��������  ˫ͨ��
    ui->lineEdit_totalsize->setText(QString::number(4*psetting.angleNum*direct_size/(1024*1024),'f',2));
}

void paraDialog::set_anglekey()														//�����
{
    psetting.anglekey = true;
    psetting.circlekey = false;
    ui->lineEdit_angleNum->setEnabled(true);
    ui->lineEdit_circleNum->setEnabled(false);
}

void paraDialog::set_circlekey()													//Բ�ܼ�
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
    psetting.continusdete = true;
    ui->lineEdit_angleNum->setEnabled(true);
    ui->lineEdit_circleNum->setEnabled(false);
}

void paraDialog::set_motorSP()														//���ת��
{
    if(ui->lineEdit_SP->text().toInt() > 90)
    {
        QMessageBox::warning(this,QString::fromLocal8Bit("��ʾ"),QString::fromLocal8Bit("���ת�ٲ��ܳ���90��/s"));
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


void paraDialog::set_sampleFreq()													//����Ƶ�� Ӱ��������������ļ�������������//psetting��ȡ�༭��ֵ
{
    psetting.sampleFreq = ui->comboBox_sampleFreq->currentText().toInt();
    int NumMax_s = psetting.sampleFreq*psetting.detRange/FACTOR;
    int judge_page_s = NumMax_s%256;
    pagePerPls = NumMax_s/256;
    if(judge_page_s != 0)
        pagePerPls++;
    psetting.sampleNum = 256*pagePerPls;
    direct_size = SIZE_OF_FILE_HEADER + psetting.plsAccNum*psetting.sampleNum;		//���������ϵ�������
    ui->lineEdit_sampleNum->setText(QString::number(psetting.sampleNum));			//��������

}

void paraDialog::set_detRange()														//̽����� Ӱ��������������ļ�������������//psetting��ȡ�༭��ֵ
{
    psetting.detRange = 1000*(ui->lineEdit_detRange->text().toFloat());
    int NumMax_d = psetting.sampleFreq*psetting.detRange/FACTOR;
    int judge_page_d = NumMax_d%256;
    pagePerPls = NumMax_d/256;
    if(judge_page_d != 0)
        pagePerPls++;
    psetting.sampleNum = 256*pagePerPls;
    direct_size = SIZE_OF_FILE_HEADER + psetting.plsAccNum*psetting.sampleNum;		//���������ϵ�������
    ui->lineEdit_sampleNum->setText(QString::number(psetting.sampleNum));			//��������
}

void paraDialog::set_plsAccNum()													//������Ӱ�쵥�ļ���������������˫ͨ����2��//psetting��ȡ�༭��ֵ
{
    psetting.plsAccNum = ui->lineEdit_plsAccNum->text().toInt();
    direct_size = SIZE_OF_FILE_HEADER + psetting.plsAccNum*psetting.sampleNum;       //�������ϵ������� ˫ͨ��
    set_filesize();
}

//˫ͨ�����ݴ�С��
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
        if(nocollecting == false)																		//������ɼ���ȷ����Ϊ��ʹ��״̬
            ui->pushButton_sure->setEnabled(false);
        else
            ui->pushButton_sure->setEnabled(true);
        ui->comboBox_sampleFreq->setStyleSheet("color: black;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
        ui->lineEdit_detRange->setStyleSheet("color: black;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
        ui->lineEdit_plsAccNum->setStyleSheet("color: black;""font-size:12pt;""font-family:'Microsoft YaHei UI';");
    }
}

//·����ʾ����
void paraDialog::show_DatafilePath(QString str)
{
    QDir mypath(str);
    if(!mypath.exists()) {
        ui->lineEdit_DatafilePath->setStyleSheet("color: red;""font-size:10pt;""font-family:'Microsoft YaHei UI';");     //·�������ڣ���ɫ
    }
    else {
        ui->lineEdit_DatafilePath->setStyleSheet("color: black;""font-size:10pt;""font-family:'Microsoft YaHei UI';");   //���ڣ���ɫ
    }
    psetting.DatafilePath = str;
    ui->lineEdit_DatafilePath->setText(str);
}

void paraDialog::on_checkBox_autocreate_datafile_clicked()
{
    psetting.autocreate_datafile = ui->checkBox_autocreate_datafile->isChecked();
    QString str = psetting.DatafilePath;

    QDir mypath(str);
    QString dirname = mypath.dirName();
    QDateTime time = QDateTime::currentDateTime();

    if(psetting.autocreate_datafile)
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
            str += time.toString("yyyyMMdd");			//������ʾ��ʽ
            qDebug()<<"Dir not Match";
        }
        qDebug()<<str<<endl;
    }
    else												//ȡ��ѡ��ʱ�������ǰ����·�������ڣ���ȡ��������ڣ��򲻱䡣
    {
        if( dirname == time.toString("yyyyMMdd"))
        {
            if (!mypath.exists())
            {
                str = mypath.absolutePath();
                int str_len = str.length();
                str.resize(str_len - 9);				//��ȥ/20xxxxxx
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
    on_checkBox_autocreate_datafile_clicked();
}


//�������ļ����λ��
//void paraDialog::check_update_SN()
//{
//    int suffix_Num = psetting.dataFileName_Suffix.toInt();				//��׺���
//    int lenNum = suffix_Num + psetting.angleNum - 1;					//�ɼ���������׺���
//    lenStr = QString::number(lenNum);									//������String��
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
    //        QMessageBox::information(this,QString::fromLocal8Bit("��ʾ"),
    //                                 QString::fromLocal8Bit("������������ţ���С����Ϊ") + QString::number(lenStr.length()));
    //        return;
    //    }
    profile_path = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("����"),".","*.ini");
    if(!profile_path.isEmpty())
    {
        if(QFileInfo(profile_path).suffix().isEmpty())
            profile_path.append(".ini");											//����޺�׺���Զ�����.ini
        dlg_setfile.writeTo_file(psetting,profile_path);
    }
}


void paraDialog::on_pushButton_cancel_clicked()
{
    reject();
}

void paraDialog::on_pushButton_load_clicked()
{
    profile_path = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("��"),".","*.ini");
    if(!profile_path.isEmpty())
    {
        dlg_setfile.readFrom_file(profile_path);
        psetting = dlg_setfile.get_setting();
        defaulsetting = dlg_setfile.get_setting();
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
    //        QMessageBox::information(this,QString::fromLocal8Bit("��ʾ"),
    //                                 QString::fromLocal8Bit("������������ţ���С����Ϊ") + QString::number(lenStr.length()));
    //        return;
    //    }

    QString Disk_Name = psetting.DatafilePath.left(3);								//·����ӦӲ�̷�����
    quint64 freeSpace = getDiskFreeSpace(Disk_Name);								//��ȡ·����ӦӲ�̷����Ŀռ��СMB
    float totalfile_Space = ui->lineEdit_totalsize->text().toFloat();
    if(freeSpace > totalfile_Space+100)
        accept();
    else
        QMessageBox::warning(this,QString::fromLocal8Bit("��ʾ"),
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
    return (quint64)liTotalFreeBytes.QuadPart/1024/1024;			//��λΪMB
}
