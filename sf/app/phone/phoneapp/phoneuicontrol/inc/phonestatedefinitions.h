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
*   Common phone state definitions.
*
*/


#ifndef PHONESTATEDEFINITIONS_H
#define PHONESTATEDEFINITIONS_H

// INCLUDES

#include "phoneconstants.h"

// DATA TYPES

typedef TInt TPhoneStateId;

enum
    { 
    EPhoneStateNull = KPhoneStateCommonFirst,
    EPhoneStateStartup,
    EPhoneStateIdle,            
    EPhoneStateCallSetup,
    EPhoneStateSingle,
    EPhoneStateIncoming,
    EPhoneStateEmergency
    };

#endif // PHONESTATEDEFINITIONS_H   
            
// End of File
