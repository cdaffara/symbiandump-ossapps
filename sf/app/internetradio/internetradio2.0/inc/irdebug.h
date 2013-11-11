/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Debug macros
*
*/


#ifndef IRDEBUG_H
#define IRDEBUG_H


#include <e32debug.h>
#include <flogger.h>
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// Debug usage
//
// Set IR_DEBUG_LEVEL to desired level to get debug logs that are equal or more severe 
// than the value set. E.g. IR_DEBUG_LEVEL_ERROR will print out ERROR and FATAL logs.
//
// Set IRDEBUG_TRACE_TO_FILE to get enable file logging.
//
///////////////////////////////////////////////////////

// Available debug levels. Internal. Don't touch these

#define IR_DEBUG_LEVEL_NONE    0
#define IR_DEBUG_LEVEL_FATAL   1
#define IR_DEBUG_LEVEL_ERROR   2
#define IR_DEBUG_LEVEL_WARNING 3
#define IR_DEBUG_LEVEL_INFO    4
#define IR_DEBUG_LEVEL_DEBUG   5

/////////////////////////////////////////////////////////////////////
// DEBUG SETTINGS:
// To enable /adjust debug output, modify values ONLY in this section
/////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
#define IRDEBUG_LOGGING_ENABLED
#endif
//#define IRDEBUG_TRACE_TO_FILE


/////////////////////////////////////////////////////////////////////
// DEBUG SETTINGS:
// DO NOT Edit below sections
/////////////////////////////////////////////////////////////////////

#ifdef IRDEBUG_LOGGING_ENABLED
#define IR_DEBUG_LEVEL IR_DEBUG_LEVEL_DEBUG
#define IRDEBUG_LEVEL  IR_DEBUG_LEVEL_DEBUG
#else
#define IR_DEBUG_LEVEL IR_DEBUG_LEVEL_NONE
#define IRDEBUG_LEVEL  IR_DEBUG_LEVEL_NONE
#endif

#ifdef IRDEBUG_TRACE_TO_FILE
    _LIT( KIRDebugLogDirectory, "internetradio" );
    _LIT( KIRDebugLogFile, "internetradiolog.txt" );
    const TFileLoggingMode KIRDebugLoggingMode = EFileLoggingModeAppend; // EFileLoggingModeOverwrite
#endif

#if IR_DEBUG_LEVEL > IR_DEBUG_LEVEL_NONE

#ifdef IRDEBUG_TRACE_TO_FILE
        #define IRRDEBUG_DESC( a ) \
            { \
            RFileLogger::WriteFormat( KIRDebugLogDirectory, \
                                      KIRDebugLogFile, \
                                      KIRDebugLoggingMode, \
                                      a ); \
            }
            
        #define IRRDEBUG( a ) \
            { \
            RFileLogger::WriteFormat( KIRDebugLogDirectory, \
                                      KIRDebugLogFile, \
                                      KIRDebugLoggingMode, \
                                      _L( a )); \
            }
                     
        #define IRRDEBUG2( a, b ) \
            { \
            RFileLogger::WriteFormat( KIRDebugLogDirectory, \
                                      KIRDebugLogFile, \
                                      KIRDebugLoggingMode, \
                                      _L( a ), b); \
            }
            
        #define IRRDEBUG3( a, b, c ) \
            { \
            RFileLogger::WriteFormat( KIRDebugLogDirectory, \
                                      KIRDebugLogFile, \
                                      KIRDebugLoggingMode, \
                                      _L( a ), b, c); \
            }
            
        #define IRRDEBUG4( a, b, c, d ) \
            { \
            RFileLogger::WriteFormat( KIRDebugLogDirectory, \
                                      KIRDebugLogFile, \
                                      KIRDebugLoggingMode, \
                                      _L( a ), b, c, d); \
            }
            
        #define IRRDEBUG5( a, b, c, d, e ) \
            { \
            RFileLogger::WriteFormat( KIRDebugLogDirectory, \
                                      KIRDebugLogFile, \
                                      KIRDebugLoggingMode, \
                                      _L( a ), b, c, d, e); \
            }        
