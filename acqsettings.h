#ifndef ACQSETTINGS_H
#define ACQSETTINGS_H
#include <QString>

typedef struct
{
    //激光参数
    bool    isPulseMode;        //脉冲探测（true）or连续探测（false）
    float   laserPulseEnergy;   //激光能量，单位μJ，连续模式下为0
    float   laserPower;         //激光功率，单位mW，脉冲模式下为0
    quint16 laserRPF;			//激光频率
    quint16 laserPulseWidth;	//脉冲宽度
    quint16 laserWaveLength;	//激光波长
    quint16 AOM_Freq;			//AOM移频量

    //扫描参数
    quint16 elevationAngle;		//俯仰角
    quint16 start_azAngle;		//起始角
    quint16 step_azAngle;		//步进角
    quint32 angleNum;			//方向数
    float   circleNum;			//圆周数
    bool    anglekey;			//方向键
    bool    circlekey;			//圆周键
    bool    continusdete;       //连续探测
    quint16 SP;					//驱动器速度
    float   direct_intervalTime;	//方向间间隔
    float   time_circle_interval;	//圆周间间隔

    //采样参数
    quint16 sampleFreq;			//采样频率
    float   detRange;   		//采样距离，删除
    quint32 sampleNum;			//采样点数，删除
    quint16 Trigger_Level;      //触发电平
    int     PreTrigger;         //预触发点数，保留，暂不提供设置

    //实时处理参数
    quint16 plsAccNum;			//单方向累加脉冲数
    quint16 nRangeBin;          //距离门数
    quint16 nPointsPerBin;      //距离门内点数

    //反演参数
    float velocity_band;        //径向风速范围,±m/s


    //文件存储
    QString DatafilePath;		//数据存储路径
    bool autocreate_datafile;	//自动创建日期文件夹

}ACQSETTING;

#endif // ACQSETTINGS_H
