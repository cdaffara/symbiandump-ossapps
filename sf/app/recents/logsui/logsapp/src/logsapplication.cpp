/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QObject>
#include <QString>
#include "logsapplication.h"
#include "logsappsettings.h"
#include "logsdefs.h"
#include "logslogger.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsApplication::LogsApplication(int &argc, char *argv[], LogsAppSettings& settings)
    : HbApplication(argc, argv, 
        settings.logsFeaturePreloadingEnabled() ? Hb::NoSplash : Hb::DefaultApplicationFlags ),
      mViewReady(false),
      mReadCompleted(false)
{
#ifdef LOGS_DEBUG_ENABLED
    LOGS_QDEBUG_2( "logs [UI] -> LogsApplication::LogsApplication args", arguments() )
#endif
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsApplication::~LogsApplication()
{
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsApplication::testLogsAppEngineReady(){
    if (mViewReady && !mReadCompleted){
        emit applicationReady();
    }
    mReadCompleted = true;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsApplication::testLogsHandleAppViewReady(){
    if (mReadCompleted && !mViewReady){
        emit applicationReady();
    }
    mViewReady = true;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsApplication::testLogsResetAppReady()
{
    // Wait only for read completion when handling application readiness after
    // hidden exit as viewready signal is not received when view is not changed
    mReadCompleted = false;
}

// end of file
