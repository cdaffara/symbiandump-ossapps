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
* Description:  Mediator event listener implementation
*
*/



// INCLUDE FILES
#include    "cvtengmdtrmessagelistener.h"
#include    "VtEngUtils.h"
#include    "CVtEngSettings.h"
#include    "CVtEngEventManager.h"
#include    "cvtengmediatorservice.h"

#include    <cvtlogger.h>
#include    <videotelcontrolmediatorapi.h>
#include    <MediatorCommandResponder.h>
#include    <MediatorDomainUIDs.h>
#include    <mediatoreventsfromtelephonyapi.h>
#include    <vtinternalmediatorapi.h>

// CONSTANTS

// mediator command timeout
static const TInt KVtEngMdtrCmdTimeout = 500000;

// array granularity is 2 (enable mic, disable mic commands)
static const TInt KVtEngMdtrCmdArrayGranularity = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngMdtrMessageListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngMdtrMessageListener* CVtEngMdtrMessageListener::NewL(
    CVtEngUtility& aUtils )
    {
    __VTPRINTENTER( "CVtEngMdtrMessageListener.NewL" )
    CVtEngMdtrMessageListener* self = new( ELeave ) 
        CVtEngMdtrMessageListener( aUtils );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    __VTPRINTEXIT( "CVtEngMdtrMessageListener.NewL" )
    return self;
    }

