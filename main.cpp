#include <QtGui>
#include <QApplication>

#include "grabberconsolewidget.h"
#include "grabberinterface.h"
#include "grabberthread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GrabberInterface grabberinterface;

    GrabberThread grabberThread;
    grabberThread.setInterface(&grabberinterface);

    GrabberConsoleWidget grabberConsoleWidget;
    grabberConsoleWidget.setInterface(&grabberinterface, &grabberThread);
    grabberConsoleWidget.show();

    return a.exec();
}
