#ifndef USERTOOLBAR_H
#define USERTOOLBAR_H
#include "QToolBar"
#include "QAction"
#include "QString"

class UserToolBar : public QToolBar
{
public:
    UserToolBar();
    QAction *quitAction;
    QAction *startAction;
public slots:
    void changeStartIcon();
private:
    bool startIcon;
};

#endif // USERTOOLBAR_H
