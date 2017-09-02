#ifndef DIRECTION_NEEDLE_H
#define DIRECTION_NEEDLE_H

#include <QWidget>
#include <QTimer>
#include <QRectF>
#include <QDebug>
class direction_needle : public QWidget
{
    Q_OBJECT
public:
    explicit direction_needle(QWidget *parent = nullptr);

public:
    void setHSpeed(double sp);
    void setHdirection(double dir);
signals:
    void data_changed();

public slots:


private:
    QRectF textRectF(double radius, int pointSize, double angle);
    double H_speed;
    double H_direction;
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // DIRECTION_NEEDLE_H
