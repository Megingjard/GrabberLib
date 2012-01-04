#include <QtGui>
#include <QApplication>

#include "grabberconsolewidget.h"
#include "grabberinterface.h"
#include "grabberthread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GrabberInterface grabberinterface;

    GrabberConsoleWidget grabberConsoleWidget;
    grabberConsoleWidget.setInterface(&grabberinterface);
    grabberConsoleWidget.show();

    GrabberThread grabberThread;
    grabberThread.setInterface(&grabberinterface);
    grabberThread.start();

    return a.exec();
}
