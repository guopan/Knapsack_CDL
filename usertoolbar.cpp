#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    startAction = this->addAction(QString::fromLocal8Bit("¿ªÊ¼"));
    quitAction = this->addAction(QString::fromLocal8Bit("ÍË³ö"));

    setIconSize(QSize(48, 48));
    startAction->setIcon(QIcon(":/images/Start.png"));
    quitAction->setIcon(QIcon(":/images/Standby.png"));
    setMovable(false);
}
