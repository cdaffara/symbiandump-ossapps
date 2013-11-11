/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneErrorMessagesHandler class.
*
*/


// INCLUDES
#include <mpeengineinfo.h>
#include <telephonydomainpskeys.h>
#include <ccpdefs.h>

#include "phoneui.pan"
#include "phonerssbase.h"
#include "cphoneerrormessageshandler.h"
#include "cphonemainerrormessageshandler.h"
#include "phonelogger.h"
#include "cphonestatehandle.h"
#include "mphoneviewcommandhandle.h"
#include "cphonepubsubproxy.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamnote.h"
#include "cphonemainresourceresolver.h"
#include "cphonecenrepproxy.h"

// CONSTANTS
const TUid KCRUidTelConfiguration = {0x102828B8};
const TUint32 KTelShowCauseCode   = 0x00000002;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneErrorMessagesHandler::CPhoneErrorMessagesHandler(
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneStateMachine* aStateMachine ) :
    iViewCommandHandle( aViewCommandHandle),
    iStateMachine( aStateMachine ),
    iCauseCodeVariation( EFalse )
    {
    if ( KErrNone != CPhoneCenRepProxy::Instance()->GetInt(
            KCRUidTelConfiguration,
            KTelShowCauseCode,
            iCauseCodeVariation ) )
        {
        iCauseCodeVariation = EFalse;
        }
    } 

// -----------------------------------------------------------
// CPhoneErrorMessagesHandler::~CPhoneErrorMessagesHandler()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------

EXPORT_C CPhoneErrorMessagesHandler::~CPhoneErrorMessagesHandler()
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneErrorMessagesHandler::ConstructL()
    {    
    CPhoneMainErrorMessagesHandler::Instance()->RegisterErrorMessagesHandler( this );
    }

// -----------------------------------------------------------
// CPhoneVoIPErrorMessagesHandler::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneErrorMessagesHandler* CPhoneErrorMessagesHandler::NewL( 
                    MPhoneViewCommandHandle* aViewCommandHandle,
                    MPhoneStateMachine* aStateMachine )
    {
    CPhoneErrorMessagesHandler* self = 
        new( ELeave ) CPhoneErrorMessagesHandler( 
                    aViewCommandHandle,
                    aStateMachine );
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------
// CPhoneErrorMessagesHandler::SendGlobalInfoNoteL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneErrorMessagesHandler::SendGlobalInfoNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {
    __LOGMETHODSTARTEND( EPhoneControl, 
        "CPhoneErrorMessagesHandler::SendGlobalInfoNoteL()" );
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
    
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( 
            EPhoneViewSetGlobalNotifiersDisabled,    
            &globalNotifierParam );
        
    TPhoneCmdParamGlobalNote globalNoteParam;
    TPhoneNotificationType type = aNotificationDialog ? 
            EPhoneNotificationDialog : EPhoneMessageBoxInformation;
    globalNoteParam.SetType( type );
    globalNoteParam.SetNotificationDialog( aNotificationDialog );

    TInt resourceID( aResourceId );
    TInt causeCode( KErrNotFound );
    TBool notification( ETrue );
    if ( iCauseCodeVariation && GetCauseCode( causeCode, resourceID, notification ) )
        {
        globalNoteParam.SetNotificationDialog( notification );
        globalNoteParam.SetCauseCode(causeCode);
        globalNoteParam.SetTextResourceId( 
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( resourceID ) );
        }
    else
        {
        globalNoteParam.SetTextResourceId( 
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( aResourceId ) );
        }
    globalNoteParam.SetToneType( EPhoneInformationTone );
    iViewCommandHandle->ExecuteCommandL( 
        EPhoneViewShowGlobalNote, &globalNoteParam );
   
    }
 
