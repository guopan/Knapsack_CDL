#ifndef PARADIALOG_H
#define PARADIALOG_H
#include "global_defines.h"
#include "settingfile.h"
#include "qmessagebox.h"
#include "qfiledialog.h"
#include <QDialog>
#include <QMessageBox>

const double Factor = 150;                    //����/2
const int SIZE_OF_FILE_HEADER = 6;			  //�������ļ�ͷ�Ĵ�С,��λ���ֽ�
//const int DATA_MEMORY = 64*1000*1000;		  //�ɼ����ڴ�ռ䣨�㣩
//const int UPLOAD_SPEED = 20;				  //�ϴ��ٶ�MB/s

namespace Ui {
class paraDialog;
}

class paraDialog : public QDialog
{
    Q_OBJECT

public:
    explicit paraDialog(QWidget *parent = 0);
    ~paraDialog();

    void init_setting(const ACQSETTING &setting, bool sop);
    void initial_para();
    ACQSETTING get_settings(void);

public slots:
     void on_checkBox_autoCreate_DateDir_clicked();

private slots:
    //�������
    void set_laserEP();
    void set_laserRPF();
    void set_laserPulseWidth();
    void set_laserWaveLength();
    void set_AOM_Freq();
    //ɨ�����
    void set_elevationAngle();
    void set_start_azAngle();
    void set_step_azAngle();
    void set_circleNum();
    void set_angleNum();
    void set_anglekey();
    void set_circlekey();
    void set_detectMode();
//    void set_SP_Interval();
    void set_motorSP();
    void set_time_direct_interval();
    void set_time_circle_interval();
    //��������
    void set_sampleFreq();
    void set_plsAccNum();
    void set_velocity_band();
    void set_Trigger_Level();
    void set_nRangeBin();
    void set_nPointsPerBin();
    void set_nDir_VectorCal();

    //�ļ��洢
    void on_pushButton_pathModify_clicked();
    void set_nMaxDir_infile();

    void on_pushButton_save_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_load_clicked();

    void on_pushButton_reset_clicked();

    void on_pushButton_sure_clicked();

    void on_comboBox_DetetectMode_currentIndexChanged(int index);

//    void show_detect_mode();                    //�ο���Ϣ����̽�ⷽʽ
    void show_deteRange();                       //̽�����
    void show_RangeReso();                       //����ֱ���



    void on_comboBox_lasermode_currentIndexChanged(int index);

private:
    Ui::paraDialog *ui;

    ACQSETTING psetting;
    ACQSETTING defaulsetting;
    settingfile dlg_setfile;
    QString profile_path;                      //�����ļ�·��

//    double triggerlevel;                      //������ѹ
    double deteRange;
    double filesize;
    double velodatafilesize;		           //�������ϵ�������
    double specdatafilesize;
    double RangeReso;                          //����ֱ���
//    int sampleNum;			               //��������Ĳ�������
    bool nocollecting;		                   //�Ƿ����ڲɼ�����
//    int pagePerPls;			                   //���������page��
    QString lenStr;

    void filesize_over();				         //�жϵ��ļ��������ɼ����ڴ�
    void show_DatafilePath(QString str);
    quint64 getDiskFreeSpace(QString driver);	 //��ȡ·���µ�Ӳ��ʣ��ռ��С

    void update_show();
//    void show_detect_mode();                     //�ο���Ϣ����̽�ⷽʽ
//    void show_deteRange();                       //̽�����
//    void show_RangeReso();                       //����ֱ���
    void show_filesize();

};

#endif // PARADIALOG_H
