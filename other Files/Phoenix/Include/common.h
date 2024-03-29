/****************************************************************************
 *
 * ACTIVE SILICON LIMITED
 *
 * File name   : common.h
 * Function    : Common routines used by the example suite
 * Project     : Phoenix
 *
 * Copyright (c) 2004-2005 Active Silicon Ltd.
 ****************************************************************************
 * Comments:
 * --------
 * This file contains the definitions for the "common" module.
 *
 ****************************************************************************
 */

#ifndef _PHX_COMMON
#define _PHX_COMMON


/* Macos X uses frameworks for the library header files
 */
#if defined _ASL_MACOSX_FMWK
#import <ASLphoenix/phx_api.h>
#import <ASLphoenixDisplay/pdl_api.h>
#import <ASLphoenixBuffer/pbl_api.h>
#import <ASLphoenixImage/pil_api.h>

/* For all other OSs include the Phoenix header files
 */
#else
#include <phx_api.h>    /* Main Phoenix library    */

#if !defined _PHX_VXWORKS
#include <pbl_api.h>    /* Phoenix Buffer library */

#if defined _PHX_DISPLAY
#include <pdl_api.h>    /* Phoenix Display library */
#endif

#include <pil_api.h>    /* Phoenix Image library */
#endif

#endif

/* Include system header files for keyboard handling, under Posix
 */
#ifdef _PHX_POSIX
#include <termios.h>
#endif


/* A structure containing various user defined parameters.
 * These are initialised by the PhxCommonParseCmd function,
 * before being passed to the appropriate subroutine.
 */
#define PHX_MAX_FILE_LENGTH 128
typedef struct {
   ui32              dwBoardNumber;
   etCamConfigLoad   eCamConfigLoad;
   char              bConfigFileName[PHX_MAX_FILE_LENGTH];
   char              bOutputFileName[PHX_MAX_FILE_LENGTH];
   char             *pszConfigFileName;
   char             *pszOutputFileName;
   ui32              dwBayerOption;
   ui32              dwGridSize;
   ui32              dwGammaOption;
   ui32              dwThresholdOption;
   ui32              dwTrackOption;
   ui32              dwFrameOption;
   ui32              dwTimeOption;
   ui32              dwSlowOption;
} tPhxCmd;



/* Application constants
 */
#define DEFAULT_CFG_FILENAME "default.pcf"

#if defined _PHX_MACOSX
#define DEFAULT_UP_DIR "/../../../../../../../../"

#elif defined _PHX_POSIX
#define DEFAULT_UP_DIR "./../../"

#elif defined _PHX_WIN32
#define DEFAULT_UP_DIR ".\\..\\..\\..\\"

#elif defined _PHX_DOS32
#define DEFAULT_UP_DIR ".\\..\\..\\"

#elif defined _PHX_VXWORKS
#define DEFAULT_UP_DIR ".\\..\\..\\..\\"

#else
#error Unrecognised target environment directive (e.g. _PHX_WIN32)

#endif


/* Function prototypes
 */
etStat PhxCommonGetPcf  ( int argc, char* argv[], char** pszCfgFile );
etStat PhxCommonParseCmd( int argc, char *argv[], tPhxCmd *ptPhxCmd );
etStat PhxCommonGetBitsPerPixel( ui32 dwFormat, ui32 *pdwBitsPerPixel );
void   PhxCommonKbClose ( void );
void   PhxCommonKbInit  ( void );
int    PhxCommonKbRead  ( void );
int    PhxCommonKbHit   ( void );

#endif   /* _PHX_COMMON */
