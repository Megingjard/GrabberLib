#include "grabberinterface.h"
#include "common.h"
#include "common.c"
#include "pdl_api.h"
#include "pbl_api.h"
#include "phx_api.h"
#include "phx_os.h"

#include <QtGui>
#include <QThread>

// global object pointer (needed for callback)
GrabberInterface* global_grabberinterface;

GrabberInterface::GrabberInterface(QObject *parent) :
    QObject(parent)
{
    _isOpened = false;
    _useImageCounter = false;
    resetImageCounter();

    global_grabberinterface = this;
}

GrabberInterface::~GrabberInterface()
{
    close();
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
        oState = startGrabber();

    if (!oState)
        messageOutput(_lastError + "\nAborting...\n");

    releaseGrabber();
}

void GrabberInterface::close()
{
    setState(false);
}

//-------------------- Error Handler functions -------------------------

void errorHandler( const char* pszFnName, etStat eErrCode, const char* pszDescString )
{
    // use of gloabal instance of the interface object
    // Call the member function or signal
    global_grabberinterface->grabberErrorHandler(pszFnName, eErrCode, pszDescString);
}

void GrabberInterface::grabberErrorHandler( const char* pszFnName, etStat eErrCode, const char* pszDescString )
{
    _lastError = "Phx Error: "
            + QString::fromAscii(pszFnName, strlen(pszFnName)) + " : "
            + QString::number(eErrCode) + " : "
            + QString::fromAscii(pszDescString, strlen(pszDescString));

    qDebug() << _lastError;

    messageOutput(_lastError);
}

//-------------------- Callback functions ------------------------------

void callback( tHandle hCamera, ui32 dwInterruptMask, void *pvParams )
{
    // use of gloabal instance of the interface object
    // Call the member function or signal
    global_grabberinterface->grabberCallack(hCamera, dwInterruptMask, pvParams);
}

void GrabberInterface::grabberCallack( tHandle hCamera, ui32 dwInterruptMask, void *pvParams )
{
    EventContext *eventContext = (EventContext*) pvParams;

    stImageBuff sBuffer;
    ui8 *pbData, bFirstPixel;


    if( PHX_INTRPT_BUFFER_READY & dwInterruptMask )
    {
        _ImageCounter++;

        // Obtain the captured buffer information.

        PHX_Acquire( hCamera, PHX_BUFFER_GET, &sBuffer );
        pbData = (ui8*)sBuffer.pvAddress;


        // ---------------------------------------------------------
        // ------------ Processing the Image Data ------------------
        // ---------------------------------------------------------

        // Pointer to the first Pixel
        bFirstPixel = *pbData;


        qDebug() << "And the first pixel is: " + QString::number(bFirstPixel);

        if (_useImageCounter)
            updateImageCounter(_ImageCounter);

        // ---------------------------------------------------------
        // ---------------------------------------------------------
        // ---------------------------------------------------------

        //release the buffer ready for the capture of further image data.
        PHX_Acquire( hCamera, PHX_BUFFER_RELEASE, NULL );
    }

    if ( PHX_INTRPT_FIFO_OVERFLOW & dwInterruptMask )
        eventContext->fFifoOverFlow = TRUE;
}

