/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains the implementation of class CSPEtelCallRequester
*
*/

#include "cspetelcallrequester.h"

#include <exterror.h>
#include <gsmerror.h>
#include <etelmmerr.h>
#include <etelsat.h>

#include "mcspcallerrorobserver.h"
#include "csplogger.h"


// ---------------------------------------------------------------------------
// Constructs the requester via two phased constructing.
// ---------------------------------------------------------------------------
//
CSPEtelCallRequester* CSPEtelCallRequester::NewL(
            MCSPCallErrorObserver& aObserver,
            RMobileCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelCallRequester::NewL()" );
    CSPEtelCallRequester* self = new ( ELeave ) CSPEtelCallRequester( 
                                        aObserver, aCall );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor of the object.
// ---------------------------------------------------------------------------
//
CSPEtelCallRequester::~CSPEtelCallRequester( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelCallRequester::~CSPEtelCallRequester()" );
    CSPLOGSTRING2(CSPINT, "CSPEtelCallRequester:: type: %d", iRequestType );
    Cancel();
    }

// ---------------------------------------------------------------------------
// CSPEtelCallRequester::MakeDialRequest
// ---------------------------------------------------------------------------
//
TInt CSPEtelCallRequester::MakeDialRequest( 
            const TDesC8& aCallParams, 
            const TDesC& aRecipient )
    {
    CSPLOGSTRING(CSPINT, "CSPEtelCallRequester::MakeDialRequest" );
    iRequestType = ERequestTypeDial;

    if ( !IsActive() )
        {
        iCall.Dial( iStatus, aCallParams, aRecipient );
        SetActive();
        }
    else
        {
        return KErrInUse;
        }
    
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CSPEtelCallRequester::MakeDialNoFdnCheckRequest
// ---------------------------------------------------------------------------
//
TInt CSPEtelCallRequester::MakeDialNoFdnCheckRequest( 
            const TDesC8& aCallParams, 
            const TDesC& aRecipient )
    {
    CSPLOGSTRING(CSPINT, "CSPEtelCallRequester::MakeDialNoFdnCheckRequest" );
    iRequestType = ERequestTypeDial;

    if ( !IsActive() )
        {
        iCall.DialNoFdnCheck( iStatus, aCallParams, aRecipient );
        SetActive();
        }
    else
        {
        return KErrInUse;
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSPEtelCallRequester::MakeAnswerRequest
// ---------------------------------------------------------------------------
//
TInt CSPEtelCallRequester::MakeAnswerRequest(
            const TDesC8& aCallParams )
    {
    CSPLOGSTRING(CSPINT, "CSPEtelCallRequester::MakeAnswerRequest" );
    if ( !IsActive() )
        {
        iRequestType = ERequestTypeAnswer;
        iCall.AnswerIncomingCall( iStatus, aCallParams );
        SetActive();
        }
    else
        {
        return KErrInUse;
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSPEtelCallRequester::MakeDialEmergencyRequest
// ---------------------------------------------------------------------------
//
TInt CSPEtelCallRequester::MakeDialEmergencyRequest( 
                            const RMobileENStore::TEmergencyNumber& aRecipient )
    {
    CSPLOGSTRING(CSPINT, "CSPEtelCallRequester::MakeDialEmergencyRequest" );
    Cancel();

    iRequestType = ERequestTypeDialEmergency;
    iCall.DialEmergencyCall( iStatus, aRecipient );
    SetActive();
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSPEtelCallRequester::MakeRequest
// ---------------------------------------------------------------------------
//
TInt CSPEtelCallRequester::MakeRequest( TRequestType aRequest )
    {
    TInt result = KErrUnknown;

    if ( !IsActive() )
        {
        if ( aRequest != ERequestTypeDial 
             && aRequest != ERequestTypeDialEmergency )
            {
            iRequestType = aRequest;
            }
        CSPLOGSTRING2( CSPREQOUT, 
            "CSP: CSPEtelCallRequester::MakeRequest %d", iRequestType );
        switch ( aRequest )
            {
            case ERequestTypeDial:
                {
                // Requested through MakeDialRequest as a special case because
                // of the parameters.
                result = KErrNotSupported;
                break;
                }
            case ERequestTypeDialEmergency:
                {
                // Requested through MakeDialEmergencyRequest as a special case because
                // of the parameters.
                result = KErrNotSupported;
                break;
                }
            case ERequestTypeAnswer:
                {
                iCall.AnswerIncomingCall( iStatus );
                result = KErrNone;
                SetActive();
                break;
                }                
            case ERequestTypeHangup:
                {
                iCall.HangUp( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            case ERequestTypeHold:
                {
                iCall.Hold( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            case ERequestTypeResume:
                {
                iCall.Resume( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            case ERequestTypeGoOneToOne:
                {
                iCall.GoOneToOne( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
            case ERequestTypeSwap:
                {
                iCall.Swap( iStatus );
                SetActive();
                result = KErrNone;
                break;
                }
                
            default:
                {
                CSPLOGSTRING2( CSPERROR, 
                    "CSP: CSPEtelCallRequester::MakeRequest: Unspecified \
                                        request type: %d", iRequestType );
                result = KErrArgument;
                }
            }        
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelCallRequester::MakeRequest: Already active" );
        result = KErrInUse;
        }
        
    return result;
    }

// ---------------------------------------------------------------------------
// CSPEtelCallRequester::CancelRequest
// ---------------------------------------------------------------------------
//
TInt CSPEtelCallRequester::CancelRequest( TRequestType aRequest )
    {
    TInt result(KErrNone);
    if ( iRequestType == aRequest )
        {
        Cancel();
        }
    else
        {
        CSPLOGSTRING3( CSPERROR, "CSPEtelCallRequester::CancelRequest: No such active, current is %d, param=%d", iRequestType, aRequest );
        result = KErrNotReady;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// From CActive
// CSPEtelCallRequester::RunL
// ---------------------------------------------------------------------------
//
void CSPEtelCallRequester::RunL()
    {
    CSPLOGSTRING2( CSPREQEND, "CSPEtelCallRequester::RunL: status: %d", iStatus.Int() );
    
    TRequestType endedRequest = iRequestType;
    iRequestType = ERequestTypeNone;
    if ( iStatus == KErrNone )
        {
        
        switch ( endedRequest )
            {            
            case ERequestTypeDial:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Dial request OK" );
                break;
                }
            case ERequestTypeAnswer:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Answer request OK" );
                break;
                }
            case ERequestTypeHangup:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Hangup request OK" );
                break;
                }
            case ERequestTypeHold:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Hold request OK" );
                break;
                }
            case ERequestTypeResume:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Resume request OK" );
                break;
                }
            case ERequestTypeGoOneToOne:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: GoOneToOne request OK" );
                break;
                }
            case ERequestTypeSwap:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Swap request OK" );
                break;
                }
            case ERequestTypeDialEmergency:
                {
                CSPLOGSTRING( CSPINT,
                "CSP CSPEtelCallRequester::RunL: DialEmergency request OK" );
                break;
                }
            default:
                {
                CSPLOGSTRING2( CSPERROR, 
                    "CSP CSPEtelCallRequester::RunL: Unspecified request \
                                                type: %d", iRequestType );
                break;
                }
            }
        }
    else
        {
        // Error situations
        TInt err = iStatus.Int();
        CSPLOGSTRING2( CSPERROR, "CSP CSPEtelCallRequester::RunL: request \
                                completed with error: %d", err );

        switch ( endedRequest )
            {
            case ERequestTypeDial:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Dial request" );
                if ( err != KErrCancel )
                    {
                    iObserver.DialRequestFailed( err );
                    return; // In case of Idle state has caused
                    // immediate destruction of this object.
                    }
                break;
                }
            case ERequestTypeAnswer:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Answer request" );
                iObserver.NotifyErrorOccurred( ECCPRequestFailure );
                break;
                }
            case ERequestTypeHangup:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Hangup request" );
                iObserver.NotifyErrorOccurred( ECCPRequestFailure );
                break;
                }
            case ERequestTypeHold:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Hold request" );
                iObserver.NotifyErrorOccurred( ECCPLocalHoldFail );

                break;
                }
            case ERequestTypeResume:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Resume request" );
                iObserver.NotifyErrorOccurred( ECCPLocalResumeFail );
                break;
                }
            case ERequestTypeGoOneToOne:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: GoOneToOne request" );
                iObserver.NotifyErrorOccurred( ECCPRequestFailure );
                break;
                }
            case ERequestTypeSwap:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallRequester::RunL: Swap request" );
                iObserver.NotifyErrorOccurred( ECCPRequestFailure );
                break;
                }
            case ERequestTypeDialEmergency:
                {
                CSPLOGSTRING2( CSPINT,
                    "CSP CSPEtelCallRequester: RunL: DialEmergency error: %d",
                     err );
                
                if ( err != KErrCancel )
                    {
                    iObserver.EmergencyDialRequestFailed(err);
                    }
                
                break;
                }
                
            default:
                {
                CSPLOGSTRING2( CSPERROR, 
                    "CSP CSPEtelCallRequester::RunL: Unspecified request \
                                                type: %d", iRequestType );
                break;
                }
            }
        
        }    
    }


// ---------------------------------------------------------------------------
// From CActive
// CSPEtelCallRequester::DoCancel
// ---------------------------------------------------------------------------
//
void CSPEtelCallRequester::DoCancel()
    {
    CSPLOGSTRING( CSPREQOUT, "CSP: CSPEtelCallRequester::DoCancel" );
    TRequestType cancelRequest = iRequestType;
    iRequestType = ERequestTypeNone;
    
    if ( IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, "CSP: CSPEtelCallRequester::DoCancel IsActive" );
        switch ( cancelRequest )
            {
            case ERequestTypeDial:
                {
                CSPLOGSTRING( CSPREQOUT, "CSPEtelCallRequester::DoCancel DialCancel" );
                iCall.DialCancel();
                CSPLOGSTRING( CSPREQOUT, "CSPEtelCallRequester::DoCancel DialCancel OK" );
                break;
                }
            case ERequestTypeAnswer:
                {
                iCall.AnswerIncomingCallCancel();
                break;
                }
            case ERequestTypeHangup:
                {
                iCall.HangUpCancel();
                break;
                }
            case ERequestTypeHold:
                {
                iCall.CancelAsyncRequest( EMobileCallHold );
                break;
                }
            case ERequestTypeResume:
                {
                iCall.CancelAsyncRequest( EMobileCallResume );
                break;
                }
            case ERequestTypeGoOneToOne:
                {
                iCall.CancelAsyncRequest( EMobileCallGoOneToOne );
                break;
                }
            case ERequestTypeSwap:
                {
                iCall.CancelAsyncRequest( EMobileCallSwap );
                break;
                }
            case ERequestTypeDialEmergency:
                {
                iCall.CancelAsyncRequest( EMobileCallDialEmergencyCall );
                break;
                }
            default:
                {
                CSPLOGSTRING2( CSPERROR, "CSP CSPEtelCallRequester::DoCancel: \
                                Unspecified request type: %d", iRequestType );
                }
            }
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelCallRequester::DoCancel: Not active" );
        }    
    }

// ---------------------------------------------------------------------------
// Constructs the requester.
// ---------------------------------------------------------------------------
//
CSPEtelCallRequester::CSPEtelCallRequester(
            MCSPCallErrorObserver& aObserver,
            RMobileCall& aCall ) : 
                CActive( EPriorityStandard ), 
                iObserver( aObserver ), 
                iCall ( aCall ),
                iRequestType( ERequestTypeNone )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelCallRequester::CSPEtelCallRequester()" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Error value mapping.
// ---------------------------------------------------------------------------
//
TCCPError CSPEtelCallRequester::MapError( TInt aErrorCode )
    {
    TCCPError ret = ECCPRequestFailure;
    CSPLOGSTRING2( CSPINT, "CSPEtelCallRequester::MapError %d", aErrorCode);
       
    switch ( aErrorCode )
        {
        case KErrGsmCCUnassignedNumber:
            {
            ret = ECCPErrorNotReached;
            break;
            }        
        case KErrGsmCCUserBusy:
            {
            ret = ECCPErrorBusy;
            break;
            }
        case KErrMMEtelCallForbidden:
        case KErrGsmCallInProgress:
        case KErrGsmNumberBarred:
        case KErrGsmNotAllowed:
        case KErrEtelCallNotActive:
        case KErrEtelCallAlreadyActive:
            {
            ret = ECCPErrorNotAllowed;
            break;
            }        
        case KErrGsmCCNumberChanged:
            {
            ret = ECCPErrorMovedPermanently;
            break;
            }        
        case KErrGsmCCInvalidNumberFormat:
            {
            ret = ECCPErrorInvalidURI;
            break;
            }        
        case KErrGsmCCNormalUnspecified:
            {
            ret = ECCPErrorNoAnswerForVideo;
            break;
            }
        case KErrGsmCCTemporaryFailure:
        case KErrGsmCCSwitchingEquipmentCongestion:
        case KErrGsmCCRequestedChannelNotAvailable:
            {
            ret = ECCPErrorNetworkBusy;
            break;
            }     
        case KErrGsmCCNetworkOutOfOrder:
            {
            ret = ECCPErrorNetworkOutOfOrder;
            break;
            }     
        case KErrGsmCCRequestedFacilityNotSubscribed:
            {
            ret = ECCPErrorCCRequestedFacilityNotSubscribed;
            break;
            } 
        case KErrGsmCCInvalidCallReferenceValue:
            {
            ret = ECCPErrorInvalidCallReferenceValue;
            break;
            } 
        case KErrGsmCCUserNotInCug:
            {
            ret = ECCPErrorUserNotInCug;
            break;
            }
        case KErrGsmCCIncompatibleDestination:
            {
            // Affects to phone redial handling
            ret = ECCPErrorBadRequest;
            break;
            }
        case KErrGsmCCInvalidTransitNetworkSelection:
            {
            ret = ECCPErrorCCInvalidTransitNetworkSelection;
            break;
            }
        case KErrGsmCCSemanticallyIncorrectMessage:
        case KErrGsmCCConditionalIEError:
        case KErrGsmCCUnspecifiedProtocolError:
        case KErrEtelNoCarrier:
            {
            ret = ECCPErrorConnectionError;
            break;
            }
        case KErrGsmCallServerFail:
        case KErrGsm0707NotFound:
        case KErrGsmCallRejected:
            {
            ret = ECCPErrorRejected;
            break;
            }
        case KErrGeneral:
            {
            ret = ECCPErrorGeneral;
            break;
            }
        case KErrNotSupported:
            {
            ret = ECCPErrorNotSupported;
            break;
            }
        case KErrNoMemory:
            {
            ret = ECCPErrorNoMemory;
            break;
            }
        case KErrInUse:
            {
            ret = ECCPErrorAlreadyInUse;
            break;
            }
        case KErrNotReady:
            {
            ret = ECCPErrorNotReady;
            break;
            }
        case KErrGsmNoService:
            {
            ret = ECCPErrorNoService;
            break;
            }
        case KErrGsmOfflineOpNotAllowed:
            {
            ret = ECCPErrorNotAllowedInOfflineMode;
            break;
            }
        case KErrDiagnosticInfoBarredWithCUG:
            {
            ret = ECCPErrorCUGOutgoingCallsBarred;
            break;
            }
        case KErrDiagnosticInfoBarredNoCUG:
            {
            ret = ECCPErrorCUGNotSelected;
            break;
            }
        case KErrDiagnosticInfoBarredUnknownCUG:
            {
            ret = ECCPErrorCUGIndexUnknown;
            break;
            }
        case KErrDiagnosticInfoBarredIncompatibleCUG:
            {
            ret = ECCPErrorCUGIndexIncompatible;
            break;
            }
        case KErrDiagnosticInfoBarredFailureCUG:
            {
            ret = ECCPErrorCUGCallsFailure;
            break;
            }
        case KErrDiagnosticInfoBarredClirNotSubscribed:
            {
            ret = ECCPErrorCLIRNotSubscribed;
            break;
            }
        case KErrDiagnosticInfoBarredCCBSPossible:
            {
            ret = ECCPErrorCCBSPossible;
            break;
            }            
        case KErrDiagnosticInfoBarredCCBSNotPossible:
            {
            ret = ECCPErrorCCBSNotPossible;
            break;
            }            
        case KErrGsmCCRequestedFacilityNotImplemented:
            {
            ret = ECCPErrorServiceSettingError;
            break;
            }
        case KErrGsm0707SimNotInserted:
        case KErrGsm0707SIMPuk1Required:
        case KErrGsm0707SimPin1Required:
        case KErrGsm0707SimPin2Required:
        case KErrGsm0707SimFailure:
        case KErrGsm0707PhoneToSimLockRequired:
        case KErrGsm0707SimWrong:
            {
            ret = ECCPErrorAuthenticationFailed;
            break;
            }            
        case KErrGsmInvalidFdn:
            {
            ret = ECCPErrorInvalidFDN;
            break;
            }
        case KErrArgument:
            {
            ret = ECCPErrorInvalidPhoneNumber;
            break;
            }
        case KErrGsmCCOperatorDeterminedBarring:
            {
            ret = ECCPErrorNumberBarred;
            break;
            }
        case KErrGsmCCChannelUnacceptable:
            {
            ret = ECCPErrorUnacceptableChannel;
            break;
            }
        case KErrGsmCCAccessInformationDiscarded:
            {
            ret = ECCPErrorAccessInformationDiscarded;
            break;
            }
        case KErrGsmCCQualityOfServiceNotAvailable:
            {
            ret = ECCPErrorQualityOfServiceNotAvailable;
            break;
            }
        case KErrAccessDenied:
            {
            ret = ECCPErrorAccessDenied;
            break;
            }
        case KErrGsmCCNoRouteToDestination:
            {
            ret = ECCPErrorCCNoRouteToDestination;
            break;
            }
        case KErrGsmCCDestinationOutOfOrder:
            {
            ret = ECCPErrorCCDestinationOutOfOrder;
            break;
            }
        case KErrGsmCCResourceNotAvailable:
            {
            ret = ECCPErrorCCResourceNotAvailable;
            break;
            }
        case KErrGsmCCIncompatibleMessageInCallState:
            {
            ret = ECCPErrorCCIncompatibleMessageInCallState;
            break;
            }
        case KErrGsmCCNormalCallClearing:
            {
            ret = ECCPErrorCCNormalCallClearing;
            break;
            }
        case KErrGsmCCUserNotResponding:
            {
            ret = ECCPErrorCCUserNotResponding;
            break;
            }
        case KErrGsmCCUserAlertingNoAnswer:
            {
            ret = ECCPErrorCCUserAlertingNoAnswer;
            break;
            }
        case KErrGsmCCCallRejected:
            {
            ret = ECCPErrorCCCallRejected;
            break;
            }
        case KErrGsmCCPreemption:
            {
            ret = ECCPErrorCCPreemption;
            break;
            }
        case KErrGsmCCFacilityRejected:
            {
            ret = ECCPErrorCCFacilityRejected;
            break;
            }
        case KErrGsmCCResponseToStatusEnquiry:
            {
            ret = ECCPErrorCCResponseToStatusEnquiry;
            break;
            }
        case KErrGsmCCInvalidMandatoryInformation:
            {
            ret = ECCPErrorCCInvalidMandatoryInformation;
            break;
            }
        case KErrGsmCCNonExistentMessageType:
            {
            ret = ECCPErrorCCNonExistentMessageType;
            break;
            }
        case KErrGsmCCIncompatibleMessageInProtocolState:
            {
            ret = ECCPErrorCCIncompatibleMessageInProtocolState;
            break;
            }
        case KErrGsmCCNonExistentInformationElement:
            {
            ret = ECCPErrorCCNonExistentInformationElement;
            break;
            }
        case KErrGsmCCRecoveryOnTimerExpiry:
            {
            ret = ECCPErrorCCRecoveryOnTimerExpiry;
            break;
            }
        case KErrGsmCCNoChannelAvailable:
            {
            ret = ECCPErrorCCNoChannelAvailable;
            break;
            }
        case KErrGsmCCIncomingCallsBarredInCug:
            {
            ret = ECCPErrorCCIncomingCallsBarredInCug;
            break;
            }
        case KErrGsmCCBearerCapabilityNotAuthorised:
            {
            ret = ECCPErrorCCBearerCapabilityNotAuthorised;
            break;
            }
        case KErrGsmCCBearerCapabilityNotCurrentlyAvailable:
            {
            ret = ECCPErrorCCBearerCapabilityNotCurrentlyAvailable;
            break;
            }
        case KErrGsmCCServiceNotAvailable:
            {
            ret = ECCPErrorCCServiceNotAvailable;
            break;
            }
        case KErrGsmCCBearerServiceNotImplemented:
            {
            ret = ECCPErrorCCBearerServiceNotImplemented;
            break;
            }
        case KErrGsmCCOnlyRestrictedDigitalInformationBCAvailable:
            {
            ret = ECCPErrorCCOnlyRestrictedDigitalInformationBCAvailable;
            break;
            }
        case KErrGsmCCServiceNotImplemented:
            {
            ret = ECCPErrorCCOnlyRestrictedDigitalInformationBCAvailable;
            break;
            }
        case KErrGsmCCUnspecifiedInterworkingError:
            {
            ret = ECCPErrorCCUnspecifiedInterworkingError;
            break;
            }
        case KErrGsmNoNumber:
            {
            ret = ECCPErrorInvalidPhoneNumberCancelRedial;
            break;
            }
        case KErrNotFound:
            {
            ret = ECCPErrorNotFound;
            break;
            }
        case KErrTimedOut:
            {
            ret = ECCPErrorTimedOut;
            break;
            }
        case KErrGsmReleaseByUser:
            {
            ret = ECCPErrorReleaseByUserForCancelRedial;
            break;
            }
        case KErrSatControl:
            {
            ret = ECCPErrorSatControl;
            break;
            }
        default:
            {
            // Use the predefined.
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Returns the active request.
// ---------------------------------------------------------------------------
//
CSPEtelCallRequester::TRequestType CSPEtelCallRequester::Request()
    {
    if ( IsActive() )
        {
        return iRequestType;
        }
    
    return ERequestTypeNone;
    }
    
// ---------------------------------------------------------------------------
// Canceling emergency or normal call dial request.
// ---------------------------------------------------------------------------
//
TInt CSPEtelCallRequester::DialCancel() 
    {
    TInt ret = KErrNotReady;
    if ( iRequestType == ERequestTypeDialEmergency 
          || ( iRequestType == ERequestTypeDial ) )
        {
        ret = CancelRequest( iRequestType );
        }
    else
        {
        CSPLOGSTRING2(CSPINT, 
          "CSPEtelCallRequester::DialCancel invalid irequest for dialcancel %d",
          iRequestType);
        }
    
    return ret;
    }


// End of File

