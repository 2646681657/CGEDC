QT += core gui widgets serialport

CONFIG += c++11

TARGET = droplet_control_qt
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

RESOURCES += \
    resources.qrc
