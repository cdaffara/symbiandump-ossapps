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
* Description:  Manual call control handler
*
*/


// INCLUDE FILES
#include "cpemanualcallcontrolhandler.h"
#include "cpemessagehandler.h"
#include <ccpdefs.h>
#include <mpecallhandling.h>
#include <mpedatastore.h>
#include <pepanic.pan>
#include <pevirtualengine.h>
#include <talogger.h>


// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None.


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// cpemanualcallcontrolhandler::CPEManualCallControlHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEManualCallControlHandler::CPEManualCallControlHandler( 
        MPECallHandling& aCallHandling,
        CPEMessageHandler& aMessageHandler,
        MPEDataStore& aDataStore )
        :   iCallHandling( aCallHandling ),
            iMessageHandler( aMessageHandler ),
            iDataStore( aDataStore )
    {
    }
  
// Destructor
CPEManualCallControlHandler::~CPEManualCallControlHandler()
    {
    }

// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEManualCallControlHandler* CPEManualCallControlHandler::NewL
        ( 
        MPECallHandling& aCallHandling,
        CPEMessageHandler& aMessageHandler,
        MPEDataStore& aDataStore )
    {
    CPEManualCallControlHandler* handler = new ( ELeave ) CPEManualCallControlHandler( 
        aCallHandling, 
        aMessageHandler,
        aDataStore);

    return handler;
    }
// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::HandleChldL
// Handles common Chld (swap, answer waiting call, ... )
// -----------------------------------------------------------------------------
//
void CPEManualCallControlHandler::HandleChldL( 
            const TPhCltChldCommand aChldCommand,
            const TUint aCallNo )
    {
    TEFLOGSTRING3( KTAMESINT, 
            "PE CPEManualCallControlHandler::HandleChldL, aChldCommand: %d, aCallNo: %d"
            , aChldCommand
            , aCallNo ); 
    TInt error( KErrNotFound );
    
    switch ( aChldCommand )
        {
        case EPhCltChldZero:
            // Release held calls or set UDUB for a waiting call.
            error = HandleChldZero();
            break;

        case EPhCltChldOne:
            // Release all active calls and accept other (held or waiting) call.
            {
            if( aCallNo )
                {
                HandleChldOneXL( aCallNo );
                error = ECCPErrorNone;
                }
            else
                {
                error = HandleChldOne();
                }
            }
            break;

        case EPhCltChldTwo:
            // Place all active calls on hold and accept the other (held or waiting) call.
            {
            if( aCallNo )
                {
                HandleChldTwoXL( aCallNo );
                error = ECCPErrorNone;
                }
            else
                {
                error = HandleChldTwo();
                }
            }
            break;
            
        case EPhCltChldThree:
            // Add a held call to the conversation.
            error = HandleChldThree();
            break;

        case EPhCltChldFour:
            // Connect the two calls and disconnect the subscriber from 
            // both calls (Explicit call transfer).
            error = HandleChldFour();
            break;

        default:
            {
            TEFLOGSTRING2( KTAERROR, 
                "PE CPEManualCallControlHandler::HandleChldL, aChldCommand; %d"
                ,aChldCommand );
            __ASSERT_DEBUG( EFalse, Panic( EPEPanicIllegalCommand ) );
            break;
            }
        }
    User::LeaveIfError( error );     
    }

// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::HandleChldZero
// -----------------------------------------------------------------------------
//
TInt CPEManualCallControlHandler::HandleChldZero()
    {
    TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldZero" );
    TInt error( ECCPErrorNotAllowed );
    
    if ( iCallHandling.IsCallInState( EPEStateRinging ) )
        {
        error = iCallHandling.RejectCall();
        }
    else if( iDataStore.ConferenceCallState( ) == EPEStateHeldConference )
        {
        iDataStore.SetCallId( KPEConferenceCallID );
        error = iMessageHandler.HandleReleaseCall();
        }
    else if ( iCallHandling.IsCallInState( EPEStateHeld ) )
        {
        TInt callId = iCallHandling.GetCallIdByState( EPEStateHeld );
        if ( callId != KPECallIdNotUsed )
            {
            iDataStore.SetCallId( callId );
            error = iMessageHandler.HandleReleaseCall();
            }
        }

    TEFLOGSTRING2( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldZero, error: %d", error );
    return error;  
    } 

// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::HandleChldOne
// -----------------------------------------------------------------------------
//
TInt CPEManualCallControlHandler::HandleChldOne()
    {
    TEFLOGSTRING( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldOne" );

    TInt error( ECCPErrorNotAllowed );
    // Find active call
    TInt callId( KPECallIdNotUsed );

    if ( ( iDataStore.ConferenceCallState( ) == EPEStateConnectedConference ) )
        {
        callId = KPEConferenceCallID;
        }
    else if ( iCallHandling.IsCallInState( EPEStateConnected ) )
        {
        callId = iCallHandling.GetCallIdByState( EPEStateConnected );
        }
    else if ( iCallHandling.IsCallInState( EPEStateConnecting ) )  
        {
        callId = iCallHandling.GetCallIdByState( EPEStateConnecting );
        }
    else if ( iCallHandling.IsCallInState( EPEStateDialing ) )  
        {
        callId = iCallHandling.GetCallIdByState( EPEStateDialing );
        }
    else if ( iCallHandling.IsCallInState( EPEStateAnswering ) )  
        {
        callId = iCallHandling.GetCallIdByState( EPEStateAnswering );
        }
    
    // Replace 
    if ( iCallHandling.IsCallInState( EPEStateConnected ) &&
         iCallHandling.IsCallInState( EPEStateRinging ) )
        {
        TInt callIdConn = iCallHandling.GetCallIdByState( EPEStateConnected );
        TInt callIdRing = iCallHandling.GetCallIdByState( EPEStateRinging );
        if ( iDataStore.CallType( callIdConn ) == iDataStore.CallType( callIdRing ) )
            {
            // Replace allowed for same call types
            error = iCallHandling.ReplaceActive();
            }
        }
    else 
        {
         // Relese active call
        if ( callId != KPECallIdNotUsed )
            {
            iDataStore.SetCallId( callId );
            error = iMessageHandler.HandleReleaseCall( ETPEHangUpResumeHeldCall );
            }
        // accept held or waiting call
        if ( iCallHandling.IsCallInState( EPEStateRinging ) )    
            {
            error = iCallHandling.AnswerCall();
            }
        else if ( iCallHandling.IsCallInState( EPEStateHeld ) )  
            {
            error = iCallHandling.ResumeCall();
            }    
        }

        
    TEFLOGSTRING2( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldOne, error: %d", error );    
    return error;         
    }

// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::HandleChldOneXL
// -----------------------------------------------------------------------------
//
void CPEManualCallControlHandler::HandleChldOneXL( TInt aCallNo )
    {
    TEFLOGSTRING2( KTAINT,
        "PE CPEManualCallControlHandler::HandleChldOneXL, aCallNo %d", 
        aCallNo );

    TInt error( ECCPErrorNotAllowed );
    TPEState callState;
    
    if ( ( KPEMaximumNumberOfVoiceCalls > aCallNo ) && ( aCallNo > 0 ) ) 
        {
        callState = iDataStore.CallState( CallIdByIndexL( aCallNo ) );
        TEFLOGSTRING2( KTAINT,
            "PE CPEManualCallControlHandler::HandleChldOneXL, callState %d", 
            callState );
        if ( callState == EPEStateRinging && 
             !( iCallHandling.IsCallInState( EPEStateDialing ) || iCallHandling.IsCallInState( EPEStateConnecting ) ) )
            {
            error = iMessageHandler.HandleAnswerCall( EFalse );
            TEFLOGSTRING2( KTAINT,
                "PE CPEManualCallControlHandler::HandleChldOneXL > HandleAnswerCall, Error code %d", 
                error );
            }
        else if ( callState == EPEStateConnected )
            {
            iDataStore.SetCallId( CallIdByIndexL( aCallNo ) );
            error = iMessageHandler.HandleReleaseCall( ETPEHangUpNotResumeHeldCall );
            TEFLOGSTRING2( KTAINT,
                "PE CPEManualCallControlHandler::HandleChldOneXL > HandleReleaseCall, Error code %d", 
                error );
            }
        }        
        
    TEFLOGSTRING2( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldOneXL, error: %d", error );    
    User::LeaveIfError( error );        
    }
    
// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::HandleChldTwo
// -----------------------------------------------------------------------------
//
TInt CPEManualCallControlHandler::HandleChldTwo()
    {
    TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldTwo");
    
    TInt error( ECCPErrorNotAllowed );
    
    if ( iCallHandling.IsCallInState( EPEStateDialing ) || 
         iCallHandling.IsCallInState( EPEStateConnecting ) )
        {
        TEFLOGSTRING( 
            KTAERROR, 
            "PE CPEManualCallControlHandler::HandleChldTwo: Call found in EPEStateDialing or EPEStateConnecting,state > swap NOT ALLOWED" );
        }
    else
        {
        if ( iCallHandling.IsCallInState( EPEStateConnected ) && 
             iCallHandling.IsCallInState( EPEStateHeld ) )
            {
            error = iCallHandling.SwapCalls();
            }
        else if ( iCallHandling.IsCallInState( EPEStateConnected ) && 
                  !iCallHandling.IsCallInState( EPEStateRinging ) )
            {
            error = iCallHandling.HoldCall();
            }
        else if ( iCallHandling.IsCallInState( EPEStateHeld ) &&
            !( iCallHandling.IsCallInState( EPEStateRinging ) ) )
            {
            error = iCallHandling.ResumeCall();
            }
        else if ( iCallHandling.IsCallInState( EPEStateRinging ) )
            {
            error = iCallHandling.AnswerCall();
            }
        }

    TEFLOGSTRING2( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldtwo, error: %d", error );    
    return error; 
    }

// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::HandleChldTwoXL
// -----------------------------------------------------------------------------
//
void CPEManualCallControlHandler::HandleChldTwoXL( TInt aCallNo )
    {
    TEFLOGSTRING( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldTwoXL" );

    TInt error( ECCPErrorNotAllowed );
    
    if ( ( KPEMaximumNumberOfVoiceCalls > aCallNo ) && ( aCallNo > 0 ) ) 
        {   
        if ( ( iDataStore.CallState( CallIdByIndexL( aCallNo ) ) ==  EPEStateConnected )
            && ( iDataStore.ConferenceCallState( ) == EPEStateConnectedConference ) )
            {
            iDataStore.SetCallId( CallIdByIndexL( aCallNo ) );
            error = iMessageHandler.HandleGoOneToOne();
            }
        }

    TEFLOGSTRING2( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldTwoXL, error: %d", error );    
    User::LeaveIfError( error );       
    }
// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::HandleChldThree
// -----------------------------------------------------------------------------
//
TInt CPEManualCallControlHandler::HandleChldThree()
    {
    TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldThree");
    
    TInt error( ECCPErrorNotAllowed );
    
    if( iDataStore.ConferenceCallState( ) == EPEStateConferenceIdle ) 
        {
        TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldThree: HandleBuildConferenceCall" );
        error = iMessageHandler.HandleBuildConferenceCall();
        }
    else if ( iDataStore.ConferenceCallState( ) == EPEStateConnectedConference )
        {
        TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldThree: EPEStateConnectedConference" );
        if ( iCallHandling.IsCallInState( EPEStateHeld ) )
            {
            TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldThree: AddConferenceMember" );
            error = iCallHandling.AddConferenceMember();
            }
        }
    else if( iDataStore.ConferenceCallState( ) == EPEStateHeldConference ) 
        {
        TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldThree: EPEStateHeldConference" );
        if ( iCallHandling.IsCallInState( EPEStateConnected ) )
            {
            TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldThree: AddConferenceMember" );
            error = iCallHandling.AddConferenceMember();
            }
        }
        
    TEFLOGSTRING2( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldTree, error: %d", error );
    return error;         
    }

// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::HandleChldFour
// -----------------------------------------------------------------------------
//
TInt CPEManualCallControlHandler::HandleChldFour()
    {
    TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldFour" );
   
    TInt error( ECCPErrorNotAllowed );
            
    if ( ( iCallHandling.IsCallInState( EPEStateConnected ) || iCallHandling.IsCallInState( EPEStateConnecting ) ) && 
         iCallHandling.IsCallInState( EPEStateHeld ) )
        {
        TEFLOGSTRING( KTAINT, "PE CPEManualCallControlHandler::HandleChldFour > TransferCalls" );
        error = iCallHandling.TransferCalls();
        }
        
    TEFLOGSTRING2( KTAINT, 
        "PE CPEManualCallControlHandler::HandleChldFour, error: %d", error );
    return error;        
    }

// -----------------------------------------------------------------------------
// CPEManualCallControlHandler::CallIdByIndexL
// -----------------------------------------------------------------------------
//
TInt CPEManualCallControlHandler::CallIdByIndexL( const TInt& aCallIndex )
    {
    TInt i, errorCode( ECCPErrorNotAllowed );
    for( i = 0; i < KPEMaximumNumberOfCalls; i++ )
        {
        if( iDataStore.CallIndex( i ) == aCallIndex )
            {
            errorCode = ECCPErrorNone;
            break;
            }
        }
    User::LeaveIfError( errorCode );
    return i;
    }

//  End of File  
