/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for handling MMS log entries
*     (sent messages and delivery reports)
*
*/



// INCLUDE FILES
#include    <badesca.h>
#include    <msvids.h>
#include    <logcli.h>
#include    <logview.h>
// common component
#include    <sysutil.h>
#include    "LogsApiConsts.h"

// MMS specific
#include    "mmsconst.h"
#include    "mmslog.h"
#include    "mmsgenutils.h"
#include    "mmsservercommon.h" // needed for logging

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  
extern void gPanic(TMmsPanic aPanic);

// CONSTANTS
_LIT(KMmsLogEventTypeName, "MMS");

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CMmsLog::CMmsLog( CLogClient& aLogClient, CLogViewEvent& aLogViewEvent )
    :CMsgActive( EPriorityStandard ),
    iState( EMmsLogIdle ),
    iError( KErrNone ),
    iLogClient( aLogClient ),
    iLogViewEvent( aLogViewEvent ),
    iLastMatchedLink( KLogNullLink )
    {
    }

// Symbian OS default constructor can leave.
void CMmsLog::ConstructL( RFs& aFs )
    {
    
    iFs = aFs;
    iEntryId = KMsvNullIndexEntryId;
    iLogUpdatedEvent = CLogEvent::NewL();
    iFilterList = new ( ELeave ) CLogFilterList;

    CActiveScheduler::Add(this);

    }

// Two-phased constructor.
CMmsLog* CMmsLog::NewL( CLogClient& aLogClient, CLogViewEvent& aLogViewEvent, RFs& aFs )
    {
    CMmsLog* self = new (ELeave) CMmsLog( aLogClient, aLogViewEvent );
    
    CleanupStack::PushL( self );
    self->ConstructL( aFs );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CMmsLog::~CMmsLog()
    {
    Cancel();
    // we don't delete those pointers that the caller owns.
    if( iFilterList )
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        }
    delete iLogUpdatedEvent;
    delete iLogEventType;
    }

// ---------------------------------------------------------
// CMmsLog::StartL
//
// ---------------------------------------------------------
//
void CMmsLog::StartL(
    CLogEvent& aLogEvent,
    CDesCArray& aRemoteParties,
    TRequestStatus& aStatus)
    {
    __ASSERT_DEBUG(iState==EMmsLogIdle,gPanic(EMmsAlreadyBusy));

    // We must save this because we are a state machine,
    // and we keep running around in circles ...
    iLastMatchedLink = KLogNullLink; 
    iLogEvent = &aLogEvent;
    iRemoteParties = &aRemoteParties;
    iCurrentRemoteParty = iRemoteParties->MdcaCount() - 1;
    if ( iCurrentRemoteParty < 0 )
        {
        // if no entries nothing to do
        TRequestStatus* status = &aStatus;
        aStatus = KRequestPending;
        User::RequestComplete( status, KErrNone );
        return;
        }
        
    TTime now;
    // the dates in log must be in universal time, not local time
    now.UniversalTime();
    if ( iLogEvent->Time().Int64() == 0 )
        {
        iLogEvent->SetTime( now );
        }
        
    iFilterList->ResetAndDestroy(); // Clear old filters - just in case

    // try adding event type only once
    iEventTypeAdded = EFalse;

    // Unfortunately neither Link nor Data (message id) can be used
    // in the filter.
    // We must filter for each remote party and match the id's separately

    Queue( aStatus );
    iStatus=KRequestPending;

    SetActive();

    // Pretend that we called an asynchronous service
    // in order to get into the state machine loop
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );

    }
    

// ---------------------------------------------------------
// CMmsLog::GetLink()
// Returns the link id which is the same as TMsvId of the 
// related message
// ---------------------------------------------------------
//
TLogLink CMmsLog::GetLink()
{
    return iLastMatchedLink;	
}