// ---------------------------------------------------------
//  CPhoneErrorMessagesHandler::SendGlobalErrorNoteL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneErrorMessagesHandler::SendGlobalErrorNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {
    __LOGMETHODSTARTEND( EPhoneControl, 
        "CPhoneErrorMessagesHandler::SendGlobalErrorNoteL()" );
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( 
         EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
        
    TPhoneCmdParamGlobalNote globalNoteParam;
    TPhoneNotificationType type = aNotificationDialog ? 
            EPhoneNotificationDialog : EPhoneMessageBoxInformation;
    globalNoteParam.SetType( type );
    globalNoteParam.SetNotificationDialog( aNotificationDialog );
    
    TInt resourceID( aResourceId );
    TInt causeCode( KErrNotFound );
    TBool notification( ETrue );
    if ( iCauseCodeVariation && GetCauseCode( causeCode, resourceID, notification ) )
        {
        globalNoteParam.SetNotificationDialog( notification );
        globalNoteParam.SetCauseCode(causeCode);
        globalNoteParam.SetTextResourceId( 
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( resourceID ) );
        }
    else
        {
        globalNoteParam.SetTextResourceId( 
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( aResourceId ) );
        }
    globalNoteParam.SetToneType( EPhoneErrorTone );
    iViewCommandHandle->ExecuteCommandL(  
        EPhoneViewShowGlobalNote, &globalNoteParam );
    
    }

// ---------------------------------------------------------
//  CPhoneErrorMessagesHandler::SendGlobalWarningNoteL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneErrorMessagesHandler::SendGlobalWarningNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneErrorMessagesHandler::SendGlobalWarningNoteL( ) ");
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
   
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL(  
        EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
        
    TPhoneCmdParamGlobalNote globalNoteParam;
        TPhoneNotificationType type = aNotificationDialog ? 
            EPhoneNotificationDialog : EPhoneMessageBoxInformation;
    globalNoteParam.SetType( type );
    globalNoteParam.SetNotificationDialog( aNotificationDialog );

    TInt resourceID( aResourceId );
    TInt causeCode( KErrNotFound );
    TBool notification( ETrue );
    if ( iCauseCodeVariation && GetCauseCode( causeCode, resourceID, notification) )
        {
        globalNoteParam.SetNotificationDialog( notification );
        globalNoteParam.SetCauseCode(causeCode);
        globalNoteParam.SetTextResourceId( 
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( resourceID ) );
        }
    else
        {
        globalNoteParam.SetTextResourceId( 
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( aResourceId ) );
        }
        globalNoteParam.SetToneType( EPhoneWarningTone );
    iViewCommandHandle->ExecuteCommandL( 
        EPhoneViewShowGlobalNote, &globalNoteParam );
    }

// ---------------------------------------------------------
// CPhoneErrorMessagesHandler::SimState()
// ---------------------------------------------------------
//
TPESimState CPhoneErrorMessagesHandler::SimState() const
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneErrorMessagesHandler::SimState()");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    return iStateMachine->PhoneEngineInfo()->SimState();
    }

