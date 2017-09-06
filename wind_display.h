#ifndef WIND_DISPLAY_H
#define WIND_DISPLAY_H
#include <QDebug>
#include <QWidget>
#include "rt_display.h"
#include "rt_title.h"
#include "rt_scale.h"
#include "global_defines.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>

class wind_display : public QWidget
{
    Q_OBJECT
public:
    explicit wind_display(QWidget *parent = 0);
    ~wind_display();
public slots:
    void setHSpeed(const double *sp);
    void setHDirection(const double *dir);
    void setVSpeed(const double *sp);
    void setHeights(const double *h);
    void setSubSize();

signals:
    void HSpeed_changed();
    void VSpeed_changed();
    void HDirection_changed();
    void Heights_changed();

private:
    rt_display *my_display [nLayers];
    rt_title *my_title;
    rt_scale *my_scale;

    QVBoxLayout *Vlayout;
    double HSpeed[nLayers];
    double H_direction[nLayers];
    double VSpeed[nLayers];
    double Heights[nLayers];
    double Height_low;
    double Heithe_high;
    int HSpeedScale;

//    void resizeEvent(QResizeEvent * event);

};

#endif // WIND_DISPLAY_H
