#include "direction_needle.h"
#include <QTimer>
#include <QtMath>
#include <QDateTime>
#include <QPainter>

direction_needle::direction_needle(QWidget *parent) : QWidget(parent)
{
    H_speed = 0;
    H_direction = 0;
    connect(this, SIGNAL(data_changed()), this, SLOT(update()));


}

QRectF direction_needle::textRectF(double radius, int pointSize, double angle)
{
    QRectF rectF;
    rectF.setX(radius*qCos(angle*M_PI/180.0) - pointSize*2);
    rectF.setY(radius*qSin(angle*M_PI/180.0) - pointSize/2.0);
    rectF.setWidth(pointSize*4);
    rectF.setHeight(pointSize);
    return rectF;
}

void direction_needle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    // 方向指针位置 - 多边形
    static const QPoint Needle_Points[4] = {
        QPoint(40, -70),
        QPoint(0, -40),
        QPoint(-40, -70),
        QPoint(0, 80)
    };

    // 定义各种颜色
    QColor bgColor(32, 56, 100, 255);
    QColor bgColor_2(0, 176, 240, 255);
    QColor MarklineColor(0, 127, 127, 150);
    QColor NeedleColor(255, 255, 255, 255);

    int side = qMin(width()/4, height());
//    QTime time = QTime::currentTime();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);    // 平移坐标系原点至中心点

    painter.scale(side / 200.0, side / 200.0);       // 缩放

    // 绘制风向大小文本
    QRectF rectF;
    rectF.setX(-400);
    rectF.setY(-100);
    rectF.setWidth(400);
    rectF.setHeight(200);
    QFont font = painter.font();
    font.setBold(true);
    font.setFamily("Microsoft YaHei");
    font.setPointSize(35);

    painter.setFont(font);
    painter.setPen(bgColor);
    QString str =  QString::number(H_direction, 'f', 2);
    str += QString::fromLocal8Bit("°");
    painter.drawText(rectF, Qt::AlignCenter, str);

    // 圆盘向右平移
    painter.translate(100, 0);
    // 绘制矩形背景
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor_2);
    painter.drawRect(0, -80, 400, 160);
    painter.restore();
    // 绘制圆形背景
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(0, 0), 100, 100);
    painter.setBrush(bgColor);
    painter.drawEllipse(QPointF(0, 0), 80, 80);
    painter.restore();

    // 绘制方向刻度线 （360度 / 8 = 45度）
    for (int i = 0; i < 8; ++i)
    {
        painter.drawLine(88, 0, 96, 0);
        painter.rotate(45.0);
    }

    int radius = 100;

    font.setBold(true);
    font.setPointSize(5);
    font.setFamily("Microsoft YaHei");
    painter.setFont(font);
    int pointSize = font.pointSize();
    pointSize = 20;
    painter.setPen(Qt::white);

    // 绘制方向角度文本
    int nHour = 0;
    for (int i = 0; i < 4; ++i) {
        nHour = i * 90+90;
        if (nHour >= 360)
            nHour -= 360;
        painter.drawText(textRectF(radius*0.6, pointSize, i * 90), Qt::AlignCenter, QString::number(nHour));
    }

    painter.setPen(MarklineColor);
    // 绘制细刻度线 （360度 / 16 = 22.5度）
    for (int j = 0; j < 16; ++j) {
        if ((j % 2) != 0)
            painter.drawLine(92, 0, 96, 0);
        painter.rotate(22.5);
    }

    // 绘制方向指针
    painter.setPen(Qt::NoPen);
    painter.setBrush(NeedleColor);

    painter.save();
    // 每圈360° = 60s 即：旋转角度 = 秒数 * 6°
    //    painter.rotate(6.0 * time.second());
    painter.rotate(H_direction);
    painter.drawConvexPolygon(Needle_Points, 4);
    painter.restore();

    // 绘制风速大小文本

    rectF.setX(100);
    rectF.setY(-100);
    rectF.setWidth(200);
    rectF.setHeight(200);

    font.setPointSize(35);

    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(rectF, Qt::AlignCenter, QString::number(H_speed, 'f', 2));




}

void direction_needle::setHSpeed(const double sp)
{
    if(H_speed != sp)
    {
        H_speed = sp;
        emit data_changed();
    }
}

void direction_needle::setHdirection(const double dir)
{
    if(H_direction != dir)
    {
        H_direction = dir;
        emit data_changed();
    }
}

