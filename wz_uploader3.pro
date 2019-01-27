#-------------------------------------------------
#
# Project created by QtCreator 2019-01-27T09:51:43
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wz_uploader3
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp packet.c crc16.c

HEADERS  += dialog.h packet.h crc16.h

FORMS    += dialog.ui
