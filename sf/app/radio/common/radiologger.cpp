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

// System includes
#include <QDebug>

// User includes
#include "radiologger.h"

/*!
 *
 */
void RadioLogger::initCombinedLogger()
{
#if defined LOGGING_ENABLED && defined COMBINE_WITH_ENGINE_LOGGER && !defined BUILD_WIN32
    TRAP_IGNORE( RadioEngineUtils::InitializeL() );
    if ( !MRadioEngineLogger::Logger() ) {
        qDebug() << "FMRadioUi: Cannot combine logs with engine. Engine logger not active";
    }
#endif // LOGGING_ENABLED
}

/*!
 *
 */
void RadioLogger::releaseCombinedLogger()
{
#if defined LOGGING_ENABLED && defined COMBINE_WITH_ENGINE_LOGGER && !defined BUILD_WIN32
    RadioEngineUtils::Release();
#endif // LOGGING_ENABLED
}

/*!
 *    enum Mode { Normal, MethodEnter, MethodExit, DecIndent };
 */
void RadioLogger::logMsg( const char* msg, Mode mode )
{
    Q_UNUSED( msg );
    Q_UNUSED( mode );
#if defined LOGGING_ENABLED && defined COMBINE_WITH_ENGINE_LOGGER && !defined BUILD_WIN32
    MRadioEngineLogger* logger = MRadioEngineLogger::Logger();
    if ( logger ) {
        if ( mode == RadioLogger::Normal ) {
            logger->AddIndent(KMarkerUi()).Add( msg ).Commit();
        } else if ( mode == RadioLogger::MethodEnter ) {
            logger->AddIndentClear( KMarkerUi() ).Add( msg ).Commit();
            logger->IncIndent();
        } else if ( mode == RadioLogger::MethodExit ) {
            logger->DecIndent();
            logger->AddIndentClear( KMarkerUi() ).Add( msg ).Commit();
        } else if ( mode == RadioLogger::DecIndent ) {
            logger->DecIndent();
        } else {}
    }
#endif // LOGGING_ENABLED
}


#ifdef LOGGING_ENABLED

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#ifdef TRACE_TO_FILE
    QFile mDebugFile;
    bool mFilterByLogMarker = false;
#endif

const char* KLogEnter = "\\ %s";
const char* KLogExit = "/ %s";
const char* KLogExitRet = "/ %s, Returning %s";
const char* KLogExitException = "/ %s, Exception raised";
const char* KLogLine = "| ";

/*!
 *
 */
#if defined(__WINS__)
static QString formatString( const char* format, long val )
{
    if ( strcmp( format, "" ) == 0 ) {
        return "";
    }
    QString string;
    string.sprintf( format, val );
    return string;
}
#endif

/*!
 *
 */
MethodLogger::MethodLogger( const char* function, const char* format ) :
    mFunction( function ),
    mFormat( format )
{
    QString msg;
    msg.sprintf( KLogEnter, mFunction );
    WRITELOG_METHOD_ENTER( msg );
}

/*!
 *
 */
MethodLogger::~MethodLogger()
{
    if ( std::uncaught_exception() ) {
        QString msg;
        msg.sprintf( KLogExitException, mFunction );
        WRITELOG_METHOD_EXIT( msg );
    } else {
        QString msg;
        if ( strlen( mFormat ) > 0 ) {
            #if defined(__WINS__)

                long retVal = 0;
                _asm( mov retVal, ebx );
                msg.sprintf( KLogExitRet, mFunction, formatString( mFormat, retVal ) );
                WRITELOG_METHOD_EXIT( msg );

            #else
                msg.sprintf( KLogExit, mFunction );
                WRITELOG_METHOD_EXIT( msg );
            #endif
        } else {
            msg.sprintf( KLogExit, mFunction );
            WRITELOG_METHOD_EXIT( msg );
        }
    }
}
#else

MethodLogger::MethodLogger( const char*, const char* )
{
}
        
MethodLogger::~MethodLogger()
{
}

#endif // LOGGING_ENABLED


#ifdef TRACE_TO_FILE

/*!
 *
 */
void FileLogger::installMessageHandler( const QString& fileName, bool filterByMarker )
{
    mDebugFile.setFileName( fileName );
    mFilterByLogMarker = filterByMarker;

    mDebugFile.open( QIODevice::Text | QIODevice::WriteOnly );

    if ( mDebugFile.isOpen() ) {
        qInstallMsgHandler( FileLogger::handleMessage );
    }
}

/*!
 *
 */
void FileLogger::uninstallMessageHandler()
{
    if ( mDebugFile.isOpen() ) {
        qInstallMsgHandler( 0 );
        mDebugFile.close();
    }
}

/*!
 *
 */
void FileLogger::handleMessage( QtMsgType type, const char* msg )
{
    QTextStream debugStream( &mDebugFile );
    QDateTime dateTime( QDateTime::currentDateTime() );

    if ( type == QtDebugMsg ) {
        QString message( msg );
        if ( mFilterByLogMarker && message.contains( QString( LOGMARKER ) ) )
        {
            debugStream << dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz") << " : " << message << endl;
        }
    }
}
#endif // TRACE_TO_FILE
