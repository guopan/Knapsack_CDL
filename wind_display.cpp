#include "wind_display.h"
#include "direction_needle.h"
#include "speed_bar.h"
#include "heights_label.h"
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDateTime>
#include <QPainter>
#include <QFont>
#include <QRectF>
#include <QPen>
#include <QTime>

wind_display::wind_display(QWidget *parent) : QWidget(parent)
{
    // 设置探测高度标签QLabel *DetectedAttitude
    DetectedAttitude = new QLabel(this) ;
    DetectedAttitude -> setText(QString::fromLocal8Bit("探测高度")); //设置label内容
    DetectedAttitude -> setStyleSheet("QLabel{background-color:rgb(200,100,0);"  //设置前景色、背景色、加粗
                                      "color: white;"
                                      "font:bold;"
                                      "}");

    QFont font ( "Microsoft YaHei", 20, 100);
    DetectedAttitude ->setFont(font);
    DetectedAttitude->setAlignment(Qt::AlignCenter); //设置文字居中

    // 水平风标签
    HorizontalDirection= new QLabel(this);
    HorizontalDirection -> setText(QString::fromLocal8Bit("水平风向"));      //设置label内容
    HorizontalDirection -> setStyleSheet("QLabel{background-color:rgb(32,26,100);"  //设置前景色、背景色、加粗
                                         "color: white;"
                                         "font:bold;"
                                         "}");

    HorizontalDirection ->setFont(font);
    HorizontalDirection->setAlignment(Qt::AlignCenter); //设置文字居中

    // 水平风速标签
    HorizontalVelocity= new QLabel(this) ;
    HorizontalVelocity -> setText(QString::fromLocal8Bit("水平风速")); //设置label内容
    HorizontalVelocity -> setStyleSheet("QLabel{background-color:rgb(0,176,240);"  //设置前景色、背景色、加粗
                                        "color: white;"
                                        "font:bold;"
                                        "}");
    HorizontalVelocity ->setFont(font);
    HorizontalVelocity->setAlignment(Qt::AlignCenter); //设置文字居中

    // 垂直风标签
    VerticalVelocity= new QLabel(this) ;
    VerticalVelocity -> setText(QString::fromLocal8Bit("垂直风")); //设置label内容
    VerticalVelocity -> setStyleSheet("QLabel{background-color:rgb(0,176,240);"  //设置前景色、背景色、加粗
                                        "color: white;"
                                        "font:bold;"
                                        "}");
    VerticalVelocity ->setFont(font);
    VerticalVelocity->setAlignment(Qt::AlignCenter); //设置文字居中

    Glayout = new QGridLayout;
    Hlayout = new QHBoxLayout;
    Vlayout = new QVBoxLayout;

    Hlayout->addWidget(DetectedAttitude); //第1行第1列
    Hlayout->addWidget(HorizontalDirection); //第1行，第2列
    Hlayout->addWidget(HorizontalVelocity); //第1行，第3列
    Hlayout->addWidget(VerticalVelocity); //第1行，第4列


    for (int j = 0; j < nLayers; ++j)
    {

        // 设置100~1000米的10个标签
        my_height[j] = new heights_label;
        my_height[j]->setHeight(0);
        Glayout->addWidget(my_height[j], j, 0);

        //对第二列水平风进行网格布局
        my_needle[j] = new direction_needle;
        my_needle[j]->setHSpeed(0);
        my_needle[j]->setHdirection(H_direction[j]);
        Glayout->addWidget(my_needle[j],j,1); //第2行，第2列

        // 对垂直风进行网格布局
        my_bar[j] = new speed_bar;
        my_bar[j]->setHSpeed(0);
        my_bar[j]->setVSpeed(0);
        Glayout->addWidget(my_bar[j],j,2);    //第2行，第2列

    }

    Glayout->setColumnStretch(0, 2 );
    Glayout->setColumnStretch(1, 4 );
    Glayout->setColumnStretch(2, 5 );
    Vlayout->addLayout(Hlayout);
    Vlayout->addLayout(Glayout);
    Vlayout->setStretch(0,1);
    Vlayout->setStretch(1,nLayers);

    Hlayout->setStretch(0,2);
    Hlayout->setStretch(1,2.25);
    Hlayout->setStretch(2,3.75);
    Hlayout->setStretch(3,2);
    setLayout(Vlayout);    //整体布局
}

void wind_display::setHSpeed(const double *sp)
{
    for(int i=0;i<nLayers;i++)
        if(HSpeed[i] != *(sp+i))
        {
            HSpeed[i] = *(sp+i);
            my_needle[i]->setHSpeed(HSpeed[i]);
            my_bar[i]->setHSpeed(HSpeed[i]);
        }
}

void wind_display::setHDirection(const double *dir)
{
    for(int i=0;i<nLayers;i++)
        if(H_direction[i] != *(dir+i))
        {
            H_direction[i] = *(dir+i);
            my_needle[i]->setHdirection(H_direction[i]);
        }
}

void wind_display::setVSpeed(const double *sp)
{
    for(int i=0;i<nLayers;i++)
        if(VSpeed[i] != *(sp+i))
        {
            VSpeed[i] = *(sp+i);
            my_bar[i]->setVSpeed(VSpeed[i]);
        }
}

void wind_display::setHeights(const double *h)
{
    for(int i=0;i<nLayers;i++)
        if(Heights[i] != *(h+i))
        {
            Heights[i] = *(h+i);
            my_height[i]->setHeight(Heights[i]);
        }
}


//int display_instantaneous_windspeed::setLayerNum(int n)
//{
//    return n;

//}






