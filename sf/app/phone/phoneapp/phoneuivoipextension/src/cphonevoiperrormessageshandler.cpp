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
* Description: Implementation of CPhoneVoIPErrorMessagesHandler class.
*
*/

// INCLUDES

#include <ccpdefs.h>
#include <mpeengineinfo.h>
#include <StringLoader.h>
#include <spsettings.h>
#include <spentry.h>
#include <telephonydomainpskeys.h>
#include <aknnotedialog.h>
#include "cphonevoiperrormessageshandler.h"
#include "cphonemainerrormessageshandler.h"
#include "cphoneerrormessageshandler.h"
#include "cphonestatehandle.h"
#include "phonerssvoip.h"
#include "phonelogger.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparaminteger.h"
#include "cphonemainresourceresolver.h"
#include "phoneui.pan"
#include "cphonepubsubproxy.h"
#include "tphonecmdparamboolean.h"
#include "cphonestatemachinevoip.h"
#include "cphonecustomizationvoip.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneVoIPErrorMessagesHandler::CPhoneVoIPErrorMessagesHandler(
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneStateMachine* aPhoneStateMachine ) :
    CPhoneErrorMessagesHandler( aViewCommandHandle, aPhoneStateMachine )
    {
    static_cast<CPhoneStateMachineVoIP*>( iStateMachine )->
        SetVoipErrorMessageHandler( *this );
    }

// -----------------------------------------------------------
// CPhoneErrorMessagesHandler::~CPhoneErrorMessagesHandler()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneVoIPErrorMessagesHandler::~CPhoneVoIPErrorMessagesHandler()
    {
     
    }

// -----------------------------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneVoIPErrorMessagesHandler::ConstructL()
    {
    // Register error handler
    CPhoneMainErrorMessagesHandler::Instance()->
        RegisterErrorMessagesHandler( this );
    }