// -----------------------------------------------------------
// CPhoneErrorMessagesHandler::ShowErrorSpecificNoteL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneErrorMessagesHandler::ShowErrorSpecificNoteL( const TPEErrorInfo& aErrorInfo )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneErrorMessagesHandler::ShowErrorSpecificNoteL()");
    __PHONELOG1( EBasic, EPhoneControl,
            "PhoneUIControl: CPhoneErrorMessagesHandler::ShowErrorSpecificNoteL - aErrorInfo.iErrorCode =%d ",
            aErrorInfo.iErrorCode);
    iCallId = aErrorInfo.iCallId;
    switch( aErrorInfo.iErrorCode )
        {
        case ECCPErrorRejected:
        case ECCPRequestFailure:
            SendGlobalErrorNoteL( EPhoneNoteTextRequestRejected, ETrue );    
            break;
            
        case ECCPErrorInvalidPhoneNumber:
            SendGlobalErrorNoteL( EPhoneInvalidPhoneNumber, ETrue );
            break;

        case ECCPErrorInvalidURI:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneInvalidPhoneNumber, ETrue );
                }
            else
                {
                SendGlobalErrorNoteL( EPhoneInvalidPhoneNumber, ETrue );
                }
            break;

        case ECCPErrorServiceSettingError:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNotePhoneOutOf3GCoverage );
                }
            else
                {
                SendGlobalInfoNoteL( EPhoneNoteTextCheckNetworkservices, ETrue );
                }
            break;

        case ECCPErrorNotAllowedInOfflineMode:
        case ECCPErrorAuthenticationFailed:
            SendGlobalWarningNoteL( EPhoneEmergencyCallsOnly, ETrue );
            break;

        case ECCPErrorNotReady:     
        case ECCPErrorGeneral:
        case ECCPErrorNotAllowed:
        case ECCPErrorNotFound:
        case ECCPErrorTimedOut:
        case ECCPErrorAccessDenied:        
            SendGlobalWarningNoteL( EPhoneNoteTextNotAllowed, ETrue );                    
            break;
            
        case ECCPErrorAlreadyInUse:
            SendGlobalErrorNoteL( EPhoneNoteTextCallNotAllowed, ETrue );                    
            break;

        case ECCPErrorInvalidFDN:
            SendGlobalWarningNoteL( EPhoneNoteTextCallNotAllowedFDN, ETrue );                    
            break;
            
        case ECCPErrorNotReached:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNumberNotInUse, ETrue );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNumberNotInUse, ETrue );    
                }
                
            break;

        case ECCPErrorUnacceptableChannel:
        case ECCPErrorCCDestinationOutOfOrder:                
        case ECCPErrorInvalidCallReferenceValue:
        case ECCPErrorCCInvalidTransitNetworkSelection:        
        case ECCPErrorConnectionError:        
        case ECCPErrorCCIncompatibleMessageInCallState:        
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteVideoCallNotPossible );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                }
            break;
           
        case ECCPErrorQualityOfServiceNotAvailable:
        case ECCPErrorAccessInformationDiscarded:
        case ECCPErrorCCResourceNotAvailable:
            SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
            break;
                        
        case ECCPErrorNumberBarred:
            SendGlobalWarningNoteL( EPhoneNumberBarred, ETrue );
            break;
            
        case ECCPErrorCCUserAlertingNoAnswer:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoAnswer, ETrue );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNoAnswer, ETrue );   
                }           
            break;
            
        case KErrPhoneEngineNoWcdmaNetwork:  // Videotel special case. Refactoring PE/CSPlugin needed
        case ECCPErrorVideoCallNotSupportedByNetwork: ////
            SendGlobalInfoNoteL( EPhoneInformationNoNetworkSupportForVideoCallNote );
            break;
                
        case KErrPhoneEngineVideoCallNotAllowedDuringRestore: // Videotel special case. Refactoring PE/CSPlugin needed
        case ECCPErrorVideoCallNotAllowedDuringRestore:
            SendGlobalInfoNoteL( EPhoneInformationVideoCallNotAllowedDuringRestoreNote );
            break;
 
        case KErrPhoneEngineVideoCallSetupFailed: // Videotel special case. Refactoring PE/CSPlugin needed
        case ECCPErrorVideoCallSetupFailed:
            SendGlobalInfoNoteL( EPhoneNoteVideoCallSetupFailed );
            break;

        case ECCPErrorNetworkBusy:
            SendGlobalWarningNoteL( EPhoneNetworkBusy, ETrue );
            break;
            
        case ECCPErrorNoService:
            SendGlobalWarningNoteL( EPhoneNoteNoService, ETrue );   
            break;
            
        case ECCPErrorBusy:
            SendGlobalWarningNoteL( EPhoneNumberBusy, ETrue );   
            break;
            
        case ECCPErrorUserNotInCug:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNumberNotInCUG, ETrue );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNumberNotInCUG, ETrue );   
                }
            break;
            
        case ECCPErrorCCNoRouteToDestination:
             if( IsVideoCall( aErrorInfo.iCallId ) )
                 {
                 SendGlobalInfoNoteL( EPhoneErrorInConnection );
                 }
             else
                 {
                 SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                 }
             break;
            
        case ECCPErrorCCNormalCallClearing:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteCallInfoCauseValue16 );
                }
            break;
                    
        case ECCPErrorCCUserNotResponding:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoAnswer ); 
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNoAnswer, ETrue );
                }
            break;
            
        case ECCPErrorCCCallRejected:
            if ( !iStateMachine->PhoneEngineInfo()->IsOutgoingCallBarringActivated() )
                {
                if( IsVideoCall( aErrorInfo.iCallId ) )
                    {
                    SendGlobalInfoNoteL( EPhoneNoteCalledNumberHasBarredIncomingCalls );
                    }
                else if ( IsVoiceCall( aErrorInfo.iCallId ))
                    {
                    SendGlobalWarningNoteL( EPhoneNoteCallInfoCauseValue21, ETrue );
                    }
                }
            break;
            
        case ECCPErrorMovedPermanently:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNumberNotInUse );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNumberNotInUse, ETrue );
                }
            break;
            
        case ECCPErrorNoAnswerForVideo:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNotePhoneOutOf3GCoverage );   
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNoAnswer, ETrue );
                }
            break;
            
        case ECCPErrorCCNoChannelAvailable:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNetworkBusy ); 
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNetworkBusy, ETrue );
                }
            break;
            
        case ECCPErrorNetworkOutOfOrder:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneErrorInConnection );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                }
            break;
            
        case ECCPErrorCCRequestedFacilityNotSubscribed:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteVideoCallOnlyPossibleUnder3GCoverage );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNoteTextNotAllowed, ETrue );
                }
            break;

        case ECCPErrorCCIncomingCallsBarredInCug:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNumberBarred );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneNumberBarred, ETrue );
                }
            break;
            
        case ECCPErrorCCIncompatibleDestination:
        case ECCPErrorCCBearerCapabilityNotAuthorised:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                }
            break;
        
        case ECCPErrorCCBearerCapabilityNotCurrentlyAvailable:
        case ECCPErrorCCInvalidMandatoryInformation:
        case ECCPErrorCCUnspecifiedInterworkingError:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteVideoCallNotPossible );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                }
            break;
            
        case ECCPErrorBadRequest:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                } 
            break;
            
        case ECCPErrorCCPreemption:                                              
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteCallInfoNotSupported );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                }     
            break;
        case ECCPErrorCCResponseToStatusEnquiry: 
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneErrorInConnection );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                }              
            break;
            
        case ECCPErrorCCFacilityRejected:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteTextRequestRejected );
                }
            else
                {
                SendGlobalErrorNoteL( EPhoneNoteTextRequestRejected, ETrue );
                }
            break;
            
        case ECCPTransferFailed:
            SendGlobalErrorNoteL( EPhoneNoteTextRequestRejected, ETrue );
            break;
 
        case ECCPErrorCCBearerServiceNotImplemented:
        case ECCPErrorCCOnlyRestrictedDigitalInformationBCAvailable:
        case ECCPErrorCCNonExistentMessageType:
        case ECCPErrorCCIncompatibleMessageInProtocolState:
        case ECCPErrorCCNonExistentInformationElement:
        case ECCPErrorCCRecoveryOnTimerExpiry:
            if ( IsVideoCall( aErrorInfo.iCallId ) ) 
                {
                SendGlobalInfoNoteL( EPhoneNoteVideoCallNotPossible ); //Video call didn't succeed to called end.
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                }
            break;
            
        case ECCPErrorCCServiceNotImplemented:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneNoteCallInfoServiceNotAvailable );
                }
            break;

        case ECCPErrorCCServiceNotAvailable:
            if( IsVideoCall( aErrorInfo.iCallId ) )
                {
                SendGlobalInfoNoteL( EPhoneErrorInConnection );
                }
            else
                {
                SendGlobalWarningNoteL( EPhoneErrorInConnection, ETrue );
                }
            break;
            
        case ECCPErrorCUGOutgoingCallsBarred:
            SendGlobalInfoNoteL( EPhoneOutgoingCallsBarredWithinCUG, ETrue );
            break;
        
        case ECCPErrorCUGNotSelected:
            SendGlobalInfoNoteL( EPhoneNoCUGSelected, ETrue );
            break;
        
        case ECCPErrorCUGIndexUnknown:
            SendGlobalInfoNoteL( EPhoneUnknownCUGIndex, ETrue );
            break;
        
        case ECCPErrorCUGIndexIncompatible:
            SendGlobalInfoNoteL( EPhoneCUGIndexIncompatible, ETrue );
            break;
        
        case ECCPErrorCUGCallsFailure:
            SendGlobalInfoNoteL( EPhoneCUGCallsFailure, ETrue );
            break;
        
        case ECCPErrorCLIRNotSubscribed:
            SendGlobalInfoNoteL( EPhoneCLIRNotSubscribed, ETrue );
            break;
            
        case ECCPErrorCCBSPossible:
        case ECCPErrorCCBSNotPossible:
        case ECCPErrorSatControl:
            break;
                
        default:
            __PHONELOG1(
                EOnlyFatal, 
                EPhoneControl,
                "PHONEUI_ERROR: CPhoneErrorMessagesHandler::HandleErrorL - Error received (err=%d)",
                aErrorInfo.iErrorCode);
            break;
        } 
    
    iCallId = KErrNotFound;
    }    

