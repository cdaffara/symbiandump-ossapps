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
* Description: Implementation of CPhoneResolverVoIP class.
*
*/


// INCLUDE FILES
#include <bautils.h>
#include "phoneresourceids.h"
#include <data_caging_path_literals.hrh>
#include "cphoneresourceresolvervoip.h"
#include "cphonemainresourceresolver.h"
#include "phonerssvoip.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneResourceResolverVoIP::CPhoneResourceResolverVoIP
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneResourceResolverVoIP::CPhoneResourceResolverVoIP()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneRourceResolverVoIP::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneResourceResolverVoIP::ConstructL()
    {
    // Register resolver
    CPhoneMainResourceResolver::Instance()->RegisterResolver( this );
    }

// -----------------------------------------------------------------------------
// CPhoneResourceResolverVoIP::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhoneResourceResolverVoIP* CPhoneResourceResolverVoIP::NewL()
    {
    CPhoneResourceResolverVoIP* self =
        new( ELeave ) CPhoneResourceResolverVoIP();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CPhoneResourceResolverVoIP::~CPhoneResourceResolverVoIP()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneResourceResolverVoIP::ResolveResourceID
// Resolve resource id of given resource.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPhoneResourceResolverVoIP::ResolveResourceID(
    const TInt& aResource ) const
    {
    TInt retVal( KErrNotFound );

    switch( aResource )
        {
        case EPhoneVoIPNumberAcqMenubar:
            retVal = R_PHONEUI_NUMBERACQ_MENUBAR_VOIP;
            break;

        case EPhoneVoIPNumberAcqMenubarWithServiceNumber:
            retVal = R_PHONEUI_NUMBERACQ_MENUBAR_VOIP_WITH_SERVICE_NUMBER;
            break;

        case EPhoneVoIPNumberAcqOkMenubar:
            retVal = R_PHONEUI_NUMBERACQ_OK_MENUBAR_VOIP;
            break;

        case EPhoneVoIPNumberAcqOkMenubarWithServiceNumber:
            retVal = R_PHONEUI_NUMBERACQ_OK_MENUBAR_VOIP_WITH_SERVICE_NUMBER;
            break;

        case EPhoneVoIPNumberAcqInternetSoftkey:
            retVal = R_PHONEUI_NUMBERACQ_INTERNETCALL_CBA;
            break;

        case EPhoneVoIPPrivateAddress:
            retVal = R_PHONEUI_VOIP_PRIVATE_ADDRESS;
            break;

        case EPhoneVoIPNoAudioSupport:
            retVal = R_PHONE_VOIP_NO_AUDIO_SUPPORT;
            break;

        case EPhoneVoIPNoService:
            retVal = R_PHONE_VOIP_NO_VOIP_SERVICE_ERROR;
            break;

        case EPhoneRemoteForwarding: 
            retVal = R_PHONE_VOIP_CALL_BEING_FORWARDED;
            break;

        case EPhoneVoIPServiceUnavailable:
            retVal = R_NO_SERVICE_AVAILABLE_INFO;
            break;

        case EPhoneVoIPAddressNotInUse:
            retVal = R_PHONE_VOIP_ADDRESS_NOT_IN_USE;
            break;

        case EPhoneVoIPRequestTimeOut:
            retVal = R_PHONE_VOIP_REQUEST_TIME_OUT;
            break;

        case EPhoneVoIPInvalidRecipientAddress:
            retVal = R_PHONE_VOIP_INVALID_RECIPIENT_ADDRESS;
            break;

        case EPhoneVoIPNumberBusy:
            retVal = R_PHONE_VOIP_NUMBER_BUSY;
            break;

        case EPhoneVoIPNotFound:
            retVal = R_PHONE_VOIP_NOT_FOUND;
            break;

        case EPhoneVoIPNoAnswer:
            retVal = R_PHONE_VOIP_NO_ANSWER;
            break;

        case EPhoneSSNotificationTextVoipSipsFailed:
            retVal = R_PHONE_SS_NOTIFICATION_TEXT_VOIP_SIPS_FAILED;
            break;

        case EPhoneVoIPNetworkError:
            retVal = R_VOIP_NETWORK_ERROR_NOTE;
            break;
            
        case EPhoneVoIPHoldFail:
            retVal = R_VOIP_HOLD_FAILED;
            break;
            
        case EPhoneVoIPHoldNumberFail:
            retVal = R_VOIP_HOLD_NUMBER_FAILED;
            break;
            
        case EPhoneVoIPResumeFail:
            retVal = R_VOIP_RESUME_FAILED;
            break;
            
        case EPhoneVoIPResumeNumberFail:
            retVal = R_VOIP_RESUME_NUMBER_FAILED;
            break;
            
        case EPhoneVoIPSwapFail:
            retVal = R_VOIP_SWAP_FAILED;
            break;
        
        case EPhoneVoIPConferenceHoldFail:
            retVal = R_VOIP_CONFERENCE_HOLD_FAILED;
            break;
        
        case EPhoneVoIPConferenceResumeFail:
            retVal = R_VOIP_CONFERENCE_RESUME_FAILED;
            break;

        case EPhoneVoIPTransferFail:
            retVal = R_VOIP_TRANSFER_FAILED_NOTE;
            break;
        case EPhoneVoIPWaitingAcceptanceNote:
            retVal = R_VOIP_TRANSFER_WAITING_ACCEPTENCE;
            break;
        case EPhoneVoIPCallTypeInternet:
            retVal = R_VOIP_CALL_TYPE_INTERNET;
            break;
        case EPhoneVoIPNumberAcqXSPCall:
            break;
        case EPhoneVoIPUnattendedTransferOption:  
            retVal = R_VOIP_UNATTENDED_TRANSFER_OPTION;       
            break;
        case EPhoneVoIPUnattendedTransferAcceptanceQueryIdentified:
            retVal = R_PHONE_VOIP_TRANSFER_ACCEPT;
            break;
        case EPhoneVoIPUnattendedTransferAcceptanceQueryAnonymous:
            retVal = R_PHONE_VOIP_TRANSFER_ACCEPT_CLI;
            break;
        case EPhoneVoIPUnattendedTransferAcceptanceQueryHeader:
            retVal = R_PHONE_VOIP_QUERY_ACC_ID_HEADER;
            break;
        case EPhoneVoIPTransferAddress:
            retVal = R_VOIP_TRANSFER_ADDRESS;
            break;
        case EPhoneVoIPTransferDialerNumberEntryEmptyMenubar:
            retVal = R_PHONEUIDIALER_NUMBERENTRY_EMPTY_TRANSFER_MENUBAR;
            break;
        case EPhoneVoIPTransferDialerNumberEntryNotEmptyMenubar:
            retVal = R_PHONEUIDIALER_NUMBERENTRY_NOT_EMPTY_TRANSFER_MENUBAR;
            break;
        case EPhoneVoIPTransferDialerSoftkeys:
            retVal = R_PHONEUI_TRANSFER_DIALER_SOFTKEYS_OPTIONS_CANCEL;
            break;
        case EPhoneVoIPTransferAddressQuery:
            retVal = R_PHONE_TRANSFER_VOIP_CALL_QUERY;
            break;
        case EPhoneVoIPTransferAddressQueryEmptySoftkeys:
            retVal = R_PHONE_VOIP_SOFTKEYS_SEARCH_CANCEL;
            break;
        case EPhoneVoIPTransferAddressQueryNotEmptySoftkeys:
            retVal = R_PHONE_VOIP_SOFTKEYS_OK_CANCEL;
            break;
        case EPhoneVoIPNewCallQuery:
            retVal = R_PHONE_NEW_VOIP_CALL_QUERY;
            break;
        case EPhoneVoIPNewCallQueryEmptySoftkeys:
            retVal = R_PHONE_NEW_VOIP_CALL_SOFTKEYS_SEARCH_CANCEL;
            break;
        case EPhoneVoIPNewCallQueryNotEmptySoftkeys:
            retVal = R_PHONE_VOIP_NEW_CALL_SOFTKEYS_OK_CANCEL;
            break;
        case EPhoneVoIPEnablingServiceWaitNoteText:
            retVal = R_PHONE_VOIP_CONNECTING_SERVICE;
            break;
        case EPhoneVoIPAuthenticationFailed:
            retVal = R_PHONEUI_VOIP_ERROR_NOTE_AUTHENTICATION_FAILED;
            break;   
        case EPhoneVoIPHandoverToWlanOption:  
            retVal = R_VOIP_HANDOVER_TO_WLAN_OPTION;      
            break;
        case EPhoneVoIPHandoverToGsmOption:  
            retVal = R_VOIP_HANDOVER_TO_GSM_OPTION;
            break;      
        case EPhoneVoIPNewCallMenuOption:  
            retVal = R_PHONE_VOIP_NEW_CALL_OPTION;       
            break;  
        case EPhoneVoIPErrorInConnection:
            retVal = R_PHONEUI_VOIP_ERROR_IN_CONNECTION;
            break;
        case EPhoneVoIPWaitHandoverFromWlan:  
            retVal = R_VOIP_VCC_WAIT_HO_FROM_WLAN;      
            break;
        case EPhoneVoIPWaitHandoverFromGsm:  
            retVal = R_VOIP_VCC_WAIT_HO_FROM_GSM;      
            break;      
        case EPhoneVoIPHandoverFail:
            retVal = R_VOIP_VCC_INFO_HO_FAILED;
            break;
        case EPhoneVoIPMovedPermanentlyQueryHeader:
            retVal = R_PHONEUI_VOIP_QUERY_HEADER_MOVED_PERMANENTLY;
            break;
        case EPhoneVoIPMovedPermanentlySoftkeys:
            retVal = R_PHONE_VOIP_SOFTKEYS_OK_END_CALL;
            break;
        case EPhoneVoIPMultipleChoicesListQuery:
            retVal = R_PHONE_VOIP_MULTIPLE_CHOICES_LIST_QUERY;
            break;
        case EPhoneVoIPNoConnectionsDefined:
            retVal = R_PHONE_VOIP_NO_CONNECTIONS_DEFINED;
            break;
        case EPhoneVoIPNoConnectionsAvailable:
            retVal = R_PHONE_VOIP_NO_CONNECTIONS_AVAILABLE;
            break;
        case EPhoneVoIPCallsNotSupported:
            retVal = R_PHONE_VOIP_CONNECTION_DOES_NOT_SUPPORT_CALLS;
            break;
        case EPhoneVoIPNotAvailableWhenConnecting:
            retVal = R_PHONE_VOIP_CONNECTION_NOT_AVAILABLE_DURING_ALR;
            break;
        case EPhoneVoIPDefectiveSettings:
            retVal = R_PHONE_VOIP_CREATING_CONN_FAILED_PERMANENTLY;
            break;
        default:
            retVal = CPhoneResourceResolverGSM::ResolveResourceID( aResource );
            break;
        }

    return retVal;
    }

//  End of File
