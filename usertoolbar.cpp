#include "usertoolbar.h"

UserToolBar::UserToolBar()
{
    quitAction = this->addAction(QString::fromLocal8Bit("�˳�"));
    startAction = this->addAction(QString::fromLocal8Bit("��ʼ"));
}
