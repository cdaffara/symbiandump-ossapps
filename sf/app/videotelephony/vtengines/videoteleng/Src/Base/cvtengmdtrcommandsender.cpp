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
* Description:  Mediator command sender implementation
*
*/



// INCLUDE FILES
#include    "cvtengmdtrcommandsender.h"
#include    "CVtEngEventManager.h"
#include    "VtEngUtils.h"
#include    "CVtEngSettings.h"
#include    "CVtEngOperation.h"
#include    "vtengcommands.h"
#include    "CVtEngStateManager.h"
#include    "VtEngPanic.h"
#include    <MediatorCommandInitiator.h>
#include    <MediatorDomainUIDs.h> 
#include    <mediatorcommandstotelephonyapi.h>
#include    <mediatoraudiocommandstotelephonyapi.h>
#include    <videoteltophonecommandsapi.h>
#include    <cvtlogger.h>


/**
* Three state classes for handling audio mute/unmute commands towards mediator when mute state
* is changed locally faster than mediator gives response.

        
                                             IDLE <-------------------------|    
                                               |                            |
             NotifyOutgoingAudioState( x ) / mediator.Send( x )             |
                                               |                            |
                                               v                            |
    |--------------------------------------> SENDING -- CommandResponseL ---| 
    |                                           |
    |                           NotifyOutgoingAudioState( y ) / saveCommand( y )
    |                                           |
    |                                           v
    |- CommandResponseL( x ) / Send( y ) ---- BUFFERED
                             
                             
                             
  
*/
NONSHARABLE_CLASS( TVtMdtrStateIdle ) : public CBase
{
    public:  
        TVtMdtrStateIdle( CVtEngMdtrCommandSender& aSender, CMediatorCommandInitiator& aMediator );
        virtual void SendL( TUid aDomain, TUid aCategory, TInt aCommand );       
        virtual void CommandResponseL( TUid aDomain, TUid aCategory, TInt aCommand, TInt aStatus );
        
        TInt DoSend( TUid aDomain, TUid aCategory, TInt aCommand );
        
    CVtEngMdtrCommandSender& iSender;
    CMediatorCommandInitiator& iMediator;
};

NONSHARABLE_CLASS( TVtMdtrStateSending ) : public TVtMdtrStateIdle
{
    public:
        TVtMdtrStateSending( CVtEngMdtrCommandSender& aSender, CMediatorCommandInitiator& aMediator );
        void SendL( TUid aDomain, TUid aCategory, TInt aCommand );
        void CommandResponseL( TUid aDomain, TUid aCategory, TInt aCommand, TInt aStatus );        
};

