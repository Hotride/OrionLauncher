#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T16:45:23
#
#-------------------------------------------------

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       += core gui concurrent

include("$$PWD/UpdateManager/UpdateManager.pri")

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OrionLauncher
TEMPLATE = app

SOURCES += main.cpp\
        orionlauncherwindow.cpp \
    serverlistitem.cpp \
    proxylistitem.cpp \
    changelogform.cpp

HEADERS  += orionlauncherwindow.h \
    serverlistitem.h \
    proxylistitem.h \
    qzipreader_p.h \
    changelogform.h

FORMS    += orionlauncherwindow.ui \
    changelogform.ui

OTHER_FILES += orionlauncher.rc

RC_FILE = orionlauncher.rc

RESOURCES += \
    res.qrc