#else // IRDEBUG_TRACE_TO_FILE
        #define IRRDEBUG_DESC( a ) RDebug::Print( a )
    
        #define IRRDEBUG( a ) RDebug::Print( _L( a ) )
                     
        #define IRRDEBUG2( a, b ) RDebug::Print( _L( a ), b )
            
        #define IRRDEBUG3( a, b, c ) RDebug::Print( _L( a ), b, c )
            
        #define IRRDEBUG4( a, b, c, d ) RDebug::Print( _L( a ), b, c, d )
            
        #define IRRDEBUG5( a, b, c, d, e ) RDebug::Print( _L( a ), b, c, d, e )
#endif // IRDEBUG_TRACE_TO_FILE
           
#else // IR_DEBUG_LEVEL > IR_DEBUG_LEVEL_NONE
    #define IRRDEBUG_DESC( a )
    #define IRRDEBUG( a )
    #define IRRDEBUG2( a, b )
    #define IRRDEBUG3( a, b, c )
    #define IRRDEBUG4( a, b, c, d )
    #define IRRDEBUG5( a, b, c, d, e )
#endif

////////////////////////////////////////////////////////////////////
// The interface macros
///////////////////////////////////////////////////////////////////


#if IRDEBUG_LEVEL > IR_DEBUG_LEVEL_NONE
    #define IRLOG_FATAL( a ) IRRDEBUG( "InternetRadio [FATAL]: " L##a )
    #define IRLOG_FATAL2( a, b ) IRRDEBUG2( "InternetRadio [FATAL]: " L##a, b )
    #define IRLOG_FATAL3( a, b, c ) IRRDEBUG3( "InternetRadio [FATAL]: " L##a, b, c )
    #define IRLOG_FATAL4( a, b, c, d ) IRRDEBUG4( "InternetRadio [FATAL]: " L##a, b, c, d )
    #define IRLOG_FATAL5( a, b, c, d, e ) IRRDEBUG5( "InternetRadio [FATAL]: " L##a, b, c, d, e )
#else    
    #define IRLOG_FATAL( a ) 
    #define IRLOG_FATAL2( a, b )
    #define IRLOG_FATAL3( a, b, c )
    #define IRLOG_FATAL4( a, b, c, d )
    #define IRLOG_FATAL5( a, b, c, d, e )
 #endif

#if IRDEBUG_LEVEL > IR_DEBUG_LEVEL_FATAL
    #define IRLOG_ERROR( a ) IRRDEBUG( "InternetRadio [ERROR]: " L##a )
    #define IRLOG_ERROR2( a, b ) IRRDEBUG2( "InternetRadio [ERROR]: " L##a, b )
    #define IRLOG_ERROR3( a, b, c ) IRRDEBUG3( "InternetRadio [ERROR]: " L##a, b, c )
    #define IRLOG_ERROR4( a, b, c, d ) IRRDEBUG4( "InternetRadio [ERROR]: " L##a, b, c, d )
    #define IRLOG_ERROR5( a, b, c, d, e ) IRRDEBUG5( "InternetRadio [ERROR]: " L##a, b, c, d, e )
#else
    #define IRLOG_ERROR( a )
    #define IRLOG_ERROR2( a, b )
    #define IRLOG_ERROR3( a, b, c )
    #define IRLOG_ERROR4( a, b, c, d ) 
    #define IRLOG_ERROR5( a, b, c, d, e )
#endif

