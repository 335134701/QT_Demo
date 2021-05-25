INCLUDEPATH         += $$PWD
INCLUDEPATH         += $$PWD/UICode
INCLUDEPATH         += $$PWD/LogicCode

HEADERS += \
    $$PWD/UICode/MessageForm.h \
    $$PWD/UICode/SIForm.h \
    $$PWD/UICode/MainForm.h 

SOURCES += \
    $$PWD/UICode/MessageForm.cpp \
    $$PWD/UICode/MainForm.cpp \
    $$PWD/UICode/SIForm.cpp 

FORMS    += \
    $$PWD/MessageForm.ui \
    $$PWD/MainForm.ui \
    $$PWD/SIForm.ui
