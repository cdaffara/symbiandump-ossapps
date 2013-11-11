/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef _RADIOLOGGER_H_
#define _RADIOLOGGER_H_

// System includes
#include <QObject>

/**
 * Logging level macro definitions
 * Level 1 - No level macro needed. The trace is shown when logging is on.
 * Level 2 - Wrap log line with LEVEL2 macro. Traces of level 1 and 2 are shown.
 * Level 3 - Wrap log line with LEVEL3 macro. Traces of level 1, 2 and 3 are shown.
 */
#if LOGGING_LEVEL == 2
#   define LEVEL2( expr ) expr
#   define LEVEL3( expr )
#elif LOGGING_LEVEL == 3
#   define LEVEL2( expr ) expr
#   define LEVEL3( expr ) expr
#else
#   define LEVEL2( expr )
#   define LEVEL3( expr )
#endif

#if LOGGED_COMPONENT == 2
#   define LOGMARKER "FMRadioHsWidget:"
#   define WRAPPER_DLL_EXPORT
#else
#   define LOGMARKER "FMRadio:"
#   ifdef BUILD_WRAPPER_DLL
#       define WRAPPER_DLL_EXPORT Q_DECL_EXPORT
#   else
#       define WRAPPER_DLL_EXPORT Q_DECL_IMPORT
#   endif
#endif

class WRAPPER_DLL_EXPORT RadioLogger
{
public:

    enum Mode { Normal, MethodEnter, MethodExit, DecIndent };

    static void initCombinedLogger();
    static void releaseCombinedLogger();

    static void logMsg( const char* msg, Mode mode = Normal );

private:
    RadioLogger();
    ~RadioLogger();
};

// ============================================================================
// START TIMESTAMP LOGGING
// ============================================================================

#ifdef TIMESTAMP_LOGGING_ENABLED

#include <QTime>
#include <QDebug>

/**
 * Timestamp logging macro.
 * When the macro is defined, timestamp logging is on
 *
 * Example. These lines in the code...
 *  LOG_TIMESTAMP( "Start operation" );
 *  ...
 *  LOG_TIMESTAMP( "End operation" );
 *
 * ... print the following log lines
 *  FMRadio: Start operation "14:13:09.042"
 *  FMRadio: End operation "14:13:09.250"
 */
//#define LOG_TIMESTAMP(comment) do{ qDebug() << LOGMARKER << comment << QTime::currentTime().toString("hh:mm:ss.zzz"); }while(0)
#define LOG_TIMESTAMP(comment) LOG_FORMAT( comment ## " %s", GETSTRING( QTime::currentTime().toString("hh:mm:ss.zzz") ) )

#else

#define LOG_TIMESTAMP(comment)

#endif // TIMESTAMP_LOGGING_ENABLED

// ============================================================================
// END TIMESTAMP LOGGING
// ============================================================================

// ============================================================================
// START FULL LOGGING
// ============================================================================

#ifdef LOGGING_ENABLED

#include <QtGlobal>
#include <QDebug>

// Macro to hide a function variable that is used only when debugging is enabled.
// Expands to the variable name when debugging is enabled and to nothing when it is not
#if LOGGING_LEVEL == 2
#   define DEBUGVAR( var ) var
#   define DEBUGVAR2( var ) var
#   define DEBUGVAR3( var )
#elif LOGGING_LEVEL == 3
#   define DEBUGVAR( var ) var
#   define DEBUGVAR2( var ) var
#   define DEBUGVAR3( var ) var
#else
#   define DEBUGVAR( var ) var
#   define DEBUGVAR2( var )
#   define DEBUGVAR3( var )
#endif

