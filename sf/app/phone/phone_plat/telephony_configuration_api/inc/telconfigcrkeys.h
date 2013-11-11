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
* Description:  Telephony configuration central repository keys
*
*/


#ifndef TELCONFIGCRKEYS_H
#define TELCONFIGCRKEYS_H

#include <e32std.h>


/******************************************************************************
* Telephony Configuration API
* Keys under this category are used in defining telephony configuration.
******************************************************************************/
const TUid KCRUidTelConfiguration = {0x102828B8};

/**
* Amount of digits to be used in contact matching.
* This allows a customer to variate the amount of digits to be matched.
*/
const TUint32 KTelMatchDigits                               = 0x00000001;

#endif      // TELCONFIGCRKEYS_H

// End of file