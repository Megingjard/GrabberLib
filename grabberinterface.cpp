#include "grabberinterface.h"
#include "common.h"
#include "common.c"
#include "pdl_api.h"
#include "phx_api.h"
#include "phx_os.h"

#include <QtGui>
#include <QThread>

#define MAX_SZ_CHARS 256   /* Maximum string length */


GrabberInterface::GrabberInterface(QObject *parent) :
    QObject(parent)
{
    _isOpened = false;
    _useEventCounter = false;
}

GrabberInterface::~GrabberInterface()
{
    close();
}

static void handleInterruptEvents(tHandle hCamera, ui32 interruptMask, void *userParameter )
{
   EventContext *eventContext = (EventContext*) userParameter;

   (void) hCamera;

   if ( PHX_INTRPT_BUFFER_READY & interruptMask )
       eventContext->nBufferReadyCount++;

   if ( PHX_INTRPT_FIFO_OVERFLOW & interruptMask )
       eventContext->fFifoOverFlow = TRUE;

    // The callback routine may be called with more than 1 event flag set.
    // Therefore all possible events must be handled here.

   if ( PHX_INTRPT_FRAME_END & interruptMask )
   {
   }
}


//--------------------------------------------------------------------

void GrabberInterface::open(QString _cameraConfigFile)
{
    if (isOpened()){
        messageOutput("Connection already Opened!");
        return;
    }

    cameraConfigFile = _cameraConfigFile;

    bool oState = configureGrabber();

    setState(oState);

    if (isOpened())
        oState = livePicture();

    if (!oState)
        messageOutput(_lastError + "\nAborting...\n");

    releaseGrabber();
}

void GrabberInterface::close()
{
    setState(false);
}

void GrabberInterface::setState(bool isOpened)
{
    _isOpened = isOpened;
    grabberStatusChanged();
}

void GrabberInterface::setEventCounterUsage(bool useEventCounter)
{
    _useEventCounter = useEventCounter;
}

QString GrabberInterface::lastError() const
{
    return _lastError;
}

bool GrabberInterface::isOpened() const
{
    return _isOpened;
}

//------ This function should be a member of GrabberInterface --------
void grabberErrorHandler( const char* pszFnName, etStat eErrCode, const char* pszDescString )
{
    QString Error = "Phx Error: "
                    + QString::fromAscii(pszFnName, strlen(pszFnName)) + " : "
                    + QString::number(eErrCode) + " : "
                    + QString::fromAscii(pszDescString, strlen(pszDescString));

    qDebug() << Error;

    //messageOutput(_lastError + "\nAborting...\n");
}

bool GrabberInterface::configureGrabber()
{
   etStat         eStatus     = PHX_OK;
   etParamValue   eParamValue;          // Parameter for use with PHX_ParameterSet/Get calls

   hCamera   = 0;
   hDisplay  = 0;
   hBuffer1  = 0;
   hBuffer2  = 0;

   char *argv[0];

   PhxCommonParseCmd( 0, argv, &phxGrabberInfo );
   PhxCommonKbInit();

   memset( &eventContext, 0, sizeof(eventContext));

   phxGrabberInfo.eCamConfigLoad = (etCamConfigLoad) ( PHX_DIGITAL | PHX_ACQ_ONLY | PHX_CHANNEL_AUTO );

   // Allocate the board with the config file
   eStatus = PHX_CameraConfigLoad( &hCamera,
                                   cameraConfigFile.toAscii().data(),
                                   phxGrabberInfo.eCamConfigLoad,
                                   &grabberErrorHandler );
   if ( PHX_OK != eStatus ) {
       _lastError = "camera configuaration error";
       return false;
   }

   // Create display with a NULL hWnd, this will automatically create an image window.
   eStatus = PDL_DisplayCreate( &hDisplay,
                                NULL,
                                hCamera,
                                &grabberErrorHandler );
   if ( PHX_OK != eStatus ) {
       _lastError = "display creation error";
       return false;
   }

   // Create two display buffers for double buffering
   eStatus = PDL_BufferCreate( &hBuffer1,
                               hDisplay,
                               (etBufferMode)PDL_BUFF_SYSTEM_MEM_DIRECT );
   if ( PHX_OK != eStatus ) {
       _lastError = "buffer allocation error: buffer 1";
       return false;
   }

   eStatus = PDL_BufferCreate( &hBuffer2,
                               hDisplay,
                               (etBufferMode)PDL_BUFF_SYSTEM_MEM_DIRECT );
   if ( PHX_OK != eStatus ) {
       _lastError = "buffer allocation error: buffer 2";
       return false;
   }

   // Initialise the display, this associates the display buffers with the display
   eStatus =  PDL_DisplayInit( hDisplay );
   if ( PHX_OK != eStatus ) {
       _lastError = "display initialization error";
       return false;
   }

   int AcquireNr = 2;
   eStatus = PHX_ParameterSet( hCamera, PHX_ACQ_NUM_IMAGES, &AcquireNr );
   if ( PHX_OK != eStatus ) {
       _lastError = "Parameter error: Acquired number of images";
       return false;
   }

   // Enable FIFO Overflow events
   eParamValue = PHX_INTRPT_FIFO_OVERFLOW;
   eStatus = PHX_ParameterSet( hCamera, PHX_INTRPT_SET, &eParamValue );
   if ( PHX_OK != eStatus ) {
       _lastError = "Parameter error: FIFO Overflow interpretor";
       return false;
   }

   // Setup an event context
   eStatus = PHX_ParameterSet( hCamera, PHX_EVENT_CONTEXT, (void *) &eventContext );
   if ( PHX_OK != eStatus ) {
       _lastError = "Parameter error: Custom Event Context";
       return false;
   }

   // Now start capture, using the callback method
   eStatus = PHX_Acquire( hCamera, PHX_START, (void*) handleInterruptEvents );
   if ( PHX_OK != eStatus ) {
       _lastError = "starting capture error";
       return false;
   }


   return true;
}

