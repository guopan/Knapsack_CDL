#ifndef WIND_DISPLAY_H
#define WIND_DISPLAY_H
#include <QDebug>
#include <QWidget>
#include "direction_needle.h"
#include "speed_bar.h"
#include "heights_label.h"
#include "global_defines.h"
#include <QRect>
#include <QPen>
#include <QPainter>
#include <QPen>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

class wind_display : public QWidget
{
    Q_OBJECT
public:
    explicit wind_display(QWidget *parent = 0);
    QLabel *DetectedAttitude;
    QLabel *HorizontalDirection;
    QLabel *HorizontalVelocity;
    QLabel *VerticalVelocity;

    heights_label *my_height [nLayers];
    direction_needle *my_needle[nLayers];
    speed_bar *my_bar[nLayers];
    QGridLayout *Glayout;
    QVBoxLayout *Vlayout;
    QHBoxLayout *Hlayout;

public slots:
    void setHSpeed(const double *sp);
    void setHDirection(const double *dir);
    void setVSpeed(const double *sp);
    void setHeights(const double *h);

signals:
    void HSpeed_changed();
    void VSpeed_changed();
    void HDirection_changed();
    void Heights_changed();

private:
    double HSpeed[nLayers];
    double H_direction[nLayers];
    double VSpeed[nLayers];
    double Heights[nLayers];
    double Height_low;
    double Heithe_high;

    int n;
};

#endif // WIND_DISPLAY_H
