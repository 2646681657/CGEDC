QT += core gui widgets serialport

CONFIG += c++11

TARGET = droplet_control_qt
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    camerathread.cpp

HEADERS += \
    mainwindow.h \
    camerathread.h

RESOURCES += \
    resources.qrc

# OpenCV
INCLUDEPATH += /usr/include/opencv4 /usr/include/opencv2
LIBS += -L/usr/lib \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_videoio \
    -lopencv_imgcodecs
