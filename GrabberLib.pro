QT += core gui
QT += core gui opengl

TARGET = GrabberLib
TEMPLATE = app

SOURCES += main.cpp \
    grabberinterface.cpp \
    grabberconsolewidget.cpp \
    grabberthread.cpp \
    previewwidget.cpp

HEADERS += \
    grabberinterface.h \
    grabberconsolewidget.h \
    grabberthread.h \
    previewwidget.h

include(PhoenixSDK.pri)

OTHER_FILES +=

RESOURCES += \
    cameraconfig.qrc




