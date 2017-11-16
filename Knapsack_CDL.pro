#-------------------------------------------------
#
# Project created by QtCreator 2017-09-02T10:19:25
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Knapsack_CDL
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    devices\compass.cpp \
    devices\compassthread.cpp \
    devices\motor.cpp \
    devices\motorthread.cpp \
    devices\adq214.cpp \
    display\wind_display.cpp \
    display\rt_display.cpp \
    display\rt_title.cpp \
    usertoolbar.cpp \
    admintoolbar.cpp \
    display\rt_scale.cpp \
    settingfile.cpp \
    devices\laserseed.cpp \
    devices\laserpulse.cpp \
    devices\laserseedthread.cpp \
    devices\laserpulsethread.cpp \
    dswf.cpp \
    parametersetdialog.cpp \
    dispsettings.cpp \
    devicescontrol.cpp \
    fswf.cpp \
    display\stdata.cpp \
    display\stgraph.cpp

HEADERS += \
        mainwindow.h \
    devices\compass.h \
    devices\compassthread.h \
    devices\motor.h \
    devices\motorthread.h \
    devices\adq214.h \
    display\wind_display.h \
    global_defines.h \
    display\rt_display.h \
    display\rt_title.h \
    usertoolbar.h \
    admintoolbar.h \
    display\rt_scale.h \
    settingfile.h \
    devices\laserseed.h \
    devices\laserpulse.h \
    devices\laserseedthread.h \
    devices\laserpulsethread.h \
    dswf.h \
    parametersetdialog.h \
    dispsettings.h \
    devicescontrol.h \
    fswf.h \
    display\stdata.h \
    display\stgraph.h

FORMS += \
        mainwindow.ui \
    parametersetdialog.ui
win32: LIBS += -L$$quote(C:/Program Files/SP Devices/ADQAPI/) -lADQAPI
INCLUDEPATH += \
    $$quote(C:/Program Files/SP Devices/ADQAPI/)\
 C:\eigen\
.\diplay\
 C:/nlopt
RESOURCES += \
    icons.qrc
#INCLUDEPATH +=C:/nlopt
LIBS += -LC:/nlopt -lnlopt
CONFIG += qwt
