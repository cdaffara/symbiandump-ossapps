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
#include "logsstatebase.h"
#include "logsstatebasecontext.h"
#include "logslogger.h"
#include <logview.h>

// CONSTANTS

// ----------------------------------------------------------------------------
// LogsStateBase::LogsStateBase
// ----------------------------------------------------------------------------
//
LogsStateBase::LogsStateBase(
    LogsStateBaseContext& context) 
 : mBaseContext(context),
   mNextState(0)
{
}

// ----------------------------------------------------------------------------
// LogsStateBase::~LogsStateBase
// ----------------------------------------------------------------------------
//
LogsStateBase::~LogsStateBase()
{

}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::setNextState
// ----------------------------------------------------------------------------
//
void LogsStateBase::setNextState(LogsStateBase& nextState)
{
    mNextState = &nextState;
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::enterNextStateL
// ----------------------------------------------------------------------------
//
bool LogsStateBase::enterNextStateL()
{
    if ( mNextState ){
        mBaseContext.setCurrentState(*mNextState);
        return mNextState->enterL();
    }
    return false;
}

// ----------------------------------------------------------------------------
// LogsStateBase::enterL
// ----------------------------------------------------------------------------
//
bool LogsStateBase::enterL()
{
    return false;
}

// ----------------------------------------------------------------------------
// LogsStateBase::continueL
// ----------------------------------------------------------------------------
//
bool LogsStateBase::continueL()
{
    return false;
}

// ----------------------------------------------------------------------------
// LogsStateBase::viewCount
// ----------------------------------------------------------------------------
//
int LogsStateBase::viewCountL() const
    {
    return mBaseContext.logView().CountL();
    }

// ----------------------------------------------------------------------------
// LogsStateBase::event
// ----------------------------------------------------------------------------
//
CLogEvent& LogsStateBase::event() const
    {
    //The RVCT compiler provides warnings "type qualifier on return type is meaningless"
    //for functions that return const values. In order to avoid these numerous warnings and 
    //const cascading, the CLogEvent is const_casted here.
    return const_cast<CLogEvent&>( mBaseContext.logView().Event() );
    }

// ----------------------------------------------------------------------------
// LogsStateBase::duplicatesL
// ----------------------------------------------------------------------------
//
bool LogsStateBase::duplicatesL(const CLogFilter* aFilter){
    bool gettingDuplicates( false );
    if ( mBaseContext.isRecentView() ){
        if ( aFilter ){
            gettingDuplicates = 
                static_cast<CLogViewRecent&>( mBaseContext.logView() ).DuplicatesL( 
                    mBaseContext.duplicatesView(), *aFilter, mBaseContext.reqStatus() ); 
        } else {
            gettingDuplicates = 
                static_cast<CLogViewRecent&>( mBaseContext.logView() ).DuplicatesL( 
                    mBaseContext.duplicatesView(), mBaseContext.reqStatus() ); 
        }
    }
    return gettingDuplicates;
}


// ----------------------------------------------------------------------------
// LogsStateSearchingEvent::LogsStateSearchingEvent
// ----------------------------------------------------------------------------
//
LogsStateSearchingEvent::LogsStateSearchingEvent(
    LogsStateBaseContext& context ) 
  : LogsStateBase(context)
{
}


// ----------------------------------------------------------------------------
// LogsStateSearchingEvent::enterL
// ----------------------------------------------------------------------------
//
bool LogsStateSearchingEvent::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsStateSearchingEvent::enterL" );
    if ( viewCountL() > 0 && mBaseContext.logView().FirstL( mBaseContext.reqStatus() ) ){
        return true;
    }    
    return enterNextStateL();    
}


// ----------------------------------------------------------------------------
// LogsStateSearchingEvent::continueL
// ----------------------------------------------------------------------------
//
bool LogsStateSearchingEvent::continueL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsStateSearchingEvent::continueL" );
    int& index = mBaseContext.index();
    if ( event().Id() != mBaseContext.currentEventId() ) {
        index++;            
        if ( index < viewCountL() ){
            return mBaseContext.logView().NextL( mBaseContext.reqStatus() );
        }
    }
    
    return enterNextStateL();
}   
