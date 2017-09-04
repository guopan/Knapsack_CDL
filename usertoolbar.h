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
};

#endif // USERTOOLBAR_H
