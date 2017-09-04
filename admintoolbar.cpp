#include "admintoolbar.h"

AdminToolBar::AdminToolBar()
{
    adqParamSetAction = this->addAction(QString::fromLocal8Bit("²É¼¯¿¨"));
    setMovable(false);
}
