#ifndef HEIGHTS_LABEL_H
#define HEIGHTS_LABEL_H
#include <QWidget>
#include <QPainter>
#include <qDebug>
#include <QRectF>


class heights_label : public QWidget
{
    Q_OBJECT
public:
    explicit heights_label (QWidget *parent = 0);

public:
    void setHeight(int hei);
signals:
    void data_changed();
public slots:
    //    void data_height();

private:
    int H;
    int n;
    QRectF RectF;

protected:
    void paintEvent(QPaintEvent *event) override;
};
#endif // HEIGHTS_LABEL_H
