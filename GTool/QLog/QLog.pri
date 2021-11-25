HEADERS += \
    $$PWD/QLoghelper.h

SOURCES += \
    $$PWD/QLoghelper.cpp




include             ($$PWD/log4qt/log4qt.pri)


INCLUDEPATH         += $$PWD
INCLUDEPATH         += $$PWD/log4qt