NONSHARABLE_CLASS( TVtMdtrStateBuffered ) : public TVtMdtrStateIdle
{
    public:
        TVtMdtrStateBuffered( CVtEngMdtrCommandSender& aSender, CMediatorCommandInitiator& aMediator );
        void SendL( TUid aDomain, TUid aCategory, TInt aCommand );
        void CommandResponseL( TUid aDomain, TUid aCategory, TInt aCommand, TInt aStatus );
        
        TUid iDomain;
        TUid iCategory;
        TInt iCommand;
};

    
// CONSTANTS
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngMdtrCommandSender* CVtEngMdtrCommandSender::NewL( CVtEngUtility& aUtils )
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.NewL" )
    CVtEngMdtrCommandSender* self = new ( ELeave )
        CVtEngMdtrCommandSender( aUtils );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();

    __VTPRINTEXIT( "CVtEngMdtrCommandSender.NewL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::~CVtEngMdtrCommandSender
// destructor
// -----------------------------------------------------------------------------
//
CVtEngMdtrCommandSender::~CVtEngMdtrCommandSender()
    {
    iUtils.EventManager().RemoveObserver( this );
    delete iMediatorInitiator;
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::EndCall
// Sends end call command via Mediator
// -----------------------------------------------------------------------------
//
void CVtEngMdtrCommandSender::EndCall( )
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.EndCall" )
    const TVersion KTelephonyCmdVersion(
        KTelephonyCommandsVersionMajor,
        KTelephonyCommandsVersionMinor,
        KTelephonyCommandsVersionBuild );
    const TInt res =
        iMediatorInitiator->IssueCommand(
           KMediatorTelephonyDomain,
           KCatCommandsToTelephony,
           EPhoneCmdEndActiveCall,
           KTelephonyCmdVersion,
           KNullDesC8() );
    
    __VTPRINTEXITR( "CVtEngMdtrCommandSender.EndCall res=%d", res )
    }
    
// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::VoiceFallback
// Sends voice fallback command to telephony via Mediator
// -----------------------------------------------------------------------------
//
void CVtEngMdtrCommandSender::VoiceFallback()
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.VoiceFallback" )
    const TVersion KVtToPhoneCmdVersion(
        KVideoTelToPhoneCmdVersionMajor,
        KVideoTelToPhoneCmdVersionMinor,
        KVideoTelToPhoneCmdVersionBuild );
#ifdef _DEBUG
    TInt err =
#endif
    iMediatorInitiator->IssueCommand(
        KMediatorTelephonyDomain,
        KCatVideoTelToPhoneCommands,
        EVtCmdFallback,
        KVtToPhoneCmdVersion,
        KNullDesC8() );
    // response is not awaited
    __VTPRINTEXITR( "CVtEngMdtrCommandSender.VoiceFallback %d>", err )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::SwitchToVoiceL
// Sends switch to voice command to telephony via Mediator
// -----------------------------------------------------------------------------
//
void CVtEngMdtrCommandSender::SwitchToVoiceL( CVtEngOperation& aOperation )
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.SwitchToVoiceL" )
    iOperation = &aOperation;
    // add this as observer. Mediator command is sent when idle state
    // is reached (i.e. Protocol has released dataport and it can be recovered
    // by telephony)
    iUtils.EventManager().AddObserverL( this );
    // let current state handle actual request
    iUtils.StateManager()->HandleOperationL( aOperation );
    __VTPRINTEXIT( "CVtEngMdtrCommandSender.SwitchToVoiceL" )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::NotifyOutgoingAudioState
// Sends microphone state to telephony via Mediator
// -----------------------------------------------------------------------------
//
void CVtEngMdtrCommandSender::NotifyOutgoingAudioState( 
    const TBool aMicEnabled )
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.NotifyOutgoingAudioState" )    

    const TInt command = ( aMicEnabled ) 
        ? EAudioCmdUnmute : EAudioCmdMute;        
#ifdef _DEBUG
    TRAPD( res, iSenderState->SendL(
            KMediatorTelephonyDomain,
            KCatAudioCommandsToTelephony,
            command ) );
#else 
    TRAP_IGNORE(iSenderState->SendL(
            KMediatorTelephonyDomain,
            KCatAudioCommandsToTelephony,
            command ) );
#endif
            
    __VTPRINTEXITR( "CVtEngMdtrCommandSender.NotifyOutgoingAudioState %d", res )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::SetSenderState
// sets new mediator sender state for audio command
// -----------------------------------------------------------------------------
//
void CVtEngMdtrCommandSender::SetSenderState( TVtMdtrStateIdle& aNewState )
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.SetSenderState" )
    __VTPRINT2( DEBUG_GEN, " delete old state %x", (TInt) iSenderState )
    __ASSERT_DEBUG( iSenderState != &aNewState, 
        Panic( EVtEngPanicWrongMediatorSenderState ) );
    delete iSenderState;
    iSenderState = &aNewState;
    __VTPRINTEXITR( "CVtEngMdtrCommandSender.SetSenderState %x", (TInt) iSenderState )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::CommandResponseL
// Mediator framework callback
// -----------------------------------------------------------------------------
//
void CVtEngMdtrCommandSender::CommandResponseL( TUid aDomain,
    TUid aCategory, 
    TInt aCommandId,
    TInt aStatus,
    const TDesC8& /*aData*/ )
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.CommandResponseL" )
    switch ( aCommandId )
        {
    case EAudioCmdUnmute:
    case EAudioCmdMute:
        iSenderState->CommandResponseL( aDomain, aCategory, aCommandId, aStatus );
        break;
    default:
        // ignored for other commands (so far)
        break;
        }             
    __VTPRINT2( DEBUG_GEN, " mediator command id=%d", aCommandId )
    __VTPRINTEXITR( "CVtEngMdtrCommandSender.CommandResponseL res=%d", aStatus )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::HandleVtEventL
// Handles state change event associated with "switch to voice" and "TerminateSession" command
// -----------------------------------------------------------------------------
//
void CVtEngMdtrCommandSender::HandleVtEventL( TInt aEvent )
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.HandleVtEventL" )
#ifdef _DEBUG
        TInt err = KErrNone;
#endif     
    if ( aEvent == KVtEngSessionStateChanged &&
         iOperation && iOperation->Command() == KVtEngSwitchToVoice )
        {
        const TVersion KVtToPhoneCmdVersion(
            KVideoTelToPhoneCmdVersionMajor,
            KVideoTelToPhoneCmdVersionMinor,
            KVideoTelToPhoneCmdVersionBuild );
        
        iUtils.EventManager().RemoveObserver( this );
        // send command (ignore possible error - no way to recover from it)
#ifdef _DEBUG
        err =
#endif        
        iMediatorInitiator->IssueCommand(
            //KMediatorVideoTelephonyDomain,
            KMediatorTelephonyDomain,
            KCatVideoTelToPhoneCommands,
            EVtCmdSwitchToVoice,
            KVtToPhoneCmdVersion,
            KNullDesC8() );
        iOperation->HandleOpComplete( KErrNone );
        iOperation = NULL;
        }
    __VTPRINTEXITR( "CVtEngMdtrCommandSender.HandleVtEventL %d", err )
    }

// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::
// c++ constructor
// -----------------------------------------------------------------------------
//
CVtEngMdtrCommandSender::CVtEngMdtrCommandSender( CVtEngUtility& aUtils ) :
    iUtils( aUtils )
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.CVtEngMdtrCommandSender" )
    __VTPRINTEXIT( "CVtEngMdtrCommandSender.CVtEngMdtrCommandSender" )
    }
    
// -----------------------------------------------------------------------------
// CVtEngMdtrCommandListener::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CVtEngMdtrCommandSender::ConstructL()
    {
    __VTPRINTENTER( "CVtEngMdtrCommandSender.ConstructL" )
    iMediatorInitiator = CMediatorCommandInitiator::NewL( this );
    
    // set initial mediator command sender state
    iSenderState = new ( ELeave ) TVtMdtrStateIdle( *this, *iMediatorInitiator );
    __VTPRINTEXIT( "CVtEngMdtrCommandSender.ConstructL" )
    }

TVtMdtrStateIdle::TVtMdtrStateIdle( CVtEngMdtrCommandSender& aSender, CMediatorCommandInitiator& aMediator ) 
: iSender( aSender), iMediator( aMediator )
    {
    __VTPRINTENTER( "TVtMdtrStateIdle.ctr" )
    __VTPRINTEXITR( "TVtMdtrStateIdle.ctr %x", (TInt) this )
    }

void TVtMdtrStateIdle::SendL( TUid aDomain, TUid aCategory, TInt aCommand )
    {
    __VTPRINTENTER( "TVtMdtrStateIdle.SendL" )
    TVtMdtrStateIdle* nextState = NULL;
    const TInt res = DoSend( aDomain, aCategory, aCommand );
    if ( res == KErrInUse )
        {
        nextState = new ( ELeave ) TVtMdtrStateBuffered( iSender, iMediator );
        CleanupStack::PushL( nextState );
        nextState->SendL( aDomain, aCategory, aCommand );
        CleanupStack::Pop( nextState );
        }
    else
        {
        nextState = new ( ELeave ) TVtMdtrStateSending( iSender, iMediator );
        }
    iSender.SetSenderState( *nextState );            
    __VTPRINTEXIT( "TVtMdtrStateIdle.SendL" )
    }

TInt TVtMdtrStateIdle::DoSend( TUid aDomain, TUid aCategory, TInt aCommand )
    {
    __VTPRINTENTER( "TVtMdtrStateIdle.DoSend" )
    const TVersion KVtToPhoneCmdVersion(
        KVideoTelToPhoneCmdVersionMajor,
        KVideoTelToPhoneCmdVersionMinor,
        KVideoTelToPhoneCmdVersionBuild );
        
    TInt res = iMediator.IssueCommand(
        aDomain,
        aCategory,
        aCommand,
        KVtToPhoneCmdVersion,
        KNullDesC8() );
    __VTPRINTEXITR( "TVtMdtrStateIdle.DoSend res=%d", res )
    return res;
    }

void TVtMdtrStateIdle::CommandResponseL( TUid /*aDomain*/, TUid /*aCategory*/, TInt /*aCommand*/, TInt /*aStatus*/ )
    {
    __VTPRINTENTER( "TVtMdtrStateIdle.CommandResponseL" )
    // base class method should never be possible to call
    __VTPRINTEXIT( "TVtMdtrStateIdle.CommandResponseL" )
    }

