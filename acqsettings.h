#ifndef ACQSETTINGS_H
#define ACQSETTINGS_H
#include <QString>

typedef struct
{
    //�������
    bool    isPulseMode;        //����̽�⣨true��or����̽�⣨false��
    float   laserPulseEnergy;   //������������λ��J������ģʽ��Ϊ0
    float   laserPower;         //���⹦�ʣ���λmW������ģʽ��Ϊ0
    quint16 laserRPF;			//����Ƶ��
    quint16 laserPulseWidth;	//������
    quint16 laserWaveLength;	//���Ⲩ��
    quint16 AOM_Freq;			//AOM��Ƶ��

    //ɨ�����
    quint16 elevationAngle;		//������
    quint16 start_azAngle;		//��ʼ��
    quint16 step_azAngle;		//������
    quint32 angleNum;			//������
    float   circleNum;			//Բ����
    bool    anglekey;			//�����
    bool    circlekey;			//Բ�ܼ�
    bool    continusdete;       //����̽��
    quint16 SP;					//�������ٶ�
    float   direct_intervalTime;	//�������
    float   time_circle_interval;	//Բ�ܼ���

    //��������
    quint16 sampleFreq;			//����Ƶ��
    float   detRange;   		//�������룬ɾ��
    quint32 sampleNum;			//����������ɾ��
    quint16 Trigger_Level;      //������ƽ
    int     PreTrigger;         //Ԥ�����������������ݲ��ṩ����

    //ʵʱ�������
    quint16 plsAccNum;			//�������ۼ�������
    quint16 nRangeBin;          //��������
    quint16 nPointsPerBin;      //�������ڵ���

    //���ݲ���
    float velocity_band;        //������ٷ�Χ,��m/s


    //�ļ��洢
    QString DatafilePath;		//���ݴ洢·��
    bool autocreate_datafile;	//�Զ����������ļ���

}ACQSETTING;

#endif // ACQSETTINGS_H