bool GrabberInterface::configureGrabber()
{
    etStat         eStatus     = PHX_OK;
    etParamValue   eParamValue;          // Parameter for use with PHX_ParameterSet/Get calls

    hCamera = 0;
    hBuffer1 = 0;
    hBuffer2 = 0;

    memset( &eventContext, 0, sizeof(eventContext));

    phxGrabberInfo.eCamConfigLoad = (etCamConfigLoad) ( PHX_DIGITAL | PHX_ACQ_ONLY | PHX_CHANNEL_AUTO );

    // Allocate the board with the config file
    eStatus = PHX_CameraConfigLoad( &hCamera,
                                    cameraConfigFile.toAscii().data(),
                                    phxGrabberInfo.eCamConfigLoad,
                                    &errorHandler );
    if ( PHX_OK != eStatus ) {
        _lastError = "camera configuaration error";
        return false;
    }

    // Create two display buffers for double buffering

    eStatus = PBL_BufferCreate(&hBuffer1,
                               PBL_BUFF_SYSTEM_MEM_DIRECT, //PBL_BUFF_VIDCARD_MEM_DIRECT,
                               0,
                               hCamera,
                               &errorHandler );
    if ( PHX_OK != eStatus ) {
        _lastError = "buffer allocation error: buffer 1";
        return false;
    }

    eStatus = PBL_BufferCreate(&hBuffer2,
                               PBL_BUFF_SYSTEM_MEM_DIRECT,
                               0,
                               hCamera,
                               &errorHandler );
    if ( PHX_OK != eStatus ) {
        _lastError = "buffer allocation error: buffer 2";
        return false;
    }

    ui32 dwBuffWidth;
    ui32 dwBuffHeight;
    ui32 dwBuffStride;

    // Obtain the required buffer dimensions from Phoenix.
    PHX_ParameterGet( hCamera, PHX_ROI_XLENGTH_SCALED, &dwBuffWidth );
    PHX_ParameterGet( hCamera, PHX_ROI_YLENGTH_SCALED, &dwBuffHeight );

    // Convert width (in pixels) to stride (in bytes).
    dwBuffStride = dwBuffWidth; // If we assume Y8 capture format

    // Set the height and stride required.
    PBL_BufferParameterSet( hBuffer1, PBL_BUFF_HEIGHT, &dwBuffHeight );
    PBL_BufferParameterSet( hBuffer1, PBL_BUFF_STRIDE, &dwBuffStride );
    PBL_BufferParameterSet( hBuffer2, PBL_BUFF_HEIGHT, &dwBuffHeight );
    PBL_BufferParameterSet( hBuffer2, PBL_BUFF_STRIDE, &dwBuffStride );

    // Initialise each buffer. This creates the buffers in system memory.
    PBL_BufferInit( hBuffer1 );
    PBL_BufferInit( hBuffer2 );


    // Build our array of image buffers.
    // PBL_BUFF_ADDRESS returns the address of the first pixel of image data.

    void *dwParamValue;

    PBL_BufferParameterGet( hBuffer1, PBL_BUFF_ADDRESS, &dwParamValue );
    asImageBuffs[ 0 ].pvAddress = (void*)dwParamValue;
    asImageBuffs[ 0 ].pvContext = (void*)hBuffer1;

    PBL_BufferParameterGet( hBuffer2, PBL_BUFF_ADDRESS, &dwParamValue );
    asImageBuffs[ 1 ].pvAddress = (void*)dwParamValue;
    asImageBuffs[ 1 ].pvContext = (void*)hBuffer2;
    asImageBuffs[ 2 ].pvAddress = NULL;
    asImageBuffs[ 2 ].pvContext = NULL;

    // Tell Phoenix we are using two buffers.
    ui32 AcquireNr = 2;
    eStatus = PHX_ParameterSet( hCamera, PHX_ACQ_NUM_IMAGES, &AcquireNr );
    if ( PHX_OK != eStatus ) {
        _lastError = "Parameter error: Acquired number of images";
        return false;
    }

    /* These are 'direct' buffers, so we must tell Phoenix about them
   * so that it can capture data directly into them.
   */
    PHX_ParameterSet( hCamera,
                      PHX_DST_PTRS_VIRT,
                      (void *) asImageBuffs );

    eParamValue = PHX_DST_PTR_USER_VIRT;
    PHX_ParameterSet( hCamera,
                      (etParam)(PHX_DST_PTR_TYPE | PHX_CACHE_FLUSH | PHX_FORCE_REWRITE ),
                      (void *) &eParamValue );

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

    return true;
}

bool GrabberInterface::startGrabber()
{
    etStat         eStatus     = PHX_OK;

    messageOutput("Starting Live Picture!...\n");
    resetImageCounter();

    // Now start capture, using the callback method
    PHX_Acquire( hCamera, PHX_START, (void*) callback);
    if ( PHX_OK != eStatus ) {
        _lastError = "starting capture error";
        return false;
    }

    // Continue processing data until the user presses "Stop" or Phoenix detects a FIFO overflow
    while(isOpened() && !eventContext.fFifoOverFlow)
    {
        _PHX_SleepMs(10);
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
    updateImageCounter(_ImageCounter);

    // stop all captures
    if ( hCamera ) PHX_Acquire( hCamera, PHX_ABORT, NULL );

    // free buffers
    if ( hBuffer1 ) PDL_BufferDestroy( (tPHX*) &hBuffer1 );
    if ( hBuffer2 ) PDL_BufferDestroy( (tPHX*) &hBuffer2 );

    // release grabber board
    if ( hCamera ) PHX_CameraRelease( &hCamera );

    PhxCommonKbClose();
    close();
    messageOutput("Grabber released\n\n - - - - - - - - - - - - - - - \n");
}

//-------------------- Parameter functions ------------------------------

void GrabberInterface::setState(bool isOpened)
{
    _isOpened = isOpened;
    grabberStatusChanged();
}

void GrabberInterface::setImageCounterUsage(bool useImageCounter)
{
    _useImageCounter = useImageCounter;
}

QString GrabberInterface::lastError() const
{
    return _lastError;
}

bool GrabberInterface::isOpened() const
{
    return _isOpened;
}

void GrabberInterface::resetImageCounter()
{
    _ImageCounter = 0;
    updateImageCounter(_ImageCounter);
}
