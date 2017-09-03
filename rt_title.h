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

signals:

public slots:

private:
    int margin;
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // RT_TITLE_H
