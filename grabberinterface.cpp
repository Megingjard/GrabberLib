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
}

GrabberInterface::~GrabberInterface()
{
    close();
}

static void handleInterruptEvents(tHandle hCamera, ui32 interruptMask, void *userParameter )
{
   PhxUserInfo *phxUserInfo = (PhxUserInfo*) userParameter;

   (void) hCamera;

   if ( PHX_INTRPT_BUFFER_READY & interruptMask )
       phxUserInfo->nBufferReadyCount++;

   if ( PHX_INTRPT_FIFO_OVERFLOW & interruptMask )
       phxUserInfo->fFifoOverFlow = TRUE;

   /* Note:
    * The callback routine may be called with more than 1 event flag set.
    * Therefore all possible events must be handled here.
    */
   if ( PHX_INTRPT_FRAME_END & interruptMask )
   {
   }
}


//--------------------------------------------------------------------

/* VxWorks requires a unique function name for each application */
//#if !defined _PHX_VXWORKS

void GrabberInterface::open(QString cameraConfigFile)
{
    _cameraConfigFile = cameraConfigFile;

    char *argv[0];

    PhxCommonParseCmd( 0, argv, &phxGrabberInfo );
    PhxCommonKbInit();

    messageOutput("Starting Live Picture!");

    _aboutLivePicture = false;
    nStatus = livePicture( phxGrabberInfo.eCamConfigLoad, _cameraConfigFile );

    PhxCommonKbClose();
}

//#endif

//--------------------------------------------------------------------

void GrabberInterface::close()
{
   _aboutLivePicture = true;
   messageOutput("Connection closed");
}

//--------------------------------------------------------------------

