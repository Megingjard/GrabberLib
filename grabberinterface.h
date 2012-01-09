#ifndef GRABBERINTERFACE_H
#define GRABBERINTERFACE_H

// Defining the OS (must be declared somewhere else)
#define _PHX_WIN32

#include <QtCore/QObject>
#include "phx_os.h"
#include "common.h"


// Define an application specific structure to hold user information
typedef struct
{
   volatile ui32 nBufferReadyCount;
   volatile tFlag fFifoOverFlow;
} EventContext;


//--------------------------------------------------------------------

class GrabberInterface : public QObject
{
    Q_OBJECT

public:
    GrabberInterface(QObject *parent = 0);
    virtual ~GrabberInterface();

    void open(QString cameraConfigFile);
    void close();

    bool isOpened() const;

    QString lastError() const;

signals:
    void messageOutput(const QString& message);
    void grabberStatusChanged();
    void updateEventCounter(const int count);

public slots:
    void setEventCounterUsage(bool useEventCounter);

protected:
    void run();

private:
    int     nStatus;
    QString _cameraConfigFile;
    QString _lastError;
    bool _isOpened;
    bool _useEventCounter;

    QString cameraConfigFile;
    tPhxCmd phxGrabberInfo;
    tHandle        hCamera;
    tPHX           hDisplay;
    tPHX           hBuffer1;
    tPHX           hBuffer2;
    EventContext   eventContext;         // User defined Event Context

    bool configureGrabber();
    bool livePicture();
    void releaseGrabber();
    void setState(bool isOpened);

};



#endif // GRABBERINTERFACE_H