// UI logs can be combined with engine logs by making the UI feed its log prints into
// the engine logger. This requires that we initialize the radio engine utils right here
// because the engine won't start up until much later. This is a bit ugly since the macros
// call Symbian code directly, but it was considered to be worth it to see UI and engine
// traces in the same file.
#if defined COMBINE_WITH_ENGINE_LOGGER && defined LOGGING_ENABLED && !defined BUILD_WIN32
#   include "../../../radioengine/utils/api/mradioenginelogger.h"
#   include "../../../radioengine/utils/api/radioengineutils.h"
#   define WRITELOG(msg) RadioLogger::logMsg( msg );
#   define WRITELOG_METHOD_ENTER(msg) RadioLogger::logMsg( GETSTRING( msg ), RadioLogger::MethodEnter );
#   define WRITELOG_METHOD_EXIT(msg) RadioLogger::logMsg( GETSTRING( msg ), RadioLogger::MethodExit );
#   define LOGGER_DEC_INDENT RadioLogger::logMsg( "", RadioLogger::DecIndent );
#   define WRITELOG_GETSTRING(msg) WRITELOG( GETSTRING( msg ) )
#   define INIT_COMBINED_LOGGER RadioLogger::initCombinedLogger();
#   define RELEASE_COMBINED_LOGGER RadioLogger::releaseCombinedLogger();
#else
#   define WRITELOG(msg) qDebug() << LOGMARKER << msg;
#   define WRITELOG_METHOD_ENTER(msg) WRITELOG(msg)
#   define WRITELOG_METHOD_EXIT(msg) WRITELOG(msg)
#   define WRITELOG_GETSTRING(msg) WRITELOG(msg)
#   define LOGGER_INC_INDENT
#   define LOGGER_DEC_INDENT
#   define INIT_COMBINED_LOGGER
#   define RELEASE_COMBINED_LOGGER
#endif

// Macro that simply logs a string
// Example:
// LOG( "This is a test" );
#define LOG(string) do{ WRITELOG( string ) }while(0)

// Helper macro to get a const char* out of a QString so that it can be logged. Can be used with LOG_FORMAT()
#define GETSTRING(qstring) qstring.toAscii().constData()

// Macro that logs a string with multiple parameters
// Examples:
// LOG_FORMAT( "This is an integer %d, and this is a float with two digits %.2f", 42, 3.14 );
// LOG_FORMAT( "This is a QString %s", GETSTRING(someQString) );
#define LOG_FORMAT(fmt,args...) do{ QString tmp; WRITELOG_GETSTRING( tmp.sprintf(fmt,args) ) }while(0)

// Macro that logs function enter, exit and exception
// Example (Simply put it in the beginning of a function):
// LOG_METHOD;
// Output:
// -> SomeFunction(int,const char*)
// <- SomeFunction(int,const char*)
// <- SomeFunction(int,const char*): Exception raised!
#define LOG_METHOD MethodLogger ___methodLogger( __PRETTY_FUNCTION__, "" )

// Same as the previous function logging macro with the addition of logging the return value
// Note! The return value can only be retrieved in the emulator.
// Example (Simply put it in the beginning of a function):
// LOG_METHOD_RET( "%d" );
// Output:
// -> SomeFunction(int,const char*)
// <- SomeFunction(int,const char*) returning 42
#define LOG_METHOD_RET(fmt) MethodLogger ___methodLogger( __PRETTY_FUNCTION__, fmt )

// Logs function enter but does not log exit or leave. This is meant to be lighter than LOG_METHOD macro
#define LOG_METHOD_ENTER LOG_FORMAT( "Enter: %s", __PRETTY_FUNCTION__ )

// Assert macro for logging. If the condition is false, the expression is performed
// Example:
// LOG_ASSERT( thisMustBeTrue, LOG_FORMAT( "OMG! That was not true: %d", thisMustBeTrue ) );
#define LOG_ASSERT(cond,expr) do{ if (!cond) { expr; } }while(0)

// Macro to log slot function caller by its class name.
#define LOG_SLOT_CALLER do { \
        QObject* caller = sender(); \
        if ( caller ) { \
            LOG_FORMAT( "SLOT %s called by %s. Objectname: %s", __PRETTY_FUNCTION__, \
                caller->metaObject()->className(), GETSTRING( caller->objectName() ) ); \
        } else { \
            LOG_FORMAT( "SLOT %s called as regular function. ", __PRETTY_FUNCTION__ ); \
        } \
    } while (0)

// Class declaration
class WRAPPER_DLL_EXPORT MethodLogger
{
public:

    MethodLogger( const char* function, const char* format );
    ~MethodLogger();

private:

    const char* mFunction;
    const char* mFormat;

};

#else // LOGGING_ENABLED