// -----------------------------------------------------------
// CPhoneErrorMessagesHandler::IsVideoCall
// -----------------------------------------------------------
//
TBool CPhoneErrorMessagesHandler::IsVideoCall( const TInt aCallId ) const
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneErrorMessagesHandler::IsVideoCall() ");

    if( aCallId == KErrNotFound )
        {
        // Illegal call id, check call type command
        return ( iStateMachine->PhoneEngineInfo()->CallTypeCommand()
            == EPECallTypeVideo );  
        }
        
    return ( iStateMachine->PhoneEngineInfo()
        ->CallType( aCallId )== EPECallTypeVideo );
    }

// -----------------------------------------------------------
// CPhoneErrorMessagesHandler::IsVoiceCall
// -----------------------------------------------------------
//
TBool CPhoneErrorMessagesHandler::IsVoiceCall( const TInt aCallId ) const
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneErrorMessagesHandler::IsVoiceCall() ");
    if( aCallId == KErrNotFound )
        {
       // Illegal call id, check call type command
        return ( iStateMachine->PhoneEngineInfo()->CallTypeCommand()
            == EPECallTypeCSVoice );  
        }
    return ( iStateMachine->PhoneEngineInfo()
            ->CallType( aCallId )== EPECallTypeCSVoice );
    }
    
