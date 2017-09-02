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
    compass.cpp \
    compassthread.cpp \
    motor.cpp \
    motorthread.cpp \
    adq214.cpp \
    direction_needle.cpp \
    heights_label.cpp \
    speed_bar.cpp \
    wind_display.cpp

HEADERS += \
        mainwindow.h \
    compass.h \
    compassthread.h \
    motor.h \
    motorthread.h \
    adq214.h \
    direction_needle.h \
    heights_label.h \
    speed_bar.h \
    wind_display.h \
    global_defines.h

FORMS += \
        mainwindow.ui
win32: LIBS += -L$$quote(C:/Program Files/SP Devices/ADQAPI/) -lADQAPI
INCLUDEPATH += $$quote(C:/Program Files/SP Devices/ADQAPI/)

RESOURCES += \
    icons.qrc
