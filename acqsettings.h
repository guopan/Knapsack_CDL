#ifndef ACQSETTINGS_H
#define ACQSETTINGS_H
#include <QString>

typedef struct
{
    //激光参数
    quint16 laserRPF;			//激光频率
    quint16 laserPulseWidth;	//脉冲宽度
    quint16 laserWaveLength;	//激光波长
    quint16 AOM_Freq;			//AOM移频量

    //扫描参数
    quint16 elevationAngle;		//俯仰角
    quint16 start_azAngle;		//起始角
    quint16 step_azAngle;		//步进角
    quint32 angleNum;			//方向数
    float circleNum;			//圆周数
    bool anglekey;				//方向键
    bool circlekey;				//圆周键
    bool continusdete;          //连续探测
    quint16 SP;					//驱动器速度
    float direct_intervalTime;	//方向间间隔
    float time_circle_interval;	//圆周间间隔

    //采样参数
    quint16 sampleFreq;			//采样频率
    float detRange;				//采样距离
    quint32 sampleNum;			//采样点数
    quint16 plsAccNum;			//单次脉冲数

    //文件存储
    QString DatafilePath;		//数据存储路径
    bool autocreate_datafile;	//自动创建日期文件夹

}ACQSETTING;

#endif // ACQSETTINGS_H