// -----------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::NewL()
// Constructor
// -----------------------------------------------------------
//
CPhoneVoIPErrorMessagesHandler* CPhoneVoIPErrorMessagesHandler::NewL(
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneStateMachine* aPhoneStateMachine )
    {
    CPhoneVoIPErrorMessagesHandler* self =
        new( ELeave ) CPhoneVoIPErrorMessagesHandler(
            aViewCommandHandle,
            aPhoneStateMachine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::ShowErrorSpecificNoteL
// -----------------------------------------------------------
//
void CPhoneVoIPErrorMessagesHandler::ShowErrorSpecificNoteL( 
    const TPEErrorInfo& aErrorInfo )
    {
    __LOGMETHODSTARTEND(PhoneUIVoIPExtension, "CPhoneVoipErrorMessagesHandler::ShowErrorSpecificNoteL()");

    __PHONELOG1( EBasic, PhoneUIVoIPExtension,
            "CPhoneVoIPErrorMessagesHandler::ShowErrorSpecificNoteL - aErrorInfo.iErrorCode =%d ",
            aErrorInfo.iErrorCode);

    if ( aErrorInfo.iErrorType == EPECcp && IsVoIPCall( aErrorInfo.iCallId ) )
        {
        switch( aErrorInfo.iErrorCode )
            {
            case ECCPErrorNotAcceptable:
            case ECCPErrorCCServiceNotImplemented:
                SendGlobalErrorNoteL( EPhoneVoIPNoAudioSupport );
                break;

            case ECCPLocalResumeFail:
                if ( HasSwapFailed() )
                    {
                    SendGlobalErrorNoteL( EPhoneVoIPSwapFail );
                    }
                else
                    {
                    HandleHoldErrorNotesL( aErrorInfo.iCallId, EFalse );
                    }
                break;
            case ECCPLocalHoldFail:
                if ( HasSwapFailed() )
                    {
                    SendGlobalErrorNoteL( EPhoneVoIPSwapFail );
                    }
                else
                    {
                    HandleHoldErrorNotesL( aErrorInfo.iCallId, ETrue );
                    }
                break;

            case ECCPServerFailure:
            case ECCPErrorNetworkBusy:
                SendGlobalErrorNoteL( EPhoneVoIPNoService );
                break;

            case ECCPErrorNotReached:
                SendGlobalErrorNoteL( EPhoneVoIPNotFound );
                break;
            case ECCPErrorCCIncompatibleDestination:
                SendGlobalErrorNoteL( EPhoneVoIPAddressNotInUse );
                break;

            case ECCPErrorTimedOut:
            case ECCPErrorCCNoRouteToDestination:
                SendGlobalErrorNoteL( EPhoneVoIPRequestTimeOut );
                break;

            case KErrArgument:
            case ECCPErrorInvalidPhoneNumber:
            case ECCPErrorInvalidURI:
                SendGlobalErrorNoteL( EPhoneVoIPInvalidRecipientAddress );
                break;

            case ECCPErrorBusy:
            case ECCPErrorCCCallRejected:
                SendGlobalErrorNoteL( EPhoneVoIPNumberBusy );
                break;

            case ECCPErrorMovedPermanently:
                SendGlobalErrorNoteL( EPhoneVoIPAddressNotInUse );
                break;

            case ECCPErrorCCUserNotResponding:
            case ECCPErrorNotResponding:

                SendGlobalErrorNoteL( EPhoneVoIPNoAnswer );
                break;
  
            case ECCPGlobalFailure:
                SendGlobalErrorNoteL( EPhoneVoIPErrorInConnection );
                break;

            case ECCPSecureCallFailed:    
            case ECCPErrorUnacceptableChannel:
                SendGlobalErrorNoteL( EPhoneSSNotificationTextVoipSipsFailed );
                break;

            case ECCPErrorNetworkOutOfOrder:
            case ECCPErrorMedia:
                SendGlobalErrorNoteL( EPhoneVoIPNetworkError );
                break;
                 
            case ECCPTransferFailed:
                iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveGlobalNote );
                SendGlobalErrorNoteL( EPhoneVoIPTransferFail );
                break;

            case ECCPErrorNoService:
                {
                TInt serviceID = iStateMachine->PhoneEngineInfo()->ServiceId(
                    aErrorInfo.iCallId );
                ShowErrorNoteWithServiceNameL( EPhoneVoIPServiceUnavailable,
                    serviceID );
                }
                break;
            default:
                CPhoneErrorMessagesHandler::ShowErrorSpecificNoteL( aErrorInfo );
                break;
            }
        }
    else if ( aErrorInfo.iErrorType == EPECch )
        {
        ShowRegistrationErrorNotesL( aErrorInfo.iErrorCode );   
        }
    else
        {
        CPhoneErrorMessagesHandler::ShowErrorSpecificNoteL( aErrorInfo );
        }
    }

// -----------------------------------------------------------
// CPhoneVoipErrorMessagesHandler::IsVoIPCall
// -----------------------------------------------------------
//
TBool CPhoneVoIPErrorMessagesHandler::IsVoIPCall( const TInt aCallId ) const
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, "CPhoneVoIPErrorMessagesHandler::IsVoipCall() ");

    if( aCallId == KErrNotFound )
        {
        // Illegal call id, check call type command
        return ( iStateMachine->PhoneEngineInfo()->CallTypeCommand()
            == EPECallTypeVoIP );
        }

    return ( iStateMachine->PhoneEngineInfo()
        ->CallType( aCallId )== EPECallTypeVoIP );
    }

// -----------------------------------------------------------
// CPhoneVoipErrorMessagesHandler::HandleHoldErrorNotesL
// -----------------------------------------------------------
//
void CPhoneVoIPErrorMessagesHandler::HandleHoldErrorNotesL( 
    const TInt aCallId, TBool aHold )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, "CPhoneVoIPErrorMessagesHandler::HandleHoldNoteL()");
    
    TPhoneCmdParamGlobalNote globalNoteParam;
    HBufC* text = HBufC::NewLC( KPhoneContactNameMaxLength );
    TPtr tmp = text->Des();

    // Should add name( or RemotePartyName or CompanyName) to Note, if someone found
    GetRemoteInfoDataL( aCallId, tmp );
    
    if ( text->Length() )
        {
        // RemotePartyName, CompanyName or RemoteName found
        // Resource text which will use found text will be loaded later
        TInt resource( EPhoneVoIPResumeFail );
        if ( aHold )
            {
            resource = EPhoneVoIPHoldFail;    
            }
        
        // qtn_voip_resume_failed / qtn_voip_hold_failed
        globalNoteParam.SetTextResourceId(
            CPhoneMainResourceResolver::Instance()->
                ResolveResourceID( resource ) );
        }
    else
        {
        // Decides note with call id
        // Resource is loaded here
        TInt resource = EPhoneVoIPResumeNumberFail;
        if ( aHold )
            {
            resource = EPhoneVoIPHoldNumberFail;    
            }
        
        // qtn_voip_resume_number_failed / qtn_voip_hold_number_failed
        const TInt tmpResourceId = CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( resource );
        
        TInt callId = aCallId;
        callId++; // Real call id index starts from 0, increase value by one
        CleanupStack::PopAndDestroy( text ); // Delete reserver buffer
        text = StringLoader::LoadLC( 
                tmpResourceId, 
                callId, 
                CCoeEnv::Static() ); // Load new buffer

        }
     
    globalNoteParam.SetText( *text );
    globalNoteParam.SetType( EPhoneMessageBoxInformation );

            
    iViewCommandHandle->ExecuteCommandL( 
            EPhoneViewShowGlobalNote, &globalNoteParam );
    
    CleanupStack::PopAndDestroy( text );
    }