QString GrabberInterface::lastError() const
{
    return _lastError;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------

int GrabberInterface::livePicture(
   etCamConfigLoad eCamConfigLoad,      /* Board number, ie 1, 2, or 0 for next available */
   QString cameraConfigFile             /* Name of config file */
)
{
   etStat         eStatus     = PHX_OK;   /* Status variable */
   etParamValue   eParamValue;          /* Parameter for use with PHX_ParameterSet/Get calls */
   tHandle        hCamera   = 0;        /* Camera Handle  */
   tPHX           hDisplay  = 0;        /* Display handle */
   tPHX           hBuffer1  = 0;        /* First Image buffer handle  */
   tPHX           hBuffer2  = 0;        /* Second Image buffer handle */
   PhxUserInfo    phxUserInfo;          /* User defined Event Context */
   ui32           nBufferReadyLast = 0; /* Previous BufferReady count value */


   /* Initialise the user defined Event context structure */
   memset( &phxUserInfo, 0, sizeof( PhxUserInfo ) );

   /* Allocate the board with the config file */
   eStatus = PHX_CameraConfigLoad( &hCamera, cameraConfigFile.toAscii().data(), eCamConfigLoad, PHX_ErrHandlerDefault );
   if ( PHX_OK != eStatus ) goto Error;


#define _PHX_DISPLAY

#if defined _PHX_DISPLAY
   /* We create our display with a NULL hWnd, this will automatically create an image window. */
   eStatus = PDL_DisplayCreate( &hDisplay, NULL, hCamera, PHX_ErrHandlerDefault );
   if ( PHX_OK != eStatus ) goto Error;

   /* We create two display buffers for our double buffering */
   eStatus = PDL_BufferCreate( &hBuffer1, hDisplay, (etBufferMode)PDL_BUFF_SYSTEM_MEM_DIRECT );
   if ( PHX_OK != eStatus ) goto Error;
   eStatus = PDL_BufferCreate( &hBuffer2, hDisplay, (etBufferMode)PDL_BUFF_SYSTEM_MEM_DIRECT );
   if ( PHX_OK != eStatus ) goto Error;

   /* Initialise the display, this associates the display buffers with the display */
   eStatus =  PDL_DisplayInit( hDisplay );
   if ( PHX_OK != eStatus ) goto Error;

   /* The above code has created 2 display (acquisition) buffers.
    * Therefore ensure that the Phoenix is configured to use 2 buffers, by overwriting
    * the value already loaded from the config file.
    */

   eParamValue = PHX_DISABLE;
   eStatus = PHX_ParameterSet( hCamera, PHX_ACQ_NUM_IMAGES, &eParamValue );
   if ( PHX_OK != eStatus ) goto Error;
#endif


   /* Enable FIFO Overflow events */
   eParamValue = PHX_INTRPT_FIFO_OVERFLOW;
   eStatus = PHX_ParameterSet( hCamera, PHX_INTRPT_SET, &eParamValue );
   if ( PHX_OK != eStatus ) goto Error;

   /* Setup our own event context */
   eStatus = PHX_ParameterSet( hCamera, PHX_EVENT_CONTEXT, (void *) &phxUserInfo );
   if ( PHX_OK != eStatus ) goto Error;


   /* Now start our capture, using the callback method */
   eStatus = PHX_Acquire( hCamera, PHX_START, (void*) handleInterruptEvents );
   if ( PHX_OK != eStatus ) goto Error;


   /* Continue processing data until the user presses a key in the console window
    * or Phoenix detects a FIFO overflow
    */
   printf("Press a key to exit\n");
   while(!_aboutLivePicture && !PhxCommonKbHit() && !phxUserInfo.fFifoOverFlow)
   {
      /* Temporarily sleep, to avoid burning CPU cycles.
       * An alternative method is to wait on a semaphore, which is signalled
       * within the callback function.  This approach would ensure that the
       * data processing would only start when there was data to process
       */
      _PHX_SleepMs(10);

      /* If there are any buffers waiting to display, then process them here */
      if ( nBufferReadyLast != phxUserInfo.nBufferReadyCount ) {
         stImageBuff stBuffer;
         int nStaleBufferCount;

         /* If the processing is too slow to keep up with acquisition,
          * then there may be more than 1 buffer ready to process.
          * The application can either be designed to process all buffers
          * knowing that it will catch up, or as here, throw away all but the
          * latest
          */
         nStaleBufferCount = phxUserInfo.nBufferReadyCount - nBufferReadyLast;
         nBufferReadyLast += nStaleBufferCount;

         /* Throw away all but the last image */
         if( 1 < nStaleBufferCount )
         {
            do
            {
               eStatus = PHX_Acquire( hCamera, PHX_BUFFER_RELEASE, NULL );
               if ( PHX_OK != eStatus ) goto Error;
               nStaleBufferCount--;
            }
            while ( nStaleBufferCount > 1 );
         }

         /* Get the info for the last acquired buffer */
         eStatus = PHX_Acquire( hCamera, PHX_BUFFER_GET, &stBuffer );
         if ( PHX_OK != eStatus ) goto Error;

         /* Process the newly acquired buffer,
          * which in this simple example is a call to display the data.
          * For our display function we use the pvContext member variable to
          * pass a display buffer handle.
          * Alternatively the actual video data can be accessed at stBuffer.pvAddress
          */
#if defined _PHX_DISPLAY
         PDL_BufferPaint( (tPHX)stBuffer.pvContext );
#else
         //printf("EventCount = %5d\r", phxUserInfo.nBufferReadyCount );
         messageOutput("EventCount = " + QString::number((int) phxUserInfo.nBufferReadyCount, 5));
#endif

         /* Having processed the data, release the buffer ready for further image data */
         eStatus = PHX_Acquire( hCamera, PHX_BUFFER_RELEASE, NULL );
         if ( PHX_OK != eStatus ) goto Error;
      }
   }

   /* In this simple example we abort the processing loop on an error condition (FIFO overflow).
    * However handling of this condition is application specific, and generally would involve
    * aborting the current acquisition, and then restarting.
    */
   if ( phxUserInfo.fFifoOverFlow )      
      messageOutput("FIFO OverFlow detected..Aborting\n");

Error:
   /* Now cease all captures */
   if ( hCamera ) PHX_Acquire( hCamera, PHX_ABORT, NULL );

#if defined _PHX_DISPLAY
   /* Free our display double buffering resources */
   if ( hBuffer1 ) PDL_BufferDestroy( (tPHX*) &hBuffer1 );
   if ( hBuffer2 ) PDL_BufferDestroy( (tPHX*) &hBuffer2 );

   /* Destroy our display */
   if ( hDisplay ) PDL_DisplayDestroy( (tPHX*) &hDisplay );
#endif

   /* Release the Phoenix board */
   if ( hCamera ) PHX_CameraRelease( &hCamera );

   messageOutput("Exiting\n");

   return 0;



}


