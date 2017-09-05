#ifndef WIND_DISPLAY_H
#define WIND_DISPLAY_H
#include <QDebug>
#include <QWidget>
#include "rt_display.h"
#include "rt_title.h"
#include "global_defines.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

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

signals:
    void HSpeed_changed();
    void VSpeed_changed();
    void HDirection_changed();
    void Heights_changed();

private:
    rt_display *my_display [nLayers];
    rt_title *my_title;

    QVBoxLayout *Vlayout;
    double HSpeed[nLayers];
    double H_direction[nLayers];
    double VSpeed[nLayers];
    double Heights[nLayers];
    double Height_low;
    double Heithe_high;

};

#endif // WIND_DISPLAY_H
