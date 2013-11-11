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

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include "irqlogger.h"

#if defined(TRACE_TO_FILE) || defined(IRDEBUG_TRACE_TO_FILE)
static const char* KTraceOutputDir = "c:/logs/internetradio";
static const char* KTraceOutputFileName = "c:/logs/internetradio/internetradiolog.txt";
static const char* KReadableFileName = "c:/data/internetradiolog.txt";
#endif

// ============================================================================
// SymbianLogger Defenition
// ============================================================================
#if defined LOGGING_ENABLED && defined COMBINE_WITH_ENGINE_LOGGER

#include "irdebug.h"

void SymbianLogger::writeLog( const QString& aMsg )
{
    TPtrC tmpDesc(reinterpret_cast<const TUint16*>(aMsg.utf16()), aMsg.length());
    IRLOG_DEBUG_DESC( tmpDesc );
}

#else // LOGGING_ENABLED && COMBINE_WITH_ENGINE_LOGGER

void SymbianLogger::writeLog( const QString& aMsg )
{
    Q_UNUSED(aMsg);
}

#endif // LOGGING_ENABLED && COMBINE_WITH_ENGINE_LOGGER

// ============================================================================
// MethodLogger Defenition
// ============================================================================
#if defined(LOGGING_ENABLED) && !defined(QT_NO_DEBUG)

const char* KLogEnter = "--> %s";
const char* KLogExit = "<-- %s";
const char* KLogExitRet = "<-- %s, Returning %s";
const char* KLogExitException = "<-- %s, Exception raised";

MethodLogger::MethodLogger( const char* aFunction ) :
    iFunction( aFunction )
{
    QString msg;
    msg.sprintf( KLogEnter, iFunction );
    WRITELOG( msg );
}

MethodLogger::~MethodLogger()
{
    QString msg;
    if ( std::uncaught_exception() )
    {
        msg.sprintf( KLogExitException, iFunction );
        WRITELOG( msg );
    }
    else
    {
        msg.sprintf( KLogExit, iFunction );
        WRITELOG( msg );
    }
}

#else // LOGGING_ENABLED && QT_NO_DEBUG

MethodLogger::MethodLogger( const char* aFunction ) :
    iFunction( aFunction )
{
}

MethodLogger::~MethodLogger()
{
}

#endif // LOGGING_ENABLED && QT_NO_DEBUG


// ============================================================================
// FileLogger Defenition
// ============================================================================
QFile gDebugFile;
bool  gFilterByLogMarker = false;

#if defined LOGGING_ENABLED && defined TRACE_TO_FILE && !defined COMBINE_WITH_ENGINE_LOGGER && !defined(QT_NO_DEBUG)

FileLogger::FileLogger(const QString& aFileName, bool aFilterByMarker)
{
    installMessageHandler( aFileName, aFilterByMarker );
}

FileLogger::~FileLogger()
{
    uninstallMessageHandler();
}

#else // LOGGING_ENABLED && TRACE_TO_FILE && !COMBINE_WITH_ENGINE_LOGGER && QT_NO_DEBUG

FileLogger::FileLogger(const QString& aFileName, bool aFilterByMarker)
{
    Q_UNUSED(aFileName);
    Q_UNUSED(aFilterByMarker);
}

FileLogger::~FileLogger()
{
}

#endif // LOGGING_ENABLED && TRACE_TO_FILE && !COMBINE_WITH_ENGINE_LOGGER && QT_NO_DEBUG

void FileLogger::installMessageHandler( const QString& aFileName, bool aFilterByMarker )
{
    gDebugFile.setFileName( aFileName );
    gFilterByLogMarker = aFilterByMarker;
    
    if (gDebugFile.open( QIODevice::Text | QIODevice::WriteOnly ))
    {
        qInstallMsgHandler( FileLogger::handleMessage );
    }
}

void FileLogger::uninstallMessageHandler()
{
    if ( gDebugFile.isOpen() )
    {
        qInstallMsgHandler( 0 );
        gDebugFile.close();
    }
}

void FileLogger::handleMessage( QtMsgType aType, const char* aMsg )
{
    if ( aType == QtDebugMsg )
    {
        QString message( aMsg );
        if ( gFilterByLogMarker && message.contains( QString( KLogMarker ) ) )
        {
            QTextStream debugStream( &gDebugFile );
            QDateTime dateTime( QDateTime::currentDateTime() );            
            debugStream<<dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz")<<" : "<<message<<endl;
        }
    }
}


// ============================================================================
// Create log dir
// ============================================================================
void installLogDir()
{
#if defined(TRACE_TO_FILE) || defined(IRDEBUG_TRACE_TO_FILE)  
    QDir logDir(KTraceOutputDir);
    if(!logDir.exists())
    {
        logDir.mkpath(KTraceOutputDir);
    }

    QFile logFile(KTraceOutputFileName);
    if(logFile.exists())
    {
        logFile.remove(KReadableFileName);
        logFile.copy(KTraceOutputFileName , KReadableFileName);
    }

    QFile logFileIRUI("c:/logs/internetradio/iruilog.txt");
    if (logFileIRUI.exists()) 
    {
        logFileIRUI.remove("c:/data/iruilog.txt");     
        logFileIRUI.copy("c:/logs/internetradio/iruilog.txt" , "c:/data/iruilog.txt");    
    }
#endif    
}         
         
         
// ============================================================================
// SIGNAL/SLOT CONNECTION CHECKER
// ============================================================================
bool connectAndTest( const QObject* aSender, const char* aSignal,
                            const QObject* aReceiver, const char* aMember,
                            Qt::ConnectionType aType )
{
    bool connected = QObject::connect( aSender, aSignal, aReceiver, aMember, aType );
    if ( !connected )
    {
        LOG( "Failed to make a signal-slot connection!" );
        LOG_FORMAT( "sender: %s", aSender->metaObject()->className() );
        LOG_FORMAT( "signal: %s", aSignal );
        LOG_FORMAT( "receiver: %s", aReceiver->metaObject()->className() );
        LOG_FORMAT( "slot: %s", aMember );
    }
    return connected;
}

bool connectAndAssert( const QObject* aSender, const char* aSignal,
                            const QObject* aReceiver, const char* aMember,
                            Qt::ConnectionType aType )
{
    bool connected = connectAndTest( aSender, aSignal, aReceiver, aMember, aType );
    Q_ASSERT( connected );
    return connected;
}



