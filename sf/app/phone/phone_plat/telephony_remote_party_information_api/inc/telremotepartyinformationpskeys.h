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
* Description:  Telephony remote party information publish and subscribe keys.
*
*/


#ifndef TELREMOTEPARTYINFOPSKEYS_H
#define TELREMOTEPARTYINFOPSKEYS_H

#include <e32std.h>

// CONSTANTS

/*****************************************************************************
* Telephony Remote Party Information API.
*****************************************************************************/
const TUid KPSUidTelRemotePartyInformation = {0x102029A8}; 

/**
* Telephony updates the key on in-call state.
* Contains remote party name (from contacts).
* @type RProperty::EText
*/

const TUint32 KTelCLIName = 0x00000001; 

/**
* Telephony updates the key on in-call state.
* Contains remote party telephone number 
* @type RProperty::EText
*/
const TUint32 KTelCLINumber = 0x00000002; 

#endif      // TELREMOTEPARTYINFOPSKEYS_H

// End of file
