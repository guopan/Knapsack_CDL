#ifndef RT_DISPLAY_H
#define RT_DISPLAY_H

#include <QWidget>
#include <QRectF>
#include <QDebug>
class rt_display : public QWidget
{
    Q_OBJECT
public:
    explicit rt_display(QWidget *parent = nullptr);
public:
    void setHeight(const int h);
    void setHSpeed(const double sp);
    void setVSpeed(const double sp);
    void setHdirection(const double dir);
signals:
    void data_changed();

public slots:

private:
    QRectF textRectF(double radius, int pointSize, double angle);
    double H_speed;
    double V_speed;
    double H_direction;
    double H;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // RT_DISPLAY_H
