/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*   VoIP-specific phone state definitions.
*
*/


#ifndef PHONESTATEDEFINITIONSVOIP_H
#define PHONESTATEDEFINITIONSVOIP_H

// INCLUDES

#include "phoneconstants.h"
#include "phonestatedefinitions.h"
#include "phonestatedefinitionsgsm.h"
#include "phoneviewcommanddefinitions.h"

// DATA TYPES

typedef TPhoneStateId TPhoneStateVoIPId;

/**
* View command definiations
*/
enum
    {
    EPhoneViewServiceSelection = EPhoneAmountOfCommands
    };

enum
    { 
    EPhoneStateVoIPIdle = EPhoneStateGSMLast,
    EPhoneStateVoIPCallSetup
    };

#endif      // PHONESTATEDEFINITIONSVOIP_H   
            
// End of File
