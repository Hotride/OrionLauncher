#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T16:45:23
#
#-------------------------------------------------
VERSION = 1.0.0.6

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       += core gui

LIBS = libshell32 libwininet libversion

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OrionLauncher
TEMPLATE = app

SOURCES += main.cpp\
        orionlauncherwindow.cpp \
    serverlistitem.cpp \
    proxylistitem.cpp \
    updatelistitem.cpp

HEADERS  += orionlauncherwindow.h \
    serverlistitem.h \
    proxylistitem.h \
    qzipreader_p.h \
    updatelistitem.h

FORMS    += orionlauncherwindow.ui
