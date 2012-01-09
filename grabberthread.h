#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include "grabberinterface.h"

class GrabberThread : public QThread
{
    Q_OBJECT

private:
    GrabberInterface *_grabberinterface;
    void run();

signals:

public slots:

public:
    void setInterface(GrabberInterface *grabberinterface);
};

#endif // CAMERATHREAD_H
