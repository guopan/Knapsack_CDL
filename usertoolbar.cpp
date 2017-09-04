#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    quitAction = this->addAction(QString::fromLocal8Bit("�˳�"));
    startAction = this->addAction(QString::fromLocal8Bit("��ʼ"));
    setIconSize(QSize(48, 48));
    quitAction->setIcon(QIcon(":/images/Standby.png"));
    setMovable(false);
}
