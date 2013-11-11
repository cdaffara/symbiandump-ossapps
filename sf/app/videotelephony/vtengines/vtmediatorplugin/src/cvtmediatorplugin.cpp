/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Telephony mediator plugin header
*
*/


// INCLUDE FILES
#include <e32def.h>
#include <w32std.h>
#include <apacmdln.h>
#include <apaflrec.h>
#include <apgcli.h>
#include <MediatorDomainUIDs.h>
#include <eikdll.h>
#include <apgcli.h>
#include <apgwgnam.h>
#include <apgtask.h>
#include <mediatorcommandstotelephonyapi.h>
#include <videotelcontrolmediatorapi.h>
#include <videoteltophonecommandsapi.h>
#include "cvtmediatorplugin.h"


// CONSTANTS
#ifdef _DEBUG
#define TRACE(x) RDebug::Print( _L(x) );
#define TRACE2(x,y) RDebug::Print( _L(x),y );
#else
#define TRACE(x)
#define TRACE2(x,y)
#endif
                       
// This array's values are used in algorithm defining if video telephone
// application should be started. Previous state value is substracted from new 
// state value and if result is greater than zero, app is started.
// This tackles transitions where a state is for some reason skipped,
// e.g. idle -> connected (dialling/ringing/answering states were not detected).
// Negative value indicates call clearing.
static const TInt KVtCallStateActionArray[] = {
    0, //ECallStateUnknown
    0, //ECallStateIdle
    1, // ECallStateDialling
    0, // ECallStateEmergencyDialling (not valid video call state)
    1, // ECallStateRinging
    1, // ECallStateConnecting
    1, // ECallStateConnected
    0, // ECallStateHangingUp
    0, // ECallStateHeld (not valid video call state)
    1, // ECallStateAnswering
    0, // ECallStateRejecting
    0  // ECallStateDisconnecting
    };

static const TInt KVtAppNotReady = 5000;

static const TInt KVtEngMdtrCmdTimeout = 500000;

static const TInt KVtInitCallId = -1000;

// array granularity is 2 (use dataport, release dataport commands)
static const TInt KVtMdtrCmdArrayGranularity = 2;

const TUid KVtCmVideoTelUiUid = { 0x101F8681 };

static const TInt KRamNeededForVideoCalls = 4000000;

// VT application path 
_LIT( KVtCmVideoTelUiPath, "\\sys\\bin\\videotelui.exe" );


// -----------------------------------------------------------------------------
// CMediatorTestPlugin::CMediatorTestPlugin
// First phase construction.
// -----------------------------------------------------------------------------
CVtMediatorPlugin::CVtMediatorPlugin() : CMediatorPluginBase(),
    iCommandList( KVtMdtrCmdArrayGranularity ),
    iState( EWaitingAppLaunch ),
    iActiveVideoCallState( ECallStateUnknown ),
    iWaitingVideoCallState( ECallStateUnknown )
    
	{
	ClearData(iActiveVideoCallInfo, iActiveVideoCallState);
	ClearData(iWaitingVideoCallInfo, iWaitingVideoCallState);
	}
	