bool GrabberInterface::livePicture()
{
    etStat         eStatus     = PHX_OK;
    ui32           nBufferReadyLast = 0; // Previous BufferReady count value

    messageOutput("Starting Live Picture!...\n");
    updateEventCounter(0);

    // Continue processing data until the user presses "Stop" or Phoenix detects a FIFO overflow
    while(isOpened() && !eventContext.fFifoOverFlow)
    {
       _PHX_SleepMs(10);

       // If there are any buffers waiting to display, then process them
       if ( nBufferReadyLast != eventContext.nBufferReadyCount ) {
          stImageBuff stBuffer;
          int nStaleBufferCount;

          /* If the processing is too slow to keep up with acquisition,
           * then there may be more than 1 buffer ready to process.
           * The application can either be designed to process all buffers
           * knowing that it will catch up, or as here, throw away all but the
           * latest
           */
          nStaleBufferCount = eventContext.nBufferReadyCount - nBufferReadyLast;
          nBufferReadyLast += nStaleBufferCount;

          // Throw away all but the last image
          if( nStaleBufferCount > 1 )
          {
             do
             {
                eStatus = PHX_Acquire( hCamera, PHX_BUFFER_RELEASE, NULL );
                if ( PHX_OK != eStatus ) {
                    _lastError = "butter releasing error";
                    return false;
                }
                nStaleBufferCount--;
             }
             while ( nStaleBufferCount > 1 );
          }

          // Get info for the last acquired buffer
          eStatus = PHX_Acquire( hCamera, PHX_BUFFER_GET, &stBuffer );
          if ( PHX_OK != eStatus ) {
              _lastError = "information error: last acquired buffer";
              return false;
          }

          // stBuffer.pvContext --> member variable to pass a display buffer handle
          // stBuffer.pvAddress --> access to the video data

          // Paint the image on the display
          PDL_BufferPaint( (tPHX)stBuffer.pvContext );

          if (_useEventCounter)
              updateEventCounter((int) eventContext.nBufferReadyCount);

          // Release the buffer ready for further image data
          eStatus = PHX_Acquire( hCamera, PHX_BUFFER_RELEASE, NULL );
          if ( PHX_OK != eStatus ) {
              _lastError = "butter releasing error";
              return false;
          }
       }
    }

    /* In this simple example we abort the processing loop on an error condition (FIFO overflow).
     * However handling of this condition is application specific, and generally would involve
     * aborting the current acquisition, and then restarting.
     */
    if ( eventContext.fFifoOverFlow ) {
        _lastError = "FIFO OverFlow detected";
        return false;
    }

    return true;
}

void GrabberInterface::releaseGrabber()
{
    updateEventCounter((int) eventContext.nBufferReadyCount);

    // stop all captures
    if ( hCamera ) PHX_Acquire( hCamera, PHX_ABORT, NULL );

    // free buffers
    if ( hBuffer1 ) PDL_BufferDestroy( (tPHX*) &hBuffer1 );
    if ( hBuffer2 ) PDL_BufferDestroy( (tPHX*) &hBuffer2 );

    // free display
    if ( hDisplay ) PDL_DisplayDestroy( (tPHX*) &hDisplay );

    // release grabber board
    if ( hCamera ) PHX_CameraRelease( &hCamera );

    PhxCommonKbClose();
    close();
    messageOutput("Grabber released\n\n - - - - - - - - - - - - - - - \n");

}

