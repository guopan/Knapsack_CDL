#ifndef PARADIALOG_H
#define PARADIALOG_H
#include "acqsettings.h"
#include "settingfile.h"
#include "qmessagebox.h"
#include "qfiledialog.h"
#include <QDialog>
#include <QMessageBox>

const int FACTOR  = 150;								//采样点计算公式系数
const int SIZE_OF_FILE_HEADER = 6;						//单个单文件头的大小
const int DATA_MEMORY = 64*1000*1000;					//采集卡内存空间（点）
const int UPLOAD_SPEED = 20;							//上传速度MB/s

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
     void on_checkBox_autocreate_datafile_clicked();

private slots:
    //激光参数
    void set_laserRPF();
    void set_laserPulseWidth();
    void set_laserWaveLength();
    void set_AOM_Freq();
    //扫描参数
    void set_elevationAngle();
    void set_start_azAngle();
    void set_step_azAngle();
    void set_circleNum();
    void set_angleNum();
    void set_anglekey();
    void set_circlekey();
    void set_continusdetekey();
    void set_SP_Interval();
    void set_motorSP();
    void set_time_direct_interval();
    void set_time_circle_interval();
    //采样设置
    void set_sampleFreq();
    void set_detRange();
    void set_plsAccNum();
    void set_filesize();
    //文件存储


    void on_pushButton_pathModify_clicked();

    void on_pushButton_save_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_load_clicked();

    void on_pushButton_reset_clicked();

    void on_pushButton_sure_clicked();

private:
    Ui::paraDialog *ui;

    ACQSETTING psetting;
    ACQSETTING defaulsetting;
    settingfile dlg_setfile;
    QString profile_path;                      //配置文件路径

    double direct_size;		                   //单方向上的数据量
    int sampleNum;			                   //单个脉冲的采样点数
    bool nocollecting;		                   //是否正在采集数据
    int pagePerPls;			                   //单个脉冲的page数
    QString lenStr;

    void set_dect_time();				         //计算预估探测时间
    void filesize_over();				         //判断单文件量超过采集卡内存
    void show_DatafilePath(QString str);
    quint64 getDiskFreeSpace(QString driver);	 //获取路径下的硬盘剩余空间大小

    void update_show();
    void show_detect_mode();    //参考信息——探测方式

};

#endif // PARADIALOG_H
