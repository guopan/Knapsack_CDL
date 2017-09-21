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
<<<<<<< HEAD
=======

>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
    connect(ui->lineEdit_laserEP,&QLineEdit::textChanged,this,&paraDialog::set_laserEP);                        //��������
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
    connect(ui->lineEdit_SP,&QLineEdit::textChanged,this,&paraDialog::set_motorSP);								//���ת�� -> ̽��ʱ��
    connect(ui->lineEdit_ITV_TimeLength,&QLineEdit::textChanged,this,&paraDialog::set_time_direct_interval);
    connect(ui->lineEdit_DET_TimeLength,&QLineEdit::textChanged,this,&paraDialog::set_time_circle_interval);

    //��������
    connect(ui->comboBox_sampleFreq,&QComboBox::currentTextChanged,this,&paraDialog::set_sampleFreq);			//����Ƶ�� -> ��������
    connect(ui->lineEdit_TriggerLevel,&QLineEdit::textChanged,this,&paraDialog::set_Trigger_Level);
    connect(ui->lineEdit_nRangeBin,&QLineEdit::textChanged,this,&paraDialog::set_nRangeBin);
    connect(ui->lineEdit_velocity_band_2,&QLineEdit::textChanged,this,&paraDialog::set_velocity_band);
    connect(ui->lineEdit_plsAccNum,&QLineEdit::textChanged,this,&paraDialog::set_plsAccNum);					//������	  -> ������

    //�洢����
    connect(ui->checkBox_autocreate_dateDir,&QCheckBox::clicked,this,&paraDialog::on_checkBox_autoCreate_DateDir_clicked);
    ui->lineEdit_DatafilePath->setReadOnly(true);
}

void paraDialog::update_show()
{
    //�������
    ui->lineEdit_laserEP->setText(QString::number(psetting.laserPower));
    ui->lineEdit_laserRPF->setText(QString::number(psetting.laserRPF));
    ui->lineEdit_laserPulseWidth->setText(QString::number(psetting.laserPulseWidth));
    ui->lineEdit_laserWaveLength->setText(QString::number(psetting.laserWaveLength));
    ui->lineEdit_AOM_Freq->setText(QString::number(psetting.AOM_Freq));

    //��������
    ui->comboBox_sampleFreq->setCurrentText((QString::number(psetting.sampleFreq)));
    ui->lineEdit_TriggerLevel->setText(QString::number(psetting.Trigger_Level));
    ui->lineEdit_nRangeBin->setText(QString::number(psetting.nRangeBin));
    ui->lineEdit_nPointsPerBin->setText(QString::number((psetting.nPointsPerBin)));
    ui->lineEdit_plsAccNum->setText(QString::number(psetting.plsAccNum));
    ui->lineEdit_velocity_band_2->setText(QString::number(psetting.velocity_band));

    //̽�ⷽʽ
    ui->comboBox_DetetectMode->setCurrentIndex(psetting.detectMode);
    on_comboBox_DetetectMode_currentIndexChanged(psetting.detectMode);
    //ɨ���������������
    ui->lineEdit_elevationAngle->setText(QString::number(psetting.elevationAngle));
    //��λ��
    ui->lineEdit_start_azAngle->setText(QString::number(psetting.start_azAngle));
    ui->lineEdit_step_azAngle->setText(QString::number(psetting.step_azAngle));
    //ɨ��̽��
    ui->groupBox_6->setEnabled(true);
    ui->radioButton_anglekey->setChecked(psetting.anglekey);
    ui->radioButton_circlekey->setChecked(psetting.circlekey);
    if(psetting.detectMode == 1)
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
    ui->lineEdit_ITV_TimeLength->setText(QString::number(psetting.IntervalTime,'f',2));
    ui->lineEdit_DET_TimeLength->setText(QString::number(psetting.GroupTime,'f',2));

    //�ļ��洢

    ui->lineEdit_DatafilePath->setText(psetting.DatafilePath);
    ui->checkBox_autocreate_dateDir->setChecked(psetting.autoCreate_DateDir);
    ui->lineEdit_nMaxDir_infile_2 ->setText(QString::number(psetting.nMaxDir_inFile));

<<<<<<< HEAD
    on_comboBox_DetetectMode_currentIndexChanged(psetting.detectMode);
=======
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
}


