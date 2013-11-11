/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Telephony internal Central Repository keys
*
*/


#ifndef TELINTERNALCRKEYS_H
#define TELINTERNALCRKEYS_H

#include <e32std.h>

/******************************************************************************
* Telephony Variation API
* Keys under this category are used in defining telephony variation.
******************************************************************************/
const TUid KCRUidTelVariation = {0x102828F8};

/**
* Telephony Local Variation Flags. Values are defined in TelephonyVariant.hrh.
*/
const TUint32 KTelVariationFlags                            = 0x00000001;

/**
* Contains a space separated list of product specific contact stores.
* Can be empty, contain 1 or more stores.
* Varies in different product variants.
* Exact match search strategy is applied to the additional stores, if match 
* is not found from additional stores default stores are checked.
*/
const TUint32 KTelAdditionalStores                          = 0x00000002;

/**
* Timeout of VoIP service enabling 
* if enabling time is longer than this value,
* cancel the enabling
*/
const TUint32 KTelEnableTimeOut                            = 0x00000003;


#endif      // TELEPHONYINTERNALCRKEYS_H

// End of file