#ifndef PARAMETERSETDIALOG_H
#define PARAMETERSETDIALOG_H
#include "global_defines.h"
#include "settingfile.h"
#include <QFileDialog>
#include <QDialog>
#include <QMessageBox>
#include <dispsettings.h>
#include <QRegExp>
#include <QRegExpValidator>

namespace Ui {
class ParameterSetDialog;
}

class ParameterSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterSetDialog(QWidget *parent = 0);
    ~ParameterSetDialog();
    SOFTWARESETTINGS getParaSettings();
    void setParaSettings(SOFTWARESETTINGS settings);

private slots:

    void on_laserLocalPowerLineEdit_textChanged(const QString &arg1);

    void on_laserPulseEnergyLineEdit_textChanged(const QString &arg1);

    void on_laserRPFLineEdit_textChanged(const QString &arg1);

    void on_laserPulseWidthLineEdit_textChanged(const QString &arg1);

    void on_laserWaveLengthLineEdit_textChanged(const QString &arg1);

    void on_laserAOMFreqLineEdit_textChanged(const QString &arg1);

    void on_sampleFreqComboBox_activated(const QString &arg1);

    void on_triggerLevelLineEdit_textChanged(const QString &arg1);

    void on_nPointsPretriggerLineEdit_textChanged(const QString &arg1);

    void on_nPointsMirrorWidthLineEdit_textChanged(const QString &arg1);

    void on_nPointsPerBinLineEdit_textChanged(const QString &arg1);

    void on_overlapRatioComboBox_activated(const QString &arg1);

    void on_nRangeBinLineEdit_textChanged(const QString &arg1);

    void on_nPulsesAccLineEdit_textChanged(const QString &arg1);

    void on_nPointsObjFreqLineEdit_textChanged(const QString &arg1);

    void on_nDirsVectorCalLineEdit_textChanged(const QString &arg1);

    void on_detectModeComboBox_activated(int index);

    void on_anglekeyRadioButton_clicked(bool checked);


//    void on_angleNumLineEdit_editingFinished();

    void on_circlekeyRadioButton_clicked(bool checked);

//    void on_circleNumLineEdit_editingFinished();

    void on_azAngleStartLineEdit_textChanged(const QString &arg1);

    void on_azAngleStepLineEdit_textChanged(const QString &arg1);

    void on_motorSpeedLineEdit_textChanged(const QString &arg1);

    void on_elevationAngleLineEdit_textChanged(const QString &arg1);

    void on_intervalTimeLineEdit_textChanged(const QString &arg1);

    void on_groupTimeLineEdit_textChanged(const QString &arg1);

    void on_nDirsPerFileLineEdit_textChanged(const QString &arg1);

    void on_autoCreateDateDirCheckBox_clicked(bool checked);

    void on_pathModifyPushButton_clicked();

    void on_angleNumLineEdit_textChanged(const QString &arg1);

    void on_circleNumLineEdit_textChanged(const QString &arg1);

    void on_nPointsObjFreqLineEdit_editingFinished();

    void on_saveSettingsPushButton_clicked();

    void on_saveasSettingsPushButton_clicked();

    void on_loadSettingsPushButton_clicked();

    void on_confirmSetPushButton_clicked();

    void on_cancelSetPushButton_clicked();

    void on_resetSettingsPushButton_clicked();

private:
    Ui::ParameterSetDialog *ui;
    void refreshDisp();
    SOFTWARESETTINGS paraSettings;
    uint getObjFreqPoints();
    double getObjVelocity();
    double getVoltageTriggerLevel();
    double getPreTriggerTimeLength();
    double getRangeResolution();
    double getMinDetectRange();
    double getMaxDetectRange();
    SettingFile *paraSettingFile;

    void mouseDoubleClickEvent(QMouseEvent *event);

    //laser Parameters
    QRegExpValidator *laserPulseEnergyRegExpV;
    QRegExpValidator *laserLocalPowerRegExpV;
    QRegExpValidator *laserRPFRegExpV;
    QRegExpValidator *laserPulseWidthRegExpV;
    QRegExpValidator *laserWaveLengthRegExpV;
    QRegExpValidator *laserAOMFreqRegExpV;
    //Scan Parameters
    QRegExpValidator *elevationAngleRegExpV;
    QRegExpValidator *azAngleStartRegExpV;
    QRegExpValidator *azAngleStepRegExpV;
    QRegExpValidator *angleNumRegExpV;
    QRegExpValidator *circleNumRegExpV;
    QRegExpValidator *motorSpeedRegExpV;
    QRegExpValidator *intervalTimeRegExpV;
    QRegExpValidator *groupTimeRegExpV;
    //Sample Parameters
    QRegExpValidator *triggerLevelRegExpV;
    QRegExpValidator *nPointsPreTriggerRegExpV;
    //Real Time Process
    QRegExpValidator *nPulsesAccRegExpV;
    QRegExpValidator *nRangeBinRegExpV;
    QRegExpValidator *nPointsPerBinRegExpV;
    QRegExpValidator *nPointsMirrorWidthRegExpV;
    //Vector Velocity Inversion
    QRegExpValidator *nPointsObjFreqRegExpV;
    QRegExpValidator *nDirsVectorCalRegExpV;
    //File Store
    QRegExpValidator *nDirsPerFileRegExpV;
};

#endif // PARADIALOG_H
