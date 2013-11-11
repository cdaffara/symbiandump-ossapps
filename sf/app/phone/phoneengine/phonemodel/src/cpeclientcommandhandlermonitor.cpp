/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  AT command request monitor
*
*/



// INCLUDE FILES
#include "cpeclientcommandhandlermonitor.h"
#include "cpemanualcallcontrolhandler.h"
#include "cpemessagehandler.h"
#include "mpecallhandling.h"
#include "mpephonemodelinternal.h"
#include <mpedatastore.h>
#include <rphcltserver.h>
#include <talogger.h>
#include <telephonyvariant.hrh>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::CPEClientCommandHandlerMonitor
// Constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEClientCommandHandlerMonitor::CPEClientCommandHandlerMonitor(
    MPECallHandling& aCallHandling,
    CPEMessageHandler& aMessageHandler,
    MPEPhoneModelInternal& aModel,
    RPhCltServer& aServer,
    CPEManualCallControlHandler& aManualCallControlHandler )
    : iServer ( aServer ),
      iCallHandling( aCallHandling ),
      iMessageHandler ( aMessageHandler),
      iModel ( aModel ),
      iManualCallControlHandler( aManualCallControlHandler )
    {
    iWaitingForResponse = EFalse;
    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::ConstructL()
    {
    iNotify = CPhCltCallNotify::NewL();
    // Open subsession
    User::LeaveIfError( iNotify->Open( iServer ) );
    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEClientCommandHandlerMonitor* CPEClientCommandHandlerMonitor::NewL(
    MPECallHandling& aCallHandling,
    CPEMessageHandler& aMessageHandler,
    MPEPhoneModelInternal& aModel,
    RPhCltServer& aPhoneServer,
    CPEManualCallControlHandler& aManualCallControlHandler )
    {
    CPEClientCommandHandlerMonitor* self = new( ELeave ) CPEClientCommandHandlerMonitor(
        aCallHandling, aMessageHandler, aModel, aPhoneServer, aManualCallControlHandler );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

 
// Destructor
CPEClientCommandHandlerMonitor::~CPEClientCommandHandlerMonitor()
    {
    Cancel();

    if ( iNotify )
        {
        iNotify->Close();
        delete iNotify;
        }
    iLibrary.Close();
    }


// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::Start
// Start listening for AT command requests.
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::Start()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEClientCommandHandlerMonitor::Start()" );
    iNotify->NotifyComHandCommand( this, iArgs );
    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::Respond
// Sends response to AT command request 
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::Respond( const TInt aErrorCode )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEClientCommandHandlerMonitor::Respond: error %d", aErrorCode );
 
    SendResponse( aErrorCode );
    Start();
    }


// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::SendResponse
// Sends response to server.
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::SendResponse( const TInt aResponse )
    {
    iNotify->RespondComHandClient( aResponse );
    }


// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::RunL
// 
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::ComHandRequest()
    {
    TEFLOGSTRING( KTAREQEND, "PE CPEClientCommandHandlerMonitor::ComHandRequest");
    TRAPD( err, HandleCommandRequestL( iArgs() ) );
    if ( err != ECCPErrorNone )
        {
        Respond( err );
        }
    }


// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::Cancel()
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::Cancel" );
    if ( iNotify )
        {
        iNotify->CancelNotifyComHandCommand();
        }
    }
    

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::HandleAtCommandRequestL
// Processing various supported BT AT commands
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::HandleCommandRequestL(
        const TPhCltComHandCommandParameters& aArgs )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEClientCommandHandlerMonitor::HandleCommandRequestL: command %d"
        , aArgs.iCommandHandlerCommand );
    
    switch ( aArgs.iCommandHandlerCommand )
        {
        case EPhCltCommandAta:
            HandleCmdAta();
            break;

        case EPhCltCommandChup:
            HandleCmdChup();
            break;
      
        case EPhCltCommandVts:
            HandleCmdVtsL( aArgs.iDtmfTone, aArgs.iDtmfAction );
            break;
            
        case EPhCltCommandAtd:
            HandleCmdAtdL( aArgs.iTelNumber );
            break;
            
        case EPhCltCommandChld:
            HandleCmdChldL( aArgs.iChldCommand, aArgs.iChldCallNumber );
            break;
            
        case EPhCltCommandMuteMic:
            HandleCmdMuteMic( aArgs.iMute );
            break;
        
        case EPhCltCommandMuteRingingTone:
            HandleCmdMuteRingingTone();
            break;
        
        default:
            Respond( ECCPErrorNotSupported );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::HandleCmdAta
// Processing ATA command
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::HandleCmdAta()
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::HandleCmdAta" );
    // Checking for available ringing call is done in CallHandling,
    // no need to do it here. If a ringing call was not found, the
    // error code equals "ECCPErrorNotFound".
    Respond( iMessageHandler.HandleAnswerCall( EFalse ));
    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::HandleCmdChup
// Processing CHUP command
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::HandleCmdChup()
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::HandleCmdChup" );
    if ( iCallHandling.IsCallInState( EPEStateRinging ) &&
         iCallHandling.GetNumberOfCalls() == 1 )
        {
        Respond( iCallHandling.RejectCall() );
        }
    else if( iCallHandling.GetNumberOfCalls() > 1 )
        {
        // Hang up the calls in the order specified by Multicall Handling UI spec. 
        // If the call has a hang up request pending already, hang up the next call. 
        TInt err = HangUp( EPEStateDisconnecting );
        if ( ECCPErrorNone != err )
             {
             err = HangUp( EPEStateCreatingConference );
             if ( ECCPErrorNone != err ) 
                 {
                 err = HangUp( EPEStateDialing ); 
                 if ( ECCPErrorNone != err ) 
                     { 
                     err = HangUp( EPEStateConnecting ); 
                     if ( ECCPErrorNone != err ) 
                         { 
                         err = HangUp( EPEStateConnectedConference ); 
                         if ( ECCPErrorNone != err ) 
                             { 
                             err = HangUp( EPEStateConnected ); 
                             if ( ECCPErrorNone != err ) 
                                 { 
                                 err = HangUp( EPEStateHeldConference ); 
                                 if ( ECCPErrorNone != err ) 
                                     { 
                                     err = HangUp( EPEStateHeld ); 
                                     } 
                                 } 
                             } 
                         } 
                     } 
                 } 
             }
        Respond( err ); // Responds KErrNone or any error code 
        }
    else if( iCallHandling.GetNumberOfCalls() == 1 )
        {
        Respond( iCallHandling.ReleaseAll() );    
        }
    else
        {
        Respond( ECCPErrorGeneral );
        }

    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::HandleCmdVtsL
// Processing VTS command
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::HandleCmdVtsL(
        const TPhCltDtmfTone aDtmfTone,
        const TPhCltDtmfAction aAction )
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::HandleCmdVtsL" );
    TInt errorCode;

    switch ( aAction )
        {
        case EPhCltDtmfNotUsed:
            iModel.DataStore()->SetKeyCode( aDtmfTone );
            iMessageHandler.HandlePlayDTMFL();
            errorCode = iMessageHandler.HandleEndDTMF();
            break;
        case EPhCltDtmfStart:
            iModel.DataStore()->SetKeyCode( aDtmfTone );
            iMessageHandler.HandlePlayDTMFL();
            errorCode = ECCPErrorNone;
            break;
        case EPhCltDtmfStop:
            errorCode = iMessageHandler.HandleEndDTMF();
            break;
        default:
            errorCode = ECCPErrorNotSupported;
            break;
        }
    Respond( errorCode );

    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::HandleCmdAtdL
// Handle AT dial.
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::HandleCmdAtdL( 
        const TPhCltTelephoneNumber& aPhoneNumber )
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::HandleCmdAtdL" );
    iModel.DataStore()->SetPhoneNumber( 
        static_cast<TPEPhoneNumber>( aPhoneNumber ) );
    iWaitingForResponse = ETrue;
    iModel.SendMessage( MEngineMonitor::EPEMessageStartATDialing );
    }
  
// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::HandleCmdChldL
// Processing CHLD command
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::HandleCmdChldL( 
        const TPhCltChldCommand aChldCommand,
        const TUint aCallNo ) // Only used in GSM.
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::HandleCmdChldL" );
    TInt errorCode = ECCPErrorNone;

    // All commands are meant for incall situations.
    if ( iCallHandling.GetNumberOfCalls() > 0 )
        {
        // Leaves on error code. The leave error is sent as a response
        // to the client.
        TRAP( errorCode, iManualCallControlHandler.HandleChldL( aChldCommand, aCallNo ));
        }
    else
        {
        errorCode = ECCPErrorGeneral;
        }

    Respond( errorCode );
    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::DoCompleteCmdAtd
// Processing response from Phone UI (dialing started indication)
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::DoCompleteCmdAtd( TBool aFlag )
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::DoCompleteCmdAtd" );
    if ( iWaitingForResponse )
        {
        iWaitingForResponse = EFalse;
        if ( aFlag )
            {
            Respond( ECCPErrorNone );
            }
        else
            {
            Respond( ECCPErrorGeneral );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::HandleCmdMuteRingingTone
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::HandleCmdMuteRingingTone()
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::HandleCmdMuteRingingTone" );
    TInt errorCode( ECCPErrorNone );

    // command is meant for incall situations.
    if ( iCallHandling.GetNumberOfCalls() > 0 )
        {
        iModel.SendMessage( MEngineMonitor::EPEMessageMuteRingingTone );
        }
    else
        {
        errorCode = ECCPErrorGeneral;
        }

    Respond( errorCode );
    }

// -----------------------------------------------------------------------------
// CPEClientCommandHandlerMonitor::HandleCmdMuteMic
// -----------------------------------------------------------------------------
//
void CPEClientCommandHandlerMonitor::HandleCmdMuteMic( TBool aMute )
    {
    TEFLOGSTRING( KTAINT, "PE CPEClientCommandHandlerMonitor::HandleCmdMuteMic" );
    // command is meant for incall situations.
    if ( iCallHandling.GetNumberOfCalls() > 0 )
        {
        iModel.DataStore()->SetAudioMuteCommand( aMute );
        iMessageHandler.HandleSetAudioMute();
        Respond( ECCPErrorNone );
        }
    else
        {
        Respond( ECCPErrorGeneral );
        }
    }

// ----------------------------------------------------------------------------- 
// CPEClientCommandHandlerMonitor::HangUp 
// Finds a conference call or a single call (in that order) in specified 
// state and tries to hang it up. 
// ----------------------------------------------------------------------------- 
// 
TInt CPEClientCommandHandlerMonitor::HangUp( TPEState aState ) 
    { 
    TEFLOGSTRING2( KTAINT, "CPEClientCommandHandlerMonitor::HangUp aState=%d", aState); 
    TInt ret( ECCPErrorNotFound ); 
  
    TPEState conferenceState = iCallHandling.GetCallState( KPEConferenceCallID ); 
    if ( conferenceState == aState ) 
        { 
        ret = iCallHandling.HangUp( KPEConferenceCallID, ETPEHangUpDefault ); 
        } 
    else 
        { 
        TInt callId = iCallHandling.GetCallIdByState( aState ); 
        if ( KPECallIdNotUsed != callId ) 
            { 
            ret = iCallHandling.HangUp( callId, ETPEHangUpDefault ); 
            } 
        } 
  
    TEFLOGSTRING2( KTAINT, "CPEClientCommandHandlerMonitor::HangUp ret=%d", ret ); 
    return ret; 
    } 

//  End of File  