TVtMdtrStateSending::TVtMdtrStateSending( CVtEngMdtrCommandSender& aSender, CMediatorCommandInitiator& aMediator )
: TVtMdtrStateIdle( aSender, aMediator )
    {
    __VTPRINTENTER( "TVtMdtrStateSending.ctr" )
    __VTPRINTEXITR( "TVtMdtrStateSending.ctr %x", (TInt) this )
    }

void TVtMdtrStateSending::SendL( TUid aDomain, TUid aCategory, TInt aCommand )
    { 
    __VTPRINTENTER( "TVtMdtrStateSending.SendL" )    
    TVtMdtrStateIdle* buffered = new ( ELeave )TVtMdtrStateBuffered( iSender, iMediator );
    CleanupStack::PushL( buffered );
    buffered->SendL( aDomain, aCategory, aCommand );
    CleanupStack::Pop( buffered );
    iSender.SetSenderState( *buffered );
    __VTPRINTEXIT( "TVtMdtrStateSending.SendL" )
    }

void TVtMdtrStateSending::CommandResponseL( TUid aDomain, TUid aCategory, TInt aCommand, TInt aStatus )
    {
    __VTPRINTENTER( "TVtMdtrStateSending.CommandResponseL" )
    __VTPRINT2( DEBUG_GEN, " command=%d", aCommand )    
    __VTPRINT2( DEBUG_GEN, " response=%d", aStatus )    
    TVtMdtrStateIdle* idle = new ( ELeave ) TVtMdtrStateIdle( iSender, iMediator );    
    iSender.SetSenderState( *idle );
    __VTPRINTEXIT( "TVtMdtrStateSending.CommandResponseL" )
    }

TVtMdtrStateBuffered::TVtMdtrStateBuffered( CVtEngMdtrCommandSender& aSender, CMediatorCommandInitiator& aMediator )
: TVtMdtrStateIdle( aSender, aMediator )
    {
    __VTPRINTENTER( "TVtMdtrStateBuffered.ctr" )
    __VTPRINTEXITR( "TVtMdtrStateBuffered.ctr %x", (TInt) this )
    }

void TVtMdtrStateBuffered::SendL( TUid aDomain, TUid aCategory, TInt aCommand )
    {
    __VTPRINTENTER( "TVtMdtrStateBuffered.SendL" )    
    
    iDomain = aDomain;
    iCategory = aCategory;
    iCommand = aCommand;
    // iCommand is sent in CommandResponseL
    __VTPRINTEXIT( "TVtMdtrStateBuffered.SendL" )
    }

void TVtMdtrStateBuffered::CommandResponseL( TUid aDomain, TUid aCategory, TInt aCommand, TInt aStatus )
    {
    __VTPRINTENTER( "TVtMdtrStateBuffered.CommandResponseL" )    
    const TVersion KVtToPhoneCmdVersion(
        KVideoTelToPhoneCmdVersionMajor,
        KVideoTelToPhoneCmdVersionMinor,
        KVideoTelToPhoneCmdVersionBuild );
    TBool nextStateIdle = ETrue;
    TInt res = KErrNone;
    if ( aCommand != iCommand )    
        {
        res = iMediator.IssueCommand(
            KMediatorTelephonyDomain,
            KCatAudioCommandsToTelephony,
            iCommand,
            KVtToPhoneCmdVersion,
            KNullDesC8() );
        if ( res == KErrNone )
            {    
            nextStateIdle = EFalse;
            }        
        }
    TVtMdtrStateIdle* nextState = NULL;
    if ( nextStateIdle )
        {
        __VTPRINT2( DEBUG_GEN, " failed to send buffered command with error=%d", res )
        nextState = new ( ELeave ) TVtMdtrStateIdle( iSender, iMediator );
        }
    else
        {
        nextState = new ( ELeave ) TVtMdtrStateSending( iSender, iMediator );
        CleanupStack::PushL( nextState );
        nextState->SendL( iDomain, iCategory, iCommand );
        CleanupStack::Pop( nextState );
        }
    iSender.SetSenderState( *nextState );    
    __VTPRINTEXIT( "TVtMdtrStateBuffered.CommandResponseL" )
    }
        
//  End of File