// -----------------------------------------------------------
// CPhoneErrorMessagesHandler::GetCauseCode
// -----------------------------------------------------------
//
TBool CPhoneErrorMessagesHandler::GetCauseCode( 
        TInt &aCauseCode, TInt &aResourceId, TBool &aNotification ) const
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneErrorMessagesHandler::CauseCode() ");
    aCauseCode = KErrNotFound;
    aNotification = ETrue;
    TInt callId = (KErrNotFound == iCallId) ? 
        iStateMachine->PhoneEngineInfo()->CallId() :
        iCallId;
    //get exit code error from call data
    TInt callError = iStateMachine->PhoneEngineInfo()->ProtocolError( 
            callId );
    
    if ( KErrNotFound == callError )
        {
        // These resources are used when there is not a protocol error
        // available.
        switch( aResourceId )
            {
            case EPhoneInvalidPhoneNumber:
                {
                callError = KErrGsmCCInvalidNumberFormat;
                break;
                }
            case EPhoneNumberBusy:
                {
                callError = KErrGsmCCUserBusy;
                break;
                }
            case EPhoneInformationNoNetworkSupportForVideoCallNote:
            case EPhoneNoteVideoCallOnlyPossibleUnder3GCoverage:
                {
                // Video call special case when call is not
                // initialized yet and the phone is out off the 3G network.
                aResourceId = EPhoneNoteVideoCallOnlyPossibleUnder3GCoverage;
                aNotification = EFalse;
                aCauseCode = 50;
                break;
                }
            default:
                break;
            }
        }
    
    switch( callError )
        {
        case KErrGsmCCUnassignedNumber:
            aResourceId = EPhoneNumberNotInUse;
            aCauseCode = 1;                
            break;
            
        case KErrGsmCCNoRouteToDestination:
            aResourceId = EPhoneErrorInConnection;
            aCauseCode = 3;                
            break;

        case KErrGsmCCChannelUnacceptable:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 6;                
            break;
            
        case KErrGsmCCOperatorDeterminedBarring:
            aResourceId = EPhoneNumberBarred;
            aCauseCode = 8;            
            break;
            
        case KErrGsmCCUserBusy:
            aResourceId = EPhoneNumberBusy;
            aCauseCode = 17;                
            break;
            
        case KErrGsmCCUserNotResponding:
            aResourceId = EPhoneNoAnswer;
            aCauseCode = 18;                
            break;
            
        case KErrGsmCCUserAlertingNoAnswer:
            aResourceId = EPhoneNoAnswer;
            aCauseCode = 19;                
            break;
            
        case KErrGsmCCCallRejected:
            if ( !iStateMachine->PhoneEngineInfo()->IsOutgoingCallBarringActivated() )
                {
                if ( IsVideoCall( callId ) )
                    {
                    aResourceId = EPhoneNoteCalledNumberHasBarredIncomingCalls;
                    aNotification = EFalse;
                    }
                else
                    {
                    aResourceId = EPhoneNoteCallInfoCauseValue21;           
                    }
                aCauseCode = 21;
                }
            break;
            
        case KErrGsmCCNumberChanged:
            aResourceId = EPhoneNumberNotInUse;
            aCauseCode = 22;
            break;
        
        case KErrGsmCCNonSelectedUserClearing:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNotePhoneOutOf3GCoverage; 
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneNoAnswer;
                }
            aCauseCode = 26;                
            break;
            
        case KErrGsmCCDestinationOutOfOrder:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 27;
            break;

        case KErrGsmCCInvalidNumberFormat:
            aResourceId = EPhoneInvalidPhoneNumber;
            aCauseCode = 28;                
            break;
            
        case KErrGsmCCFacilityRejected:
            aResourceId = EPhoneNoteTextRequestRejected;
            aCauseCode = 29;                
            break;
     
        case KErrGsmCCResponseToStatusEnquiry:
            aResourceId = EPhoneErrorInConnection;
            aCauseCode = 30;                
            break;
            
        case KErrGsmCCNormalUnspecified:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNotePhoneOutOf3GCoverage; 
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneNoAnswer;
                }
            aCauseCode = 31;                
            break;
            
        case KErrGsmCCNoChannelAvailable:
            aResourceId = EPhoneNetworkBusy;
            aCauseCode = 34;                
            break;
            
        case KErrGsmCCNetworkOutOfOrder:
            aResourceId = EPhoneErrorInConnection;
            aCauseCode = 38;                
            break;
            
        case KErrGsmCCTemporaryFailure:
            aResourceId = EPhoneNetworkBusy;
            aCauseCode = 41;                
            break;
            
        case KErrGsmCCSwitchingEquipmentCongestion:
            aResourceId = EPhoneNetworkBusy;
            aCauseCode = 42;                
            break;
            
        case KErrGsmCCAccessInformationDiscarded:
            aResourceId = EPhoneErrorInConnection;
            aCauseCode = 43;                
            break;
            
        case KErrGsmCCRequestedChannelNotAvailable:
            aResourceId = EPhoneNetworkBusy;
            aCauseCode = 44;                
            break;
            
        case KErrGsmCCResourceNotAvailable:
            aResourceId = EPhoneErrorInConnection;
            aCauseCode = 47;                
            break;
            
        case KErrGsmCCQualityOfServiceNotAvailable:
            aResourceId = EPhoneErrorInConnection;
            aCauseCode = 49;                
            break;
            
        case KErrGsmCCRequestedFacilityNotSubscribed:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallOnlyPossibleUnder3GCoverage; 
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneNoteTextNotAllowed;
                }
            aCauseCode = 50;                
            break;
            
        case KErrGsmCCIncomingCallsBarredInCug:
            aResourceId = EPhoneNumberBarred;
            aCauseCode = 55;                
            break;
            
        case KErrGsmCCBearerCapabilityNotAuthorised:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone; 
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 57;                
            break;
   
        case KErrGsmCCBearerCapabilityNotCurrentlyAvailable:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 58;                
            break;
            
        case KErrGsmCCServiceNotAvailable:
            aResourceId = EPhoneErrorInConnection;
            aCauseCode = 63;                
            break;
            
        case KErrGsmCCBearerServiceNotImplemented:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 65;                
            break;
 
        case KErrGsmCCAcmGreaterThanAcmMax:
            aResourceId = EPhoneErrorInConnection;
            aCauseCode = 68;                
            break;
            
        case KErrGsmCCRequestedFacilityNotImplemented:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNotePhoneOutOf3GCoverage;
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneNoteTextCheckNetworkservices;
                }
            aCauseCode = 69;                
            break;
            
        case KErrGsmCCOnlyRestrictedDigitalInformationBCAvailable:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible;
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneNoteTextCheckNetworkservices;
                }
            aCauseCode = 70;                
            break;
            
        case KErrGsmCCServiceNotImplemented:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible;
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 79;                
            break;
            
        case KErrGsmCCInvalidCallReferenceValue:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible;
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 81;                
            break;
            
        case KErrGsmCCUserNotInCug:
            aResourceId = EPhoneNumberNotInCUG;
            aCauseCode = 87;                
            break;
            
        case KErrGsmCCIncompatibleDestination:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone;
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 88;                
            break;
       
        case KErrGsmCCInvalidTransitNetworkSelection:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 91;                
            break;
            
        case KErrGsmCCSemanticallyIncorrectMessage:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 95;                
            break;
            
        case KErrGsmCCInvalidMandatoryInformation:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 96;                
            break;
            
        case KErrGsmCCNonExistentMessageType:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 97;                
            break;
            
        case KErrGsmCCIncompatibleMessageInProtocolState:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 98;                
            break;
            
        case KErrGsmCCNonExistentInformationElement:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 99;                
            break;
            
        case KErrGsmCCConditionalIEError:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 100;                
            break;
            
        case KErrGsmCCIncompatibleMessageInCallState:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 101;                
            break;
            
        case KErrGsmCCRecoveryOnTimerExpiry:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 102;                
            break;
            
        case KErrGsmCCUnspecifiedProtocolError:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 111;                
            break;
            
        case KErrGsmCCUnspecifiedInterworkingError:
            if ( IsVideoCall( callId ) ) 
                {
                aResourceId = EPhoneNoteVideoCallNotPossible; //Video call didn't succeed to called end.
                aNotification = EFalse;
                }
            else
                {
                aResourceId = EPhoneErrorInConnection;
                }
            aCauseCode = 127;
            break;
        default:
            break;
        }  
    return (aCauseCode != KErrNotFound);
    }

// End of File
