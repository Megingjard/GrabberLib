#include "grabberthread.h"
#include <QtGui>


void GrabberThread::setInterface (GrabberInterface *grabberinterface)
{
    _grabberinterface = grabberinterface;
}

void GrabberThread::run()
{
    _grabberinterface->open("D:/BaslerDefault.pcf");
}
