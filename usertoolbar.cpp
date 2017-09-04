#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    quitAction = this->addAction(QString::fromLocal8Bit("ÍË³ö"));
    startAction = this->addAction(QString::fromLocal8Bit("¿ªÊ¼"));
    setIconSize(QSize(48, 48));
    quitAction->setIcon(QIcon(":/images/Standby.png"));
    setMovable(false);
}
