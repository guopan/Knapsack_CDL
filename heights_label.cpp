#include "heights_label.h"
#include <QPainter>
#include <QFont>
#include <QRectF>
#include <QPen>
#include <qDebug>
#include <QtMath>

heights_label::heights_label(QWidget *parent) : QWidget(parent)
{
    H = 0;
    connect(this, SIGNAL(data_changed()), this, SLOT(update()));
}

void heights_label::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int side = qMin(width()/2, height());
    // 平移坐标系原点至中心点
    painter.translate(width() / 2, height() / 2);
    // 缩放
    painter.scale(side / 200.0, side / 200.0);

    // 颜色定义
    QColor bgColor_DA(200, 100, 0, 255);
    painter.setPen(QColor(bgColor_DA));
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(35);
    font.setBold(true);
    painter.setFont(font);

    QRectF RectF;
    RectF.setX(-200);
    RectF.setY(-100);
    RectF.setWidth(400);
    RectF.setHeight(200);

    QString str =  QString::number(H, 'f', 2);
    str += "m";
    painter.drawText(RectF, Qt::AlignCenter, str);
//    qDebug()<<str;
}

void heights_label::setHeight(const int h)
{
    if(H != h)
    {
        H = h;
        emit data_changed();
    }
}
