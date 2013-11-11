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

#ifndef _IRQLOGGER_H_
#define _IRQLOGGER_H_

#include <QDebug>
#include <QString>
#include "irqloggerexport.h"

static const char* KLogMarker = "[QtInternetRadio]";
static const char* KTraceOutputFile = "c:/logs/internetradio/iruilog.txt";
#define FILTER_BY_LOGMARKER true

#if defined(LOGGING_ENABLED) && !defined(QT_NO_DEBUG)

#ifdef COMBINE_WITH_ENGINE_LOGGER
    #define WRITELOG(msg) \
            { \
                QString tmpStr(msg); \
                SymbianLogger::writeLog(tmpStr); \
            }  
    #define INSTALL_MESSAGE_HANDLER
#else // COMBINE_WITH_ENGINE_LOGGER

    //enable 3.0 log a new line
    #define WRITELOG(msg) qDebug()<<KLogMarker<<msg<<"\r\n"
    
    #ifdef TRACE_TO_FILE
        #define INSTALL_MESSAGE_HANDLER FileLogger __fileLogger(QString(KTraceOutputFile), FILTER_BY_LOGMARKER)
    #else // TRACE_TO_FILE
        #define INSTALL_MESSAGE_HANDLER
    #endif // TRACE_TO_FILE
    
#endif // COMBINE_WITH_ENGINE_LOGGER

// Helper macro to get a const char* out of a QString so that it can be logged. Can be used with LOG_FORMAT()
#define STRING2CHAR(qstring) qstring.toAscii().constData()

// Macro that simply logs a string
// Example:
// LOG( "This is a test" );
#define LOG(string) WRITELOG( string )


// Macro that logs a string with multiple parameters
// Examples:
// LOG_FORMAT( "This is an integer %d, and this is a float with two digits %.2f", 42, 3.14 );
// LOG_FORMAT( "This is a QString %s", STRING2CHAR(someQString) );
#define LOG_FORMAT(fmt,args...) \
    { \
        QString tmp; \
        WRITELOG( tmp.sprintf(fmt,args) ); \
    }

// Macro that logs function enter, exit and exception
// Example (Simply put it in the beginning of a function):
// LOG_METHOD;
// Output:
// --> SomeFunction(int,const char*)
// <-- SomeFunction(int,const char*)
// <-- SomeFunction(int,const char*): Exception raised!
#define LOG_METHOD MethodLogger ___methodLogger( __PRETTY_FUNCTION__ )

// Logs function enter but does not log exit or leave. This is meant to be lighter than LOG_METHOD macro
#define LOG_METHOD_ENTER LOG_FORMAT( "<--> %s", __PRETTY_FUNCTION__ )


// Assert macro for logging. If the condition is false, the expression is performed
// Example:
// LOG_ASSERT( thisMustBeTrue, LOG_FORMAT( "OMG! That was not true: %d", thisMustBeTrue ) );
#define LOG_ASSERT(cond,expr) \
    { \
        if (!cond) { expr; } \
    }

// Macro to log slot function caller by its class name.
#define LOG_SLOT_CALLER \
    { \
        QObject* caller = sender(); \
        if ( caller ) \
        { \
            LOG_FORMAT( "SLOT %s called by %s. Objectname: %s", __PRETTY_FUNCTION__, \
                        caller->metaObject()->className(), STRING2CHAR( caller->objectName() ) ); \
        } else { \
            LOG_FORMAT( "SLOT %s called as regular function. ", __PRETTY_FUNCTION__ ); \
        } \
    }

#else // LOGGING_ENABLED && QT_NO_DEBUG
    #define LOG(string)
    #define STRING2CHAR(qstring)
    #define LOG_FORMAT(fmt,args...)
    #define LOG_METHOD
    #define LOG_METHOD_ENTER
    #define LOG_ASSERT(cond,expr)
    #define LOG_SLOT_CALLER
    #define INSTALL_MESSAGE_HANDLER
#endif // LOGGING_ENABLED && QT_NO_DEBUG




// Class declaration
class IRQLOGGER_DLL_EXPORT SymbianLogger
{
public:
    static void writeLog( const QString& aMsg );
};

// Class declaration
class IRQLOGGER_DLL_EXPORT MethodLogger
{
public:
    MethodLogger( const char* aFunction );
    ~MethodLogger();

private:
    const char* iFunction;
};

// Class declaration
class IRQLOGGER_DLL_EXPORT FileLogger
{
public:
    FileLogger(const QString& aFileName, bool aFilterByMarker);
    ~FileLogger();
private:
    void installMessageHandler( const QString& aFileName, bool aFilterByMarker );
    void uninstallMessageHandler();    
    static void handleMessage( QtMsgType aType, const char* aMsg );
};

// ============================================================================
// Create log dir
// ============================================================================
IRQLOGGER_DLL_EXPORT void installLogDir();
                                
// ============================================================================
// SIGNAL/SLOT CONNECTION CHECKER
// ============================================================================
IRQLOGGER_DLL_EXPORT bool connectAndTest( const QObject* aSender, const char* aSignal,
                            const QObject* aReceiver, const char* aMember,
                            Qt::ConnectionType aType = Qt::AutoConnection );

IRQLOGGER_DLL_EXPORT bool connectAndAssert( const QObject* aSender, const char* aSignal,
                            const QObject* aReceiver, const char* aMember,
                            Qt::ConnectionType aType = Qt::AutoConnection );

#endif // _IRQLOGGER_H_