// -----------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::GetRemoteInfoDataL
// -----------------------------------------------------------
//
void CPhoneVoIPErrorMessagesHandler::GetRemoteInfoDataL( 
    TInt aCallId,
    TDes& aData ) const 
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
            "CPhoneVoIPErrorMessagesHandler::GetRemoteInfoDataL()" );
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
            Panic( EPhoneCtrlInvariant ) );

    // Note next if-statements are in priority order so be careful if you change order
    // or add new if-statements.
    if ( iStateMachine->PhoneEngineInfo()->RemoteName( aCallId ).Length() )
        {
        __PHONELOG1( EBasic, PhoneUIVoIPExtension, 
            "CPhoneVoIPErrorMessagesHandler::GetRemoteInfoDataL - remote name = %S", 
            &iStateMachine->PhoneEngineInfo()->RemoteName( aCallId ) );

        if ( iStateMachine->PhoneEngineInfo()->RemoteName( aCallId ).Length()
             < KPhoneContactNameMaxLength )
            {
            // Display the contact name if it is available
            aData.Copy( iStateMachine->PhoneEngineInfo()->RemoteName( aCallId ) );
            }
        }
    else if ( iStateMachine->PhoneEngineInfo()->RemoteCompanyName( aCallId ).Length() )
         {
         __PHONELOG1( EBasic, PhoneUIVoIPExtension, 
             "CPhoneVoIPErrorMessagesHandler::GetRemoteInfoDataL - remote company name = %S", 
             &iStateMachine->PhoneEngineInfo()->RemoteCompanyName( aCallId ) );
         
         if ( iStateMachine->PhoneEngineInfo()->RemoteCompanyName( aCallId ).Length()
              < KPhoneContactNameMaxLength )
             {            
             // Display the company name if it is available
             aData.Copy( iStateMachine->PhoneEngineInfo()->RemoteCompanyName( aCallId ) );
             }
         }
    else if( iStateMachine->PhoneEngineInfo()->RemotePartyName( aCallId ).Length() )
        {
        __PHONELOG1( EBasic, PhoneUIVoIPExtension, 
            "CPhoneVoIPErrorMessagesHandler::GetRemoteInfoDataL - remote party name = %S", 
            &iStateMachine->PhoneEngineInfo()->RemotePartyName( aCallId ) );
        
        if ( iStateMachine->PhoneEngineInfo()->RemotePartyName( aCallId ).Length()
             < KPhoneContactNameMaxLength )
            {            
            // Display the remote party name if it is available
            aData.Copy( iStateMachine->PhoneEngineInfo()->RemotePartyName( aCallId ) );
            }
        }
    else
        {
        __PHONELOG( EBasic, PhoneUIVoIPExtension, 
            "CPhoneVoIPErrorMessagesHandler::GetRemoteInfoDataL - not found" );
        aData.Zero();
        }
    }

// -----------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::ShowRegistrationErrorNotesL
// -----------------------------------------------------------
//
void CPhoneVoIPErrorMessagesHandler::ShowRegistrationErrorNotesL( 
    TInt /*aErrorCode*/ )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
              "CPhoneVoIPErrorMessagesHandler::ShowRegistrationErrorNotesL()" );
    // Dismiss service enabling wait note
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveGlobalWaitNote );    
    }

