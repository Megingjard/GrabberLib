QT += core gui

TARGET = GrabberLib
TEMPLATE = app

SOURCES += main.cpp \
    grabberinterface.cpp \
    grabberconsolewidget.cpp

HEADERS += \
    grabberinterface.h \
    grabberconsolewidget.h

include(PhoenixSDK.pri)

OTHER_FILES +=




