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
#include "logsremovestates.h"
#include "logsstatebasecontext.h"
#include "logsremovestatecontext.h"
#include "logslogger.h"
#include "logsremoveobserver.h"
#include "logsmodel.h"
#include "logsevent.h"
#include <logview.h>
#include <logwraplimits.h>

_LIT( KMaxLogsTime, "99991130:235959.999999");
    
// ----------------------------------------------------------------------------
// LogsRemoveStateBase
// ----------------------------------------------------------------------------
//
LogsRemoveStateBase::LogsRemoveStateBase(
    LogsStateBaseContext& context, LogsRemoveStateContext& removeContext) : 
    LogsStateBase(context), mContext(removeContext)
{

}

// ----------------------------------------------------------------------------
// LogsRemoveStateClearAll
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateClearAll::enterL()
{
    TTime time( KMaxLogsTime );
    if (mContext.clearType() == LogsModel::TypeLogsClearAll){
        mBaseContext.logClient().ClearLog( time, mBaseContext.reqStatus() );
    }
    else{
        mBaseContext.logClient().ClearLog( mContext.clearType(), mBaseContext.reqStatus());
    }
    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateClearAll::continueL()
{
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsRemoveStateInit
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateInit::enterL()
{
    mBaseContext.index() = 0;
    if ( mBaseContext.isRecentView() && 
            static_cast<CLogViewRecent&>( mBaseContext.logView() ).SetRecentListL( 
                KLogNullRecentList, mBaseContext.reqStatus() ) ){
        return true;
    }
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateInit::continueL()
{
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsRemoveStateDelete
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateDelete::enterL()
{
    mRemoveIndex = 0;
    if ( deleteNextEvent() ){
        return true;
    }
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateDelete::continueL()
{
    if ( deleteNextEvent() ){
        return true;
    }
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateDelete::deleteNextEvent()
{
    bool deleting(false);
    if ( mRemoveIndex < mContext.removedEvents().count() ){
        int currId = mContext.removedEvents().at(mRemoveIndex).logId();
        LOGS_QDEBUG_2( "logs [ENG]  LogsRemove::DeleteNextEvent, id: ", currId )
        mBaseContext.logClient().DeleteEvent( currId, mBaseContext.reqStatus() );
        deleting = true;
        mRemoveIndex++;
    }
    return deleting;
}

// ----------------------------------------------------------------------------
// LogsRemoveStateDeleteDuplicates
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateDeleteDuplicates::enterL()
{
    mDeleting = false;
    mBaseContext.index() = 0;
    mDeleteDuplicateIds.clear();
    
    // Duplicate deletion supported only for one event at the time
    if ( mContext.removedEvents().count() == 1 && 
         event().Id() == mBaseContext.currentEventId() &&
         duplicatesL() ){
        return true;
    }
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateDeleteDuplicates::continueL()
{
    bool continueFindingDuplicates = false;
    if ( !mDeleting && mBaseContext.duplicatesView().CountL() > 0  && 
         mBaseContext.index() <= mBaseContext.duplicatesView().CountL() ) {
        LOGS_QDEBUG( "logs [ENG] duplicates exist!");
        if ( mBaseContext.index() == 0 ){
            continueFindingDuplicates = 
                mBaseContext.duplicatesView().FirstL(mBaseContext.reqStatus());
            mBaseContext.index()++; 
        } else {
            mDeleteDuplicateIds.append( mBaseContext.duplicatesView().Event().Id() );
            continueFindingDuplicates = 
                mBaseContext.duplicatesView().NextL(mBaseContext.reqStatus());
            mBaseContext.index()++; 
        } 
    } 
    if ( continueFindingDuplicates || deleteNextEvent() ){
        return true;
    }
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateDeleteDuplicates::deleteNextEvent()
{
    mDeleting = false;
    if ( !mDeleteDuplicateIds.isEmpty() ){
        int currId = mDeleteDuplicateIds.takeFirst();
        LOGS_QDEBUG_2( "logs [ENG]  LogsRemoveStateDeleteDuplicates::deleteNextEvent id: ", 
                       currId )
        mBaseContext.logClient().DeleteEvent( currId, mBaseContext.reqStatus() );
        mDeleting = true;
    }
    return mDeleting;
}

// ----------------------------------------------------------------------------
// LogsRemoveStateDone
// ----------------------------------------------------------------------------
//
bool LogsRemoveStateDone::enterL()
{
    mContext.observer().removeCompleted();
    return false;
}

// CONSTANTS
