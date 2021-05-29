INCLUDEPATH         += $$PWD
INCLUDEPATH         += $$PWD/UICode
INCLUDEPATH         += $$PWD/LogicCode

HEADERS += \
    $$PWD/UICode/SIForm.h \
    $$PWD/UICode/MainForm.h \  
    $$PWD/UICode/LogForm.h \
    $$PWD/LogicCode/LogFormMetod.h \
    $$PWD/LogicCode/MainFormMethod.h \
    $$PWD/LogicCode/SIFormMethod.h

SOURCES += \
    $$PWD/UICode/MainForm.cpp \
    $$PWD/UICode/SIForm.cpp \  
    $$PWD/UICode/LogForm.cpp \
    $$PWD/LogicCode/LogFormMetod.cpp \
    $$PWD/LogicCode/MainFormMethod.cpp \
    $$PWD/LogicCode/SIFormMethod.cpp

FORMS    += \
    $$PWD/MainForm.ui \
    $$PWD/SIForm.ui \
    $$PWD/LogForm.ui
