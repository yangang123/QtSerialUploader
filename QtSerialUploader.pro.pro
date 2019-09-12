#-------------------------------------------------
#
# Project created by QtCreator 2019-01-27T09:51:43
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialUploader
TEMPLATE = app


SOURCES +=src/crc16.c \
    src/dialog.cpp \
    src/LinkInterface.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/RtkConfig.cpp \
    src/SerialLink.cpp \
    src/packet.c

HEADERS  += include/crc16.h \
    include/dialog.h \
    include/LinkInterface.h \
    include/mainwindow.h \
    include/packet.h \
    include/RtkConfig.h \
    include/SerialLink.h

FORMS    +=  mainwindow.ui \
    dialog.ui


RESOURCES += \
    res.qrc

SUBDIRS += \
    wz_uploader3.pro \
    wz_uploader3.pro \
    wz_uploader3.pro

INCLUDEPATH += include
