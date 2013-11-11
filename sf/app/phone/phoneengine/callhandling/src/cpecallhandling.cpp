/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPECallHandling class 
*                member functions.
*
*/


// INCLUDE FILES
#include    "cpecallhandling.h"
#include    "cpevideocallhandling.h"
#include    "cpesinglecall.h"
#include    "cpedtmfhandling.h"
#include    "cpesupplementaryservicesmonitor.h"
#include    "mpephonemodelinternal.h"
#include    "cpeconferencecall.h"
#include    "cpecallarrayowner.h"
#include    "mpecallhandling.h"
#include    "cpecceobserver.h"
#include    "cpesystemcallstate.h"
#include    "tpematcher.h"

#include    <gsmerror.h>
#include    <mpedatastore.h>
#include    <pepanic.pan>
#include    <talogger.h>
#include    <PSVariables.h>
#include    <ctsydomainpskeys.h>
#include    <mccecall.h>
#include    <psetsaobserver.h>
#include    <cccecallparameters.h>
#include    <centralrepository.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
/******************************************************************************
* Telephony Configuration API
* Keys under this category are used in defining telephony configuration.
******************************************************************************/
const TUid KCRUidTelConfiguration = {0x102828B8};

/**
* Amount of digits to be used in contact matching.
* This allows a customer to variate the amount of digits to be matched.
*/
const TUint32 KTelMatchDigits                               = 0x00000001;

const TInt KPEMatchDefault = 7;

// MACROS
// None

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================
// None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPECallHandling::CPECallHandling
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPECallHandling::CPECallHandling(
    MPEPhoneModelInternal& aModel,
    CCCE& aConvergedCallEngine,
    MCCEDtmfInterface& aDtmfInterface
    ) : iModel( aModel ),
        iConvergedCallEngine( aConvergedCallEngine ),
        iDtmfInterface( aDtmfInterface ),
        iReplaceActive( EFalse ),
        iDialRequestCallId( KErrNotFound )
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPECallHandling::CPECallHandling()");
    }

// Destructor
EXPORT_C CPECallHandling::~CPECallHandling()
    {  
    TEFLOGSTRING( KTAOBJECT, "CALL CPECallHandling::~CPECallHandling() start");
    
    delete iRepository;
    delete iSystemCallState;
    delete iDtmfHandling;
    
    // No need to uninitialize feature manager - this is done in MPEPhoneModelInternal .cpp
        
    delete iSupplementaryServicesMonitor;
    delete iCCEObserver;

    delete iVideoCallHandling;
    delete iConferenceCall;
    delete iCallArrayOwner;
    delete iPsetSAObserver;
    delete iCallOpenParams;
    
    TEFLOGSTRING( KTAOBJECT, "CALL CPECallHandling::~CPECallHandling() complete");    
    }

