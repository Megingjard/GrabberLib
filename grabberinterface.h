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
    void grabberErrorHandler( const char* pszFnName, etStat eErrCode, const char* pszDescString);
    void grabberCallack( tHandle hCamera, ui32 dwInterruptMask, void *pvParams );

signals:

    void grabberStatusChanged();
    void messageOutput(const QString& message);
    void updateImageCounter(const uint count);

public slots:
    void setImageCounterUsage(bool useEventCounter);

protected:
    void run();

private:

    QString cameraConfigFile;
    tPhxCmd phxGrabberInfo;
    tHandle hCamera;
    tPHX hBuffer1;
    tPHX hBuffer2;
    stImageBuff asImageBuffs[3];
    EventContext   eventContext;
    bool _isOpened;

    QString _lastError;

    bool _useImageCounter;
    uint _ImageCounter;

    // - - - - - - - - - -

    bool configureGrabber();
    bool startGrabber();
    void releaseGrabber();

    void setState(bool isOpened);
    void resetImageCounter();
};


#endif // GRABBERINTERFACE_H
