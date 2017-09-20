#ifndef ADQ214_H
#define ADQ214_H

#include <QObject>
#include "ADQAPI.h"
#include <QDebug>
#include <memory.h>
#include <stdio.h>
#include <QFile>
#include "global_defines.h"

class ADQ214 : public QObject
{
    Q_OBJECT
public:
    explicit ADQ214(QObject *parent = nullptr);
    void connectADQDevice();
    void Start_Capture();
    bool Config_ADQ214();           // 配置采集卡
    bool CaptureData2Buffer();      // 采集数据到缓存
    void WriteSpecData2disk();      // 写入采集数据到文件
    void ConvertData2Spec();        // 转换采集数据到功率谱
    void Transfer_Settings(const ACQSETTING &settings);     //传递进来全局设置
    PSD_DATA *get_PSD_data();

signals:
    void collectFinish();
public slots:
private:
    bool isADQ214Connected;
    void *adq_cu;
    setupADQ setupadq;
    ACQSETTING mainSettings;
    bool success;
    PSD_DATA *psd_res;

    int num_of_devices;
    int num_of_failed;
    int num_of_ADQ214;
};

#endif // ADQ214_H
