#ifndef RT_TITLE_H
#define RT_TITLE_H

#include <QWidget>
#include <QRectF>
#include <QDebug>
class rt_title : public QWidget
{
    Q_OBJECT
public:
    explicit rt_title(QWidget *parent = nullptr);
public:
    void setHSizeHint(int hhh);
signals:

public slots:

private:
    int margin;
    int HSizeHint;      // Widget¸ß¶È
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // RT_TITLE_H