// ---------------------------------------------------------
//  CPhoneVoIPErrorMessagesHandler::SendGlobalErrorNoteWithTextL
// ---------------------------------------------------------
//
void CPhoneVoIPErrorMessagesHandler::SendGlobalErrorNoteWithTextL( 
    TInt aResourceId,
    RBuf& aText )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneVoIPErrorMessagesHandler::SendGlobalErrorNoteL()" );
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );


    // Re-enable global notes
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( 
         EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
        
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetType( EPhoneMessageBoxInformation );
    globalNoteParam.SetTextResourceId( 
        CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( aResourceId ) );
    globalNoteParam.SetText( aText );

    iViewCommandHandle->ExecuteCommandL(  
        EPhoneViewShowGlobalNote, &globalNoteParam );
    }

// -----------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::GetServiceProviderNameL
// -----------------------------------------------------------
//
void CPhoneVoIPErrorMessagesHandler::GetServiceProviderNameL( 
    RBuf& aServiceName, 
    TUint32 aServiceId )
    { 
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
                 "CPhoneVoIPErrorMessagesHandler::GetServiceProviderNameL" )
    CSPEntry* serviceEntry = CSPEntry::NewLC();
    
    CSPSettings* serviceProviderSettings = CSPSettings::NewLC();
    
    User::LeaveIfError(
        serviceProviderSettings->FindEntryL( aServiceId, *serviceEntry ) );
    
    User::LeaveIfError(
        aServiceName.Create( serviceEntry->GetServiceName() ) );
      
    CleanupStack::PopAndDestroy( serviceProviderSettings );
    CleanupStack::PopAndDestroy( serviceEntry );
    }

// -----------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::ShowErrorNoteContainingServiceNameL
// -----------------------------------------------------------
//
void CPhoneVoIPErrorMessagesHandler::ShowErrorNoteWithServiceNameL( 
    TInt aResourceId, 
    TUint32 aServiceId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneVoIPErrorMessagesHandler::ShowErrorNoteWithServiceNameL" )
    RBuf serviceName;
    CleanupClosePushL( serviceName );
    GetServiceProviderNameL( serviceName, aServiceId );
    SendGlobalErrorNoteWithTextL( aResourceId, serviceName );
    CleanupStack::PopAndDestroy( &serviceName );
    }

// -----------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::HasSwapFailed
// -----------------------------------------------------------
//
TBool CPhoneVoIPErrorMessagesHandler::HasSwapFailed() const
    {
    TPEState callState( EPEStateUnknown );
    TPECallType callType( EPECallTypeUninitialized );
    TPEState confCallState( EPEStateUnknown );
    
    TInt connectedCalls( 0 );
    TInt heldCalls( 0 );
    TInt voipCalls( 0 );
    TBool ret( EFalse );
    
    for ( TInt i=0; i < KPEMaximumNumberOfVoiceCalls; i++ )
        {
        callState = iStateMachine->PhoneEngineInfo()->CallState( i );
        callType = iStateMachine->PhoneEngineInfo()->CallType( i );
        
        if ( EPEStateConnected == callState )
            {
            connectedCalls++;
            
            if ( EPECallTypeVoIP == callType )
                {
                voipCalls++;
                }
            }
        else if ( EPEStateHeld == callState )
            {
            heldCalls++;
            
            if ( EPECallTypeVoIP == callType )
                {
                voipCalls++;
                }
            }
        }
    
    confCallState = iStateMachine->PhoneEngineInfo()->CallState( KPEConferenceCallID );

    // At least one of the calls is VoIP call (conference or single)
    if ( voipCalls )
        {
        if ( EPEStateConferenceIdle == confCallState )
            {
            // Look for connected/hold and hold/hold combinations
            if ( ( 1 == connectedCalls && 1 == heldCalls ) ||
                   2 == heldCalls )
                {
                ret = ETrue;
                }
            }
        /* Check scenario that the other call is conference
         * 1) Putting VoIP call on hold fails, as a result there is a single connected 
         *    call and conference is on hold
         * 2) Resuming VoIP call fails, all calls are on hold (because conference is 
         *    first put on hold successfully)
         */
        else if ( ( 1 == connectedCalls && EPEStateHeldConference == confCallState ) ||
                 ( 0 == connectedCalls && EPEStateHeldConference == confCallState ) )
            {
            ret = ETrue;
            }
        }

    __PHONELOG1( EBasic, PhoneUIVoIPExtension, "CPhoneVoIPErrorMessagesHandler::HasSwapFailed() returned %D", ret );
    
    return ret;
    }

// End of File
