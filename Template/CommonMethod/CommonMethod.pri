INCLUDEPATH         += $$PWD
INCLUDEPATH         += $$PWD/SIForm
INCLUDEPATH         += $$PWD/AutomationForm


HEADERS += \
    $$PWD/CommonMethod.h \
    $$PWD/SIForm/SICommonMethod.h \
    $$PWD/SIForm/SIExcelOperateMethod.h \
    $$PWD/SIForm/SIExcelOperateThread.h \
    $$PWD/SIForm/SIFileOperateThread.h \
    $$PWD/SIForm/SIFileOperateMethod.h \
    $$PWD/AutomationForm/AuExcelOperateMethod.h \
    $$PWD/AutomationForm/AuExcelOperateThread.h \
    $$PWD/AutomationForm/AuCommonMethod.h \
    $$PWD/AutomationForm/AuFileOperateMethod.h \
    $$PWD/AutomationForm/AuFileOperateThread.h

SOURCES += \
    $$PWD/CommonMethod.cpp \
    $$PWD/SIForm/SICommonMethod.cpp \
    $$PWD/SIForm/SIFileOperateThread.cpp \
    $$PWD/SIForm/SIFileOperateMethod.cpp \
    $$PWD/SIForm/SIExcelOperateThread.cpp \
    $$PWD/SIForm/SIExcelOperateMethod.cpp \
    $$PWD/AutomationForm/AuExcelOperateMethod.cpp \
    $$PWD/AutomationForm/AuExcelOperateThread.cpp \
    $$PWD/AutomationForm/AuCommonMethod.cpp \
    $$PWD/AutomationForm/AuFileOperateMethod.cpp \
    $$PWD/AutomationForm/AuFileOperateThread.cpp