// -----------------------------------------------------------------------------
// CPECallHandling::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPECallHandling* CPECallHandling::NewL(
        MPEPhoneModelInternal& aModel,                    
        CCCE& aConvergedCallEngine,
        MCCEDtmfInterface& aDtmfInterface )
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPECallHandling::NewL start." );
    CPECallHandling* self = new (ELeave) CPECallHandling( aModel,
        aConvergedCallEngine, aDtmfInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING( KTAOBJECT, "CALL CPECallHandling::NewL complete." );
    return self;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPECallHandling::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPECallHandling::BaseConstructL() start"); 

    // Creates a supplementary services monitor
    iSupplementaryServicesMonitor = new (ELeave) CPESupplementaryServicesMonitor( 
        *this, *iModel.DataStore() );
    
    iCCEObserver = CPECCEObserver::NewL( *this, iModel );
    
    iConvergedCallEngine.SetObserver( *iCCEObserver, *iSupplementaryServicesMonitor );

    // Creates CPEDtmfSender object
    iDtmfHandling = CPEDtmfHandling::NewL( *this, iDtmfInterface );

    // Creates a call array owner object
    iCallArrayOwner = CPECallArrayOwner::NewL( 
            iConvergedCallEngine, 
            *this );
    
    iVideoCallHandling = CPEVideoCallHandling::NewL( 
            *this, 
            iModel, 
            iConvergedCallEngine,
            *iCallArrayOwner );
                                                          
    iCallOpenParams = CCCECallParameters::NewL();
    
    iSystemCallState = CPESystemCallState::NewL( *iCallArrayOwner, *iModel.DataStore() );
    
    iRepository = CRepository::NewL( KCRUidTelConfiguration );
    
    TEFLOGSTRING( KTAOBJECT, "CALL CPECallHandling::BaseConstructL() complete");
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SendErrorMessage
// Reroutes error messages to the Phone Engine, usable by other than call objects
// -----------------------------------------------------------------------------
//
void CPECallHandling::SendErrorMessage(
        const TInt aErrorCode )
    {
    TEFLOGSTRING2( KTAINT, 
        "CALL CPECallHandling::SendErrorMessage, error code: %d", 
        aErrorCode );

    iModel.DataStore()->SetErrorCode( aErrorCode );
    iModel.SendMessage( MEngineMonitor::EPEMessageCallHandlingError );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SendErrorMessage
// Reroutes error messages to the Phone Engine, usable by call objects
// -----------------------------------------------------------------------------
//
void CPECallHandling::SendErrorMessage( 
        const TInt aCallId,
        const TInt aErrorCode )
    {
    TEFLOGSTRING3( KTAINT, 
        "CALL CPECallHandling::SendErrorMessage, error code: %d, call id: %d", 
        aErrorCode,
        aCallId );
    
    // Dial request failed - resume held call
    if( aErrorCode == ECCPErrorInvalidFDN )
        {
        HandleAutoResume();
        }

    if ( aCallId == KPECallIdNotUsed )
        {
        SendErrorMessage( aErrorCode );
        }
    else
        {
        iModel.DataStore()->SetErrorCode( aErrorCode );
        iModel.SendMessage( MEngineMonitor::EPEMessageCallHandlingError, aCallId );
        
        //Handle call objet deleting if dialing fail.
        if( ECCPErrorNone != aErrorCode )
            {
            CPESingleCall* callData = iCallArrayOwner->GetCallObject( aCallId );
            if( callData )
                {
                TEFLOGSTRING2( KTAINT, 
                    "CALL CPECallHandling::SendErrorMessage, call state %d", callData->GetCallState() );
                if( callData->GetCallState() == EPEStateIdle )
                    {
                    ReleaseCallObject( aCallId  );
                    iDialRequestCallId = KErrNotFound;
                    }
                }
            }

        if( ( KPEConferenceCallID == aCallId ) && ( ECCPConferenceErrorAddCall == aErrorCode ) &&
              iConferenceCall && ( iConferenceCall->EnumerateCalls() == 0 ) )
           { 
           delete iConferenceCall; 
           iConferenceCall = NULL;
           iModel.DataStore()->ResetCallInfo( aCallId );
           } 
        }
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SendMessage
// Reroutes messages to the Phone Engine, usable by other than call objects
// -----------------------------------------------------------------------------
//
void CPECallHandling::SendMessage( 
    MEngineMonitor::TPEMessagesFromPhoneEngine aMessage )
    {
    TEFLOGSTRING2( KTAINT,
        "CALL CPECallHandling::SendMessage, message id: %d", aMessage );
   
    iModel.SendMessage( aMessage );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SendMessage
// Reroutes messages to the Phone Engine, usable by call objects
// -----------------------------------------------------------------------------
//
void CPECallHandling::SendMessage( 
    MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
    TInt aCallId )
    {
    TEFLOGSTRING3( KTAINT, 
        "CALL CPECallHandling::SendMessage, message id: %d, call id: %d", 
        aMessage, 
        aCallId );
    
    TInt errorCode( KErrNone );
    
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageDialing:
            {
            iDialRequestCallId = KErrNotFound;
            CPESingleCall* callData = iCallArrayOwner->GetCallObject( aCallId );
            if( callData )
                {
                MCCECall& call = callData->Call();
                iModel.DataStore()->SetCallIndex(call.CallIndex(), aCallId );                
                SetCallOrigin( aCallId, call );                        
                }  
            break;
            }
        case MEngineMonitor::EPEMessageChangedCallDuration:
            {
            TTimeIntervalSeconds duration;
 
            errorCode = GetCallDuration( duration, aCallId );
            if ( errorCode == KErrNone )
                {
                iModel.DataStore()->SetCallDuration( duration, aCallId );
                }
            break;
            }
        case MEngineMonitor::EPEMessageIdle:
            {
            // If Idle is received to pending dial request, clear request
            if ( aCallId == iDialRequestCallId )
                {
                iDialRequestCallId = KErrNotFound;
                }
            HandleAutoResume();
            break;
            }
        case MEngineMonitor::EPEMessageAddedConferenceMember:
            {
            TName memberName;
            TInt count;

            errorCode = GetNumberOfParticipants( count );
            TEFLOGSTRING2( 
                KTAMESIN, 
                "CALL CPECallHandling::SendMessage EPEMessageAddedConferenceMember errorCode %d", 
                errorCode );
            if ( errorCode == KErrNone )
                {
                iModel.DataStore()->SetNumberOfConferenceMembers( count, KPEConferenceCallID );

                iConferenceCall->GetConferenceMemberName( memberName );

                if ( errorCode == KErrNone )
                    {
                    iModel.DataStore()->SetConferenceMemberInfo( 
                        memberName, 
                        KPEConferenceCallID, 
                        aCallId );
                    iModel.DataStore()->SetCallConference( aCallId, 
                            KPEConferenceCallID );
                    }
                }
            break;
            }
        case MEngineMonitor::EPEMessageDroppedConferenceMember:                
            {
            TName memberName;
            TInt memberCallId;
            TInt count;

            errorCode = GetNumberOfParticipants( count );
            TEFLOGSTRING2( 
                KTAMESIN, 
                "CALL CPECallHandling::SendMessage EPEMessageAddedConferenceMember EPEMessageDroppedConferenceMember %d", 
                errorCode );
            if ( errorCode == KErrNone )
                {
                iModel.DataStore()->SetNumberOfConferenceMembers( count, KPEConferenceCallID );

                errorCode = GetConferenceMemberNameAndId( memberName, 
                                                          memberCallId );

                // Member has been taken to OneToOne conversation and is still alive
                if ( errorCode == KErrNone )
                    {
                    iModel.DataStore()->SetConferenceMemberInfo( 
                        memberName, 
                        KPECallIdNotUsed, 
                        aCallId );
                    iModel.DataStore()->SetCallConference( 
                        KPENormalVoiceCall,
                        memberCallId );
                    }
                else
                    {
                    // Member has been dropped and its state is idle, so no 
                    // information can be found...
                    // So actually not an error situation memberName is empty
                    // (set in GetConferenceMemberNameAndId method) 
                    iModel.DataStore()->SetConferenceMemberInfo( 
                        memberName, 
                        KPECallIdNotUsed, 
                        aCallId );
                    errorCode = KErrNone;
                    }
                }                
            break;    
            }
        case MEngineMonitor::EPEMessageCallControlCapsChanged: 
            {
            TPECallControlCaps callControlCaps;
            errorCode = GetCallControlCaps( callControlCaps, aCallId );
            if ( errorCode == KErrNone )
                {
                iModel.DataStore()->SetCallControlCaps( 
                    callControlCaps, 
                    aCallId );
                }
            break;
            }
        case MEngineMonitor::EPEMessageConferenceCapsChange:                             
            {
            TUint32 caps;
            errorCode = GetConferenceCallCaps( caps );
            if ( errorCode == KErrNone )
                {
                iModel.DataStore()->SetConferenceCallCaps( caps, aCallId );
                }
            break;    
            }
        case MEngineMonitor::EPEMessageDisconnecting:
            {
            CPESingleCall* callData = iCallArrayOwner->GetCallObject( aCallId );
            if ( callData )
                {
                TCCPTone inbandTone = callData->Tone();
                iModel.DataStore()->SetInbandTone( inbandTone );
                }
            break;
            }
        case MEngineMonitor::EPEMessageConferenceErrorAddCall:
        case MEngineMonitor::EPEMessageConferenceErrorRemoveCall:
        case MEngineMonitor::EPEMessageConferenceErrorSwap:
        case MEngineMonitor::EPEMessageConferenceErrorHold:
        case MEngineMonitor::EPEMessageConferenceErrorResume:
        case MEngineMonitor::EPEMessageConferenceErrorGoOneToOne:
        case MEngineMonitor::EPEMessageConferenceErrorCurrentCallsToConference:
            {
            errorCode = KErrGeneral;
            if( iConferenceCall && ( iConferenceCall->EnumerateCalls() == 0 ) )
                {
                delete iConferenceCall;
                iConferenceCall = NULL;
                }
            break;
            }
        case MEngineMonitor::EPEMessageCallSecureStatusChanged:
            {
            iModel.DataStore()->SetCallSecureStatus( 
                IsSecureCall( aCallId ), 
                aCallId );
            // Secure is specified  
            iModel.DataStore()->SetSecureSpecified( ETrue );
            break;
            }          
        case MEngineMonitor::EPEMessageSecureNotSpecified:
            {
            iModel.DataStore()->SetSecureSpecified( EFalse );
            break;
            }                     
            
        case MEngineMonitor::EPEMessageRemotePartyInfoChanged:
            {
            TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::SendMessage -> EPEMessageRemotePartyInfoChanged");
            // HO cases call type can changes
            CPESingleCall* call;
            call = static_cast<CPESingleCall*>( iCallArrayOwner->CallByCallId( aCallId ) );
                                 
            MCCECall& connectedCall = call->Call();
            CCPCall::TCallType callType = connectedCall.Parameters().CallType();
 
            if ( EPEStateConnected == call->GetCallState() )
                {
                if ( callType == CCPCall::ECallTypePS ) 
                    {
                    TEFLOGSTRING( KTAMESINT, 
                        "CALL CPECallHandling::SendMessage -> EPEMessageRemotePartyInfoChanged->update call type to PS");
                    iModel.DataStore()->SetCallType( EPECallTypeVoIP, aCallId ); 
                    iModel.DataStore()->SetServiceIdCommand( call->Call().Parameters().ServiceId() );
                    iCallOpenParams->SetCallType( CCPCall::ECallTypePS); 
                    }
                else if ( callType == CCPCall::ECallTypeCSVoice )
                    {
                     TEFLOGSTRING( KTAMESINT, 
                        "CALL CPECallHandling::SendMessage -> EPEMessageRemotePartyInfoChanged->update call type to CS");
                    iCallOpenParams->SetCallType( CCPCall::ECallTypeCSVoice );
                    iModel.DataStore()->SetServiceIdCommand( 1 );
                    iModel.DataStore()->SetCallType( EPECallTypeCSVoice, aCallId ); 
                    if ( UpdateColpNumber( aCallId, connectedCall ))
                        {
                        iModel.SendMessage( MEngineMonitor::EPEMessageColpNumberAvailable, aCallId );
                        }
                    }
                }
            // CNAP informations must be in incoming call
            iModel.DataStore()->SetRemotePartyName( connectedCall.RemotePartyName(), aCallId );                
            iModel.DataStore()->SetRemotePhoneNumber( connectedCall.RemoteParty().Left( KPEPhoneNumberMaxLength ), aCallId );
            iModel.DataStore()->SetCallIndex( connectedCall.CallIndex(), aCallId );            
            break;
            }
                   
        case MEngineMonitor::EPEMessageIncoming:
            {
            TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::SendMessage -> EPEMessageIncoming");
            CPESingleCall* callData = iCallArrayOwner->GetCallObject( aCallId );
            if( callData )
                {
                MCCECall& call = callData->Call();
                iModel.DataStore()->SetRemotePartyName( call.RemotePartyName(), aCallId );
                iModel.DataStore()->SetRemotePhoneNumber( call.RemoteParty().Left( KPEPhoneNumberMaxLength ), aCallId );
                iModel.DataStore()->SetCallIndex(call.CallIndex(), aCallId );
                }
            break;
            }
        case MEngineMonitor::EPEMessageForwardUnconditionalModeActive:
            {
            UpdateSaSetting();
            break;
            }
        case MEngineMonitor::EPEMessageUnattendedTransferRequest:
            {
            CPESingleCall* call = iCallArrayOwner->GetCallObject( aCallId );
            __ASSERT_DEBUG( NULL != call, Panic( EPEPanicIllegalCommand ) );
            if ( call )
                {
                iModel.DataStore()->SetUnattendedTransferTarget( 
                    call->UnattendedTransferTarget(), aCallId );
                }
            
            break;
            }
        case MEngineMonitor::EPEMessageALSLineChanged:
            {
            SetActiveLine();
            break;
            }
        case MEngineMonitor::EPEMessageMovedPermanently:
        case MEngineMonitor::EPEMessageMultipleChoices:
            {
            /* Incoming moved permanently request from server,
             * store target addresses for later use
             */
            CPESingleCall* call = iCallArrayOwner->GetCallObject( aCallId );
            __ASSERT_DEBUG( NULL != call, Panic( EPEPanicIllegalCommand ) );
            if ( call )
                {
                iModel.DataStore()->SetForwardAddressChoices( 
                    call->ForwardAddressChoices(), aCallId );
                }
            
            break;
            }
        default:
            // Other messages cause no action.
            break;
        } 

    if( aCallId != KPECallIdNotUsed )
        {
        // Preferred to be before sendmessage
        // f.e active idle update before display to screen
        iSystemCallState->NotifySystemCallStateChanged( aMessage, aCallId );
        }
    
    if ( errorCode )
        {
        SendErrorMessage( aCallId, errorCode );
        }
    else
        {
        iModel.SendMessage( aMessage, aCallId );
        }

    // Call obect deleting in idle state is handled here because phoneengine 
    // uses call object during idle state handling
    if( aMessage == MEngineMonitor::EPEMessageIdle )
        {
        TEFLOGSTRING2( KTAINT, 
           "CALL CPECallHandling::SendMessage DeleteCallObject %d", aCallId );
        iCallArrayOwner->DeleteCallObject( aCallId );
        }
    else if( aMessage == MEngineMonitor::EPEMessageConferenceIdle )
        {
        delete iConferenceCall;
        iConferenceCall = NULL;
        if ( iReplaceActive )
            {
            AnswerCall();
            iReplaceActive = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPECallHandling::AnswerCall
// Answers an incoming call
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::AnswerCall()
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::AnswerCall");
    TInt callIndex;
    CPESingleCall* callData;
    CPESingleCall* connectedCall;
    TInt errorCode( ECCPErrorNotFound );

    callData = VoiceCallDataByState( EPEStateRinging, callIndex );
    connectedCall = iCallArrayOwner->CallPointerByState( EPEStateConnected );
    if( connectedCall && !iReplaceActive )
        {
        CCPCall::TCallType callType = connectedCall->Call().Parameters().CallType();
        TEFLOGSTRING2( KTAINT, 
            "CALL CPECallHandling::AnswerCall EPEStateConnected type %d", callType );
        if( callType == CCPCall::ECallTypeVideo )
            {
            TEFLOGSTRING( KTAINT, 
                "CALL CPECallHandling::AnswerCall ECCPErrorNotAllowed" );
            return ECCPErrorNotAllowed;
            }
        }
        
    if ( callData )
        {
        TEFLOGSTRING2( KTAINT, 
            "CALL CPECallHandling::AnswerCall > CPESingleCall::Answer %d", 
            callData->GetCallId() );
        errorCode = callData->Answer();        
        }
    else
        {
        TEFLOGSTRING( KTAINT, 
            "CALL CPECallHandling::AnswerCall > iVideoCallHandling->AnswerCall");
        errorCode = iVideoCallHandling->AnswerCall();
        }
        
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::DialCall
// creates dial request to the CPESingleCall object
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::DialCall( 
    const TPEPhoneNumber& aNumber,
    TInt& aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DialCall" );
    CPESingleCall* callData( NULL );
    TInt errorCode( KErrNone );

    // If there is allready video call, one dial in connecting, dialing or disconnecting state,
    // we just ignore new dial request and send KErrInUse back to UI
    if( iDialRequestCallId != KErrNotFound )
        {
        // Dial request already send, waiting for dialing state.
        errorCode = KErrGeneral;
        TEFLOGSTRING( KTAERROR, 
            "CALL CPECALLHANDLING::DIALCALL ! DIAL FAILED: DIAL REQUEST ALREADY ACTIVE" );
        }
    else if ( IsDialAllowed ( EFalse ) )
        {
        // Set user to user info call params.
                // Set user to user info call params.
                
        const CCCECallParameters& params = iModel.DataStore()->CallParameters();
        iCallOpenParams->SetBearer( params.Bearer() );
        iCallOpenParams->SetSubAddress( params.SubAddress() );
        iCallOpenParams->SetOrigin( params.Origin() );
        iCallOpenParams->SetUUSId( iModel.DataStore()->UserToUserInformation() );
        
        TRAP( errorCode, callData = OpenNewCallL( aNumber ) );
        if( errorCode == KErrNone )
            {
            if( iModel.DataStore()->CallOriginCommand() == EPECallOriginSAT )
                {
                // disable number matching for SAT calls
                callData->DisableFDNCheck();
                }
            
            TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DialCall > Dial" );
            iDialRequestCallId = aCallId;
            errorCode = callData->Dial( aNumber );
            
            if ( errorCode != KErrNone )
                {
                iDialRequestCallId = KErrNotFound;
                // Dial failed: clean up
                ReleaseCallObject( callData->GetCallId() );
                TEFLOGSTRING2( KTAERROR, 
                    "CALL CPECALLHANDLING::DIALCALL ! DIAL FAILED: MAY NOT PROCEED! %d", errorCode );
                }
            else
                {
                // Dial request passed on successfully: forward new call id
                aCallId = callData->GetCallId();
                }
            }
        }
    else
        {
        errorCode = KErrInUse;
        }
        
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::DialMultimedia
// creates dial request to the CPESingleCall object
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::DialMultimedia( 
        const TPEPhoneNumber& aNumber,
        TInt& aCallId
        )
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::DialMultimedia");
    TInt errorCode( ECCPErrorNone );
    
    // If there is allready call, one dial in connecting, dialing or disconnecting state,
    // we just ignore new dial request and send KErrInUse back to UI    
    if ( IsDialAllowed ( ETrue ) )
        {
        errorCode = iVideoCallHandling->DialCall( aNumber, aCallId );
        }
    else
        {
        errorCode = ECCPErrorAlreadyInUse;
        }
    return errorCode;
    }
// -----------------------------------------------------------------------------
// CPECallHandling::IsDialAllowed
//  
// -----------------------------------------------------------------------------
//
TBool CPECallHandling::IsDialAllowed( TBool aMultimediaDial )
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::IsDialAllowed");
    
    CPESingleCall* call = iCallArrayOwner->CallPointerByState( EPEStateConnected );
    CPESingleCall* heldCall = iCallArrayOwner->CallPointerByState( EPEStateHeld );
    
    TBool status = EFalse;
    
    if( ( call || heldCall ) && aMultimediaDial )
        {
        // A call was found when tried video call
        TEFLOGSTRING( KTAERROR, 
            "CALL CPECALLHANDLING::IsDialAllowed() CANNOT CREATE A VIDEO CALL IN CALL STATE!");
        }
    else if( call && iVideoCallHandling->IsMultimedia( call->GetCallId() ) )
        {
        // A video call was found when tried voice or video call
        TEFLOGSTRING( KTAERROR, 
            "CALL CPECALLHANDLING::IsDialAllowed() CANNOT CREATE A CALL IN VIDEO CALL STATE!");
        }
    else if( iCallArrayOwner->CallPointerByState( EPEStateDialing ) ) 
        {
        // A dialing call was found
            TEFLOGSTRING( KTAERROR, 
                "CALL CPECALLHANDLING::IsDialAllowed() CANNOT CREATE A CALL IN DIALING STATE!");
         }
    else if( iCallArrayOwner->CallPointerByState( EPEStateConnecting ) ) 
        { 
        // A connecting call was found
         TEFLOGSTRING( KTAERROR, 
             "CALL CPECALLHANDLING::IsDialAllowed() CANNOT CREATE A CALL IN CONNECTING STATE!");

        }
    else if( iCallArrayOwner->CallPointerByState( EPEStateDisconnecting ) ) 
        {
        // A disconnecting call was found
        TEFLOGSTRING( KTAERROR, 
            "CALL CPECALLHANDLING::IsDialAllowed() CANNOT CREATE A CALL IN DISCONNECTING STATE!");
         }
    else if( ( iConferenceCall && iConferenceCall->GetCallState() == EPEStateConnectedConference && ( heldCall ) ) ||
            ( iConferenceCall && iConferenceCall->GetCallState() == EPEStateHeldConference && ( call ) ) )
        {
        // coference and single call found
        TEFLOGSTRING( KTAERROR, 
            "CALL CPECALLHANDLING::IsDialAllowed() CANNOT CREATE A CALL IN CONFERENCE AND SINGLE STATE!");
        }
    else
        {
        status = ETrue; // Dial allowed
        }
    return status; 
    }

// -----------------------------------------------------------------------------
// CPECallHandling::HangUp
// Terminates an ongoing call
// -----------------------------------------------------------------------------
//
EXPORT_C TInt  CPECallHandling::HangUp( 
        TInt aCallId, 
        TPEHangUpOptions aAutoResumeOption )
    {
    TEFLOGSTRING3( KTAMESINT, "CALL CPECallHandling::HangUp aCallId= %d aAutoResumeOption= %d ", aCallId, aAutoResumeOption );
    TInt errorCode( ECCPErrorNotFound );
    
    CPESingleCall* heldcall = iCallArrayOwner->CallPointerByState( EPEStateHeld );

    if( aAutoResumeOption == ETPEHangUpResumeHeldCall )
        {
        if( heldcall )
            {
            iModel.DataStore()->SetResumeHeldCall( ETrue, heldcall->GetCallId() );
            }
        else if( iConferenceCall && ( iConferenceCall->GetCallState() == EPEStateHeldConference ) )
            {
            iModel.DataStore()->SetResumeHeldCall( ETrue, KPEConferenceCallID );
            }
        }
    else if( aAutoResumeOption == ETPEHangUpNotResumeHeldCall )
        {
        if( heldcall )
            {
            iModel.DataStore()->SetResumeHeldCall( EFalse, heldcall->GetCallId() );
            }
        else if( iConferenceCall && ( iConferenceCall->GetCallState() == EPEStateHeldConference ) )
            {
            iModel.DataStore()->SetResumeHeldCall( EFalse, KPEConferenceCallID );
            }
        }

    if ( CallIdCheck::IsVoice( aCallId )) 
        {
        CPESingleCall* callData = iCallArrayOwner->GetCallObject( aCallId );
        if( callData )
            {
            // conference call that is not yet created, must be cancelled.
            if( iConferenceCall && iConferenceCall->GetCallState() == EPEStateCreatingConference )
                {
                TEFLOGSTRING( KTAERROR, 
                    "CPECallHandling::HangUp Delete conference call.");
                delete iConferenceCall;
                iConferenceCall = NULL;
                }
            
            errorCode = callData->HangUp();
            }
        else
            {
            TEFLOGSTRING( KTAERROR, 
                "CALL CPECALLHANDLING::HANGUP ! CALL OBJECT IN IDLE STATE OR ALREADY HANGING UP" );
            }
        }

    else if ( CallIdCheck::IsVideo( aCallId ) ) 
        {
        errorCode = iVideoCallHandling->HangUp( aCallId );
        }

    else if ( CallIdCheck::IsConference( aCallId ) )
        {
        if( iConferenceCall )
            {
            errorCode = iConferenceCall->HangUp();
            }
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::TerminateAllConnections
// Terminates all ringing voice and data calls
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::TerminateAllConnections()
    {
    CPESingleCall* callData;
    TPEState callState;

    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::TerminateAllConnections");
    
    RejectCall(); // Rejects ringing call if one exists.

    if ( iConferenceCall )
        {
        TEFLOGSTRING( KTAMESOUT, 
            "CALL CPECallHandling::TerminateAllConnections: Hanging Up conference call" );
        iConferenceCall->HangUp();
        }
            
    // Hangup normal Voice Calls
    for( TInt callId = 0; callId < KPEMaximumNumberOfVoiceCalls; callId++ )
        {
        callData = iCallArrayOwner->GetCallObject( callId );
        if( callData )
            {
            callState = callData->GetCallState();
            if ( callState != EPEStateIdle ) 
                {
                TEFLOGSTRING2( KTAREQOUT, 
                    "CALL CPECallHandling::TerminateAllConnections: Hanging Up call id %d...", 
                    callId );
                callData->HangUp();
                }
            }
        }
    //Terminate all ringing data calls, connected data calls and packet data connections
    return iVideoCallHandling->TerminateAllConnections();
    }

// -----------------------------------------------------------------------------
// CPECallHandling::IsCallInState
// returns ETrue if there is a call in given state; otherwise EFalse
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPECallHandling::IsCallInState( TPEState aState ) const
    {
    TBool isCallInState( EFalse );
    iCallArrayOwner->CallPointerByState( aState ) ? isCallInState = ETrue : isCallInState = EFalse;
    
    return isCallInState;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GetCallIdByState
// returns return callid is there is a call; otherwise KPECallIdNotUsed( -1 ).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::GetCallIdByState( TPEState aState ) const
    {
    TInt callId( KPECallIdNotUsed );
    CPESingleCall* call = iCallArrayOwner->CallPointerByState( aState );
    if( call )
        {
        callId = call->GetCallId();
        }
    TEFLOGSTRING2( KTAREQOUT, 
                   "CALL CPECallHandling::GetCallIdByState: callid %d", 
                   callId );
    return callId;
    }
    
// -----------------------------------------------------------------------------
// CPECallHandling::GetMissedCall
// Returns the missed voice call indicator.
// Method checks that call id is valid number
// Method gets CPECallData object from the CArrayPtrFlat and
// Method gets the missed voice call indicator from CPECallData object
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::GetMissedCall( 
        TBool& aMissedCall,
        TInt aCallId )
    {
    TInt errorCode( ECCPErrorNotFound );

    CPESingleCall* voiceCall = iCallArrayOwner->GetCallObject( aCallId );
    if( voiceCall )
        {
        errorCode = voiceCall->GetMissedCall( aMissedCall );
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic( EPEPanicIllegalCommand) );
        }

    return errorCode;
    }
    
// -----------------------------------------------------------------------------
// CPECallHandling::GetCallInfo
// Returns call info
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::GetCallInfo( 
    RMobileCall::TMobileCallInfoV3& aCallInfo,
    TInt aCallId )
    {
    CPESingleCall* callData;
    TInt errorCode( ECCPErrorNotFound );

    if ( CallIdCheck::IsVoice( aCallId ))
        {
        callData = iCallArrayOwner->GetCallObject( aCallId );
        if( callData )
            {
            errorCode = callData->GetCallInfo( aCallInfo );
            }
        else
            {
            __ASSERT_DEBUG( EFalse, Panic( EPEPanicIllegalCommand) );
            }
        }
        
    else if ( CallIdCheck::IsVideo( aCallId ) )
        {
        errorCode = iVideoCallHandling->GetCallInfo( aCallInfo, aCallId );
        }
        
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GetCallState
// Returns call state
// -----------------------------------------------------------------------------
//
EXPORT_C TPEState CPECallHandling::GetCallState( 
        TInt aCallId )
    {
    TPEState callState( EPEStateUnknown );
    
    if ( CallIdCheck::IsConference( aCallId ) )
        {
        if( iConferenceCall )
            {
            callState = iConferenceCall->GetCallState();
            }
        else
            {
            callState = EPEStateConferenceIdle;
            }
        }
    else
        {
        CPESingleCall* callData = iCallArrayOwner->GetCallObject( aCallId );
        if( callData )
            {
            callState = callData->GetCallState();
            }
        else
            {
            callState = EPEStateIdle;
            }
        }

    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::GetCallState, callState: %d", callState );
    return callState;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GetNumberOfCalls
// Returns number of non-idle calls
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::GetNumberOfCalls()
    {
    // Count ongoing calls
    return iCallArrayOwner->ActiveCallCount();
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GetCallDuration
// Returns voice call duration.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::GetCallDuration( 
    TTimeIntervalSeconds& aDuration,
    TInt aCallId )
    {
    TInt errorCode( KErrNone );

    if ( CallIdCheck::IsVoice( aCallId ))
        {
        CPESingleCall* callData = iCallArrayOwner->GetCallObject( aCallId );
        if( callData )
            {
            callData->GetCallDuration( aDuration );
            }
        else
            {
            __ASSERT_DEBUG( EFalse, Panic( EPEPanicIllegalCommand) );
            }
        }
    else if ( CallIdCheck::IsVideo( aCallId ) )
        {
        iVideoCallHandling->GetCallDuration( aDuration, aCallId );
        }
    else if ( CallIdCheck::IsConference( aCallId ) )
        {
        if( iConferenceCall )
            {
            iConferenceCall->GetCallDuration( aDuration );
            errorCode = KErrNone;
            }
        else
            {
            errorCode = ECCPErrorNotFound;
            }
        }
    else
        {
        errorCode = ECCPErrorNotFound;
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::RejectCall
// rejects the incoming call
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::RejectCall()
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::RejectCall");
    TInt callIndex;
    TInt errorCode( ECCPErrorNotFound );
    
    CPESingleCall* callData = VoiceCallDataByState( EPEStateRinging, callIndex );
    if( callIndex >= 0 )
        {
        callData->HangUp();
        errorCode = KErrNone;
        }
    else
        {
        // Data call reject
        errorCode = iVideoCallHandling->RejectCall();
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::ReleaseAll
// Release ongoing calls
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::ReleaseAll()
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::ReleaseAll");
    CPESingleCall* callData;
    TInt errorCode( ECCPErrorNotFound );
    
    // Normal Voice Calls
    for ( TInt callId=0; callId < KPEMaximumNumberOfVoiceCalls; callId++ )
        {
        callData = iCallArrayOwner->GetCallObject( callId );
        if( callData && 
            callData->GetCallState() != EPEStateIdle &&
            callData->GetCallState() != EPEStateRinging )
            {
            callData->HangUp();
            errorCode = KErrNone;
            }
        }
    
    // Release ongoing data calls
    errorCode ? errorCode = iVideoCallHandling->ReleaseAll() : iVideoCallHandling->ReleaseAll();
      
    TEFLOGSTRING2( KTAINT, "PE CPECallHandling::ReleaseAll, error id: %d", 
        errorCode );
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SendDtmf
// sends dtmf string
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::SendDtmf( 
    const TPEDtmfString& aDtmfString )
    {
    return iDtmfHandling->SendDtmfString( aDtmfString );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::ContinueDtmfSending
// Continues dtmf sending after 'w'-character
// -----------------------------------------------------------------------------
//
EXPORT_C void CPECallHandling::ContinueDtmfSending()
    {
    iDtmfHandling->ContinueDtmfSending();
    }

// -----------------------------------------------------------------------------
// CPECallHandling::StartDtmfTone
// sends dtmf tone to the remote party
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::StartDtmfTone( 
    const TChar& aTone )
    {
    iDtmfHandling->StartDtmfTone(aTone);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::StopDtmfSending
// Stops dtmf sending after 'w'-character
// -----------------------------------------------------------------------------
//
EXPORT_C void CPECallHandling::StopDtmfSending()
    {
    iDtmfHandling->StopDtmfSending();
    }

// -----------------------------------------------------------------------------
// CPECallHandling::StopDtmfTone
// stops sending dtmf tone to the remote party
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::StopDtmfTone()
    {
    iDtmfHandling->StopDtmfTone();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::CancelDtmfPlay
// Cancels Dtmf string sending
// -----------------------------------------------------------------------------
//
EXPORT_C void CPECallHandling::CancelDtmfPlay()
    {
    iDtmfHandling->CancelDtmfString();
    }

// -----------------------------------------------------------------------------
// CPECallHandling::VoiceCallDataByState
// returns CPESingleCall voice call object by State
// -----------------------------------------------------------------------------
//
EXPORT_C CPESingleCall* CPECallHandling::VoiceCallDataByState( 
    TPEState aState,
    TInt& aIndex ) const
    {
    aIndex = KPECallIdNotUsed;
    CPESingleCall* returnValue = NULL;
    if ( aState != EPEStateIdle )
        {
        for ( TInt callId=0; callId < KPEMaximumNumberOfVoiceCalls; callId++ )
            {        
            CPESingleCall* callData = iCallArrayOwner->GetCallObject( callId );
            if( callData )
                {
                if ( callData->GetCallState() == aState )
                    {
                    TEFLOGSTRING2( 
                        KTAREQEND, 
                        "CALL CPECallHandling::VoiceCallDataByState: , aState: %d", 
                        aState );
                    TEFLOGSTRING2( 
                        KTAREQEND, 
                        "CALL CPECallHandling::VoiceCallDataByState: , callId: %d", 
                        aState );
                    aIndex = callId;
                    returnValue = callData;
                    break;
                    }
                }
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// From base class MPECallInitiator
// Initialises (incoming,external) voice call.
// -----------------------------------------------------------------------------
//
void CPECallHandling::InitVoiceCall( MCCECall& aNewCall )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::InitVoiceCall" );
    CPESingleCall* callData( NULL );
    TRAP_IGNORE( ( callData = iCallArrayOwner->CreateCallL( *this ) ) );

    if ( callData )
        {
        iModel.DataStore()->SetPhoneNumber( KNullDesC() );
        iModel.DataStore()->SetServiceId( callData->GetCallId(), aNewCall.ServiceId() );
        const CCCPCallParameters& callParameters = aNewCall.Parameters();
        
        TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::InitVoiceCall call type: %d",  callParameters.CallType() );
        switch ( callParameters.CallType() )
            {
            case CCPCall::ECallTypeCSVoice:
                {
                iModel.DataStore()->SetCallType( EPECallTypeCSVoice, callData->GetCallId() );
                // Set incoming Call's Als line
                const CCCECallParameters& cceparams = 
                    static_cast<const CCCECallParameters&> (callParameters);
                iModel.DataStore()->SetCallALSLine( cceparams.LineType(), callData->GetCallId() );
                }
                break;
            case CCPCall::ECallTypePS:
                {
                iModel.DataStore()->SetCallType( EPECallTypeVoIP, callData->GetCallId() );
                }
                break;
            default:
                __ASSERT_DEBUG( EFalse, Panic( EPEPanicIndexOutOfRange ));
                break;
            }
        callData->SetCall( aNewCall );
        TPECallControlCaps callControlCaps;
        TInt err = GetCallControlCaps( callControlCaps, callData->GetCallId() );
        if ( err == KErrNone )
            {
            iModel.DataStore()->SetCallControlCaps( callControlCaps, callData->GetCallId() );
            }
        iModel.DataStore()->SetCallSecureStatus( IsSecureCall( callData->GetCallId() ), callData->GetCallId() );
        iModel.DataStore()->SetSecureSpecified( callData->SecureSpecified() );
        RMobileCall::TMobileCallInfoV3 callInfo;
        GetCallInfo(callInfo, callData->GetCallId());
        iModel.DataStore()->SetIncomingCallForwarded( callInfo.iForwarded, callData->GetCallId() );
        }
    else
        {
        Panic( EPEPanicNoFreeCalls );
        }
    }

// -----------------------------------------------------------------------------
// From base class MPECallInitiator
// Initialises (incoming,external) video call.
// -----------------------------------------------------------------------------
//
void CPECallHandling::InitVideoCall( MCCECall& aNewCall )
    {
    iVideoCallHandling->InitCall( aNewCall );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::ConnectedCalls
// Indicates if there is any connected calls
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPECallHandling::ConnectedCalls()
    {
    TInt index;
    return ( VoiceCallDataByState( EPEStateConnected, index ) ||
             VoiceCallDataByState( EPEStateHeld, index ) ||
             iVideoCallHandling->ConnectedCalls() );
    }


// -----------------------------------------------------------------------------
// CPECallHandling::StartUp
// Starts monitoring incoming data calls
// -----------------------------------------------------------------------------
//
EXPORT_C void CPECallHandling::StartUp()
    {
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SetCallParams
//
// -----------------------------------------------------------------------------
//
void CPECallHandling::SetCallParams( TInt aCallId )
    {
    iCallOpenParams->SetLineType( iActiveLine );
    iModel.DataStore()->SetCallALSLine( iActiveLine, aCallId );
    switch ( iModel.DataStore()->CallTypeCommand() )
        {
        case EPECallTypeCSVoice:
            {
            iCallOpenParams->SetCallType( CCPCall::ECallTypeCSVoice );
            iModel.DataStore()->SetServiceIdCommand( 1 );
            iModel.DataStore()->SetCallType( EPECallTypeCSVoice, aCallId );
            }
            break;
        case EPECallTypeVideo:
            {
            iCallOpenParams->SetCallType( CCPCall::ECallTypeVideo);
            iModel.DataStore()->SetServiceIdCommand( 1 );
            iModel.DataStore()->SetCallType( EPECallTypeVideo, aCallId );
            }
            break;
        case EPECallTypeVoIP:
            {
            iCallOpenParams->SetCallType( CCPCall::ECallTypePS);
            iModel.DataStore()->SetCallType( EPECallTypeVoIP, aCallId );
            //PhoneApp set service id in voip call case
            }
            break;
        case EPECallTypeUninitialized:
        default:
            iCallOpenParams->SetCallType( CCPCall::ECallTypeCSVoice);
            iModel.DataStore()->SetServiceIdCommand( 1 );
            iModel.DataStore()->SetCallType( EPECallTypeCSVoice, aCallId );
            break;
        } 
    iCallOpenParams->SetServiceId( iModel.DataStore()->ServiceIdCommand() );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SendMessage
// Reroutes messages to the Phone Engine
// -----------------------------------------------------------------------------
//
void CPECallHandling::SendMessage( 
    const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
    const TName& aName )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::SendMessage, message id: %d", aMessage );
    TInt callId( KPECallIdNotUsed );
    MPECall* call = iCallArrayOwner->CallByName( aName );
    if( call )
        {
        callId = call->GetCallId();
        }

    SendMessage( aMessage, callId );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::HandleInternalMessage
// Reroutes messages to the Phone Engine
// -----------------------------------------------------------------------------
//
void CPECallHandling::HandleInternalMessage(
    TInt aMessage )
    {
    iModel.HandleInternalMessage( aMessage );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::OpenNewCallL
// returns ETrue if hangup active
// -----------------------------------------------------------------------------
//
CPESingleCall* CPECallHandling::OpenNewCallL( const TPEPhoneNumber& aNumber )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::OpenNewCallL");
    TInt errorCode( KErrNone );
    CPESingleCall* callData( NULL );
    MCCECall* cceCall( NULL );
    callData = iCallArrayOwner->CreateCallL( *this );
    SetCallParams( callData->GetCallId() );
    
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::OpenNewCallL > CCCE::OpenNewCallL");
    TRAP( errorCode, 
          cceCall = &( iConvergedCallEngine.OpenNewCallL( aNumber, 
                                                          *iCallOpenParams, 
                                                          *callData ) ) );
    if ( errorCode == KErrNone )
        {
        callData->SetCall( *cceCall );
        iModel.DataStore()->SetServiceId( callData->GetCallId(), cceCall->ServiceId() );
        iModel.DataStore()->SetCallSecureStatus( callData->IsSecureCall(), callData->GetCallId() );
        iModel.DataStore()->SetSecureSpecified( callData->SecureSpecified() ); 
        TPECallControlCaps callControlCaps;
        TInt err = GetCallControlCaps( callControlCaps, callData->GetCallId() );
        if ( err == KErrNone )
            {
            iModel.DataStore()->SetCallControlCaps( callControlCaps, callData->GetCallId() );
            }
        }
    else
        {
        // Open new call failed
        ReleaseCallObject( callData->GetCallId() );
        TEFLOGSTRING2( KTAERROR, 
            "CALL CPECALLHANDLING::OPENNEWCALLL ! OPENNEWCALL FAILED: MAY NOT PROCEED! %d", errorCode );
        User::Leave( errorCode );
        }
    return callData;
    }
 
// -----------------------------------------------------------------------------
// CPECallHandling::SetActiveLine
// Sets active line
// -----------------------------------------------------------------------------
//
EXPORT_C void CPECallHandling::SetActiveLine()
    {
    TEFLOGSTRING( KTAINT, "PE CPECallHandling::SetActiveLine" );
    iActiveLine = iModel.DataStore()->ALSLine();
    }

// -----------------------------------------------------------------------------
// CPECallHandling::IsSecureCall
// Returns the call secure status
// -----------------------------------------------------------------------------
//
TBool CPECallHandling::IsSecureCall( const TInt aCallId ) const
    {
    TEFLOGSTRING2( 
        KTAINT, 
        "PE CPECallHandling::IsSecureCall:  aCallId = %d", 
        aCallId );
    CPESingleCall* call = iCallArrayOwner->GetCallObject( aCallId );
    
    __ASSERT_DEBUG( call, Panic( EPEPanicIndexOutOfRange ) );  
    TBool secured( EFalse );
    if( call )
        {
        secured = call->IsSecureCall();
        }
    return secured;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GetNumberOfParticipants
// returns number of conference members
// -----------------------------------------------------------------------------
//
TInt CPECallHandling::GetNumberOfParticipants( 
        TInt& aCount )    // The Number of participants 
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::GetNumberOfParticipants" );
    TInt errorCode( ECCPErrorNotFound );

    if ( iConferenceCall )
        {
        aCount = iConferenceCall->EnumerateCalls();
        TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::GetNumberOfParticipants count %d", aCount );
        errorCode = KErrNone;
        }
    return errorCode;
    }
 
// -----------------------------------------------------------------------------
// CPECallHandling::GetConferenceMemberNameAndId
// Returns added or removeds members TName information and CallId.
// -----------------------------------------------------------------------------
//
TInt CPECallHandling::GetConferenceMemberNameAndId( 
        TName& aCallName,   // Added or removed members TName information is returned here
        TInt& aMemberCallId ) // Added or removed members callid is returned here
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::GetConferenceMemberNameAndId" );
    TInt errorCode( ECCPErrorNotFound );

    if ( iConferenceCall )
        {
        iConferenceCall->GetConferenceMemberName( aCallName );
        MPECall* call = iCallArrayOwner->CallByName( aCallName );
        
        if( call )
            {
            aMemberCallId = call->GetCallId();
            TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::GetConferenceMemberNameAndId CallId %d", aMemberCallId );
            errorCode = KErrNone;
            }
        else
            {
            aMemberCallId = -1;
            aCallName = KNullDesC;
            }   
        }
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::GetConferenceMemberNameAndId errorCode %d", errorCode );
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GetCallControlCaps
// returns call info
// Method checks that call id is valid number
// Method gets CPESingleCall object from the CArrayPtrFlat and
// Method gets call info from CPESingleCall object
// -----------------------------------------------------------------------------
//
TInt CPECallHandling::GetCallControlCaps(
        TPECallControlCaps& aCallControlCaps,
        TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::GetCallControlCaps %d", aCallId );
    CPESingleCall* callData;
    TInt errorCode( ECCPErrorNotFound );
    MCCECallObserver::TCCECallControlCaps callControlCaps;
    callData = iCallArrayOwner->GetCallObject( aCallId );

    if ( callData )
        {
        callData->GetCallControlCaps( callControlCaps );
        aCallControlCaps = static_cast<TPECallControlCaps>( callControlCaps );
        errorCode = KErrNone;
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GetConferenceCallCaps
// returns conference call capabilities
// Checks that call id is valid and gets CPEConferenceCall object and calls its method
// to get capabilities
// -----------------------------------------------------------------------------
//
TInt CPECallHandling::GetConferenceCallCaps( 
        TUint32& aCaps )     // capabilities are returned in this parameter
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::GetConferenceCallCaps" );
    TInt errorCode( ECCPErrorNotFound );
    if ( iConferenceCall  )
        {
        aCaps = iConferenceCall->CallCaps();
        errorCode = KErrNone;
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::AddMember
// Adds member to the conference call
// -----------------------------------------------------------------------------
//
TInt CPECallHandling::AddMember(
        TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::AddMember %d", aCallId );
    TInt errorCode( KErrNone );
    TRAP( errorCode, AddMemberL( aCallId ));
    return errorCode;
    }


// -----------------------------------------------------------------------------
// CPECallHandling::AddMemberL
// Adds member to the conference call
// -----------------------------------------------------------------------------
//
void CPECallHandling::AddMemberL(
        TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::AddMemberL" );
    CPESingleCall* callData;
    callData = iCallArrayOwner->GetCallObject( aCallId );
    
    if ( callData && iConferenceCall && CallIdCheck::IsVoice( aCallId ))
        {
        iConferenceCall->AddCallL( callData->Call() );
        }
    else
        {
        User::Leave( ECCPErrorNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CPECallHandling::CallTerminatedError
// Handles RemoteTerminated. 
// -----------------------------------------------------------------------------
// 
EXPORT_C TInt CPECallHandling::CallTerminatedError(
        const TInt aCallId )
    {
    TInt errorCode( KErrNone );
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::CallTerminatedError" );
    CPESingleCall* callObject = iCallArrayOwner->GetCallObject( aCallId );
    if ( callObject )
        {
        callObject->GetErrorCode( errorCode );
        }
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::BuildConference
// creates conference call
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::BuildConference()
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::BuildConference" );
    TInt heldcallid;
    CPESingleCall* heldcall( NULL );
    TInt connectedcallid;
    CPESingleCall* connectedcall( NULL );
    TInt errorCode( ECCPErrorNotAllowed );

    heldcall = VoiceCallDataByState( EPEStateHeld, heldcallid );
    if ( heldcall )
        {
        connectedcall = VoiceCallDataByState( EPEStateConnected, connectedcallid );
        
        if ( connectedcall )
            {
            TRAP( errorCode, CreateConferenceCallL( *heldcall, *connectedcall ) );
            }
        }
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SwapCalls
// resumes a held call
// Method search active and held call
// Method gets CPESingleCall object from the CArrayPtrFlat and
// Method checks that call is in held state
// Method makes swap request to CPESingleCall object.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::SwapCalls()
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::SwapCalls" );
    TInt callIndex;
    CPESingleCall* callData;
    TInt errorCode( ECCPErrorNotFound );
    TBool onlyConferenceCall( ( iConferenceCall && 
            ( iConferenceCall->EnumerateCalls() == GetNumberOfCalls()) ));

    if ( ( iConferenceCall ) && 
         ( iConferenceCall->GetCallState() == EPEStateConnectedConference ||
           (iConferenceCall->GetCallState() == EPEStateHeldConference &&
            onlyConferenceCall ) ) )
        {
        errorCode = iConferenceCall->Swap();
        }
    else
        {
        callData = VoiceCallDataByState( EPEStateConnected, callIndex );
        if( callData )
            {
            errorCode = callData->Swap();
            }
        }

    return errorCode;
    }   

// -----------------------------------------------------------------------------
// CPECallHandling::AddConferenceMember
// Handles add conference member
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::AddConferenceMember()
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::AddConferenceMember" );

    CPESingleCall* callData;
    TInt errorCode( ECCPErrorNotFound );
    TPEState callState;
    
    // Checks that call is single call and state of call is connected or held
    for( TInt callId = 0; callId < KPEMaximumNumberOfVoiceCalls; callId++ )
        {
        callData = iCallArrayOwner->GetCallObject( callId );
        if( callData )
            {
            // Check that call is not already join to Conference
            if ( iModel.DataStore()->IsConferenceMemberId( callId ) == KPECallIdNotUsed )
                {
                callState = callData->GetCallState();
                if ( callState == EPEStateConnected || callState == EPEStateHeld )
                    {
                    TEFLOGSTRING2( KTAINT, 
                        "CALL CPECallHandling::AddConferenceMember > AddMember, CallId =  %d"
                        , callId );
                    errorCode = AddMember( callId );
                    break;
                    }
                }
            }
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::DropMember
// drops member from the conference call
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::DropMember( 
        TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::DropMember %d", aCallId );
    CPESingleCall* callData;
    TInt errorCode( ECCPErrorNotFound );

    if ( CallIdCheck::IsVoice( aCallId ))
        {
        callData = iCallArrayOwner->GetCallObject( aCallId );
        if ( callData )
            {
            callData->HangUp();
            errorCode = KErrNone;
            }
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GoOneToOne
// Splits one call to private conversation.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::GoOneToOne( 
        TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::GoOneToOne %d", aCallId );
    TInt errorCode( ECCPErrorNotFound );
    CPESingleCall* callData;
    
    callData = iCallArrayOwner->GetCallObject( aCallId );
            
    if ( iConferenceCall && callData )
        {
        TRAP( errorCode, iConferenceCall->GoOneToOneL( callData->Call() ) );
        }
    return errorCode;
    }
 
// -----------------------------------------------------------------------------
// CPECallHandling::HoldCall
// holds an active call
// Method checks that call id is valid number and
// Method gets CPESingleCall object from the CArrayPtrFlat and
// Method checks that call is in connected state
// Method makes hold request to CPESingleCall object.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::HoldCall()
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::HoldCall" );
    CPESingleCall* callData;
    TInt callIndex;
    TInt errorCode( ECCPErrorNotAllowed );

    if ( iConferenceCall && ( iConferenceCall->GetCallState() == EPEStateConnectedConference ) )
        {
        // hold was explicitly requested by the user, 
        // update information to engine info
        iModel.DataStore()->SetResumeHeldCall( EFalse, KPEConferenceCallID );
        errorCode = iConferenceCall->Swap();
        }
    else
        {
        callData = VoiceCallDataByState( EPEStateConnected, callIndex );   
        if( callData )
            {
            // hold was explicitly requested by the user, 
            // update information to engine info
            iModel.DataStore()->SetResumeHeldCall( EFalse, callIndex );
            errorCode = callData->Hold();
            }
        }

    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::ResumeCall
// resumes a held call
// Method checks that call id is valid number and
// Method gets CPESingleCall object from the CArrayPtrFlat and
// Method checks that call is in held state
// Method makes resume request to CPESingleCall object.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::ResumeCall()
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::ResumeCall" );
    CPESingleCall* callData;
    TInt callIndex;
    TInt errorCode( ECCPErrorNotAllowed );

    if ( iConferenceCall && ( iConferenceCall->GetCallState() == EPEStateHeldConference ) )
        {
        // resume was explicitly requested by the user, update information to engine info
        iModel.DataStore()->SetResumeHeldCall( ETrue, KPEConferenceCallID );
        errorCode = iConferenceCall->Swap();
        }
    else
        {
        callData = VoiceCallDataByState( EPEStateHeld, callIndex );
        if( callData )
            {
            // resume was explicitly requested by the user, update information to engine info
            iModel.DataStore()->SetResumeHeldCall( ETrue, callIndex );
            errorCode = callData->Resume();
            }
        }

    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::TransferCalls
// transfers the held party to the active party
// Method search active and held call
// Method gets CPESingleCall object from the CArrayPtrFlat and
// Method checks that call is in held state
// Method makes transfer request to CPESingleCall object.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::TransferCalls()
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::TransferCalls" );
    TInt callIndex;
    CPESingleCall* callData;
    CPESingleCall* callDataHeld;
    TInt errorCode( ECCPErrorNotAllowed );

    callDataHeld = VoiceCallDataByState( EPEStateHeld, callIndex );
    if( callDataHeld && !iConferenceCall )
        { // found one held call
        callData = VoiceCallDataByState( EPEStateConnected, callIndex );
        if ( callData ) // found the connected call
           {
           TEFLOGSTRING( KTAINT, "CALL CPECallHandling::TransferCalls > Transfer" );
           callDataHeld->Transfer( callData->Call().DialledParty() );
           errorCode = KErrNone;
           }
        else
           {
           callData = VoiceCallDataByState( EPEStateConnecting, callIndex );
           if ( callData ) // The connecting call found
              {
              TEFLOGSTRING( KTAINT, "CALL CPECallHandling::TransferCalls > Transfer" );
              callDataHeld->Transfer( callData->Call().DialledParty() );
              errorCode = KErrNone;
              }
           }
        }
        
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::DialEmergencyCall
// creates emergency dial request to the CPESingleCall object
//    Method gets CPESingleCall object from the CArrayPtrFlat
//    Method makes emergency dial request
// -----------------------------------------------------------------------------
//
EXPORT_C void CPECallHandling::DialEmergencyCall( const TPEPhoneNumber& aEmergencyNumber )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DialEmergencyCall" );
    
    SendMessage( MEngineMonitor::EPEMessageInitiatedEmergencyCall );
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DialEmergencyCall start emergency dialing" );
    CPESingleCall* callData = iCallArrayOwner->GetCallObject( KPEEmergencyCallId );
    if ( callData )
        {
        callData->DialEmergency( aEmergencyNumber );   
        }
    }

// -----------------------------------------------------------------------------
// CPECallHandling::GetCallTerminatedDiagnostics
// Returns call terminated diagnostics of a call
// Method returns valid info only after call state has changed to Idle.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::GetCallTerminatedDiagnostics( 
        TName& /*aCallName*/ ) const
    {
        
    TInt diagnosticsInfo = 0/*iCustomAPI.GetDiagnosticInfo( aCallName )*/;
    TEFLOGSTRING2( KTAMESIN, 
            "CALL CPECallHandling::GetCallTerminatedDiagnostics: RMmCustomAPI::GetDiagnosticInfo, diagnosticInfo: %d", 
            diagnosticsInfo );
    return diagnosticsInfo;        
    }

// -----------------------------------------------------------------------------
// CPECallHandling::CreateConferenceCallL
// -----------------------------------------------------------------------------
//
void CPECallHandling::CreateConferenceCallL( 
    CPESingleCall& aCall1, 
    CPESingleCall& aCall2 )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::CreateConferenceCallL start" );
    if( !iConferenceCall )
        {
        iConferenceCall = CPEConferenceCall::NewL( *this, iConvergedCallEngine );
        iConferenceCall->SetCallId( KPEConferenceCallID );
        iConferenceCall->AddCallL( aCall1.Call() );
        iConferenceCall->AddCallL( aCall2.Call() );
        InitialiseConferenceCallInfo( aCall1.Call(), aCall2.Call() );
        }
    else
        {
        TEFLOGSTRING( KTAERROR, "CALL CPECallHandling::CreateConferenceCallL already exist" );
        User::Leave( KErrAlreadyExists );
        }

    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::CreateConferenceCallL end" );
    }


// -----------------------------------------------------------------------------
// CPECallHandling::UpdateSaSetting
// -----------------------------------------------------------------------------
//
void CPECallHandling::UpdateSaSetting()
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::UpdateSaSetting start" );
    
    TUnconditionalCFStatus status( KCFIndicatorUnknown );
    TSelectedLine line( ENotSupportedLine );
    CCCECallParameters::TCCELineType lineType( CCCECallParameters::ECCELineTypePrimary );
    CPESingleCall* call( NULL );
    TCallDivertNotifySetting notifySet;
    notifySet.Initialize();
    notifySet.iCfActivated = ETrue;
    
    // Get dialing call object
    call = iCallArrayOwner->CallPointerByState( EPEStateDialing );
     
    // Determinate als support and used line
    lineType = iModel.DataStore()->ALSLine();
    if( iModel.DataStore()->ALSLineSupport() )
        {
        if( CCCECallParameters::ECCELineTypePrimary == lineType )
            {
            line = EPrimaryLine;
            }
        else
            {
            line = EAuxiliaryLine;
            }
        }
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::UpdateSaSetting line %d", line );
    
    // Determinate basic service code
    notifySet.iBasicServiceCode = DefineDivertBsc( lineType, call  ); 
    
    // Create phonesetting connection
    if( !iPsetSAObserver )
        {
        TRAPD( errorCode, iPsetSAObserver = CPsetSAObserver::NewL() );
        if( errorCode != KErrNone )
            {
            TEFLOGSTRING2( 
               KTAERROR, 
               "CALL CPECallHandling::UpdateSaSetting FAIL with error %d", 
               errorCode );
            return;
            }
        }
        
    // Get diver status
    TInt error = iPsetSAObserver->GetCurrentDivertStatus( status );
    if ( error != KErrNone )
        {
        status = KCFNoCallsForwarded;
        }
    notifySet.iPreviousCfStatus = status;
    
    // Update dovert indicator.
    iPsetSAObserver->NotifyDivertChange( 
        line, 
        notifySet,
        0 );
        
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::UpdateSaSetting end" );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::DefineDivertBsc
// -----------------------------------------------------------------------------
//  
TInt CPECallHandling::DefineDivertBsc( CCCECallParameters::TCCELineType aLineType, 
                                       CPESingleCall* call )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DefineDivertBsc start" );
    TInt bsc( EAllTele );
    
    if ( !call )
        {
        TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DefineDivertBsc EUnknown" );
        bsc = EUnknown;
        return bsc;
        }
    else if ( call->Call().Parameters().CallType() == CCPCall::ECallTypeVideo  )
        {
        // If initiated call is video set bsc as ESyncData.
        TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DefineDivertBsc ESyncData" );
        bsc = ESyncData;
        return bsc;
        }

    switch ( aLineType )
        {
        case CCCECallParameters::ECCELineTypePrimary:
            // Call is done using line 1.
            TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DefineDivertBsc EAllTele" );
            bsc = EAllTele;  
            break;
        case CCCECallParameters::ECCELineTypeAux:
            // Call is done using line 2.
            TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DefineDivertBsc EAltTele" );
            bsc = EAltTele;  
            break;
        default:
            // Defauld case no alternate line service.
            TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DefineDivertBsc EAllTele default" );
            bsc = EAllTele;  
            break;
        }
        
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::DefineDivertBsc end, bsc = %d", bsc );
    return bsc;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::ReplaceActive
// Terminates an active call and answer waiting call
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::ReplaceActive()
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::ReplaceActive");
    TInt errorCode( ECCPErrorNotFound );
    TInt callIdConnected = KPECallIdNotUsed;
         
    CPESingleCall* replacingcall = iCallArrayOwner->CallPointerByState( EPEStateRinging );

    if( iConferenceCall && iConferenceCall->GetCallState() == EPEStateConnectedConference )
        {
        callIdConnected = iConferenceCall->GetCallId(); 
        }
    else
        {
        CPESingleCall* connectedcall = iCallArrayOwner->CallPointerByState( EPEStateConnected );
        if ( connectedcall )
            {
            callIdConnected = connectedcall->GetCallId();
            }
        }
    if ( callIdConnected != KPECallIdNotUsed && replacingcall ) 
        {
        if( CallIdCheck::IsConference( callIdConnected ) || CallIdCheck::IsVoice( callIdConnected ) )
            {
            iReplaceActive = ETrue;
            errorCode = HangUp( callIdConnected , ETPEHangUpNotResumeHeldCall );
            TEFLOGSTRING2( KTAMESINT, "CALL CPECallHandling::ReplaceActive HangUp error %d", errorCode );
            if ( !CallIdCheck::IsConference( callIdConnected ) && errorCode == KErrNone ) // Conference replace when idle
                {
                errorCode = AnswerCall();
                iReplaceActive = EFalse;
                TEFLOGSTRING2( KTAMESINT, "CALL CPECallHandling::ReplaceActive AnswerCall error %d", errorCode );
                }
            else if ( errorCode != KErrNone )
                {
                iReplaceActive = EFalse;
                }
            }
        else
            {
            errorCode = iVideoCallHandling->ReplaceActive( callIdConnected );
            TEFLOGSTRING2( KTAMESINT, "CALL CPECallHandling::ReplaceActive DataCallHandling::ReplaceActive error %d", errorCode );
            }
        }
    else
        {
        TEFLOGSTRING( KTAERROR, "CALL CPECALLHANDLING::REPLACEACTIVE ! CALL OBJECT NOT FOUND" );
        }

    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::AcceptUnattendedTransfer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::AcceptUnattendedTransfer()
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::AcceptUnattendedTransfer" );
    
    TInt callId = iModel.DataStore()->CallId();
    CPESingleCall* call = iCallArrayOwner->GetCallObject( callId );
    __ASSERT_DEBUG( NULL != call, Panic( EPEPanicIllegalCommand ) );
    if ( NULL != call )
        {
        return call->AcceptUnattendedTransfer();
        }
    else
        {
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CPECallHandling::RejectUnattendedTransfer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::RejectUnattendedTransfer()
    {
    TEFLOGSTRING( KTAMESINT, "CALL CPECallHandling::RejectUnattendedTransfer" );
    
    TInt callId = iModel.DataStore()->CallId();
    CPESingleCall* call = iCallArrayOwner->GetCallObject( callId );
    __ASSERT_DEBUG( NULL != call, Panic( EPEPanicIllegalCommand ) );
    if ( NULL != call )
        {
        return call->RejectUnattendedTransfer();
        }
    else
        {
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CPECallHandling::DoUnattendedTransfer
// Does unattended transfer request for the connected VoIP call
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::DoUnattendedTransfer( const TDesC& aTransferTarget )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::DoUnattendedTransfer" );
    CPESingleCall* callData;
    TInt callIndex;
    TInt errorCode( ECCPErrorNotAllowed );
    callData = VoiceCallDataByState( EPEStateConnected, callIndex );  
   
    if( callData )
        { 
        errorCode = callData->UnattendedTransfer( aTransferTarget );
        }
       
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::ForwardCallToAddress
// Forwards call by user selected address
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPECallHandling::ForwardCallToAddress( TInt aIndex )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::ForwardCallToAddress" );
    CPESingleCall* callData;
    TInt callIndex;
    TInt errorCode( ECCPErrorNotAllowed );
    callData = VoiceCallDataByState( EPEStateDialing, callIndex );  
   
    if ( NULL == callData )
        {
        callData = VoiceCallDataByState( EPEStateConnecting, callIndex );
        }
    
    if ( NULL == callData )
        {
        callData = VoiceCallDataByState( EPEStateConnected, callIndex );
        }

    if( callData )
        { 
        errorCode = callData->ForwardCallToAddress( aIndex );
        }
       
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SecureSpecified
// Returns the call secure specified status
// -----------------------------------------------------------------------------
//
TBool CPECallHandling::SecureSpecified( const TInt aCallId ) const
    {
    CPESingleCall* call = iCallArrayOwner->GetCallObject( aCallId );
    
    __ASSERT_DEBUG( call, Panic( EPEPanicIndexOutOfRange ) );  
    TBool secureSpecified( ETrue );
    if( call )
        {
        secureSpecified = call->SecureSpecified();
        }

    TEFLOGSTRING3( 
        KTAINT, 
        "PE CPECallHandling::SecureSpecified securespecified:%d  aCallId:%d", 
        secureSpecified, aCallId );

    return secureSpecified;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::InitConferenceCall
// -----------------------------------------------------------------------------
//
void CPECallHandling::InitConferenceCall( MCCEConferenceCall& aConference )
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::InitConferenceCall start" );
    if( iConferenceCall )
        {
        TEFLOGSTRING( KTAINT, "CALL CPECallHandling::InitConferenceCall already exist" );
        delete iConferenceCall;
        iConferenceCall = NULL;
        }
    
    TRAPD( error, ( iConferenceCall = CPEConferenceCall::NewL( *this, aConference ) ) );
    if( error != KErrNone )
        {
        SendMessage( MEngineMonitor::EPEMessageConferenceErrorCurrentCallsToConference );
        RPointerArray<MCCECall> callArray; 
        TInt err = aConference.GetCallArray( callArray );
        for( TInt index = 0 ; index < callArray.Count() ; index++ )
            {
            TRAP_IGNORE( aConference.RemoveCallL( *callArray[index] ) );
            }
        aConference.Release();
        callArray.Close();
        TEFLOGSTRING( KTAERROR, "CALL CPECallHandling::InitConferenceCall FAIL" );
        }
    else
        {
        iConferenceCall->SetCallId( KPEConferenceCallID );
                
        RPointerArray<MCCECall> callArray;
        TInt err = aConference.GetCallArray( callArray );
        
        if( err == KErrNone && callArray.Count() >= 2 )
            {
            InitialiseConferenceCallInfo( *callArray[0], *callArray[1] );
            iConferenceCall->CallStateChanged( MCCEConferenceCallObserver::ECCEConferenceActive );
            }
        callArray.Close();
        }    
    
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::InitConferenceCall end" );
    }

// -----------------------------------------------------------------------------
// CPECallHandling::CallOwner
// -----------------------------------------------------------------------------
//
EXPORT_C MPECallOwner& CPECallHandling::CallOwner() const
    {
    return *iCallArrayOwner;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::InitialiseConferenceCallInfo
// -----------------------------------------------------------------------------
//
void CPECallHandling::InitialiseConferenceCallInfo( 
    MCCECall& aCall1, 
    MCCECall& aCall2 )
    {
    TTimeIntervalSeconds call1Duration, call2Duration;
    call1Duration = aCall1.CallDuration();
    call2Duration = aCall2.CallDuration();
    
    // Set conference call duration to match the longest running call.
    // If this is not done, zero call duration is shown in case the
    // conference call fails or user rejects it instantly
    // --- See CPEGsmMessageHandler::HandleConferenceIdleState how
    // the duration is handled in these scenarios.
    if ( call1Duration > call2Duration )
       {
       iModel.DataStore()->SetCallDuration( call1Duration.Int(), KPEConferenceCallID );
       }
    else
       {
       iModel.DataStore()->SetCallDuration( call2Duration.Int(), KPEConferenceCallID );
       }
       
    if ( aCall1.Parameters().CallType()  
        == CCPCall::ECallTypeCSVoice )
       {
       iModel.DataStore()->SetCallType( EPECallTypeCSVoice, KPEConferenceCallID );
       }
    else if ( aCall1.Parameters().CallType()  
        == CCPCall::ECallTypePS )
       {
       iModel.DataStore()->SetCallType( EPECallTypeVoIP, KPEConferenceCallID );
       }
    
    __ASSERT_DEBUG( aCall1.ServiceId() == aCall2.ServiceId(), 
        Panic( EPEPanicInvalidParameter ) );
    iModel.DataStore()->SetServiceId( KPEConferenceCallID, aCall1.ServiceId() );
    }
    
// -----------------------------------------------------------------------------
// CPECallHandling::HandleAutoResume
// -----------------------------------------------------------------------------
//
void CPECallHandling::HandleAutoResume()
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::HandleAutoResume" );
    if ( iConferenceCall && ( iConferenceCall->GetCallState() == EPEStateHeldConference ) )
        {
        // Check that no conference + single case
        CPESingleCall* callData = iCallArrayOwner->CallPointerByState( EPEStateConnected );
        if( !callData && iModel.DataStore()->ResumeHeldCall( KPEConferenceCallID ))
            {
            TEFLOGSTRING( KTAINT, "CALL CPECallHandling::HandleAutoResume conference" );
            iConferenceCall->Swap();
            }
        }
    else
        {
        CPESingleCall* callData = iCallArrayOwner->CallPointerByState( EPEStateHeld );
        // Check that no actice and held call, if waiting call gets idle
        CPESingleCall* connectedCallData = iCallArrayOwner->CallPointerByState( EPEStateConnected );
        if( callData && ( iModel.DataStore()->ResumeHeldCall( callData->GetCallId() ) )
                    && !connectedCallData )
            {
            TEFLOGSTRING( KTAINT, "CALL CPECallHandling::HandleAutoResume single" );
            callData->Resume();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPECallHandling::ReleaseCallObject
// -----------------------------------------------------------------------------
//
TInt CPECallHandling::ReleaseCallObject( const TInt aCallId  )
    {
    TInt error(KErrNone);
    TEFLOGSTRING2( KTAINT, 
            "CALL CPECallHandling::ReleaseCallObject ReleaseCallObject %d", aCallId );
    error = iCallArrayOwner->DeleteCallObject( aCallId );
    // Reset datastore to prevent invalid usage in next call
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::ReleaseCallObject, Reset CallInfo" );
    iModel.DataStore()->ResetCallInfo( aCallId );
    return error;
    }

// -----------------------------------------------------------------------------
// CPECallHandling::SetCallOrigin
// -----------------------------------------------------------------------------
//
void  CPECallHandling::SetCallOrigin( const TInt aCallId, const MCCECall& aCall ) const
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::SetCallOrigin" );
    
    //ToDo: casting not needed when CCE API is fixed.
    const CCCPCallParameters& parameters = aCall.Parameters();
    CCPCall::TCallType callType = parameters.CallType();
    if ( callType == CCPCall::ECallTypeCSVoice || callType == CCPCall::ECallTypeVideo )
        {
        const CCCECallParameters& params = static_cast<const CCCECallParameters&>( parameters );
                                                    
        if ( params.Origin() == CCCECallParameters::ECCECallOriginSAT )
            {
            iModel.DataStore()->SetCallOrigin( EPECallOriginSAT, aCallId );
            iModel.DataStore()->SetRemoteName( params.AlphaId(), aCallId );
            iModel.DataStore()->SetRemotePhoneNumber( KNullDesC(), aCallId );
            }                 
        }                
    }


// -----------------------------------------------------------------------------
// CPECallHandling::UpdateColpNumber
// -----------------------------------------------------------------------------
//
TBool CPECallHandling::UpdateColpNumber( TInt aCallId, const MCCECall& aCall ) const
    {
    TEFLOGSTRING( KTAINT, "CALL CPECallHandling::UpdateColpNumber" );
    
    TBool updateDone( EFalse );
    TInt errorCode( KErrNone );
    TInt value( KPEMatchDefault );
    TPEMatcher matcher;       
    TPEPhoneNumber remoteNumber;
    
    MPEDataStore* dataStore = iModel.DataStore();
    
    if ( dataStore->RemoteColpNumber( aCallId ).Length() )
        {
        remoteNumber = dataStore->RemoteColpNumber( aCallId );
        }
    else
        {
        remoteNumber = dataStore->RemotePhoneNumber( aCallId );
        }    
        
    const TPEPhoneNumber& updatedNumber = aCall.RemoteParty();
    
    errorCode = iRepository->Get( KTelMatchDigits, value );
    if ( !errorCode == KErrNone )
        {
        TEFLOGSTRING( KTAOBJECT, "Reading KTelMatchDigits failed, use default value for matching");
        } 
    
    //check if remote number is different from dialled number
    if ( !matcher.numbersMatch( remoteNumber, updatedNumber, value ) )
        {
        //set COLP number        
        dataStore->SetRemoteColpNumber( updatedNumber, aCallId );
        
        TEFLOGSTRING3( KTAMESINT, 
                "CPECallHandling::UpdateColpNumber, colp number: '%S', call id: %d", 
                &updatedNumber, aCallId );
        updateDone = ETrue;
        }  
    
    return updateDone;
    }
       

//  End of File 