#if IRDEBUG_LEVEL > IR_DEBUG_LEVEL_ERROR
    #define IRLOG_WARNING( a ) IRRDEBUG( "InternetRadio [WARNING]: " L##a )
    #define IRLOG_WARNING2( a, b ) IRRDEBUG2( "InternetRadio [WARNING]: " L##a, b )
    #define IRLOG_WARNING3( a, b, c ) IRRDEBUG3( "InternetRadio [WARNING]: " L##a, b, c )
    #define IRLOG_WARNING4( a, b, c, d ) IRRDEBUG4( "InternetRadio [WARNING]: " L##a, b, c, d )
    #define IRLOG_WARNING5( a, b, c, d, e ) IRRDEBUG5( "InternetRadio [WARNING]: " L##a, b, c, d, e )
#else
    #define IRLOG_WARNING( a )
    #define IRLOG_WARNING2( a, b )
    #define IRLOG_WARNING3( a, b, c )
    #define IRLOG_WARNING4( a, b, c, d )
    #define IRLOG_WARNING5( a, b, c, d, e )
#endif

#if IRDEBUG_LEVEL > IR_DEBUG_LEVEL_WARNING
    #define IRLOG_INFO( a ) IRRDEBUG( "InternetRadio [INFO]: " L##a )
    #define IRLOG_INFO2( a, b ) IRRDEBUG2( "InternetRadio [INFO]: " L##a, b )
    #define IRLOG_INFO3( a, b, c ) IRRDEBUG3( "InternetRadio [INFO]: " L##a, b, c )
    #define IRLOG_INFO4( a, b, c, d ) IRRDEBUG4( "InternetRadio [INFO]: " L##a, b, c, d )
    #define IRLOG_INFO5( a, b, c, d, e ) IRRDEBUG5( "InternetRadio [INFO]: " L##a, b, c, d, e )
#else
    #define IRLOG_INFO( a )
    #define IRLOG_INFO2( a, b )
    #define IRLOG_INFO3( a, b, c )
    #define IRLOG_INFO4( a, b, c, d )
    #define IRLOG_INFO5( a, b, c, d, e )
#endif

#if IRDEBUG_LEVEL > IR_DEBUG_LEVEL_INFO
    _LIT( KIRDebugMarker, "InternetRadio [DEBUG]: " );
    #define IRLOG_DEBUG_DESC( desc ) \
        { \
        RBuf tmp; \
        tmp.Create(KIRDebugMarker, KIRDebugMarker().Length() + desc.Length() ); \
        tmp.Append(desc); \
        IRRDEBUG_DESC(tmp); \
        tmp.Close(); \
        }
    #define IRLOG_DEBUG( a ) IRRDEBUG( "InternetRadio [DEBUG]: " L##a )
    #define IRLOG_DEBUG2( a, b ) IRRDEBUG2( "InternetRadio [DEBUG]: " L##a, b )
    #define IRLOG_DEBUG3( a, b, c ) IRRDEBUG3( "InternetRadio [DEBUG]: " L##a, b, c )
    #define IRLOG_DEBUG4( a, b, c, d ) IRRDEBUG4( "InternetRadio [DEBUG]: " L##a, b, c, d )
    #define IRLOG_DEBUG5( a, b, c, d, e ) IRRDEBUG5( "InternetRadio [DEBUG]: " L##a, b, c, d, e )
#else
    #define IRLOG_DEBUG_DESC( desc )
    #define IRLOG_DEBUG( a )
    #define IRLOG_DEBUG2( a, b )
    #define IRLOG_DEBUG3( a, b, c )
    #define IRLOG_DEBUG4( a, b, c, d )
    #define IRLOG_DEBUG5( a, b, c, d, e )
#endif     

////////////////////////////////////////////////////////////////////
// Other debug related definitions
///////////////////////////////////////////////////////////////////

#if IRDEBUG_LEVEL > IR_DEBUG_LEVEL_NONE
    #define IRDEBUGVAR( a ) a
    #define IRDEBUGCODE( a ) a
#else    
    #define IRDEBUGVAR( a )   
    #define IRDEBUGCODE( a )
#endif


          
#endif          
            
// End of File
