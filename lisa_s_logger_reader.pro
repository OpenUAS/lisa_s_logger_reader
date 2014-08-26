#-------------------------------------------------
#
# Project created by QtCreator 2014-08-21T14:12:42
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lisa_s_logger_reader
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qpushbuttonprogress.cpp

HEADERS  += mainwindow.h \
    qpushbuttonprogress.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
