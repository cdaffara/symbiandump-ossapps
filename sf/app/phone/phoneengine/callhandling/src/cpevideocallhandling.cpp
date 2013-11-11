/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains implementation of CPEVideoCallHandling class.
*
*/


// INCLUDE FILES
#include "cpevideocallhandling.h"
#include "cpesinglecall.h"
#include "pevirtualengine.h"
#include "cpecallarrayowner.h"
#include "cpecallarrayowner.h"
#include "cpegprstermination.h"

#include <mpephonemodelinternal.h>
#include <e32property.h>
#include <gsmerror.h>
#include <mpedatastore.h>
#include <networkhandlingdomainpskeys.h>
#include <pepanic.pan>
#include <pevirtualengine.h>
#include <talogger.h>
#include <ctsydomainpskeys.h>
#include <connect/sbdefs.h>
#include <ccce.h>
#include <mccecall.h>

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::CPEVideoCallHandling
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEVideoCallHandling::CPEVideoCallHandling(
        MPEMessageSender& aOwner,
        MPEPhoneModelInternal& aModel,        
        CCCE& aConvergedCallEngine,
        CPECallArrayOwner& aCallArrayOwner
        ) : iOwner( aOwner ),
            iModel( aModel ),
            iConvergedCallEngine( aConvergedCallEngine ),
            iDataCallName( KNullDesC ),
            iCallArrayOwner( aCallArrayOwner ),
            iDataStore( *( aModel.DataStore( ) ) ),
            iRestoreCauseVTCallReject( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEVideoCallHandling::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "VID CPEVideoCallHandling::ConstructL start." );

    iCallOpenParams = CCCECallParameters::NewL();
    TEFLOGSTRING( KTAOBJECT, "VID CPEVideoCallHandling::ConstructL complete." );
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEVideoCallHandling* CPEVideoCallHandling::NewL(
        MPEMessageSender& aOwner,
        MPEPhoneModelInternal& aModel,         
        CCCE& aConvergedCallEngine,
        CPECallArrayOwner& aCallArrayOwner )
    {
    TEFLOGSTRING( KTAOBJECT, "VID CPEVideoCallHandling::NewL start." );
    CPEVideoCallHandling* self = new ( ELeave ) CPEVideoCallHandling( aOwner, 
                                                                      aModel, 
                                                                      aConvergedCallEngine,
                                                                      aCallArrayOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING( KTAOBJECT, "VID CPEVideoCallHandling::NewL complete." );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::~CPEVideoCallHandling()
// Destructor
// -----------------------------------------------------------------------------
//
CPEVideoCallHandling::~CPEVideoCallHandling()
    {
    TFLOGSTRING( "CallHandling: CPEVideoCallHandling::~CPEVideoCallHandling() Start");
    delete iGprsInterface;
    iGprsInterface = NULL;
    delete iCallOpenParams;
    TFLOGSTRING( "CallHandling: CPEVideoCallHandling::~CPEVideoCallHandling() complete");
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::AnswerCall
// Answers the incoming call
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::AnswerCall()
    {
    TEFLOGSTRING( KTAINT, "VID CPEVideoCallHandling::AnswerCall");
    TInt errorCode( KErrNotFound );
    CPESingleCall* dataCallData = VideoCallByState( EPEStateRinging );
    if ( dataCallData )
        {
        if ( GetNumberOfCalls() == 1 )
            {
            TEFLOGSTRING( KTAINT, "VID CPEVideoCallHandling::AnswerCall > CPEDataCall::Answer" );
            dataCallData->Answer();
            errorCode = KErrNone;
            }
        else
            {
            TEFLOGSTRING( KTAERROR, 
                    "VID CPEVIDEOCALLHANDLING::ANSWERCALL: NOT ALLOWED WITH EXISTING CALLS!" );
            errorCode = ECCPErrorNotAllowed;
            }
        }
    else
        {
        TEFLOGSTRING( KTAERROR, 
                "VID CPEVIDEOCALLHANDLING::ANSWERCALL: NO RINGING CALLS FOUND!" );
        }

    return errorCode;
    }
    
// -----------------------------------------------------------------------------
// CPEVideoCallHandling::VideoCallByState
// Finds a data call object that is in specified  state
// -----------------------------------------------------------------------------
//  
CPESingleCall* CPEVideoCallHandling::VideoCallByState( 
        const TPEState aState ) const
    {
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::DataCallByState" );
    CPESingleCall* returnValue( NULL );
    CPESingleCall* callData;
        
    for ( TInt callId = KPEDataCallIdBase; 
          callId < ( KPEDataCallIdBase + KPEMaximumNumberOfDataCalls ); 
          callId++ )
        {
        callData = iCallArrayOwner.GetCallObject( callId );
        if ( callData 
             && ( callData->GetCallState() == aState ) )
            {
            returnValue = callData;
            TEFLOGSTRING2( KTAINT, 
                "CALL CPEVideoCallHandling::DataCallByState %d", callId );
            break;
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::CallTerminatedError
// Handles RemoteTerminated. 
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::CallTerminatedError( const TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, 
            "VID CPEVideoCallHandling::CallTerminatedError, aCallId: %d",
            aCallId );
    
    TInt errorCode( KErrNone );    
    CPESingleCall* dataCall = iCallArrayOwner.GetCallObject( aCallId );
    if ( dataCall )
        {
        dataCall->GetErrorCode( errorCode );
        }
    
    TEFLOGSTRING2( KTAINT, 
                "VID CPEVideoCallHandling::CallTerminatedError, errorCode: %d",
                errorCode );
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::DataPortInUse
// Tells if the call has reserved serial communications data port for use
// The default value is EFalse, i.e. the data port has not been reserved for
// use. The data port is recovered when no longer needed.
// -----------------------------------------------------------------------------
// 
TBool CPEVideoCallHandling::DataPortInUse(
        TInt aCallId ) const
    {
    TBool dataPortInUse( EFalse );
    TPEState state;
    
    if ( CallIdCheck::IsVideo( aCallId ) )
        {
        CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
        if( callData )
            {
            state = callData->GetCallState();
            if( state == EPEStateConnected )
                {
                dataPortInUse = ETrue;
                }
            }
        }
    return dataPortInUse;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::DialCall
// Creates dialing request to the Multimode ETel. 
// This request is for creating a new multimedia (=data) call
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::DialCall(
        const TPEPhoneNumber& aNumber,
        TInt& aCallId )
    {
    TEFLOGSTRING( KTAINT, "VID CPEVideoCallHandling::DialCall" );
    TInt errorCode( KErrNone );
    TInt networkMode( ENWNetworkModeUnknown );
    CPESingleCall* callData( NULL );
    
    RProperty::Get( KPSUidNetworkInfo, KNWTelephonyNetworkMode, networkMode );   

    TInt restoreValue;
    RProperty::Get( KUidSystemCategory, conn::KUidBackupRestoreKey, restoreValue );       
    
    if ( networkMode == ENWNetworkModeWcdma && 
        !( restoreValue & ( conn::EBURRestoreFull | conn::EBURRestorePartial )) )
        {
        TRAP( errorCode, callData = OpenNewCallL( aNumber ) );
        if( errorCode == KErrNone )
            {
            TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::DialCall > Dial" );
            errorCode = callData->Dial( aNumber );
            if ( errorCode != KErrNone )
                {
                // Dial failed: clean up
                ReleaseCallObject( callData->GetCallId() );
                TEFLOGSTRING2( KTAERROR, 
                        "VID CALL CPEVideoCallHandling::DialCall ! DIAL FAILED: MAY NOT PROCEED! %d",
                        errorCode );
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
        TEFLOGSTRING( KTAERROR, 
            "VID CPEVideoCallHandling::DialCall: Attempting video call in 2G network!" );
        errorCode = ( networkMode != ENWNetworkModeWcdma ) ? 
                      ECCPErrorVideoCallNotSupportedByNetwork : 
                      ECCPErrorVideoCallNotAllowedDuringRestore;
        }    
        
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::HangUp
// terminates an ongoing call
// Method checks that call id is valid number and
// hangups conference or data call or reroutes call to base class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::HangUp( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "VID CPEVideoCallHandling::HangUp" );
    TInt errorCode( KErrNotFound );
    CPESingleCall* videoCall = iCallArrayOwner.GetCallObject( aCallId );
    
    if ( videoCall && CallIdCheck::IsVideo( aCallId ) ) 
        {

        errorCode = videoCall->HangUp();
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::InitCall
// Handles the initializing of a single data/video call.
// -----------------------------------------------------------------------------
//
void CPEVideoCallHandling::InitCall( MCCECall& aCall )
    {
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::InitCall" );
    CPESingleCall* callData( NULL );
      TRAP_IGNORE( callData = iCallArrayOwner.CreateDataCallL( *this ) );

    if ( callData )
        {
        iModel.DataStore()->SetPhoneNumber( KNullDesC() );
        iModel.DataStore()->SetServiceId( callData->GetCallId(), aCall.ServiceId() );
        callData->SetCall( aCall );
        iModel.DataStore()->SetCallSecureStatus( callData->IsSecureCall(), callData->GetCallId() );
        iModel.DataStore()->SetSecureSpecified( callData->SecureSpecified() );
        
        RMobileCall::TMobileCallInfoV3 callInfo;
        GetCallInfo(callInfo, callData->GetCallId());
        iModel.DataStore()->SetIncomingCallForwarded( callInfo.iForwarded, callData->GetCallId() );
        
        const CCCPCallParameters& callParameters = aCall.Parameters();
        switch ( callParameters.CallType() )
            {
            case CCPCall::ECallTypeCSVoice:
                {
                iModel.DataStore()->SetCallType( EPECallTypeCSVoice, callData->GetCallId() );
                }
                break;
            case CCPCall::ECallTypeVideo:
                {
                iModel.DataStore()->SetCallType( EPECallTypeVideo, callData->GetCallId() );
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
        }
    else
        {
        Panic( EPEPanicNoFreeCalls );
        }
    }
 
// -----------------------------------------------------------------------------
// CPEVideoCallHandling::IsMultimedia
// Checks if the data call is a multimedia call. Requires that the data call has
// already been opened.
// -----------------------------------------------------------------------------
//
TBool CPEVideoCallHandling::IsMultimedia( TInt aCallId ) const
    {
    TEFLOGSTRING2( KTAINT, "VID CPEVideoCallHandling::IsMultimedia %d", aCallId );
    
    TBool multimedia( EFalse );
    CPESingleCall* videoCall = iCallArrayOwner.GetCallObject( aCallId );
    
    if ( videoCall && CallIdCheck::IsVideo( aCallId ) 
         && videoCall->Call().Parameters().CallType()  
                == CCPCall::ECallTypeVideo )
        {
        multimedia = ETrue;
        }
        
    return multimedia;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::NotifySystemCallStateChanged
// Notifies system for changes to the call state.
// -----------------------------------------------------------------------------
//
void CPEVideoCallHandling::NotifySystemCallStateChanged( 
        MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,  // The enumeration number of the message
        TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "VID CPEVideoCallHandling::NotifySystemCallStateChanged %d", aCallId );
    // this needs to be checked because this function is called for all data calls
    if ( IsMultimedia( aCallId ) )
        {
        if ( aMessage == MEngineMonitor::EPEMessageIncoming ||
           aMessage == MEngineMonitor::EPEMessageDialing )
            {
            TInt updateError = RProperty::Set( KPSUidCtsyCallInformation, 
                KCTsyCallType, EPSCTsyCallTypeH324Multimedia );
            TEFLOGSTRING2( updateError?KTAERROR:KTAMESOUT, 
                "VID CPEVideoCallHandling::NotifySystemCallStateChanged request RProperty::Set, EPSCTsyCallTypeH324Multimedia, updateError: ",
                updateError );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::SendMessage
// Overrides CPECallHandling::SendMessage to provide internal handling for gsm events
// and common events related to multimedia calls.
// -----------------------------------------------------------------------------
//
void CPEVideoCallHandling::SendMessage(
        MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
        TInt aCallId )
    {
    TEFLOGSTRING3( KTAINT, 
        "VID CPEVideoCallHandling::SendMessage, message id: %d, call id: %d", 
        aMessage, 
        aCallId );
        
    TInt error( KErrNone );
    CPESingleCall* videoCall = iCallArrayOwner.GetCallObject( aCallId );
    CPESingleCall* connectedVideoCall = VideoCallByState( EPEStateConnected );

    if ( IsMultimedia( aCallId ) )
        {
        switch ( aMessage )
            {
            case MEngineMonitor::EPEMessageIncoming:
            case MEngineMonitor::EPEMessageDialing:
                {
                TInt restoreValue;
                RProperty::Get( KUidSystemCategory, conn::KUidBackupRestoreKey, restoreValue );
                if( ( restoreValue & ( conn::EBURRestoreFull | conn::EBURRestorePartial ) ) )
                    {
                    // First phase VT and restore: Check if there is a arriving VT 
                    // call and Restore ongoing.
                    TEFLOGSTRING( KTAINT, 
                        "VID CPEVideoCallHandling::SendMessage > HangUp restore active " );
                    iRestoreCauseVTCallReject = ETrue;
                    if( videoCall )
                        {
                        videoCall->HangUp();
                        }
                    aMessage = MEngineMonitor::EPEMessageDoNotSendMe;
                    }
                break;
                }
            case MEngineMonitor::EPEMessageDisconnecting:
                {
                if( iRestoreCauseVTCallReject )
                    {
                    // Second phase VT and restore: Check if there is a rejected arriving VT call 
                    // and Restore ongoing, then don't notify observers.
                    iRestoreCauseVTCallReject = EFalse;
                    TEFLOGSTRING( KTAINT, 
                        "VID CPEVideoCallHandling::SendMessage Second phase Restore and VT call" );
                    aMessage = MEngineMonitor::EPEMessageDoNotSendMe;
                    }
                break;
                }
            case MEngineMonitor::EPEMessageIdle:
                {
                iRestoreCauseVTCallReject = EFalse;
                // Check reject video cace, if there is connect VT call don't disconnect
                if( !connectedVideoCall )
                    {
                    if ( CallTerminatedError( aCallId ) == KErrGsmCCIncompatibleDestination )
                        {
                        error = KErrGsmCCIncompatibleDestination;
                        }
                    }
                break;
                }
            default:
                {
                TEFLOGSTRING2( 
                    KTAINT, 
                    "VID CPEVideoCallHandling::SendMessage: multimedia call, message: %d", 
                    aMessage );
                break;
                }
            } 
        }
    else
        {
        TEFLOGSTRING2( 
            KTAINT, 
            "VID CPEVideoCallHandling::SendMessage, isMultimedia: %d", 
            IsMultimedia( aCallId ) );
        }
    if ( error == KErrNone )
        {
        iOwner.SendMessage( aMessage, aCallId );
        }
    else
        {
        SendErrorMessage( aCallId, error );
        }
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::VideoCallInState
// Checks if there are any data calls in the specified state
// -----------------------------------------------------------------------------
//
TBool CPEVideoCallHandling::VideoCallInState( const TPEState& aState ) const
    {
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::DataCallInState" );
    TBool returnValue( EFalse );
    if( VideoCallByState( aState ) )
        {
        returnValue = ETrue;
        }
    return returnValue;
    }


// -----------------------------------------------------------------------------
// CPEVideoCallHandling::IsPhoneNumberAvailable
// -----------------------------------------------------------------------------
//
TBool CPEVideoCallHandling::IsPhoneNumberAvailable( const TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, 
        "VID CPEVideoCallHandling::IsPhoneNumberAvailable, aCallId: %d"
        , aCallId );
    TBool phoneNumberAvailable ( EFalse );
        
    if ( iDataStore.CallDirection( aCallId ) == RMobileCall::EMobileOriginated )
        {
        phoneNumberAvailable = ETrue;   
        }
    else
        {
        RMobileCall::TMobileCallRemoteIdentityStatus remoteIdentity;
        remoteIdentity = iDataStore.RemoteIdentity( aCallId );
        TEFLOGSTRING2( KTAINT, 
            "VID CPEVideoCallHandling::IsPhoneNumberAvailable, remoteIdentity: %d", 
            remoteIdentity);
        phoneNumberAvailable = ( remoteIdentity == RMobileCall::ERemoteIdentityAvailable );     
        }
    TEFLOGSTRING2( KTAINT, 
        "VID CPEVideoCallHandling::IsPhoneNumberAvailable, isAvailable: %d", 
        phoneNumberAvailable );
    return phoneNumberAvailable;
    }
    
// -----------------------------------------------------------------------------
// CPEVideoCallHandling::VideoCallByState
// returns CPEVoiceCall video call object by State
// -----------------------------------------------------------------------------
//
CPESingleCall* CPEVideoCallHandling::VideoCallByState( TPEState aState )
    {
    TEFLOGSTRING2( KTAINT, 
            "CPEVideoCallHandling::VideoCallByState, aState: %d", aState );    
    TPEState state;
    CPESingleCall* returnValue( NULL );

    for ( TInt callId = KPEDataCallIdBase; 
          callId < ( KPEDataCallIdBase + KPEMaximumNumberOfDataCalls );
          callId++ )
        {        
        CPESingleCall* callData = iCallArrayOwner.GetCallObject( callId );
        if( callData )
            {
            state = callData->GetCallState();

            if ( state == aState )
                {
                returnValue = callData;
                break;
                }
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// Replace active video call with a waiting video call
// NOT SUPPORTED AS OF 5.1
// -----------------------------------------------------------------------------
//    
TInt CPEVideoCallHandling::ReplaceActive( TInt /*aCallId*/  )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::SetCallParams
//
// -----------------------------------------------------------------------------
//
void CPEVideoCallHandling::SetCallParams( TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, 
            "VID CCPEVideoCallHandling::SetCallParams, aCallId: %d", aCallId );
    iCallOpenParams->SetLineType( CCCECallParameters::ECCELineTypePrimary );

    switch ( iModel.DataStore()->CallTypeCommand() )
        {
        case EPECallTypeVideo:
            {
            iCallOpenParams->SetCallType( CCPCall::ECallTypeVideo );
            iModel.DataStore()->SetServiceIdCommand( 1 );
            iModel.DataStore()->SetCallType( EPECallTypeVideo, aCallId );
            }
            break;
        default:
            __ASSERT_DEBUG( EFalse, Panic( EPEPanicIndexOutOfRange ));
            iCallOpenParams->SetCallType( CCPCall::ECallTypeVideo );
            iModel.DataStore()->SetServiceIdCommand( 1 );
            iModel.DataStore()->SetCallType( EPECallTypeVideo, aCallId );
            break;
        } 
    iCallOpenParams->SetServiceId( iModel.DataStore()->ServiceIdCommand() );
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::OpenNewCallL
// returns ETrue if hangup active
// -----------------------------------------------------------------------------
//
CPESingleCall* CPEVideoCallHandling::OpenNewCallL( const TPEPhoneNumber& aNumber )
    {
    TEFLOGSTRING( KTAINT, "VID CPEVideoCallHandling::OpenNewCallL");
    TInt errorCode( KErrNone );
    CPESingleCall* callData( NULL );
    MCCECall* cceCall( NULL );
    
    callData = iCallArrayOwner.CreateDataCallL( *this );
    SetCallParams( callData->GetCallId() );
    
    TEFLOGSTRING( KTAINT, 
        "VID CPEVideoCallHandling::OpenNewCallL > CCCE::OpenNewCallL");
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
        }
    else
        {
        // Open new call failed
        ReleaseCallObject( callData->GetCallId() );
        TEFLOGSTRING2( KTAERROR, 
                "VID CPEVideoCallHandling::DIALCALL ! OPENNEWCALL FAILED: MAY NOT PROCEED! %d", 
                errorCode );
        User::Leave( errorCode );
        }
    return callData;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::GetCallState
// Returns call state information.
// -----------------------------------------------------------------------------
//
TPEState CPEVideoCallHandling::GetCallState( TInt aCallId  )
    {
    TEFLOGSTRING2( KTAINT, 
            "CALL CPEVideoCallHandling::GetCallState %d", aCallId );
    TPEState callState( EPEStateUnknown );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( callData )
        {
        callState = callData->GetCallState();
        }
    else
        {
        callState = EPEStateIdle;
        }
    TEFLOGSTRING2( KTAINT, 
            "CALL CPEVideoCallHandling::GetCallState, callState: %d", 
            callState );
    return callState;
    }
    
// -----------------------------------------------------------------------------
// CPEVideoCallHandling::VideoCallById
// returns CPESingleCall video call object by State
// -----------------------------------------------------------------------------
//
CPESingleCall* CPEVideoCallHandling::VideoCallById( TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "VID CPEVideoCallHandling::VideoCallById %d", aCallId );
    return iCallArrayOwner.GetCallObject( aCallId );
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::GetNumberOfCalls
// Returns number of data calls.
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::GetNumberOfCalls()
    {
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::GetNumberOfCalls" );
    // Count ongoing calls
    return iCallArrayOwner.ActiveCallCount();
    }
    

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::SendMessage
// Reroutes messages to the Phone Engine
// -----------------------------------------------------------------------------
//
void CPEVideoCallHandling::SendMessage( 
    MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
    const TName& aName )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPECallHandling::SendMessage, message id: %d", aMessage );
    CPESingleCall* videoCall = VideoCallByName( aName );
    if( videoCall )
        {
        SendMessage( aMessage, videoCall->GetCallId() );
        }
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::VideoCallByName
// returns data call object by name
// -----------------------------------------------------------------------------
//
CPESingleCall* CPEVideoCallHandling::VideoCallByName( const TName& aName )
    {       
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::DataCallByName" );
    CPESingleCall* returnValue( NULL );
    CPESingleCall* callData;
    for ( TInt callId = KPEDataCallIdBase; 
          callId < ( KPEDataCallIdBase + KPEMaximumNumberOfDataCalls ); 
          callId++ )
        {
        callData = iCallArrayOwner.GetCallObject( callId );

        if ( callData 
             && ( VideoCallName( callId ) == aName ) )
            {
            returnValue = callData;
            TEFLOGSTRING2( KTAINT, 
                "CALL CPEVideoCallHandling::DataCallByName %d", callId );
            break;
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::VideoCallName
// Finds data call name given the call id.
// -----------------------------------------------------------------------------
//  
TName& CPEVideoCallHandling::VideoCallName( const TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::DataCallName" );
    
    iDataCallName = KNullDesC;
    TInt errorCode( KErrNone );

    if ( CallIdCheck::IsVideo( aCallId ) )
        {
        RMobileCall::TMobileCallInfoV3* dataCallInfo = NULL;
        dataCallInfo = new RMobileCall::TMobileCallInfoV3;
        
        if ( dataCallInfo )
            {
            errorCode = GetCallInfo( *dataCallInfo, aCallId );
            TEFLOGSTRING2( KTAINT, 
                "CALL CPEVideoCallHandling::DataCallName > GetCallInfo, error code: %d", 
                errorCode );
 
            if ( errorCode == KErrNone )
                {
                iDataCallName = dataCallInfo->iCallName;
                }
            }
        else
            {
            errorCode = KErrNoMemory;
            TEFLOGSTRING( KTAINT, 
                "CALL CPEVideoCallHandling::DataCallName, error code = KErrNoMemory" );
            }
        delete dataCallInfo;
        }
    return iDataCallName;
    }


// -----------------------------------------------------------------------------
// CPEVideoCallHandling::GetCallInfo
// Returns data call information.
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::GetCallInfo( 
        RMobileCall::TMobileCallInfoV3& aCallInfo,
        TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPEVideoCallHandling::GetCallInfo %d", aCallId );
    CPESingleCall* videoCall( NULL );
    TInt errorCode( KErrNotFound );
    videoCall = iCallArrayOwner.GetCallObject( aCallId );
    if ( videoCall && CallIdCheck::IsVideo( aCallId ) )
        {
        errorCode = videoCall->GetCallInfo( aCallInfo );
        errorCode = KErrNone;
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic( EPEPanicIllegalCommand) );
        }
    return errorCode;
    }
 
// -----------------------------------------------------------------------------
// CPEVideoCallHandling::SendErrorMessage
// Reroutes error code to Call Handling interface.
// -----------------------------------------------------------------------------
//
void CPEVideoCallHandling::SendErrorMessage(
    const TInt aCallId,
    const TInt aErrorCode )
    {
    TEFLOGSTRING3( KTAINT, 
        "CALL CPEVideoCallHandling::SendErrorMessage, error code: %d, error cause: %d, call id: %d", 
        aErrorCode, 
        aCallId );
    iOwner.SendErrorMessage( aCallId, aErrorCode );
    }


// -----------------------------------------------------------------------------
// CPEVideoCallHandling::HandleInternalMessage
// Reroutes messages to the Phone Engine
// -----------------------------------------------------------------------------
//
void CPEVideoCallHandling::HandleInternalMessage( TInt aMessage )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPEVideoCallHandling::HandleInternalMessage, message: %d", aMessage );
    iOwner.HandleInternalMessage( aMessage );
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::ConnectedCalls
// Indicates if there are any connected calls.
// -----------------------------------------------------------------------------
//
TBool CPEVideoCallHandling::ConnectedCalls()
    {
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::ConnectedCalls" );
    TBool returnValue( EFalse );
    if( VideoCallByState( EPEStateConnected ) )
        {
        returnValue = ETrue;
        }
    return returnValue ;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::RejectCall
// Rejects the incoming call.
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::RejectCall()
    {
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::RejectCall" );
    TInt errorCode( KErrNotFound );
    
    CPESingleCall* videoCall = VideoCallByState( EPEStateRinging );
    if( videoCall )
        {
        videoCall->HangUp();
        errorCode = KErrNone;
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::ReleaseAll
// Releases all ongoing calls.
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::ReleaseAll()
    {
    CPESingleCall* callData;
    TInt errorCode( KErrNotFound );
    TPEState callState;
    
    TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::ReleaseAll " );
    
    for ( TInt callId = KPEDataCallIdBase; 
          callId < ( KPEDataCallIdBase + KPEMaximumNumberOfDataCalls ); 
          callId++ )
        {
        callData = iCallArrayOwner.GetCallObject( callId );
        if ( callData )
            {
            callState = callData->GetCallState();
            if ( callState != EPEStateIdle && callState != EPEStateRinging )
                {
                TEFLOGSTRING2( KTAINT, 
                    "CALL CPEVideoCallHandling::ReleaseAll, call id: %d", 
                    callId );
                HangUp( callId );
                errorCode = KErrNone;
                }
            }
        }
        
    TEFLOGSTRING2( KTAINT, 
        "CALL CPEVideoCallHandling::ReleaseAll, error code: %d", 
        errorCode );
            
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::TerminateAllConnections
// Terminates all ringing data calls, connected data calls and packet data
// connections.
// -----------------------------------------------------------------------------
//
TInt CPEVideoCallHandling::TerminateAllConnections()
    {
    TEFLOGSTRING( KTAMESINT, 
        "CALL CPEVideoCallHandling::TerminateAllConnections");    
    // Terminate GPRS connection.
    // If GPRS connections are being terminated, 
    // then we do not start again.
    TBool canBeDeleted = ETrue;
    if ( iGprsInterface )
        {
        canBeDeleted = !iGprsInterface->IsTerminating();
        }

    TInt error( KErrNone );
    if ( canBeDeleted )
        {
        delete iGprsInterface;
        iGprsInterface = NULL;
        TRAP( error,
            iGprsInterface = new (ELeave) CPEGprsTermination;
            iGprsInterface->StartL() );
        }
    // Hangup Data calls
    ReleaseAll();
    
    return error;
    }

// -----------------------------------------------------------------------------
// CPEVideoCallHandling::GetCallDuration
// Returns voice and conference (master call) call duration.
// Method checks that call id is valid number and gets
// call duration from conference or data call object or from
// calls base class for voice call duration.
// -----------------------------------------------------------------------------
//
 TInt CPEVideoCallHandling::GetCallDuration( 
         TTimeIntervalSeconds& aDuration, 
         TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPEVideoCallHandling::GetCallDuration %d", aCallId ); 
    TInt errorCode( KErrNotFound );
    CPESingleCall* callData( NULL );
    callData = iCallArrayOwner.GetCallObject( aCallId );
    
    if ( callData && CallIdCheck::IsVideo( aCallId ) )
        {
        callData->GetCallDuration( aDuration );
        errorCode = KErrNone;
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic( EPEPanicIllegalCommand) );
        }

    return errorCode;
    }
 
 // -----------------------------------------------------------------------------
 // CPEVideoCallHandling::ReleaseCallObject
 // -----------------------------------------------------------------------------
 //
 TInt CPEVideoCallHandling::ReleaseCallObject( const TInt aCallId  )
     {
     TInt error(KErrNone);
     TEFLOGSTRING2( KTAINT, 
             "CALL CPEVideoCallHandling::ReleaseCallObject ReleaseCallObject %d", aCallId );
     error = iCallArrayOwner.DeleteCallObject( aCallId );
     // Reset datastore to prevent invalid usage in next call
     TEFLOGSTRING( KTAINT, "CALL CPEVideoCallHandling::ReleaseCallObject, Reset CallInfo" );
     iModel.DataStore()->ResetCallInfo( aCallId );
     return error;
     }
 
//  End of File  
