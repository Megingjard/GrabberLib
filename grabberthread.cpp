#include "grabberthread.h"
#include <QtGui>


void GrabberThread::setInterface (GrabberInterface *grabberinterface)
{
    _grabberinterface = grabberinterface;
}

void GrabberThread::run()
{
    _grabberinterface->open("D:/Basler_8Bit_Mono.pcf");

//    _grabberinterface->open(":/cameraconfig/Basler_8Bit_Mono.pcf");
}
