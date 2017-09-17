#ifndef SETTINGFILE_H
#define SETTINGFILE_H
#include <QString>
#include <QSettings>
#include <QDebug>
#include <QtCore>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "global_defines.h"

class settingfile
{
public:
    settingfile();

    //�����ļ�
    ACQSETTING get_settings(void);
    void init_fsetting(const ACQSETTING &setting);
    void writeTo_file(const ACQSETTING &setting,const QString &a);	//д���ļ���
    void readFrom_file(const QString &b);							//���ļ��ж�ȡ
    void checkValid();												//�����ص������ļ��Ƿ���Ч
    void test_create_file(const QString &c);						//��������ļ��Ƿ���ڣ�����������
    bool isSettingsChanged(const ACQSETTING &setting);				//���ô��ڲ����޸ĺ�δ����ʱ��ʾ�Ƿ񱣴�

    //��¼�ļ�
    void updatelogFile(const QString &addInstruct);					//��¼�ɼ�����

private:
    ACQSETTING fsetting;
    QString LF_path;												//��¼�ɼ��ļ���·��
    QString instruct_str;											//����˵��
};

#endif // SETTINGFILE_H
