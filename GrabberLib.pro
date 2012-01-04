QT += core gui

TARGET = GrabberLib
TEMPLATE = app

SOURCES += main.cpp \
    grabberinterface.cpp \
    grabberconsolewidget.cpp \
    grabberthread.cpp

HEADERS += \
    grabberinterface.h \
    grabberconsolewidget.h \
    grabberthread.h

include(PhoenixSDK.pri)

OTHER_FILES +=




