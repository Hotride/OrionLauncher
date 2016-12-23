#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T16:45:23
#
#-------------------------------------------------

QT       += core gui

LIBS = libshell32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OrionLauncher
TEMPLATE = app

SOURCES += main.cpp\
        orionlauncherwindow.cpp \
    serverlistitem.cpp \
    proxylistitem.cpp

HEADERS  += orionlauncherwindow.h \
    serverlistitem.h \
    proxylistitem.h

FORMS    += orionlauncherwindow.ui
