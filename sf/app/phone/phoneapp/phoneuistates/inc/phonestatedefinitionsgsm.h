/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*   GSM-specific phone state definitions.
*
*/


#ifndef __PHONESTATEDEFINITIONSGSM_H
#define __PHONESTATEDEFINITIONSGSM_H

// INCLUDES

#include "phoneconstants.h"
#include "phonestatedefinitions.h"

// DATA TYPES

typedef TPhoneStateId TPhoneStateGSMId;

enum
    {
    EPhoneStateAlerting = KPhoneStateProtocolFirst,
    EPhoneStateAlertingInSingle,
    EPhoneStateWaitingInSingle,
    EPhoneStateTwoSingles,
    EPhoneStateCallSetupInSingle,
    EPhoneStateConference,
    EPhoneStateConferenceAndCallSetup,
    EPhoneStateConferenceAndSingle,
    EPhoneStateConferenceAndWaiting,
    EPhoneStateSingleAndCallSetupAndWaiting,
    EPhoneStateCallSetupAndWaiting,
    EPhoneStateTwoSinglesAndWaiting,
    EPhoneStateConferenceAndSingleAndWaiting,
    EPhoneStateConferenceAndWaitingAndCallSetup,
    EPhoneStateGSMLast
    };

#endif      // __PHONESTATEDEFINITIONSGSM_H   
            
// End of File
