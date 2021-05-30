INCLUDEPATH         += $$PWD
INCLUDEPATH         += $$PWD/SIForm
INCLUDEPATH         += $$PWD/AutomationForm


HEADERS += \
    $$PWD/CommonMethod.h \
    $$PWD/SIForm/SICommonMethod.h \
    $$PWD/SIForm/SIExcelOperateMethod.h \
    $$PWD/SIForm/SIExcelOperateThread.h \
    $$PWD/SIForm/SIFileOperateThread.h \
    $$PWD/SIForm/SIFileOperateMethod.h

SOURCES += \
    $$PWD/CommonMethod.cpp \
    $$PWD/SIForm/SICommonMethod.cpp \
    $$PWD/SIForm/SIFileOperateThread.cpp \
    $$PWD/SIForm/SIFileOperateMethod.cpp \
    $$PWD/SIForm/SIExcelOperateThread.cpp \
    $$PWD/SIForm/SIExcelOperateMethod.cpp