void paraDialog::set_laserEP()
{
<<<<<<< HEAD
    psetting.laserPulseEnergy = ui->lineEdit_laserEP->text().toDouble();
=======
    psetting.laserPulseEnergy = ui->lineEdit_laserEP->text().toInt();
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
}

void paraDialog::set_laserRPF()
{
    psetting.laserRPF = ui->lineEdit_laserRPF->text().toDouble();
}

void paraDialog::set_laserPulseWidth()
{
    psetting.laserPulseWidth = ui->lineEdit_laserPulseWidth->text().toDouble();
}

void paraDialog::set_laserWaveLength()
{
    psetting.laserWaveLength = ui->lineEdit_laserWaveLength->text().toDouble();
}

void paraDialog::set_AOM_Freq()
{
    psetting.AOM_Freq = ui->lineEdit_AOM_Freq->text().toDouble();
}

void paraDialog::set_elevationAngle()
{
<<<<<<< HEAD
    psetting.elevationAngle = ui->lineEdit_elevationAngle->text().toDouble();
=======
    psetting.elevationAngle = ui->lineEdit_elevationAngle->text().toInt();
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
}

void paraDialog::set_start_azAngle()
{
    psetting.start_azAngle = ui->lineEdit_start_azAngle->text().toDouble();
}

void paraDialog::set_step_azAngle()
{
<<<<<<< HEAD
    psetting.step_azAngle = ui->lineEdit_step_azAngle->text().toDouble();
=======
    psetting.step_azAngle = ui->lineEdit_step_azAngle->text().toInt();
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
}

void paraDialog::set_SP_Interval()
{
    if(ui->lineEdit_step_azAngle->text().toDouble() == 0)
    {
        ui->lineEdit_SP->setEnabled(false);
        ui->radioButton_anglekey->setChecked(true);
        ui->radioButton_circlekey->setEnabled(false);
        ui->radioButton_circlekey->setChecked(false);
        set_anglekey();
        ui->lineEdit_DET_TimeLength->setEnabled(false);
    }
    else
    {
        ui->lineEdit_SP->setEnabled(true);
        ui->radioButton_circlekey->setEnabled(true);
        ui->lineEdit_DET_TimeLength->setEnabled(true);
    }
}

//����Բ������������ʾ��ֵ
void paraDialog::set_circleNum()													//Բ���� Ӱ�췽����,psetting��ȡ�༭��ֵ
{
    if(psetting.circlekey == true)
        psetting.circleNum = ui->lineEdit_circleNum->text().toDouble();
    else
    {
        psetting.angleNum = ui->lineEdit_angleNum->text().toDouble();
        psetting.step_azAngle = ui->lineEdit_step_azAngle->text().toDouble();
        psetting.circleNum = psetting.angleNum/360*psetting.step_azAngle;
        ui->lineEdit_circleNum->setText(QString::number(psetting.circleNum,'f',2));
    }

}

//���÷�������������ʾ��ֵ
void paraDialog::set_angleNum()														//������ ����Բ������Ӱ������������˫ͨ����2����̽����ʱ��//psetting��ȡ�༭��ֵ
{
    if(psetting.anglekey == true)
        psetting.angleNum = ui->lineEdit_angleNum->text().toDouble();
    else
    {
        psetting.circleNum = ui->lineEdit_circleNum->text().toDouble();
        psetting.step_azAngle = ui->lineEdit_step_azAngle->text().toDouble();
        psetting.angleNum = (psetting.circleNum*360/psetting.step_azAngle);
        ui->lineEdit_angleNum->setText(QString::number(psetting.angleNum,'f',2));
    }

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

void paraDialog::set_detectMode()
{
    psetting.anglekey = false;
    psetting.circlekey = false;
    psetting.detectMode = 2;
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
    }
}

void paraDialog::set_time_direct_interval()
{
<<<<<<< HEAD
    psetting.IntervalTime = ui->lineEdit_ITV_TimeLength->text().toDouble();
=======
    psetting.IntervalTime = ui->lineEdit_ITV_TimeLength->text().toFloat();
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
}

