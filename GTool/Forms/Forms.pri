INCLUDEPATH         += $$PWD
INCLUDEPATH         += $$PWD/UI
INCLUDEPATH         += $$PWD/UIFont

HEADERS += \
    $$PWD/UI/OtherForm.h \
    $$PWD/UI/FirstForm.h \
    $$PWD/UIFont/FirstFormMethod.h \
    $$PWD/UI/LogForm.h \
    $$PWD/UI/GTool.h

SOURCES += \
    $$PWD/UI/GTool.cpp \
    $$PWD/UI/OtherForm.cpp \
    $$PWD/UI/FirstForm.cpp \
    $$PWD/UIFont/FirstFormMethod.cpp \
    $$PWD/UI/LogForm.cpp

FORMS    += \
    $$PWD/UI/GTool.ui \
    $$PWD/UI/OtherForm.ui \
    $$PWD/UI/FirstForm.ui \
    $$PWD/UI/LogForm.ui
