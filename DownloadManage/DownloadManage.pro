QT += core network
CONFIG += cmdline

SOURCES += main.cpp \
    downloadmanager.cpp \
    textprogressbar.cpp

HEADERS += \
    downloadmanager.h \
    textprogressbar.h

OTHER_FILES += \
    debian/changelog \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/README \
    debian/rules