// ---------------------------------------------------------
// CMmsLog::RunL()
//
// ---------------------------------------------------------
//
void CMmsLog::RunL()
//
// When the AO is state driven, this form of Run() is very effective
// DoRunL() takes the AO through the states, queuing another asynch step as required
// if DoRunL() detects the end of the cycle it returns without queuing another cycle.
//
// If Run() would exit without queuing another cycle it reports completion to the client.
// This is true if the asynch step or DoRunL() fails, or the state cycle is complete
//
    {
    iError = iStatus.Int();
    
#ifndef _NO_MMSS_LOGGING_
    if ( iError != KErrNone )
        {
        TMmsLogger::Log( _L("CMmsLog RunL iError = %d"), iError);
        }
#endif
    
    if ( iError >= KErrNone ||
        ( iError == KErrNotFound && iState == EMmsLogCreatingEntry ) )
        {
        TRAPD( error,DoRunL() );      // continue operations, may re-queue
        __ASSERT_DEBUG( error==KErrNone || !IsActive(),User::Invariant() );   // must not requeue in error situations
        if ( IsActive() )             // requeued
            return;
        iError = error;
        }
    Complete( iError );
    }

// ---------------------------------------------------------
// CMmsLog::DoRunL()
//
// ---------------------------------------------------------
//
void CMmsLog::DoRunL()
    {

    // This routine takes the state machine through the states
    // until an error is encountered or the cycle completes.

    if ( iError != KErrNone &&
        !( iState == EMmsLogCreatingEntry && iError == KErrNotFound 
           && iEventTypeAdded == EFalse ) )
        {
        // We encountered an error, and cannot continue
        iStatus = iError;
        iState = EMmsLogIdle;
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        return;
        }

    if ( iError == KErrNotFound && iState == EMmsLogCreatingEntry )
        {
        iState = EMmsLogAddingEventType;
        iError = KErrNone;
        iEventTypeAdded = ETrue;
        // CreateEntryL decrements remote party
        // We get here only if we have tried creating entry and failed
        // after we have tried adding the event type, we retry
        // creating log entry
        iCurrentRemoteParty++;
        }
    else
        {
        SelectNextState();
        }

    if ( iState != EMmsLogFinal )
        {
        // If appropriate, ChangeState makes us active again
        ChangeStateL();
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        }
    else
        {
        iState = EMmsLogIdle;
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        }

    }

// ---------------------------------------------------------
// CMmsLog::DoComplete
//
// ---------------------------------------------------------
//
void CMmsLog::DoComplete( TInt& /* aStatus */ )
    {
    // We are exiting the loop - we say we are idle now
    // This is needed when the mms event type does not exist
    // in the log database.
    // In that case the entries cannot be logged, and the
    // state machine exits before reaching the final state.
    iState = EMmsLogIdle;
    }

// ---------------------------------------------------------
// CMmsLog::SelectNextState
//
// ---------------------------------------------------------
//
void CMmsLog::SelectNextState()
   {

    // If appropriate, the functions called within the switch statement
    // will make us active again. If all is done, the asynchronous request
    // will complete

    switch ( iState )
        {
        case EMmsLogIdle:
            iState = EMmsLogFiltering;
            break;
        case EMmsLogFiltering:
            if ( iEvents )
                {
                // Check if any of found entries matches our criteria
                iState = EMmsLogMatchingEntry;
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("CMmsLog matching log entry"));
#endif
                }
            else
                {
                iState = EMmsLogCreatingEntry;
                }
            break;
        case EMmsLogMatchingEntry:
            if ( iEventMatched )
                {
                iState = EMmsLogUpdatingEntry;
                }
            else if ( iEvents )
                {
                // Something found from the database
                // Check if any of found entries matches our criteria
                iState = EMmsLogMatchingEntry;
                }
            else
                {
                // no match, create new entry
                iState = EMmsLogCreatingEntry;
                }
            break;
        case EMmsLogUpdatingEntry:
            if ( iCurrentRemoteParty >= 0 )
                {
                iState = EMmsLogFiltering;
                }
            else
                {
                // done
                iState = EMmsLogFinal;
                }
            break;
        case EMmsLogCreatingEntry:
            if ( iCurrentRemoteParty >= 0 )
                {
                iState = EMmsLogFiltering;
                }
            else
                {
                // done
                iState = EMmsLogFinal;
                }
            break;
        case EMmsLogAddingEventType:
            iState = EMmsLogCreatingEntry;
            break;
        case EMmsLogFinal:
            break;
        default:
            break;
        }

   }

