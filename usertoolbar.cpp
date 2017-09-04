#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    quitAction = this->addAction(QString::fromLocal8Bit("ÍË³ö"));
    startAction = this->addAction(QString::fromLocal8Bit("¿ªÊ¼"));
}
