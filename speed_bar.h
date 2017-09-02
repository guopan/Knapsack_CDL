#ifndef SPEED_BAR_H
#define SPEED_BAR_H
#include <QDebug>
#include <QWidget>
#include <QTimer>
#include <QRectF>
#include<QPalette>
#include<QLabel>
class speed_bar : public QWidget
{
    Q_OBJECT
public:
    explicit speed_bar(QWidget *parent = nullptr);

public:
    void setHSpeed(double sp);
    void setVSpeed(double sp);
signals:
    void data_changed();

public slots:


private:
    QRectF textRectF(double radius, int pointSize, double angle);
    double H_speed;
    double V_speed;
    QLabel  *VerticalWind1;
    QRectF rectF_v;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // speed_bar_H
