#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    startAction = this->addAction(QString::fromLocal8Bit("开始"));
    quitAction = this->addAction(QString::fromLocal8Bit("退出"));

    setIconSize(QSize(48, 48));
    startAction->setIcon(QIcon(":/images/Start.png"));
    quitAction->setIcon(QIcon(":/images/Standby.png"));
    setMovable(false);
    startIcon = true;       // 当前图标，start
    connect(startAction,&QAction::triggered,this, &UserToolBar::changeStartIcon);

}

void UserToolBar::changeStartIcon()
{
    if(startIcon)
    {
        startAction->setIcon(QIcon(":/images/Stop.png"));
        startIcon = false;
    }
    else
    {
        startAction->setIcon(QIcon(":/images/Start.png"));
        startIcon = true;
    }
}

void UserToolBar::set_to_stopped()      //设定为停止状态，即图标为可以开始
{
    startAction->setIcon(QIcon(":/images/Start.png"));
    startIcon = true;
}
