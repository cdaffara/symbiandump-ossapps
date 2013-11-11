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

// INCLUDE FILES
#include <exception>
#include <logview.h>
#include "logsworker.h"
#include "logsstatebase.h"
#include "logslogger.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// LogsWorker::LogsWorker
// ----------------------------------------------------------------------------
//
LogsWorker::LogsWorker(bool readAllEvents) 
 : CActive( EPriorityStandard ),
   mReadAllEvents(readAllEvents),
   mLogClient(0),
   mLogViewRecent(0),
   mLogViewEvent(0),
   mDuplicatesView(0),
   mIndex(0),
   mCurrentStateIndex(0),
   mCurrentStateMachine(0),
   mCurrentEventId(-1),
   mLocked(false)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsWorker::LogsWorker()" )
    
    CActiveScheduler::Add( this ); 
  
    LOGS_QDEBUG( "logs [ENG] <- LogsWorker::LogsWorker()" )
}

// ----------------------------------------------------------------------------
// LogsWorker::~LogsWorker
// ----------------------------------------------------------------------------
//
LogsWorker::~LogsWorker()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsWorker::~LogsWorker()" )
    
    Cancel();
    
    LOGS_QDEBUG( "logs [ENG] <- LogsWorker::~LogsWorker()" )
}

// ----------------------------------------------------------------------------
// LogsWorker::lock
// ----------------------------------------------------------------------------
//
int LogsWorker::lock(bool locked)
{
    mLocked = locked;
    return 0;
}

// ----------------------------------------------------------------------------
// LogsWorker::RunL
// ----------------------------------------------------------------------------
//
void LogsWorker::RunL()
{
    LOGS_QDEBUG_3( "logs [ENG] -> LogsWorker::RunL(), (state, status):", 
                   mCurrentStateIndex, iStatus.Int() )
    
    // Error handling in RunError
    __ASSERT_ALWAYS( iStatus.Int() == KErrNone, User::Leave( iStatus.Int() ) );
    
    if ( currentState().continueL() ){
        SetActive();
    }
    
    LOGS_QDEBUG( "logs [ENG] <- LogsWorker::RunL()" )
}

// ----------------------------------------------------------------------------
// LogsWorker::RunError
// ----------------------------------------------------------------------------
//
TInt LogsWorker::RunError(TInt error)
{
    LOGS_QDEBUG_2( "logs [ENG] <-> LogsWorker::RunError(), err:", error )
    
    return KErrNone;
}

// ----------------------------------------------------------------------------
// LogsWorker::DoCancel
// ----------------------------------------------------------------------------
//
void LogsWorker::DoCancel()
{
    if ( mLogViewEvent ){
        mLogViewEvent->Cancel();
    }
    if ( mLogViewRecent ){
        mLogViewRecent->Cancel();
    }
    if ( mDuplicatesView ){
        mDuplicatesView->Cancel();    
    }
    if ( mLogClient ){
        mLogClient->Cancel();
    }
}

// ----------------------------------------------------------------------------
// LogsWorker::setCurrentState
// ----------------------------------------------------------------------------
//
void LogsWorker::setCurrentState(const LogsStateBase& state)
{
    bool found(false);
    for( int i = 0; i < mCurrentStateMachine->count() && !found; i++ ){
        if ( mCurrentStateMachine->at(i) == &state ){
            mCurrentStateIndex = i;
            found = true;
            LOGS_QDEBUG_2( "logs [ENG] <-> LogsWorker::setCurrentState, index:", 
                           mCurrentStateIndex )
        }
    }
}

// ----------------------------------------------------------------------------
// LogsWorker::logView
// ----------------------------------------------------------------------------
//
CLogView& LogsWorker::logView()
{
    if ( mLogViewRecent ){
        return *mLogViewRecent;
    }
    return *mLogViewEvent;
}

// ----------------------------------------------------------------------------
// LogsWorker::duplicatesView
// ----------------------------------------------------------------------------
//
CLogViewDuplicate& LogsWorker::duplicatesView()
{
    return *mDuplicatesView;
}

// ----------------------------------------------------------------------------
// LogsWorker::index
// ----------------------------------------------------------------------------
//
int& LogsWorker::index()
{
    return mIndex;
}

// ----------------------------------------------------------------------------
// LogsWorker::reqStatus
// ----------------------------------------------------------------------------
//
TRequestStatus& LogsWorker::reqStatus()
{
    return iStatus;
}

// ----------------------------------------------------------------------------
// LogsWorker::currentEventId
// ----------------------------------------------------------------------------
//
int LogsWorker::currentEventId()
{
    return mCurrentEventId;
}

// ----------------------------------------------------------------------------
// LogsWorker::logClient
// ----------------------------------------------------------------------------
//
CLogClient& LogsWorker::logClient()
{
    return *mLogClient;
}

// ----------------------------------------------------------------------------
// LogsWorker::isRecentView
// ----------------------------------------------------------------------------
//
bool LogsWorker::isRecentView()
{
    return ( mLogViewRecent != 0 );
}

// ----------------------------------------------------------------------------
// LogsWorker::currentState
// ----------------------------------------------------------------------------
//
LogsStateBase& LogsWorker::currentState()
{
    return *(mCurrentStateMachine->at(mCurrentStateIndex));
}
