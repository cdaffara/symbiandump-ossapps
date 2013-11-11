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
* Description: 
*     File containing enumeration for phoneapp resources used from VoIP
*     specific code.
*
*/


#ifndef PHONERSSVOIP_H
#define PHONERSSVOIP_H

//  INCLUDES
#include <e32std.h>
#include "phonerssbase.h"


// DATA TYPES
enum TPhoneRssVoIP
    {
    EPhoneVoIPNumberAcqMenubar = EPhoneRssBaseLast,
    EPhoneVoIPNumberAcqMenubarWithServiceNumber,
    EPhoneVoIPNumberAcqOkMenubar,
    EPhoneVoIPNumberAcqOkMenubarWithServiceNumber,
    EPhoneVoIPNumberAcqInternetSoftkey,
    EPhoneVoIPPrivateAddress,
    EPhoneVoIPNoAudioSupport,
    EPhoneVoIPNoService,
    EPhoneVoIPAddressNotInUse,
    EPhoneVoIPServiceUnavailable,
    EPhoneVoIPRequestTimeOut,
    EPhoneVoIPInvalidRecipientAddress,
    EPhoneVoIPNumberBusy,
    EPhoneVoIPNotFound,
    EPhoneVoIPNoAnswer,
    EPhoneSSNotificationTextVoipSipsFailed,
    EPhoneVoIPNetworkError,
    EPhoneVoIPHoldFail,
    EPhoneVoIPResumeFail,
    EPhoneVoIPHoldNumberFail,
    EPhoneVoIPResumeNumberFail,
    EPhoneVoIPSwapFail,
    EPhoneVoIPConferenceHoldFail,
    EPhoneVoIPConferenceResumeFail,
    EPhoneVoIPTransferFail,
    EPhoneVoIPUnattendedTransferOption,
    EPhoneVoIPCallTypeInternet,
    EPhoneVoIPNumberAcqXSPCall,
    EPhoneVoIPUnattendedTransferAcceptanceQueryIdentified,
    EPhoneVoIPUnattendedTransferAcceptanceQueryAnonymous,
    EPhoneVoIPUnattendedTransferAcceptanceQueryHeader,
    EPhoneVoIPTransferAddress,
    EPhoneVoIPTransferDialerNumberEntryEmptyMenubar,
    EPhoneVoIPTransferDialerNumberEntryNotEmptyMenubar,
    EPhoneVoIPTransferDialerSoftkeys,
    EPhoneVoIPTransferAddressQuery,
    EPhoneVoIPTransferAddressQueryEmptySoftkeys,
    EPhoneVoIPTransferAddressQueryNotEmptySoftkeys,
    EPhoneVoIPNewCallQuery,
    EPhoneVoIPNewCallQueryEmptySoftkeys,
    EPhoneVoIPNewCallQueryNotEmptySoftkeys,
    EPhoneVoIPNewCallMenuOption,
    EPhoneVoIPEnablingServiceWaitNoteText,
    EPhoneVoIPAuthenticationFailed,
    EPhoneVoIPErrorInConnection,
    EPhoneVoIPWaitingAcceptanceNote,
    EPhoneRemoteForwarding,
    EPhoneVoIPHandoverToGsmOption,
    EPhoneVoIPHandoverToWlanOption,
    EPhoneVoIPWaitHandoverFromWlan,
    EPhoneVoIPWaitHandoverFromGsm,
    EPhoneVoIPHandoverFail,
    EPhoneVoIPMovedPermanentlyQueryHeader,
    EPhoneVoIPMovedPermanentlySoftkeys,
    EPhoneVoIPMultipleChoicesListQuery,
    EPhoneVoIPNoConnectionsDefined,
    EPhoneVoIPNoConnectionsAvailable,
    EPhoneVoIPCallsNotSupported,
    EPhoneVoIPNotAvailableWhenConnecting,
    EPhoneVoIPDefectiveSettings,
    // Last
    EPhoneVoIPLast
    };

#endif      // PHONERSSVOIP_H   
            
// End of File
