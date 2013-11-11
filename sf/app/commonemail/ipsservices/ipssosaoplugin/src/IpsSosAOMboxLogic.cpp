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
*     Contains mailbox specified always online logic
*
*/

#include "ipssosaopluginheaders.h"

//<QMail>
const TInt KAOSecondsInMinute = 60;
//</QMail>
const TInt KIpsSosAOMboxLogicMinGra = 1;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic::CIpsSosAOMBoxLogic( 
        CMsvSession& aSession, 
        TMsvId aMailboxId ) :
    iSession( aSession ), iMailboxId( aMailboxId ), iState( EStateError ),
    iErrorCounter( 0 ), iError( KErrNone ), iTimer( NULL ),
	//<QMail>
    iFetchMsgArray( KIpsSosAOMboxLogicMinGra ), iAgent( NULL ),
    iIsRoaming( EFalse )
	//</QMail>
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic::~CIpsSosAOMBoxLogic()
    {
    FUNC_LOG;
    delete iTimer;
	//<QMail>
	//</QMail>
    delete iAgent;
    iFetchMsgArray.Close();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic* CIpsSosAOMBoxLogic::NewL( 
        CMsvSession& aSession,
        TMsvId aMailboxId )
    {
    FUNC_LOG;
    CIpsSosAOMBoxLogic* self = new (ELeave) CIpsSosAOMBoxLogic( 
            aSession,  aMailboxId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::ConstructL()
    {
    FUNC_LOG;
    iTimer = CIpsSosAOPluginTimer::NewL( CActive::EPriorityStandard, *this );
	//<QMail>

	
    iAgent = CIpsSosAOBaseAgent::CreateAgentL( iSession, *this, iMailboxId );
    
    //</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//   
void CIpsSosAOMBoxLogic::TimerFiredL()
    {
    FUNC_LOG;
    HandleEventAndSwitchStateL( EEventTimerFired );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//   
void CIpsSosAOMBoxLogic::OperationCompletedL( TInt aError )
    {
    FUNC_LOG;
    iError = aError;
    HandleEventAndSwitchStateL( EEventOperationCompleted );
    iError = KErrNone;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
void CIpsSosAOMBoxLogic::SendCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( aCommand )
        {
        case ECommandStart:
            HandleEventAndSwitchStateL( EEventStart );
            break;
        case ECommandStop:
            HandleEventAndSwitchStateL( EEventStopAndRemoveOps );
            break;
        case ECommandSuspend:
            HandleEventAndSwitchStateL( EEventSuspendOperations );
            break;
        case ECommandContinue:
            HandleEventAndSwitchStateL( EEventContinueOperations );
            break;
        case ECommandStartSync:
            HandleEventAndSwitchStateL( EEventStartSync );
            if ( iState != EStateSyncOngoing )
                {
                User::Leave( KErrGeneral );
                }
            break;
        case ECommandStartedRoaming:
            iIsRoaming = ETrue;
            if ( !CanConnectIfRoamingL() )
                {
                HandleEventAndSwitchStateL( EEventStopAndRemoveOps );
                }
            break;
        case ECommandStoppedRoaming:
            iIsRoaming = EFalse;
            HandleEventAndSwitchStateL( EEventStart );
            break;
        case ECommandCancelAndDisconnect:
            iAgent->CancelAllAndDisconnectL();
            CalculateToNextIntervalL();
            break;
        case ECommandCancelDoNotDiconnect:
            if ( iAgent->GetState() != CIpsSosAOBaseAgent::EStateCompleted &&
                    iAgent->GetState() != CIpsSosAOBaseAgent::EStateIdle  )
                {
                iAgent->CancelAllAndDoNotDisconnect();
                CalculateToNextIntervalL();
                }
            break;
        case ECommandClearDoNotDisconnect:
            iAgent->ClearDoNotDisconnect();
            break;
        case ECommandDoNotDisconnect:
            iAgent->DoNotDisconnect();
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
void CIpsSosAOMBoxLogic::FetchMessagesL( const RArray<TMsvId> aArray )
    {
    FUNC_LOG;
    //<Qmail>
    
    //</Qmail>
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        iFetchMsgArray.AppendL( aArray[i] );
        }
    HandleEventAndSwitchStateL( EEventFetchMessages );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
TMsvId CIpsSosAOMBoxLogic::GetMailboxId()
    {
    FUNC_LOG;
    return iMailboxId;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
CIpsSosAOBaseAgent& CIpsSosAOMBoxLogic::GetAgent() const
    {
    FUNC_LOG;
    return *iAgent;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TBool CIpsSosAOMBoxLogic::IsEmnOn() const
    {
    FUNC_LOG;
    TBool ret = ETrue;
	//<QMail>
    CIpsSosAOSettingsHandler* settings = NULL;
    TRAP_IGNORE( settings = 
            CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId));
    
    if( settings )
        {
        IpsServices::TIpsSetDataEmnStates state = IpsServices::EMailEmnOff;
        state = settings->EmailNotificationState();
        
        if ( state == IpsServices::EMailEmnOff )
            {
            ret = EFalse;
            }
        
        delete settings;
        }
    
	//</QMail>
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
//<QMail>
void CIpsSosAOMBoxLogic::GetEmailAddress(  
    TBuf<KIpsSosAOTextBufferSize>& aEmailAddress ) const
    {
    FUNC_LOG;
    CIpsSosAOSettingsHandler* settings = NULL;

    TRAPD( err, settings = CIpsSosAOSettingsHandler::NewL(
            iSession, iMailboxId) );
    
    if( settings )
        {
        HBufC* addr = NULL;
        TRAP(err, addr = settings->EmailAddressL());
        
        if( addr )
            {
            __ASSERT_DEBUG( ( 
                    addr->Length() <= KIpsSosAOTextBufferSize ), 
                    User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
            
            if ( addr->Length() <= KIpsSosAOTextBufferSize )
                {
                aEmailAddress.Copy( *addr );
                }
            delete addr;
            }   
        
        delete settings;
        }
    
	//<QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//<QMail>
void CIpsSosAOMBoxLogic::SetEmnReceivedFlagL( TBool aNewValue )
    {
    
    CIpsSosAOSettingsHandler* settings = 
            CIpsSosAOSettingsHandler::NewL(
                iSession, iMailboxId );
    CleanupStack::PushL(settings);
    
    settings->SetEmnReceivedButNotSyncedFlag( aNewValue );
    
    CleanupStack::PopAndDestroy(settings);
	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TBool CIpsSosAOMBoxLogic::IsMailboxRoamingStoppedL()
    {
    TBool ret = EFalse;
    if ( iIsRoaming && !CanConnectIfRoamingL() )
        {
        ret = ETrue;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  
void CIpsSosAOMBoxLogic::HandleEventAndSwitchStateL( 
        TInt aEvent )
    {
    FUNC_LOG;
    TInt event = aEvent;
    TInt cycles = 0;
    
    if ( event == EEventStart && iState == EStateError && 
          ( (iIsRoaming && CanConnectIfRoamingL())||
             !iIsRoaming ) )
        {
        NM_COMMENT("CIpsSosAOMBoxLogic: event: EEventStart");
        event = EEventNop;
        iAgent->LoadSettingsL();
        iTimer->Cancel();
        INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", KIpsSosAOStartDelaySeconds);
        iTimer->After( KIpsSosAOStartDelaySeconds );
        iState = EStateWaitSyncStart;
        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart");
        }
    
    while ( event != EEventNop && iState != EStateError )
        {
        switch ( event )
            {
            case EEventTimerFired:
                NM_COMMENT("CIpsSosAOMBoxLogic: event: EEventTimerFired");
                event = HandleTimerFiredL();
              break;
            case EEventStartSync:
                NM_COMMENT("CIpsSosAOMBoxLogic: event: EEventStartSync");
                if ( iState == EStateWaitSyncStart )
                    {         
                    iError = KErrNone;
                    iAgent->StartSyncL();
                    iState = EStateSyncOngoing;
                    NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateSyncOngoing");
                    }
                else if ( iState == EStateIdleAndWaitCommands )
                    {
                    // start sync command in idle state is currently 
                    // received when emn arrives, checking scheduling 
                    // if user have selected week days / hours in setting
					//<QMail>
                    CIpsSosAOSettingsHandler* settings = 
                             CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId);
                    CleanupStack::PushL(settings);
                    
                    CIpsSosAOSchedulerUtils* scheduler = 
                        CIpsSosAOSchedulerUtils::NewLC( *settings );
                        
                    TTimeIntervalSeconds seconds;
                    TIpsSosAOConnectionType type = 
                            scheduler->SecondsToNextMark( seconds );
                    
					//</QMail>
                    if ( type == EAOCConnectAfter )
                        {
                        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart");
                        iState = EStateWaitSyncStart;
                        INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", seconds);
                        iTimer->After( seconds );
                        
                        SetEmnReceivedFlagL( ETrue );
                        }
                    else
                        {
                        iError = KErrNone;
                        iAgent->StartSyncL();
                        iState = EStateSyncOngoing;
                        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateSyncOngoing");
                        }
					//<QMail>
                    CleanupStack::PopAndDestroy( 2, settings );
					//</QMail>
                    }
                // ignore in other states
                event = EEventNop;
                break;
            case EEventFetchMessages:
                NM_COMMENT("CIpsSosAOMBoxLogic: event: EEventFetchMessages");
                if ( ( iState == EStateWaitSyncStart || 
                       iState == EStateIdleAndWaitCommands ) &&
                        iFetchMsgArray.Count() > 0 )
                    {
                    iError = KErrNone;
                    iAgent->StartFetchMessagesL( iFetchMsgArray );
                    iFetchMsgArray.Reset();
                    NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateFetchOngoing");
                    iState = EStateFetchOngoing;
                    }
                else 
                    {
                    // ignore event in other states
                    iFetchMsgArray.Reset();
                    }
                event = EEventNop;
                break;
            case EEventOperationCompleted:
                NM_COMMENT("CIpsSosAOMBoxLogic: event: EEventOperationCompleted");
                event = HandleOperationCompletionL();
                break;
            case EEventSuspendOperations:
                NM_COMMENT("CIpsSosAOMBoxLogic: event: EEventSuspendOperations");
                SuspendOperations();
                iState = EStateSuspended;
                event = EEventNop;
                break;
            case EEventContinueOperations:
                NM_COMMENT("CIpsSosAOMBoxLogic: event: EEventContinueOperations");
                if ( iState == EStateSuspended )
                    {
                    INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", KIpsSosAOContinueWaitTime);
                    iTimer->After( KIpsSosAOContinueWaitTime );
                    }
                event = EEventNop;
                // ignore if in other states
                break;
            case EEventStopAndRemoveOps:
                NM_COMMENT("CIpsSosAOMBoxLogic: event: EEventStopAndRemoveOps");
                // notify deletion
                iAgent->CancelAllAndDisconnectL();
                iTimer->Cancel();
                NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateError");
                iState = EStateError;
                event = EEventNop;
                break;
            case EEventStart:
            default:
                // ignore other events
                event = EEventNop;
                break;
            }
        
        // TO Prevent forever loop (and freezing) 
        // if there is programming error in state machine logic
        __ASSERT_ALWAYS( ( 
                cycles < KIpsSosAOMaxStateCycles ), 
                User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
        ++cycles;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TInt CIpsSosAOMBoxLogic::GetCurrentState() const
    {
    FUNC_LOG;
    return iState;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TInt CIpsSosAOMBoxLogic::HandleTimerFiredL()
    {
    FUNC_LOG;
    // panic if we are in wrong state;
    __ASSERT_DEBUG( ( iState == EStateWaitSyncStart || 
            iState == EStateSuspended ), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    // handle emn
    TInt event = EEventNop;

    CIpsSosAOBaseAgent::TAgentState agentState = iAgent->GetState();
    if ( iState == EStateSuspended &&
        ( agentState == CIpsSosAOBaseAgent::EStateConnectAndSyncOnHold || 
          agentState == CIpsSosAOBaseAgent::EStatePopulateOnHold ) )
        {
        iTimer->Cancel();
        iAgent->ContinueHoldOperations();
        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateSyncOngoing");
        iState = EStateSyncOngoing;
        }
    else if ( iState == EStateSuspended && 
            agentState == CIpsSosAOBaseAgent::EStateFetchOnHold )
        {
        iTimer->Cancel();
        iAgent->ContinueHoldOperations();
        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateFetchOngoing");
        iState = EStateFetchOngoing;
        }
    else
        {
        event = CheckSchedulingAndSwitchStateL();
        }
    // in other states, timer event is ignored
    
    return event;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
TInt CIpsSosAOMBoxLogic::HandleOperationCompletionL()
    {
    FUNC_LOG;
    TInt event = EEventNop;
	//<Qmail>
    CIpsSosAOSettingsHandler* settings = 
             CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId);
    CleanupStack::PushL(settings);
	//</Qmail>     
    if ( iState == EStateSyncOngoing )
        {
        INFO_1("CIpsSosAOMBoxLogic: operation completed error: %d", iError);
        bool doSaveSyncTime = EFalse;
        if ( !( iError == KErrNone || iError == KErrCancel ) )
            {
            ++iErrorCounter;
            }
        else
            {
            if (iError == KErrNone)
                {
                doSaveSyncTime = ETrue;
                }
            iError = KErrNone;
            iErrorCounter = 0;
            }
        
        if ( iError != KErrNone && 
                IsErrorFatalL( iError ) )
            {
            iAgent->CancelAllAndDisconnectL();
            // switch ao off
			//<QMail>
            
            settings->SetAlwaysOnlineState( IpsServices::EMailAoOff );
            // emn not swithced of if its going to be "always on" in
            // future
            
            settings->SetEmnReceivedButNotSyncedFlag( EFalse );
            //</QMail>
            NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateError");
            iState = EStateError;
            iErrorCounter = 0;
            }
        else if ( iError != KErrNone && 
                iErrorCounter < KIpsSosAOMaxReTryTimes )
            {
            // not fatal error
			//<QMail>
            IpsServices::TIpsSetDataAoStates state = 
                    settings->AlwaysOnlineState();
            
            if ( state == IpsServices::EMailAoOff )
                {
                NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateIdleAndWaitCommands");
                iState = EStateIdleAndWaitCommands;
                }
            else
                {
                if( !iTimer->IsActive() )
                    {
                    INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", KIpsSosAOReTryInterval);
                    iTimer->After( KIpsSosAOReTryInterval );
                    }
                NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart");
                iState = EStateWaitSyncStart;
                }

			//</QMail>
            }
        else if ( iError != KErrNone && 
                iErrorCounter >= KIpsSosAOMaxReTryTimes )
            {
            
            SetEmnReceivedFlagL( EFalse );
            // not fatal, but all re trys are gone
            CalculateToNextIntervalL();
            }
        else
            {
            // no errors
            // update successfull sync time to settings
            if (doSaveSyncTime)
                {
                SaveSuccessfulSyncTimeL();
                }
            // and adjust timer to sync interval
            CalculateToNextIntervalL();
            }
        }
    else if ( iState == EStateFetchOngoing )
        {
        iError = KErrNone;
        if ( iTimer->IsActive() )
            {
            NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart");
            iState = EStateWaitSyncStart;
            }
        else
            {
            event = CheckSchedulingAndSwitchStateL();          
            }
        }
		//<Qmail>
    CleanupStack::PopAndDestroy(settings);
	//</Qmail>
    return event;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// 
void CIpsSosAOMBoxLogic::SuspendOperations()
    {
    FUNC_LOG;
    if ( iState == EStateSyncOngoing || iState == EStateFetchOngoing )
        {
        iAgent->HoldOperations();
        }
    // set suspend watchdog, if clien not continue this
    // ensure ao logic to continue
    if ( !iTimer->IsActive() || iState == EStateSyncOngoing || iState == EStateFetchOngoing)
        {
        INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", KIpsSosAOSuspendWatchdogTime);
        iTimer->After( KIpsSosAOSuspendWatchdogTime );
        }
    NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateSuspended");
    iState = EStateSuspended;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//<QMail>

TBool CIpsSosAOMBoxLogic::IsErrorFatalL( TInt aError )
    {
    FUNC_LOG;
    // error can be fatal only if we haven't got successful connect
    // in mailbox ever and error is something else than KErrNone or
    // KErrCancel, and retry interval is reached
    TBool ret = EFalse;
    //<Qmail>
    
    //</Qmail>
    
    CIpsSosAOSettingsHandler* settings = 
             CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId);
    CleanupStack::PushL(settings);
    
    IpsServices::TAOInfo info = settings->LastUpdateInfo();
    if ( aError != KErrNone && aError != KErrCancel 
            && !info.iUpdateSuccessfulWithCurSettings
            && iErrorCounter >= KIpsSosAOMaxReTryTimes )
        {
        ret = ETrue;
        }
    
    CleanupStack::PopAndDestroy(settings);
    //</QMail>
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOMBoxLogic::TMBoxLogicEvent 
    CIpsSosAOMBoxLogic::CheckSchedulingAndSwitchStateL()
    {
	//<Qmail>
    FUNC_LOG;
    iTimer->Cancel();
    TMBoxLogicEvent event = EEventNop;
    TTimeIntervalSeconds secondsToConnect = CalculateScheduledSyncTimeL();
    
    CIpsSosAOSettingsHandler* settings = 
             CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId);
    CleanupStack::PushL(settings);
      
    if ( secondsToConnect.Int() == 0 )
        {
        //check last successfull sync time and check is interval reached

    
        IpsServices::TAOInfo info = settings->LastUpdateInfo();
        if ( !info.iUpdateSuccessfulWithCurSettings )
            {
            event = EEventStartSync;
            }
        else 
            {
            TTime now;
            now.HomeTime();
            TInt interval = 
                    settings->InboxRefreshTime() * KAOSecondsInMinute;
            TTimeIntervalSeconds secsFromLastSync;
            now.SecondsFrom( info.iLastSuccessfulUpdate, secsFromLastSync );
            if ( (secsFromLastSync.Int() > 0) && ( 
                    secsFromLastSync.Int() < interval ) )
                {
                // adjust timer to correct sync time
                TInt syncAfter = interval - secsFromLastSync.Int();
                INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", syncAfter);
                iTimer->After(syncAfter); 
                NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart");
                iState = EStateWaitSyncStart;
                }
            else
                {
                event = EEventStartSync;
                }
            }
        
        }
    else if ( secondsToConnect.Int() == KErrNotFound )
        {
        // means that ao is not on (but emn is)
        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateIdleAndWaitCommands");
        iState = EStateIdleAndWaitCommands;
      
        if ( settings->EmnReceivedButNotSyncedFlag() )
            {
            // got emn when logic was stopped
            event = EEventStartSync;
            }
        }
    else
        {
        INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", secondsToConnect);
        iTimer->After( secondsToConnect );
        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart");
        iState = EStateWaitSyncStart;
        

        }
    CleanupStack::PopAndDestroy(settings);
    //</Qmail>
    return event;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::CalculateToNextIntervalL()
    {
    FUNC_LOG;
    
    iTimer->Cancel();
    TTimeIntervalSeconds interval;
    // first check and calculate scheduling function also loads settings
    interval = CalculateScheduledSyncTimeL();
    
    if ( interval.Int() == 0 )
        {
        // add next interval, if interval is grater than 0 it means
        // that off-line time is going, user have selected days / times from
        // settings
		
		//<QMail>
        CIpsSosAOSettingsHandler* settings = 
                 CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId);
        CleanupStack::PushL(settings);
        TInt inboxRefreshTime = settings->InboxRefreshTime();
        TInt secs = inboxRefreshTime * KAOSecondsInMinute;
        INFO_1("CIpsSosAOMBoxLogic: inboxRefreshTime: %d", inboxRefreshTime);
        INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", secs);
        if (secs > 0) 
            {
            iTimer->After(TTimeIntervalSeconds(secs));
            }
        else 
            {
            // panic at this point when we get zero time from settings
            __ASSERT_DEBUG( EFalse, User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
            iTimer->After(TTimeIntervalSeconds(KAODefaultInboxSyncTimeSecs));
            }
            
        CleanupStack::PopAndDestroy(settings);
	   //</QMail>
        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart");
        iState = EStateWaitSyncStart;
        }
    else if ( interval.Int() > 0 )
        {
        INFO_1("CIpsSosAOMBoxLogic: timer scheduled: %d", interval);
        iTimer->After( interval );
        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateWaitSyncStart");
        iState = EStateWaitSyncStart;
        }
    else 
        {
        NM_COMMENT("CIpsSosAOMBoxLogic: switching state: EStateIdleAndWaitCommands");
        iState = EStateIdleAndWaitCommands;
        }
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TTimeIntervalSeconds CIpsSosAOMBoxLogic::CalculateScheduledSyncTimeL()
    {
    FUNC_LOG;    
	//<Qmail>
    
    TTimeIntervalSeconds seconds=KErrNotFound;
	
    
    CIpsSosAOSettingsHandler* settings = 
             CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId);
    CleanupStack::PushL(settings);
            
    // first check is ao or emn on at all
    if ( (settings->AlwaysOnlineState() == IpsServices::EMailAoOff && 
            settings->EmailNotificationState() == IpsServices::EMailEmnOff  )||
            settings->SelectedWeekDays() == 0 ) // no week days chosen
        {
        // no timed sync on
        CleanupStack::PopAndDestroy(settings);    
        return KErrNotFound;
        }
    
    if ( settings->EmailNotificationState() != IpsServices::EMailEmnOff && 
         !settings->EmnReceivedButNotSyncedFlag()   )
        {
        CleanupStack::PopAndDestroy(settings);
        return KErrNotFound;
        }
    
    
    CIpsSosAOSchedulerUtils* scheduler = CIpsSosAOSchedulerUtils::NewLC(
        *settings );

    TIpsSosAOConnectionType type = scheduler->SecondsToNextMark( seconds );
    if ( type == EAOCDisconnectAfter ) 
        {
        // if type is EAOCDisconnectAfter it means that scheduled days / times
        // end after interval function returned in seconds reference
        // when connection is not kept open we not need timer for 
        // disconnection. Just return 0 as a mark that sync can be started
        seconds = 0;
        }
    else
        {
        // this means that user have choosed time/day schedule at 
        // timed sync and now we have to wait sync time
        }
    CleanupStack::PopAndDestroy( 2, settings );
    
	//</QMail>
    return seconds;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::LoadSettingsL()
    {
    FUNC_LOG;
    //<QMail>
    //removed loading
	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOMBoxLogic::SaveSuccessfulSyncTimeL()
    {
    FUNC_LOG;
    TTime now;
    now.HomeTime();
    //<QMail>
    
	
    CIpsSosAOSettingsHandler* settings = 
             CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId);
    CleanupStack::PushL(settings);
        
    IpsServices::TAOInfo info;
    info.iLastSuccessfulUpdate = now;
    info.iUpdateSuccessfulWithCurSettings = ETrue;
    settings->SetLastUpdateInfo( info );
    // clear flag
    settings->SetEmnReceivedButNotSyncedFlag( EFalse );
            
    CleanupStack::PopAndDestroy(settings);
	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOMBoxLogic::CanConnectIfRoamingL()
    {
    FUNC_LOG;
    TBool ret = EFalse;
    //<Qmail>
    RCmManager mgr;
    CleanupClosePushL(mgr);
    TRAPD(err, mgr.OpenL());
    
    TCmGenConnSettings set = { ECmUsageOfWlanKnown, 
            ECmCellularDataUsageConfirm, ECmCellularDataUsageConfirm };
    if(err==KErrNone){
        TRAP(err, mgr.ReadGenConnSettingsL(set));
        }
    
    CleanupStack::PopAndDestroy(&mgr);
    
    if( err==KErrNone && 
            set.iCellularDataUsageVisitor == ECmCellularDataUsageAutomatic){
        ret = ETrue;
        }
    else{
        ret = EFalse;
        }
   
    //</QMail>
    return ret;          
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOMBoxLogic::FirstEMNReceived()
    {
	//<QMail>
    CIpsSosAOSettingsHandler* settings = NULL;
    TBool ret = EFalse;
    TRAP_IGNORE(settings = CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId));
    if(settings){
        ret = settings->FirstEmnReceived();
        delete settings;
        }
    
    return ret;
    
	//</QMail>
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOMBoxLogic::SetFirstEMNReceived()
    {
    //<QMail>
    
    CIpsSosAOSettingsHandler* settings = NULL;
    TRAP_IGNORE( settings = 
            CIpsSosAOSettingsHandler::NewL(iSession, iMailboxId));
    
    if(settings){
        settings->SetFirstEmnReceived( ETrue );
        
    
        //if alwaysonline was allowed to roam, so will EMN be.
        settings->SetEmailNotificationState(
            settings->AlwaysOnlineState() == 
            IpsServices::EMailAoAlways ? 
                IpsServices::EMailEmnAutomatic : IpsServices::EMailEmnHomeOnly );
        
        
        // set always online state off when emn is on
        settings->SetAlwaysOnlineState( IpsServices::EMailAoOff );
        delete settings;
        }
    
	//</QMail>
    }

// End of file

