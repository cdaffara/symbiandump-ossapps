/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef LOGSLOGGER_H
#define LOGSLOGGER_H

#include <QDebug>


// Disabled PC_lint warning for "Undefined preprocessor variable 'UNIT_TESTING'"
//lint -e553 
#if ( !defined(_DEBUG) || defined(QT_NO_DEBUG_OUTPUT) ) // release or unit test build; no logs

#define LOGS_QDEBUG( a )
#define LOGS_QDEBUG_2( a, b)
#define LOGS_QDEBUG_3( a, b, c)
#define LOGS_QDEBUG_4( a, b, c, d)
#define LOGS_QDEBUG_5( a, b, c, d, e)
#define LOGS_QWARNING( a )
#define LOGS_QWARNING_2( a, b )
#define LOGS_QCRITICAL( a )
#define LOGS_QCRITICAL_2( a, b )
#define LOGS_QCRITICAL_3( a, b, c)
#define LOGS_QDEBUG_EVENT( ev )
#define LOGS_QDEBUG_EVENT_ARR( arr )

#else // debug build; logs

const char logsDebugPrefix[]    = "Debug: ";
const char logsWarningPrefix[]  = "Warning: ";
const char logsCriticalPrefix[] = "Critical: ";
const char logsFatalPrefix[]    = "Fatal: ";

#include <e32debug.h>    // RDebug
#include <QFile>
#include <QTextStream>
#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>

const char logsLogFile[] = "C:/logs/logsapp/logsappdebug.txt";// Logs application log filename.


#define LOGS_DEBUG_ENABLED

#define LOGS_QDEBUG( a )\
    qDebug() << a;

#define LOGS_QDEBUG_2( a, b)\
    qDebug() << a << b;

#define LOGS_QDEBUG_3( a, b, c)\
    qDebug() << a << b << c;

#define LOGS_QDEBUG_4( a, b, c, d)\
    qDebug() << a << b << c << d;

#define LOGS_QDEBUG_5( a, b, c, d, e)\
    qDebug() << a << b << c << d << e;

#define LOGS_QWARNING( a )\
    qWarning() << a;

#define LOGS_QWARNING_2( a, b )\
    qWarning() << a << b;      

#define LOGS_QCRITICAL( a )\
    qCritical() << a;  

#define LOGS_QCRITICAL_2( a, b )\
    qCritical() << a << b;  
    
#define LOGS_QCRITICAL_3( a, b, c)\
    qCritical() << a << b << c;

// Event logging can be disabled by commenting line below
#define LOGS_EVENT_LOGGING_ENABLED

#ifdef LOGS_EVENT_LOGGING_ENABLED
#define LOGS_QDEBUG_EVENT( ev ) \
    qDebug() << "Logs event index:" << ev->index() << "type:" << ev->eventType() \
    << "dir:" << ev->direction() << "remote:" << ev->remoteParty() << "num:" << ev->number() \
    << "inView:" << ev->isInView() << "state:" << ev->eventState() << "id:" << ev->logId();

#define LOGS_QDEBUG_EVENT_ARR( arr )\
    qDebug() << "Logs event arr:";\
    foreach( LogsEvent* e, arr ) { \
    LOGS_QDEBUG_EVENT( e ) }
#else
#define LOGS_QDEBUG_EVENT( ev )
#define LOGS_QDEBUG_EVENT_ARR( arr )
#endif


#endif

#endif // LOGSLOGGER_H

// End of File
