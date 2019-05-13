#-------------------------------------------------
#
# Project created by QtCreator 2019-01-27T09:51:43
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialUploader
TEMPLATE = app


SOURCES += main.cpp\
         packet.c crc16.c \
    LinkInterface.cpp \
    SerialLink.cpp \
    RtkConfig.cpp \
    mainwindow.cpp

HEADERS  +=  packet.h crc16.h \
    LinkInterface.h \
    SerialLink.h \
    RtkConfig.h \
    mainwindow.h

FORMS    +=  mainwindow.ui


RESOURCES += \
    res.qrc
