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
    bool Config_ADQ214();           // ���òɼ���
    bool CaptureData2Buffer();      // �ɼ����ݵ�����
    void WriteData2disk();          // д��ɼ����ݵ��ļ�
    void WriteSpecData2disk();      // д��ɼ����ݵ��ļ�
    void ConvertData2Spec();        // ת���ɼ����ݵ�������

signals:
    void collectFinish();
public slots:
private:
    bool isADQ214Connected;
    void *adq_cu;
    setupADQ setupadq;
    ACQSETTING mainSettings;
    FILE* outfileA = NULL;
    FILE* outfileB = NULL;
    bool success;
    PSD_DATA *psd_res;

    int num_of_devices;
    int num_of_failed;
    int num_of_ADQ214;
};

#endif // ADQ214_H