CMediatorPluginBase* CVtMediatorPlugin::NewL()
    {
    TRACE("CVtMediatorPlugin::NewL<")
    CVtMediatorPlugin* self = new (ELeave) CVtMediatorPlugin;
    TRACE("CVtMediatorPlugin::NewL>")
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CVtMediatorPlugin::~CVtMediatorPlugin()
	{
	TRACE("CVtMediatorPlugin::~CVtMediatorPlugin<")
    
	if ( iMediatorNotifications )
	    {
	    iMediatorNotifications->UnregisterNotificationObserver();
	    delete iMediatorNotifications;
	    }

	// ignore error
	if ( iEventConsumer )
	    {
	    iEventConsumer->UnsubscribeEvent(
	            KMediatorTelephonyDomain,
	            KCatEventsFromTelephony,
	            EPhoneEventCallData );     
	    delete iEventConsumer;
	    }
        
    // ignore error     
	if ( iEventProvider )
	    {
	    iEventProvider->UnregisterEvent(
	            KMediatorVideoTelephonyDomain,
	            KCatVideotelInternalEvents,
	            EVtMediatorEventVideoCallInformation );   
	    delete iEventProvider;
	    }
        
    // ignore error
	if ( iCommandResponder )
	    {
	    iCommandResponder->UnregisterCommand(
	                KMediatorVideoTelephonyDomain,
	                KCatPhoneToVideotelCommands,
	                iCommandList );
	    iCommandList.Close();
	    delete iCommandResponder;
	    }
	
	delete iCommandInitiator;
    
           
    if( iAppDeathActive )
        {
        delete iAppDeathActive;
        iAppThread.Close();
        }

    TRACE("CVtMediatorPlugin::~CVtMediatorPlugin>")
	}

// -----------------------------------------------------------------------------
// StartL.
// -----------------------------------------------------------------------------
void CVtMediatorPlugin::StartL()
	{
    TRACE("CVtMediatorPlugin.StartL<")
    
    // for monitoring VT app starting
    iMediatorNotifications = CMediatorNotifications::NewL();
    iMediatorNotifications->RegisterNotificationObserver( this );
    
    // consumer for call data events
    iEventConsumer = CMediatorEventConsumer::NewL( this );
    
    iCommandInitiator = CMediatorCommandInitiator::NewL( this );
    
    
    RegisterVtInternalEventL();
    
    RegisterDataportCommandL();       
          
    TRACE("CVtMediatorPlugin.StartL>")
	}

// -----------------------------------------------------------------------------
// MediatorEventL.
// -----------------------------------------------------------------------------
void CVtMediatorPlugin::MediatorEventL( TUid /*aDomain*/,
                             TUid aCategory,
                             TInt aEventId,
                             const TDesC8& aData )
    {
    TRACE("CVtMediatorPlugin.MediatorEventL<")
    if ( aCategory == KCatEventsFromTelephony &&
         aEventId == EPhoneEventCallData )
        {
        TTelephonyCallDataParamPackage pckg;
        pckg.Copy( aData );
        const TTelephonyCallDataParam telCallData = pckg();        
        HandleCallStateChangeL( telCallData );        
        }
    TRACE("CVtMediatorPlugin.MediatorEventL>")
    }
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::MediatorEventsAddedL
//
// subscribes to CLI event when it is registered
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::MediatorEventsAddedL( TUid aDomain/*aDomain*/,
                                   TUid aCategory,
                                   const REventList& aEvents )
    {
    TRACE("CVtMediatorPlugin.MediatorEventsAddedL<")
    
    if ( aCategory == KCatEventsFromTelephony )
        {
        ChangeCallDataEventSubscriptionL( aEvents, ETrue );
        }    
    TRACE("CVtMediatorPlugin.MediatorEventsAddedL>")
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::MediatorCommandsAddedL( TUid /*aDomain*/,
                                     TUid aCategory,
                                     const RCommandList& /*aCommands*/ )
    {
    TRACE("CVtMediatorPlugin.MediatorCommandsAddedL<")
    if ( aCategory == KCatPhoneToVideotelCommands )
        {
        HandleVtCommandRegistrationL();
        }
    TRACE("CVtMediatorPlugin.MediatorCommandsAddedL>")
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::MediatorCategoryRemovedL( TUid /*aDomain*/,
    TUid /*aCategory*/ )
    {
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::MediatorEventsRemovedL( TUid /*aDomain*/,
                                     TUid aCategory,
                                     const REventList& aEvents )
    {
    TRACE("CVtMediatorPlugin.MediatorEventsRemovedL<")
    if ( aCategory == KCatEventsFromTelephony )
        {
        ChangeCallDataEventSubscriptionL( aEvents, EFalse );
        }    
    TRACE("CVtMediatorPlugin.MediatorEventsRemovedL>")
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::MediatorCommandsRemovedL( TUid /*aDomain*/,
                                       TUid aCategory,
                                       const RCommandList& /*aCommands*/ )
    {
    TRACE("CVtMediatorPlugin.MediatorCommandsRemovedL<")  
    if ( aCategory == KCatPhoneToVideotelCommands )
        {
        // VT has unregistered commands, meaning it is shutting down
        HandleVtCommandUnregistrationL();
        }
    TRACE("CVtMediatorPlugin.MediatorCommandsRemovedL>")
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::MediatorCommandL
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::MediatorCommandL( TUid aDomain,
                                 TUid aCategory, 
                                 TInt aCommandId,
                                 TVersion /*aVersion*/, 
                                 const TDesC8& aData )
    {
    TRACE("CVtMediatorPlugin.MediatorCommandL<") 
    if ( aCategory == KCatPhoneToVideotelCommands )
        {
        if ( aCommandId == EVtCmdUseDataport )
            {
#ifdef _DEBUG           
            // error in responding is ignored but printed on debug build
            const TInt err =
#endif            
            iCommandResponder->IssueResponse( 
                aDomain, aCategory, aCommandId, KErrNone, KNullDesC8() );
            TRACE2("CVtMediatorPlugin.MediatorCommandL EVtCmdUseDataport err=%d", err )
            HandleDataportCommandL( aData );
            }
        else if ( aCommandId == EVtCmdReleaseDataport )
            {
            if ( EReady == iState )
                {
                const TVersion version( 
                        KVideotelMdtrCommandsVersionMajor,
                        KVideotelMdtrCommandsVersionMinor,
                        KVideotelMdtrCommandsVersionBuild );

#ifdef _DEBUG           
            // error in responding is ignored but printed on debug build
            const TInt err =
#endif                          
                iCommandInitiator->IssueCommand(
                        KMediatorVideoTelephonyDomain,
                        KCatVideotelInternalCommands,
                        EVtMediatorReleaseDataport,
                        version,
                        KNullDesC8() );
            TRACE2("CVtMediatorPlugin.MediatorCommandL EVtCmdReleaseDataport, send to VT err=%d", err )
                
                }
            else
                {
#ifdef _DEBUG           
                // error in responding is ignored but printed on debug build
                const TInt err =
#endif            
                iCommandResponder->IssueResponse( 
                        aDomain, 
                        aCategory, 
                        aCommandId, 
                        KErrNone, 
                        KNullDesC8() );
                TRACE2("CVtMediatorPlugin.MediatorCommandL EVtCmdReleaseDataport, just Resp err=%d", err ) 
                }
           
            }
        }
    TRACE("CVtMediatorPlugin.MediatorCommandL>")
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::CommandResponseL
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::CommandResponseL( TUid aDomain, TUid aCategory, 
    TInt aCommandId, TInt /*aStatus*/, const TDesC8& /*aData*/ )
    {
    TRACE("CVtMediatorPlugin.CommandResponseL<")
    if( ( aDomain == KMediatorVideoTelephonyDomain ) &&
        ( aCategory == KCatVideotelInternalCommands ) )
        {
        TInt res =
        iCommandResponder->IssueResponse( 
                KMediatorVideoTelephonyDomain,
                KCatPhoneToVideotelCommands,
                EVtCmdReleaseDataport, 
                KErrNone, 
                KNullDesC8() );         
         TRACE2("CVtMediatorPlugin.CommandResponseL, IssueResponse res: %d", res )  
         User::LeaveIfError( res );      
        }
    TRACE("CVtMediatorPlugin.CommandResponseL>")
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::CancelMediatorCommand
//
// no-op
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::CancelMediatorCommand( TUid /*aDomain*/,
                                      TUid /*aCategory*/, 
                                      TInt /*aCommandId*/ )
    {
    }
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::ChangeCallDataEventSubscriptionL
//
// Takes care of 'call data' event subscription.
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::ChangeCallDataEventSubscriptionL(
    const REventList& aEvents,
    const TBool aEventRegistered )
    {
    TRACE("CVtMediatorPlugin.ChangeCLIEventSubscription<")
   
    TInt res( KErrNone );
    TInt eventCount = aEvents.Count();
    while ( eventCount-- )
        {
        const TEvent& aEvent = aEvents[ eventCount ];
        if ( aEvent.iEventId == EPhoneEventCallData )
            {
            if ( aEventRegistered )
                {
                // Phone has registered Call data event => subscribe it                
                const TVersion version(
                    KTelephonyEventsVersionMajor,
                    KTelephonyEventsVersionMinor,
                    KTelephonyEventsVersionBuild );
                res = iEventConsumer->SubscribeEvent(
                    KMediatorTelephonyDomain,
                    KCatEventsFromTelephony,
                    EPhoneEventCallData, version );
                }
            else
                {
                // Phone has unregistered Call data event => unsubscribe it
                res = iEventConsumer->UnsubscribeEvent(
                    KMediatorTelephonyDomain,
                    KCatEventsFromTelephony,
                    EPhoneEventCallData );
                }
            TRACE2("   (un)subscribe result=%d", res )
            eventCount = 0; // break loop
            }
        }
    
    TRACE2("CVtMediatorPlugin.ChangeCLIEventSubscription result=%d>", res )
    User::LeaveIfError( res );
    }
    
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::HandleVtCommandRegistrationL
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::HandleVtCommandRegistrationL()
    {
    TRACE("CVtMediatorPlugin.HandleVtCommandRegistrationL<" )
    if ( iState == EWaitingEventRegistration )
        {
        // VT app registered commands => it can also receive events
        // NOTE: it is expected that VT application first subscribes
        // to internal events and only after that registers commands to
        // avoid timing problems.
        iState = EReady;
        delete iCallBack;
        iCallBack = NULL;
        TCallBack cb( &EventRaiserCallback, this );
        iCallBack = new ( ELeave ) CAsyncCallBack( cb, 
            CActive::EPriorityStandard );        
        TRACE("CVtMediatorPlugin enque async callback" )
        iCallBack->CallBack();        
        }
    TRACE("CVtMediatorPlugin.HandleVtCommandRegistrationL>" )
    }

TInt CVtMediatorPlugin::EventRaiserCallback( TAny* aAny )
    {
    TRACE("CVtMediatorPlugin.EventRaiserCallback<" )
    CVtMediatorPlugin* plugin = reinterpret_cast<CVtMediatorPlugin*>( aAny );
    delete plugin->iCallBack;
    plugin->iCallBack = NULL;
    TRAP_IGNORE( plugin->RaiseVtEventL() );
    TRACE("CVtMediatorPlugin.EventRaiserCallback>" )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::HandleVtCommandUnregistrationL()
    {
    // unregistration means that VT app is shutting down.    
    TRACE("CVtMediatorPlugin.HandleVtCommandUnregistrationL<" )
    ClearData(iActiveVideoCallInfo, iActiveVideoCallState);
    //ClearData(iWaitingVideoCallInfo);
    TRACE("CVtMediatorPlugin.HandleVtCommandUnregistrationL>" )
    }


// -----------------------------------------------------------------------------
// CVtMediatorPlugin::
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::LaunchVtAppL()
    {
        
    TRACE("CVtMediatorPlugin.LaunchVtAppL<" )
    
    if ( !IsEnoughMemory() )
        {
        const TVersion KTelephonyCmdVersion(
                KTelephonyCommandsVersionMajor,
                KTelephonyCommandsVersionMinor,
                KTelephonyCommandsVersionBuild );
        const TInt res =
        iCommandInitiator->IssueCommand(
                KMediatorTelephonyDomain,
                KCatVideoTelToPhoneCommands,
                EVtCmdLowMemory,
                KTelephonyCmdVersion,
                KNullDesC8() );
        ClearData(iActiveVideoCallInfo, iActiveVideoCallState);
        ClearData(iWaitingVideoCallInfo, iWaitingVideoCallState);
        TRACE("CVtMediatorPlugin.LaunchVtAppL, Insufficient Memory" )
        return;
        }

    iAppDeathActive = new ( ELeave ) CAppDeathActive( *this, iAppThread );
       
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL( wsSession );

    TInt wgId = 0;
    TBool found = EFalse;

    // Check if there is already application running. Then we do not 
    // start new one - rather we just monitor the existing one.
    while ( ( wgId != KErrNotFound ) && !found )
        {
        CApaWindowGroupName::FindByAppUid( 
            KVtCmVideoTelUiUid, 
            wsSession, 
            wgId );

        TApaTask task( wsSession );
        task.SetWgId( wgId );
        if ( task.Exists() )
            {
            if ( iAppThread.Open( task.ThreadId() ) == KErrNone )
                {
                TExitType exitType = iAppThread.ExitType();
                found = ( exitType == EExitPending );

                if ( found )
                    {
                    iAppThreadId = task.ThreadId();
                    }
                }
            }
            
        if ( !found )
            {
            iAppThread.Close();
             }
        }
    CleanupStack::PopAndDestroy(); // CleanupClosePushL
    
    // If application was not found, then launch new application.
    if ( !found )
        {
        TThreadId threadId;
#ifndef SYMBIAN_SUPPORT_UI_FRAMEWORKS_V1
        CApaCommandLine* cmd = CApaCommandLine::NewLC();
        cmd->SetExecutableNameL( KVtCmVideoTelUiPath );
#else // !SYMBIAN_SUPPORT_UI_FRAMEWORKS_V1
        CApaCommandLine* cmd = CApaCommandLine::NewLC( KVtCmVideoTelUiPath );
#endif // SYMBIAN_SUPPORT_UI_FRAMEWORKS_V1
        cmd->SetCommandL( EApaCommandBackground );
            
        RApaLsSession session;
        User::LeaveIfError( session.Connect() );
        CleanupClosePushL( session );
            
        TInt err = session.StartApp( *cmd, threadId );
        if ( err > KErrNone )
            {
            err = KErrGeneral;
            }
        User::LeaveIfError( err );
        CleanupStack::PopAndDestroy( 2, cmd ); // CleanupClosePushL, cmd
        User::LeaveIfError( iAppThread.Open( threadId ) );
        iAppThreadId = threadId;
        }

    // Start active objects.
    iState = EWaitingEventRegistration;
    iAppDeathActive->Start();
    TRACE("CVtMediatorPlugin.LaunchVtAppL>" )
    }


// -----------------------------------------------------------------------------
// CVtMediatorPlugin::
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::SaveCallData( const TTelephonyCallDataParam& aData, TVtVideoTelephonyCallInformation& iVtCallInfo )
    {
    TRACE("CVtMediatorPlugin.SaveCallData<" )
    iVtCallInfo.iDisplayText = aData.iCLIText.Left(
        TVtVideoTelephonyCallInformation::TDisplayTextMaxLength );
        
    TRACE2("CVtMediatorPlugin.SaveCallData iDisplayText=%S",
        &iVtCallInfo.iDisplayText );
                
    iVtCallInfo.iCallId = aData.iCallId;
    
    if ( KNullDesC() != aData.iRemotePhoneNumber )
        {
        iVtCallInfo.iVoiceCallPossible = ETrue;
        }
    iVtCallInfo.iPhoneNumText = aData.iRemotePhoneNumber;
    iVtCallInfo.iEventDataValidity |= 
        TVtVideoTelephonyCallInformation::EDisplayTextValid;
    TRACE2("CVtMediatorPlugin.SaveCallData data saved=%d>",
        aData.iCallType == ECallTypeVideo )
    }

                     
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::HandleDataportCommandL( const TDesC8& aData )
    {
    TRACE("CVtMediatorPlugin.HandleDataportCommandL<" ) 
    TDataPortPackage pckg;
    pckg.Copy( aData );
    iActiveVideoCallInfo.iDataport = pckg();
    iActiveVideoCallInfo.iEventDataValidity |= 
        TVtVideoTelephonyCallInformation::EDataportValid;     
    RaiseVtEventL();
    TRACE("CVtMediatorPlugin.HandleDataportCommandL>" )
    }

                         
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::ClearData
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::ClearData(TVtVideoTelephonyCallInformation& aVtCallInfo, TCallState& aCallState)
    {
    TRACE("CVtMediatorPlugin.ClearData<" )
    if ( aVtCallInfo.iCallId == iActiveVideoCallInfo.iCallId )
        {
        iState = EWaitingAppLaunch;
        }
    aVtCallInfo.iEventDataValidity = 0;
    aVtCallInfo.iDataport.Zero();
    aVtCallInfo.iVoiceCallPossible = EFalse;
    //for video call, it should be 9/10
    //for other call, it will be from -1 to 8
    aVtCallInfo.iCallId = KVtInitCallId;
    aVtCallInfo.iDisplayText.Zero();
    aCallState = ECallStateIdle;
    TRACE("CVtMediatorPlugin.ClearData>" )
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::RegisterVtInternalEventL
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::RegisterVtInternalEventL()
    {
    TRACE("CVtMediatorPlugin.RegisterVtInternalEventL<" )
    iEventProvider = CMediatorEventProvider::NewL();
    
    TVersion version(
        KVideotelMdtrEventVersionMajor,
        KVideotelMdtrEventVersionMinor,
        KVideotelMdtrEventVersionBuild );

    TCapabilitySet capSet;
    capSet.SetEmpty();
    capSet.AddCapability( ECapabilityReadDeviceData );
            
    const TInt err = iEventProvider->RegisterEvent(
        KMediatorVideoTelephonyDomain,
        KCatVideotelInternalEvents,
        EVtMediatorEventVideoCallInformation,
        version,
        capSet );
          
   
    TRACE2("CVtMediatorPlugin.RegisterVtInternalEventL err=%d>", err )
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::RegisterDataportCommandL
//
// 
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::RegisterDataportCommandL()
    {
    TRACE("CVtMediatorPlugin.RegisterDataportCommandL<" )
    iCommandResponder = CMediatorCommandResponder::NewL( this );
    
    TCapabilitySet capSet;
    capSet.SetEmpty();
    capSet.AddCapability( ECapabilityWriteDeviceData );
    MediatorService::TCommand command;
    
    command.iCaps = capSet;
    command.iVersion = TVersion( 
            KPhoneToVideotelCmdVersionMajor, 
            KPhoneToVideotelCmdVersionMinor,
            KPhoneToVideotelCmdVersionBuild );
    command.iTimeout = KVtEngMdtrCmdTimeout;
        
    // enable microphone command
    command.iCommandId = EVtCmdUseDataport;
    iCommandList.Append( command );
        
    // Releasedataport command
    capSet.SetEmpty();
    capSet.AddCapability( ECapabilityPowerMgmt );
    command.iCommandId = EVtCmdReleaseDataport;
    iCommandList.Append( command );
    
    const TInt err = iCommandResponder->RegisterCommand( 
        KMediatorVideoTelephonyDomain,
        KCatPhoneToVideotelCommands,
        iCommandList );    
    
    TRACE2("CVtMediatorPlugin.RegisterDataportCommandL err=%d>", err )
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::RaiseVtEventL
//
// Raises VT call info event if in suitable state, i.e. VT app has subscribed 
// the event.
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::RaiseVtEventL()
    {
    TRACE("CVtMediatorPlugin.RaiseVtEventL<" )
    TInt result = KVtAppNotReady;  // does not cause leave
    TRACE2("CVtMediatorPlugin.RaiseVtEventL iState=%d>", 
            iState )
    TRACE2("CVtMediatorPlugin.RaiseVtEventL iActiveVideoCallState=%d>", 
            iActiveVideoCallState )            
    if ( iState == EReady && // application subscribed to the event
    
        // don't send event if video call is not starting/ongoing
         ( iActiveVideoCallState >= ECallStateDialling &&
         iActiveVideoCallState <= ECallStateConnected ) )
        {
        const TVersion version(
            KVideotelMdtrEventVersionMajor,
            KVideotelMdtrEventVersionMinor,
            KVideotelMdtrEventVersionBuild );            
            
        const TVtMediatorInfoPackage pckg( iActiveVideoCallInfo );
        result = iEventProvider->RaiseEvent(
            KMediatorVideoTelephonyDomain,
            KCatVideotelInternalEvents,
            EVtMediatorEventVideoCallInformation,
            version,
            pckg
            );
        }
    TRACE2("CVtMediatorPlugin.RaiseVtEventL result=%d>", result )
    User::LeaveIfError( result );
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::HandleCallStateChangeL
//
// Compares previous and new video call states and resolves based on result
// if Video telephone applicaton should be launched.
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::HandleCallStateChangeL(
    const TTelephonyCallDataParam& aData )
    {
    TRACE("CVtMediatorPlugin.HandleCallStateChangeL<" )    
    
    TRACE2("CVtMediatorPlugin.HandleCallStateChangeL calltype=%d>", 
            aData.iCallType )
    TRACE2("CVtMediatorPlugin.HandleCallStateChangeL saved Activecallid=%d>", 
            iActiveVideoCallInfo.iCallId )
    TRACE2("CVtMediatorPlugin.HandleCallStateChangeL saved Waitingcallid=%d>", 
            iWaitingVideoCallInfo.iCallId )
    TRACE2("CVtMediatorPlugin.HandleCallStateChangeL callid=%d>", 
            aData.iCallId )   
    TRACE2("CVtMediatorPlugin.HandleCallStateChangeL ActiveCallOldState=%d>", 
            iActiveVideoCallState )
    TRACE2("CVtMediatorPlugin.HandleCallStateChangeL WaitingCallOldState=%d>", 
            iWaitingVideoCallState )
    TRACE2("CVtMediatorPlugin.HandleCallStateChangeL NewState=%d>",
            aData.iCallState )
            
    //if the call is waitingcall, just save/clear
    if ( aData.iCallType == ECallTypeVideo ||       
        // check also call id because in call clearing states call type may
        // be unspecified but call id is saved in call setup and we can
        // compare to it.
         iActiveVideoCallInfo.iCallId == aData.iCallId  ||
         iWaitingVideoCallInfo.iCallId == aData.iCallId )
        {
        TBool isWaitingCall = ETrue;
        
        /**
         * firstly we should check the callid to identify if it is a waitingcal/activecall
         * imagine the usecase that long press endkey to shutdown both calls.
         * after that checking iState
         */
        if ( iWaitingVideoCallInfo.iCallId == aData.iCallId )
            {
            isWaitingCall =  ETrue;
            }
        else if ( iActiveVideoCallInfo.iCallId == aData.iCallId )
            {
            isWaitingCall = EFalse;
            }
        //no vt app launched, this happens while vt first launching or end key to shutdown both calls
        else if ( iState == EWaitingAppLaunch )
            {
            isWaitingCall =  EFalse;
            }
        
        TRACE2("CVtMediatorPlugin.HandleCallStateChangeL isWaitingCall=%d>",
                isWaitingCall)
        
        TCallState& callState = isWaitingCall?iWaitingVideoCallState:iActiveVideoCallState;
        TVtVideoTelephonyCallInformation& vtCallInfo = isWaitingCall?iWaitingVideoCallInfo:iActiveVideoCallInfo;
            
        TBool launchNeeded = KVtCallStateActionArray[ aData.iCallState ] -
                    KVtCallStateActionArray[callState] > 0;
        if ( isWaitingCall )
            {
            launchNeeded = EFalse;
            }
        callState = aData.iCallState;
            
        switch ( callState )
            {
            case ECallStateDialling:
            case ECallStateRinging:
            case ECallStateConnecting:
            case ECallStateConnected:
                SaveCallData( aData,  vtCallInfo);
                break;
            default:
                // data becomes invalid in other states (=call clearing/idle)
                ClearData(vtCallInfo, callState);
                break;
            }
        if ( launchNeeded )
            {
            LaunchVtAppL();
            }
        if ( !isWaitingCall )
            {
            RaiseVtEventL();
            }
        }
        
    TRACE("CVtMediatorPlugin.HandleCallStateChangeL>" )
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::IsEnoughMemory
//
// Check if there is enough memory to launch
// -----------------------------------------------------------------------------
//
TBool CVtMediatorPlugin::IsEnoughMemory()
    {
    TRACE("CVtMediatorPlugin::IsEnoughMemory<" )    
    // Fetch amount of free memory.
    TMemoryInfoV1Buf memory;
    UserHal::MemoryInfo( memory );
    TInt freeRam = (TInt)( memory().iFreeRamInBytes );
    TRACE2("CVtMediatorPlugin::IsEnoughMemory: freeRam = %d", freeRam )
    
    TBool enoughRam = ETrue;

    if ( freeRam < KRamNeededForVideoCalls )
        {
        FreeRam();
        freeRam = (TInt)( memory().iFreeRamInBytes );
        TRACE2("CVtMediatorPlugin::IsEnoughMemory: after free, freeRam = %d", freeRam )
        if ( freeRam < KRamNeededForVideoCalls )
            {
            enoughRam = EFalse;
            TRACE ("CVtMediatorPlugin::IsEnoughMemory : Not enough RAM")
                
            }
        }
    TRACE("CVtMediatorPlugin::IsEnoughMemory>" )    
    return enoughRam;
    }  
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::FreeRam
// Try to free memory to match the memory usage of VT
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::FreeRam()
    {
    TRACE("CVtMediatorPlugin.FreeRam()<")
    User::CompressAllHeaps();
    TRACE("CVtMediatorPlugin.FreeRam()>")
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::StopDeathActiveL
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::StopDeathActiveL()
    {
    TRACE("CVtMediatorPlugin.StopDeathActive<")
    delete iAppDeathActive;
    iAppThread.Close();
    //if there is a waiting call, check if we need to launch it
    TRACE2("CVtMediatorPlugin.StopDeathActive WaitintCallID=%d>",
            iWaitingVideoCallInfo.iCallId)
    if ( iWaitingVideoCallInfo.iCallId != KVtInitCallId )
        {
        iActiveVideoCallInfo = iWaitingVideoCallInfo;
        iActiveVideoCallState = iWaitingVideoCallState;
        ClearData(iWaitingVideoCallInfo, iWaitingVideoCallState);
        
        if ( iActiveVideoCallState == ECallStateDialling ||
                iActiveVideoCallState ==  ECallStateRinging ||
                iActiveVideoCallState == ECallStateConnecting ||
                iActiveVideoCallState == ECallStateConnected ||
                iActiveVideoCallState == ECallStateAnswering )
                {
                LaunchVtAppL();
                RaiseVtEventL();
                }
        }
    TRACE("CVtMediatorPlugin.StopDeathActive>")
    }
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::CAppDeathActive::CAppDeathActive
// -----------------------------------------------------------------------------
//
CVtMediatorPlugin::CAppDeathActive::CAppDeathActive( 
        CVtMediatorPlugin& aMediatorPlugin,
        RThread& aAppThread
    )
    : CActive( CActive::EPriorityStandard ),
      iMediatorPlugin( aMediatorPlugin ),
      iAppThread( aAppThread )
      
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::CAppDeathActive::~CAppDeathActive
// -----------------------------------------------------------------------------
//
CVtMediatorPlugin::CAppDeathActive::~CAppDeathActive()
    {
    TRACE("CVtMediatorPlugin.~CAppDeathActive")
    Cancel();
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::CAppDeathActive::Start
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::CAppDeathActive::Start()
    {
    TRACE("CVtMediatorPlugin.Start<")
    Cancel();
    iAppThread.Logon( iStatus );
    SetActive();
    TRACE("CVtMediatorPlugin.Start>")
    }

// -----------------------------------------------------------------------------
// CVtMediatorPlugin::CAppDeathActive::RunL
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::CAppDeathActive::RunL()
    {
    //do something here
    TRACE("CVtMediatorPlugin.RunL<")
    iMediatorPlugin.StopDeathActiveL();
    TRACE("CVtMediatorPlugin.RunL>")
      
    }
        
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::CAppDeathActive::DoCancel
// -----------------------------------------------------------------------------
//
void CVtMediatorPlugin::CAppDeathActive::DoCancel()
    {
    iAppThread.LogonCancel( iStatus );
    }
        
// -----------------------------------------------------------------------------
// CVtMediatorPlugin::CAppDeathActive::RunError
// -----------------------------------------------------------------------------
//
TInt CVtMediatorPlugin::CAppDeathActive::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
