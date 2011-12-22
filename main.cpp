#include <QtGui>
#include <QApplication>

#include "grabberconsolewidget.h"
#include "grabberinterface.h"
#include "livewidget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GrabberInterface grabberinterface;

    GrabberConsoleWidget grabberConsoleWidget;
    grabberConsoleWidget.setInterface(&grabberinterface);
    grabberConsoleWidget.show();

    grabberinterface.open("D:/BaslerDefault.pcf");

    return a.exec();
}