void paraDialog::set_time_circle_interval()
{
<<<<<<< HEAD
    psetting.GroupTime = ui->lineEdit_DET_TimeLength->text().toDouble();
=======
    psetting.GroupTime = ui->lineEdit_DET_TimeLength->text().toFloat();
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
}


void paraDialog::set_sampleFreq()													//����Ƶ��
{
<<<<<<< HEAD
    psetting.sampleFreq = ui->comboBox_sampleFreq->currentText().toDouble();
=======
    psetting.sampleFreq = ui->comboBox_sampleFreq->currentText().toInt();
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
}

void paraDialog::set_Trigger_Level()
{
<<<<<<< HEAD
    psetting.Trigger_Level = ui->lineEdit_TriggerLevel->text().toDouble();
=======
    psetting.Trigger_Level = ui->lineEdit_TriggerLevel->text().toFloat();
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
}

void paraDialog::set_nRangeBin()
{
    psetting.nRangeBin = ui->lineEdit_nRangeBin->text().toInt();
}

void paraDialog::set_nPointsPerBin()
{
    psetting.nPointsPerBin = ui->lineEdit_nPointsPerBin->text().toInt();
}

void paraDialog::set_plsAccNum()
{
    psetting.plsAccNum = ui->lineEdit_plsAccNum->text().toInt();
}

void paraDialog::set_velocity_band()
{
<<<<<<< HEAD
    psetting.velocity_band = ui->lineEdit_velocity_band_2 ->text().toDouble();
=======
    psetting.velocity_band = ui->lineEdit_velocity_band_2 ->text().toInt();
>>>>>>> 29a516f3a8eb99e7d1d50f689e2d086dbb1b3ae7
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
    on_checkBox_autoCreate_DateDir_clicked();
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
    //        QMessageBox::information(this,QString::fromLocal8Bit("��ʾ"),
    //                                 QString::fromLocal8Bit("������������ţ���С����Ϊ") + QString::number(lenStr.length()));
    //        return;
    //    }

//    QString Disk_Name = psetting.DatafilePath.left(3);								//·����ӦӲ�̷�����
//    quint64 freeSpace = getDiskFreeSpace(Disk_Name);								//��ȡ·����ӦӲ�̷����Ŀռ��СMB
//    float totalfile_Space = ui->lineEdit_totalsize->text().toFloat();
//    if(freeSpace > totalfile_Space+100)
    accept();
//    else
//        QMessageBox::warning(this,QString::fromLocal8Bit("��ʾ"),
//                             Disk_Name.left(1)+QString::fromLocal8Bit("The remaining space of the disk")
//                             + QString::number(freeSpace) + QString::fromLocal8Bit("MB"));
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


// ̽��ģʽ�л�ѡ��
void paraDialog::on_comboBox_DetetectMode_currentIndexChanged(int index)
{
    psetting.detectMode = index;
    switch (index) {
    case 0:     //����̽��
        ui->radioButton_anglekey->setEnabled(false);
        ui->radioButton_circlekey->setEnabled(false);
        ui->lineEdit_angleNum->setEnabled(false);
        ui->lineEdit_circleNum->setEnabled(false);
        ui->lineEdit_ITV_TimeLength->setEnabled(false);
        ui->lineEdit_DET_TimeLength->setEnabled(false);
        break;
    case 1:     //����̽��
        ui->radioButton_anglekey->setEnabled(true);
        ui->radioButton_circlekey->setEnabled(true);
        ui->lineEdit_angleNum->setEnabled(true);
        ui->lineEdit_circleNum->setEnabled(true);
        ui->lineEdit_ITV_TimeLength->setEnabled(false);
        ui->lineEdit_DET_TimeLength->setEnabled(false);
        break;
    case 2:     //��ʱ̽��
        ui->radioButton_anglekey->setEnabled(false);
        ui->radioButton_circlekey->setEnabled(false);
        ui->lineEdit_angleNum->setEnabled(false);
        ui->lineEdit_circleNum->setEnabled(false);
        ui->lineEdit_ITV_TimeLength->setEnabled(true);
        ui->lineEdit_DET_TimeLength->setEnabled(true);
        break;
    default:
        break;
    }
}
