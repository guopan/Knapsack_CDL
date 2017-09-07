#include "admintoolbar.h"

AdminToolBar::AdminToolBar()
{
    setIconSize(QSize(48, 48));

    adqParamSetAction = this->addAction(QString::fromLocal8Bit("²É¼¯¿¨"));
    adqParamSetAction->setIcon(QIcon(":/images/Gear.png"));

    setMovable(false);
}
