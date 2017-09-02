#include "speed_bar.h"
#include <QTimer>
#include <QtMath>
#include <QDateTime>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include <QFont>
speed_bar::speed_bar(QWidget *parent) : QWidget(parent)
{
    H_speed = 0;
    V_speed = 0;
    connect(this, SIGNAL(data_changed()), this, SLOT(update()));
}

QRectF speed_bar::textRectF(double radius, int pointSize, double angle)
{
    QRectF rectF;
    rectF.setX(radius*qCos(angle*M_PI/180.0) - pointSize*2);
    rectF.setY(radius*qSin(angle*M_PI/180.0) - pointSize/2.0);
    rectF.setWidth(pointSize*4);
    rectF.setHeight(pointSize);
    return rectF;
}

void speed_bar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    // 方向指针位置 - 多边形
    QPoint VS_Triangle_P[3] = {
        QPoint(0, -80),
        QPoint(80, 80),
        QPoint(0, 80)
    };

    QPoint VS_Triangle_N[3] = {
        QPoint(0, 80),
        QPoint(80, -80),
        QPoint(0, -80)
    };

    // 各种颜色定义
    QColor bgColor(240, 240, 240, 255);
    QColor bgColor_2(0, 176, 240, 255);

    int side = qMin(width()/5, height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // 平移坐标系原点至中心点
    painter.translate(0, height() / 2);
    // 缩放
    painter.scale(side / 200.0, side / 200.0);

    // 绘制矩形条
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor_2);
    painter.drawRect(0, -80, H_speed*60, 160);
    painter.restore();

//     绘制垂直风速三角形
    QRectF rectF;
    rectF.setX(0);
    rectF.setY(0);
    rectF.setWidth(50);
    rectF.setHeight(qAbs(V_speed)/3*80);

    painter.save();
    painter.translate(H_speed*60, 0);
    painter.setPen(Qt::NoPen);
//    qDebug()<<V_speed;
    if(V_speed>=0)
    {
        painter.setBrush(Qt::red);
        VS_Triangle_P[1].setX(qRound(V_speed*8/16*80/3));
        VS_Triangle_P[1].setY(qRound(V_speed*80/1.5-80));
        VS_Triangle_P[2].setY(qRound(V_speed*80/1.5-80));
        painter.drawConvexPolygon(VS_Triangle_P, 3);
    }
    else
    {
        painter.setBrush(QColor(0,176,0));
        VS_Triangle_N[1].setX(qRound(V_speed*(-8)/16*80/3));
        VS_Triangle_N[1].setY(qRound(V_speed*80/1.5+80));
        VS_Triangle_N[2].setY(qRound(V_speed*80/1.5+80));
        painter.drawConvexPolygon(VS_Triangle_N, 3);
    }
    painter.restore();

    // 绘制垂直风速文本
    painter.save();
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(35);
    font.setFamily("Microsoft YaHei");
    painter.setFont(font);

    QRectF rectF_v;
    rectF_v.setX(800);
    rectF_v.setY(-100);
    rectF_v.setWidth(200);
    rectF_v.setHeight(200);

    if(V_speed>=0)
    {
        painter.setPen(Qt::red);
        painter.drawText(rectF_v, Qt::AlignCenter, QString::number(V_speed, 'f', 2)); //// 保留2位小数
    }
    else
    {
        painter.setPen(QColor(0,176,0));
        painter.drawText(rectF_v, Qt::AlignCenter, QString::number(V_speed, 'f', 2));
    }
    painter.restore();

}

// 缩放
void speed_bar::setHSpeed(const double sp)
{
    if(H_speed != sp)
    {
        H_speed = sp;
        emit data_changed();
    }
}

void speed_bar::setVSpeed(const double sp)
{
    if(V_speed != sp)
    {
        V_speed = sp;
        emit data_changed();
    }
}