#   define LOG(string)
#   define GETSTRING(qstring)
#   define LOG_FORMAT(fmt,args...)
#   define LOG_METHOD
#   define LOG_METHOD_RET(fmt)
#   define LOG_METHOD_ENTER
#   define LOG_ASSERT(cond,expr)
#   define DEBUGVAR(a)
#   define DEBUGVAR2(a)
#   define DEBUGVAR3(a)
#   define LOG_SLOT_CALLER
#   define INIT_COMBINED_LOGGER
#   define RELEASE_COMBINED_LOGGER


// Dummy class
class WRAPPER_DLL_EXPORT MethodLogger
{
public:
    MethodLogger( const char*, const char* );
    ~MethodLogger();
};

#endif // LOGGING_ENABLED

#ifdef TRACE_TO_FILE

#   define INSTALL_MESSAGE_HANDLER FileLogger::installMessageHandler(QString(TRACE_OUTPUT_FILE), FILTER_BY_LOGMARKER);
#   define UNINSTALL_MESSAGE_HANDLER FileLogger::uninstallMessageHandler();

// Class declaration
class WRAPPER_DLL_EXPORT FileLogger
{
public:

    static void installMessageHandler( const QString& fileName, bool filterByMarker = true );
    static void uninstallMessageHandler();

private:

    static void handleMessage( QtMsgType type, const char* msg );

};

#else
#   define INSTALL_MESSAGE_HANDLER
#   define UNINSTALL_MESSAGE_HANDLER
#endif // TRACE_TO_FILE

// ============================================================================
// END FULL LOGGING
// ============================================================================

class Radio
{
    Radio();
    ~Radio();
public:

    // ============================================================================
    // SIGNAL/SLOT CONNECT CHECKER
    // ============================================================================
    static bool connect( const QObject* sender, const char* signal,
                         const QObject* receiver, const char* member,
                         Qt::ConnectionType type = Qt::AutoConnection )
    {
        bool connected = QObject::connect( sender, signal, receiver, member, type );

        #ifdef CONNECT_TEST_MODE

        if ( !connected ) {
            LOG( "Failed to make a signal-slot connection!" );
            LOG_FORMAT( "sender: %s", sender->metaObject()->className() );
            LOG_FORMAT( "signal: %s", signal );
            LOG_FORMAT( "receiver: %s", receiver->metaObject()->className() );
            LOG_FORMAT( "slot/signal: %s", signal );

            #if CONNECT_TEST_MODE == 2
                Q_ASSERT( false );
            #endif

            // ----------------------------------------------------------------
            // SIGNAL-SLOT CONNECT FAILED!
            // ----------------------------------------------------------------
        }

        #endif

        return connected;
    }

    // ============================================================================
    // SIGNAL/SLOT DISCONNECT CHECKER
    // ============================================================================
    static bool disconnect( const QObject* sender, const char* signal,
                            const QObject* receiver, const char* member )
    {
        bool disconnected = QObject::disconnect( sender, signal, receiver, member );

        #ifdef CONNECT_TEST_MODE

        if ( !disconnected ) {
            LOG( "Failed to disconnect a signal-slot connection!" );
            LOG_FORMAT( "sender: %s", sender->metaObject()->className() );
            LOG_FORMAT( "signal: %s", signal );
            LOG_FORMAT( "receiver: %s", receiver->metaObject()->className() );
            LOG_FORMAT( "slot/signal: %s", signal );

            #if CONNECT_TEST_MODE == 2
                Q_ASSERT( false );
            #endif

            // ----------------------------------------------------------------
            // SIGNAL-SLOT DISCONNECT FAILED!
            // ----------------------------------------------------------------
        }

        #endif

        return disconnected;
    }
};

#ifdef ENABLE_ASSERTS

    #define RADIO_ASSERT(cond,where,what) Q_ASSERT_X(cond,where,what)

#else
#   ifdef LOGGING_ENABLED
#       define RADIO_ASSERT(cond,where,what) \
            do { \
                if ( !cond ) { \
                    LOG_FORMAT( "ASSERT Failed! %s, %s", where, what ); \
                } \
            } while ( false )
#   else
#       define RADIO_ASSERT(cond,where,what)
#   endif // LOGGING_ENABLED
#endif // ENABLE_ASSERTS

#endif // _RADIOLOGGER_H_
