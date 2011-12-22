/****************************************************************************
 *
 * ACTIVE SILICON LIMITED
 *
 * File name   : pal_api.h
 * Function    : User API for all Phoenix Application Library
 * Project     : AP06
 * Authors     : Martin Bone, Richard Brown
 * Systems     : ANSI C
 * Version     : 1.0
 * Release Date: 1-Oct-00
 *
 * Copyright (c) 2008 Active Silicon Ltd.
 ****************************************************************************
 * Comments:
 * --------
 * This file is the only include file a user (or higher level library) needs 
 * to include in their application in order to use the PAL library.
 *
 ****************************************************************************
 */
#ifndef _PAL_API_H_
#define _PAL_API_H_


/* Include all the OS dependent definitions */
/* Need to use #import if using native MacOS X with frameworks */
#if defined _ASL_MACOSX_FMWK
#import <ASLphoenix/phx_os.h>
#import <ASLphoenix/phx_api.h>

#else
#include <phx_os.h>
#include <phx_api.h>

#endif

/* Prototype Definitions
 * =====================
 */

#if defined __cplusplus
extern "C" {
#endif
etStat PHX_EXPORT_FN PAL_CameraConfigLoad( tHandle*, char*, etCamConfigLoad, void (*) (const char*, etStat, const char*) );
etStat PHX_EXPORT_FN PAL_CameraConfigSave( tHandle,  char*, etCamConfigSave );
etStat PHX_EXPORT_FN PAL_CameraRelease   ( tHandle* );
etStat PHX_EXPORT_FN PAL_ParameterGet    ( tHandle, etParam, void* );
etStat PHX_EXPORT_FN PAL_ParameterSet    ( tHandle, etParam, void* );
etStat PHX_EXPORT_FN PAL_Acquire         ( tHandle, etAcq,   void* );

etStat PHX_EXPORT_FN PAL_CommsReceive    ( tHandle, ui8*, ui32*, ui32 );
etStat PHX_EXPORT_FN PAL_CommsTransmit   ( tHandle, ui8*, ui32*, ui32 );

void   PHX_EXPORT_FN PAL_ErrHandlerDefault ( const char*, etStat, const char* );
void   PHX_EXPORT_FN PAL_ErrCodeDecode     ( char*,       etStat );

etStat PHX_EXPORT_FN PAL_MemoryAlloc      ( tHandle, void **, ui32, ui32, ui32 );
void   PHX_EXPORT_FN PAL_MemoryFreeAndNull( tHandle, void ** );

#if defined __cplusplus
};
#endif

#endif  /* _PAL_API_H_ */