// ---------------------------------------------------------
// CMmsLog::ChangeState
//
// ---------------------------------------------------------
//
void CMmsLog::ChangeStateL()
    {

    switch ( iState )
        {
        case EMmsLogIdle:
            break;
        case EMmsLogFiltering:
            FilterL();
            break;
        case EMmsLogMatchingEntry:
            MatchEntryL();
            break;
        case EMmsLogUpdatingEntry:
            UpdateEntryL();
            break;
        case EMmsLogCreatingEntry:
            CreateEntryL();
            break;
        case EMmsLogAddingEventType:
            AddEventTypeL();
            break;
        case EMmsLogFinal:
            break;
        default:
            break;
        }

    }

// ---------------------------------------------------------
// CMmsLog::Filter
//
// ---------------------------------------------------------
//
void CMmsLog::FilterL()
    {

    iEvents = EFalse;
    iEventMatched = EFalse;
    
    // Try filtering by status
    // We cannot filter by remote party as it may be either a phone number
    // or an email address.
    // The best filter would be message id but it is stored in data field
    // and events cannot be filtered by data field.
    
    // We need to use several statuses depending on what we are doing
    // - If we are adding an entry in pending state, we only search pending entries
    // - If we are handling a delivery or read report, we must use all states
        
    TLogString logString;
    iLogClient.GetString( logString, R_LOG_DEL_PENDING );
    
    iFilterList->ResetAndDestroy(); // Clear old filters - just in case
    
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( iLogEvent->EventType() );
    // We always filter with pending entries
    iLogClient.GetString( logString, R_LOG_DEL_PENDING );
    filter->SetStatus( logString );
    iFilterList->AppendL(filter);
    // Filter is now in the list - popped from cleanup stack		
    CleanupStack::Pop( filter );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsLog Filtering with status %S"), &filter->Status() );
#endif
    filter = NULL; // this is out of our hands now

    // If we are handling reports, we must filter other statuses, too
    // logstring was set to pending, and that is the state when
    // creating new entries after sending.
    // If we are handling a delivery report or a read report, the status is
    // R_LOG_DEL_DONE, R_LOG_DEL_FAILED, or KLogsMsgReadText
    // We don't care about the failed ones.
    // If the entry has gone into failed state, we create a new one.
    // That should never happen - no new reports should arrive once something has
    // been put into failed state.
    
    if ( iLogEvent->Status().CompareF( logString ) != 0 )
        {
        // Filter entries that are already in delivered state
        filter = CLogFilter::NewL();
        CleanupStack::PushL( filter );
        filter->SetEventType( iLogEvent->EventType() );
        iLogClient.GetString( logString, R_LOG_DEL_DONE );
        filter->SetStatus( logString );
        iFilterList->AppendL(filter);
        // Filter is now in the list - popped from cleanup stack		
        CleanupStack::Pop( filter );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsLog Filtering with status %S"), &filter->Status() );
#endif
        filter = NULL; // this is out of our hands now
        
        // Filter entries in "read" state just to prevent creation of a new one
        filter = CLogFilter::NewL();
        CleanupStack::PushL( filter );
        filter->SetEventType( iLogEvent->EventType() );
        logString.Copy( KLogsMsgReadText );
        filter->SetStatus( logString );
        iFilterList->AppendL(filter);
        // Filter is now in the list - popped from cleanup stack		
        CleanupStack::Pop( filter );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsLog Filtering with status %S"), &filter->Status() );
#endif
        filter = NULL; // this is out of our hands now
        }
    
#ifndef _NO_MMSS_LOGGING_
    if ( iLogEvent->EventType() == KLogMmsEventTypeUid )
        {
        TMmsLogger::Log( _L(" - and event type MMS") );
        }
    else
        {
        TMmsLogger::Log( _L("wrong event type %d"), iLogEvent->EventType() );
        }
#endif
    
    iEvents = iLogViewEvent.SetFilterL( *iFilterList, iStatus );

    if ( iEvents )
        {
        SetActive();
        }
    else
        {
        // If there are no events, CLogViewEvent will not issue an asynchronous request
        // In that case we must complete ourselves
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsLog found no MMS events in any state searched") );
#endif
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    }

// ---------------------------------------------------------
// CMmsLog::MatchEntryL
//
// ---------------------------------------------------------
//
void CMmsLog::MatchEntryL()
    {

    iEventMatched = EFalse;
    // When a view is created, it is positioned on the first event
    // If we know our message entry id, we try to match it
    if ( iLogEvent->Link() != KLogNullLink )
        {
        if ( iLogEvent->Link() == iLogViewEvent.Event().Link() )
            {
            iEventMatched = ETrue;
            }
        }
    else
        {
        // Our message id (returned by MMSC) is stored to Data field.
        if ( iLogEvent->Data().Compare( iLogViewEvent.Event().Data() ) == 0 )
            {
            iEventMatched = ETrue;
            }
        }

    TPtrC dummy;
    TPtrC remoteParty;
    dummy.Set( iRemoteParties->MdcaPoint( iCurrentRemoteParty ) );
    if ( TMmsGenUtils::IsValidMMSPhoneAddress( dummy, ETrue ) )
        {
        dummy.Set( dummy.Right( Min( KMmsNumberOfDigitsToMatch, dummy.Length() ) ) );
        remoteParty.Set( iLogViewEvent.Event().Number().Right(
            Min ( dummy.Length(), iLogViewEvent.Event().Number().Length() ) ) );
        }
    else
        {
        dummy.Set( dummy.Right( Min( KLogMaxRemotePartyLength, dummy.Length() ) ) );
        remoteParty.Set( iLogViewEvent.Event().RemoteParty() );
        }

    if ( dummy.Compare( remoteParty ) != 0 )
        {
        iEventMatched = EFalse;
        }
        
    iStatus = KRequestPending;
    if ( iEventMatched )
        {
        iLastMatchedLink = iLogViewEvent.Event().Link();
        
        // found matching event, switch state to updating
        iLogUpdatedEvent->CopyL( iLogViewEvent.Event() );
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    else
        {
        // get next entry
        iEvents = iLogViewEvent.NextL( iStatus );
        SetActive();
        if ( iEvents == EFalse )
            {
            // If the LogViewEvent did not issue an asynchronous
            // request, we must complete ourselves
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            }
        }
    }

// ---------------------------------------------------------
// CMmsLog::UpdateEntryL
//
// ---------------------------------------------------------
//
void CMmsLog::UpdateEntryL()
    {

    // Update status and message id
    // If contact database id is not defined, try to update it too
    
    // We update the date only if it is later than what is already in the log
    // Otherwise we take the date that is already in the log
    if ( iLogUpdatedEvent->Time() < iLogEvent->Time() )
        {
        iLogUpdatedEvent->SetTime( iLogEvent->Time() );
        }
        
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsLog updating log"));
    TTime time = iLogUpdatedEvent->Time(); 
    TBuf<KMmsDateBufferLength> dateString;
    time.FormatL(dateString,(_L("%*E%*D%X%*N%*Y %1 %2 '%3")));
    TMmsLogger::Log( _L(" - event date %S"), &dateString );
    time.FormatL(dateString,(_L("%-B%:0%J%:1%T%:2%S%:3%+B")));
    TMmsLogger::Log( _L(" - event time %S"), &dateString );
#endif

    // This will mark the entry as "sent" or "delivered" or "failed"
    // or something similar...
    TBool doNotUpdate = EFalse;
     if ( iLogUpdatedEvent->Status().CompareF( KLogsMsgReadText ) == 0 )
         {
         // If the status is already "read" we don't change it
         // This is a case where a delivery report arrives after a read report.
         // Highly unlikely, but possible
         doNotUpdate = ETrue;
 #ifndef _NO_MMSS_LOGGING_
         TMmsLogger::Log( _L("Status already updated to read - do not update again"));
 #endif
         }
     else
         {
         iLogUpdatedEvent->SetStatus( iLogEvent->Status() );
         // Clear the event read flag in case the user has cleared the log view
         // We want this to become visible again.
         iLogUpdatedEvent->ClearFlags( KLogEventRead );
         }
    // if we have a message id, store it
    if ( ( iLogUpdatedEvent->Data().Length() <= 0 ) &&
        ( iLogEvent->Data().Length() > 0 ) )
        {
        iLogUpdatedEvent->SetDataL( iLogEvent->Data() );
        }

    // done with this remote party
    iCurrentRemoteParty--;
    
    if ( !doNotUpdate )
        {
        // update the entry
        iLogClient.ChangeEvent( *iLogUpdatedEvent, iStatus );
        SetActive();
        }
    else
        {
        // backward change - do not touch
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete( status, KErrNone );
        }

    }

// ---------------------------------------------------------
// CMmsLog::CreateEntry
//
// ---------------------------------------------------------
//
void CMmsLog::CreateEntryL()
    {

    TPtrC dummy;
    iLogUpdatedEvent->CopyL( *iLogEvent );
    dummy.Set( iRemoteParties->MdcaPoint( iCurrentRemoteParty ) );
    iLogUpdatedEvent->SetRemoteParty( dummy.Left(
        Min( dummy.Length(), KLogMaxRemotePartyLength ) ) );
        
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsLog creating log entry"));
#endif

    TInt error = KErrNone;

    // search contact database only if remote party is phone number
    // we don't log email recipients when sending.
    // However, if we get a delivery report from an email address, we log it.

    if ( TMmsGenUtils::IsValidMMSPhoneAddress( dummy, ETrue ) )
        {
        iLogUpdatedEvent->SetNumber( dummy.Right(
            Min( dummy.Length(), KLogMaxRemotePartyLength ) ) );
        error = TMmsGenUtils::GetAlias(
            iRemoteParties->MdcaPoint( iCurrentRemoteParty ),
            iAlias,
            KLogMaxRemotePartyLength,
            iFs );
            
        if ( error == KErrNone )
            {
            if ( iAlias.Length() > 0 )
                {
                iLogUpdatedEvent->SetRemoteParty( iAlias );
                }
            }
        }

    // done with this remote party
    TInt size = KMmsIntegerSize; // room for integer types - estimate - 32 bytes
    size += iLogUpdatedEvent->RemoteParty().Size();
    size += iLogUpdatedEvent->Direction().Size();
    size += iLogUpdatedEvent->Status().Size();
    size += iLogUpdatedEvent->Number().Size();
    size += iLogUpdatedEvent->Description().Size();
    size += iLogUpdatedEvent->Data().Size();
    iCurrentRemoteParty--;

    // Query about disk space.
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( &iFs, size, EDriveC ) )
        {
        // we use standard error code here
        iError = KErrDiskFull;
        }
    if ( iError != KErrDiskFull )
        {
        iLogClient.AddEvent( *iLogUpdatedEvent, iStatus );
        SetActive();
        }
    else
        {
        User::Leave( iError );
        }
    }

// ---------------------------------------------------------
// CMmsLog::AddEventTypeL
//
// ---------------------------------------------------------
//
void CMmsLog::AddEventTypeL()
    {
    // Event type is added if create event returns KErrNotFound
    iLogEventType = CLogEventType::NewL();
    iLogEventType->SetUid( KLogMmsEventTypeUid );
    iLogEventType->SetDescription( KMmsLogEventTypeName );
    iLogEventType->SetLoggingEnabled( ETrue );
    iLogClient.AddEventType( *iLogEventType, iStatus );
    SetActive();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

