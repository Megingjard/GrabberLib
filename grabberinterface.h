#ifndef GRABBERINTERFACE_H
#define GRABBERINTERFACE_H

// Defining the OS (must be declared somewhere else)
#define _PHX_WIN32

#include <QtCore/QObject>
#include "phx_os.h"
#include "common.h"


//--------------------------------------------------------------------

class GrabberInterface : public QObject
{
    Q_OBJECT

public:
    GrabberInterface(QObject *parent = 0);
    virtual ~GrabberInterface();

    void open(QString cameraConfigFile);
    void close();

    QString lastError() const;

signals:
    void messageOutput(const QString& message);

public slots:

protected:
    void run();

private:
    int     nStatus;
    QString _cameraConfigFile;
    QString _lastError;
    bool _aboutLivePicture;

    tPhxCmd phxGrabberInfo;

    int livePicture(etCamConfigLoad eCamConfigLoad, QString cameraConfigFile);


};

/* Define an application specific structure to hold user information */
typedef struct
{
   volatile ui32 nBufferReadyCount;
   volatile tFlag fFifoOverFlow;
} PhxUserInfo;


#endif // GRABBERINTERFACE_H
