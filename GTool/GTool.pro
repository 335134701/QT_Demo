#-------------------------------------------------
#
# Project created by QtCreator 2021-11-13T10:14:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GTool
TEMPLATE = app

DESTDIR             = bin

RC_ICONS +=         Config/Images/GTool.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES           += Config/Config.qrc

include             ($$PWD/Forms/Forms.pri)
include             ($$PWD/QLog/QLog.pri)

INCLUDEPATH         += $$PWD
INCLUDEPATH         += $$PWD/Forms
INCLUDEPATH         += $$PWD/QLog


SOURCES += \
        main.cpp 

HEADERS += \
        COMMONDEFINE.h