// Destructor
CVtEngMdtrMessageListener::~CVtEngMdtrMessageListener()
    {
    // ignore error in unregistering
    if ( iMediatorResponder )
        {
        iMediatorResponder->UnregisterCommand( 
                KMediatorVideoTelephonyDomain,
                KCatPhoneToVideotelCommands,
                iCommandList );
        iCommandList.Close();
        iMediatorResponder->UnregisterCommand(
                KMediatorVideoTelephonyDomain,
                KCatVideotelInternalCommands,
                EVtMediatorReleaseDataport );
                
        delete iMediatorResponder;
        }
    
             
    // ignore error in unregistering
    if ( iMediatorEvents )
        {
        iMediatorEvents->UnsubscribeEvent( KMediatorVideoTelephonyDomain,
                KCatVideotelInternalEvents,
                EVtMediatorEventVideoCallInformation );
        delete iMediatorEvents;
        }
    
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrMessageListener::MediatorCommandL
// Receives mediator command and passes it to service object
// -----------------------------------------------------------------------------
//
void CVtEngMdtrMessageListener::MediatorCommandL( 
    TUid /*aDomain*/,
    TUid aCategory, 
    TInt aCommandId, 
    TVersion /*aVersion*/,
    const TDesC8& /*aData*/ )
    {
    __VTPRINTENTER( "CVtEngMdtrMessageListener.MediatorCommandL" )
    // service object destroys itself after sending command response
    CVtEngMediatorService::StartLD( *iMediatorResponder, iUtils, aCategory, aCommandId );    
    __VTPRINTEXIT( "CVtEngMdtrMessageListener.MediatorCommandL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrMessageListener::CancelMediatorCommand
// Cancallation of mediator command. no-op, just returns response.
// -----------------------------------------------------------------------------
//
void CVtEngMdtrMessageListener::CancelMediatorCommand( 
    TUid /*aDomain*/,
    TUid /*aCategory*/, 
    TInt /*aCommandId*/ )
    {
    __VTPRINTENTER( "CVtEngMdtrMessageListener.CancelMediatorCommand" )
    // No-op    
    __VTPRINTEXIT( "CVtEngMdtrMessageListener.CancelMediatorCommand" )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrEventListener::MediatorEventL
// Receives mediator event from VT Mediator Plugin. The event
// contains CLI and/or Dataport name.
// -----------------------------------------------------------------------------
//
void CVtEngMdtrMessageListener::MediatorEventL( TUid /*aDomain*/,
    TUid aCategory, 
    TInt aEventId, 
    const TDesC8& aData )
    {
    __VTPRINTENTER( "CVtEngMdtrMessageListener.MediatorEventL" )
    if  ( aCategory == KCatVideotelInternalEvents && 
        aEventId == EVtMediatorEventVideoCallInformation )
        {        
        TVtMediatorInfoPackage package;
        package.Copy( aData );
        TVtVideoTelephonyCallInformation data = package();
        __VTPRINT2( DEBUG_GEN, " VT call info data validity value=%d", 
            data.iEventDataValidity)
        // if message contains dataport name save it
        CVtEngSettings& settings = iUtils.Settings();
        if ( data.iEventDataValidity & 
            TVtVideoTelephonyCallInformation::EDataportValid )
            {
            //__VTPRINT(DEBUG_GEN, " Dataport is ready")
            settings.SetDataportInfoL( data.iDataport );
            }                                       
        // if message contains CLI info handle it (save and notify if state 
        // allows)
        if ( data.iEventDataValidity & 
            TVtVideoTelephonyCallInformation::EDisplayTextValid )
            {            
            MVtEngSessionInfo::TCLI cli;
            cli.iCallId = data.iCallId;
            cli.iName = data.iDisplayText.Left( cli.iName.MaxLength() );
            cli.iVoiceCallPossible = data.iVoiceCallPossible;
            cli.iCallNumber = data.iPhoneNumText.Left( cli.iCallNumber.MaxLength() );
            settings.SetCLI( cli );
            
            // ready == call is answered (allows setting app to foreground)
            if ( IsReadyForCLIEvent() )
                {
                // send event about CLI availability
                CVtEngEventManager::NotifyEvent( KVtEngCLIAvailable );                
                }
            else
                {
                // Wait until connected state is reached (see HandleVtEventL)
                __VTPRINT( DEBUG_GEN, 
                    "CVtEngMdtrMessageListener defer CLI event" )
                iUtils.EventManager().AddObserverL( this );
                }
            }
        }
    __VTPRINTEXIT( "CVtEngMdtrMessageListener.MediatorEventL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrMessageListener::HandleVtEventL
//
// Handles defered CLI event dispatching (to UI). CLI event was defered because
// engine state was not suitable in MediatorEventL which then started observing
// state changes.
// -----------------------------------------------------------------------------
//
void CVtEngMdtrMessageListener::HandleVtEventL( TInt aEvent )
    {
    __VTPRINTENTER( "CVtEngMdtrMessageListener.HandleVtEventL" )
    if ( aEvent == KVtEngSessionStateChanged &&
         IsReadyForCLIEvent() ) // <- ready == call is answered
        {
        iUtils.EventManager().RemoveObserver( this );
        __VTPRINT( DEBUG_GEN, 
            "CVtEngMdtrMessageListener dispatch defered CLI event" )
        CVtEngEventManager::NotifyEvent( KVtEngCLIAvailable );
        }
    
    __VTPRINTEXIT( "CVtEngMdtrMessageListener.HandleVtEventL" )
    }

        
// -----------------------------------------------------------------------------
// CVtEngMdtrMessageListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngMdtrMessageListener::ConstructL()
    {
    __VTPRINTENTER( "CVtEngMdtrMessageListener.ConstructL" )
    
    // for sending command responses
    iMediatorResponder = CMediatorCommandResponder::NewL( this );
    
    // for receiving events from VT Mediator Plugin (dataport/CLI info)
    iMediatorEvents = CMediatorEventConsumer::NewL( this );
    
    TCapabilitySet capSet;
    capSet.SetEmpty();
    // for audio control
    capSet.AddCapability( ECapabilityWriteDeviceData ); 
    
    // define common attributes for all commands that engine is responsible for
    MediatorService::TCommand command;
    command.iCaps = capSet;
    command.iVersion = TVersion( 
        KPhoneToVideotelCmdVersionMajor, 
        KPhoneToVideotelCmdVersionMinor,
        KPhoneToVideotelCmdVersionBuild );
    command.iTimeout = KVtEngMdtrCmdTimeout;
    
    // enable microphone command
    command.iCommandId = EVtCmdUnmute;
    iCommandList.Append( command );
    
    // disable microphone command
    command.iCommandId = EVtCmdMute;
    iCommandList.Append( command );
        
    // register VT events that are raised by VT Mediator Plugin. 
    // !!! Note !!!!
    // event subscription must be done before command registration because
    // command registration triggers raising the event in the plugin. Different 
    // order would cause the engine to miss the event.
    const TVersion commandVersion( 
            KVideotelMdtrCommandsVersionMajor,
            KVideotelMdtrCommandsVersionMinor,
            KVideotelMdtrCommandsVersionBuild );
    //capSet.SetEmpty();
    //capSet.AddCapability( ECapabilityPowerMgmt );
        
    iMediatorResponder->RegisterCommand( 
                KMediatorVideoTelephonyDomain,
                KCatVideotelInternalCommands,
                EVtMediatorReleaseDataport,
                commandVersion,
                capSet,
                KVtEngMdtrCmdTimeout );
    
    const TVersion eventVersion( 
                KTelephonyEventsVersionMajor,
                KTelephonyEventsVersionMinor,
                KTelephonyEventsVersionBuild );
    
    TInt res = iMediatorEvents->SubscribeEvent( KMediatorVideoTelephonyDomain,
            KCatVideotelInternalEvents,
            EVtMediatorEventVideoCallInformation, 
            eventVersion );
    
    if ( res == KErrNone )
        {
        // register Mediator commands that we handle
        res = iMediatorResponder->RegisterCommand( 
            KMediatorVideoTelephonyDomain,
            KCatPhoneToVideotelCommands,
            iCommandList );
        
        }
    __VTPRINTEXITR( "CVtEngMdtrMessageListener.ConstructL res=%d",res )
    // registering must not fail
    User::LeaveIfError( res );
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrMessageListener::CVtEngMdtrMessageListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngMdtrMessageListener::CVtEngMdtrMessageListener( CVtEngUtility& aUtils)
 : iUtils( aUtils ), iCommandList( KVtEngMdtrCmdArrayGranularity )
    {    
    __VTPRINTENTER( "CVtEngMdtrMessageListener.CVtEngMdtrMessageListener" )
    __VTPRINTEXIT( "CVtEngMdtrMessageListener.CVtEngMdtrMessageListener" )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrMessageListener::IsReadyForCLIEvent
// Checks if engine state is appropriate to send CLI event.
// -----------------------------------------------------------------------------
//
TBool CVtEngMdtrMessageListener::IsReadyForCLIEvent() const
    {
    __VTPRINTENTER( "CVtEngMdtrMessageListener.IsReadyForCLIEvent" )
    const MVtEngSessionInfo::TSessionState state = 
        iUtils.StateManager()->SessionState();
    const TBool isReady =
       ( state == MVtEngSessionInfo::EConnected || 
         state == MVtEngSessionInfo::ENegotiating ||
         state == MVtEngSessionInfo::EOpen );    
    __VTPRINTEXITR( "CVtEngMdtrMessageListener.IsReadyForCLIEvent %d>", 
        isReady )
    return isReady;
    }

//  End of File

