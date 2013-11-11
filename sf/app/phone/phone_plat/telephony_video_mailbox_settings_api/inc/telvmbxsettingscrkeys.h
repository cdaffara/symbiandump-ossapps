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
* Description:  Telephony video mailbox settings central repository keys
*
*/


#ifndef TELVMBXSETTINGSCRKEYS_H
#define TELVMBXSETTINGSCRKEYS_H

#include <e32std.h>

/******************************************************************************
* Telephony Video Mailbox Settings API
* This API provides information related to Video MailBox Settings.
******************************************************************************/
const TUid KCRUidTelVideoMailbox = {0x102830B6}; 

/**
* Video mailbox accessed via keypad '1' or '2';
* Possible values: 1 (= keypad '1'), 2 (= keypad '2') 
* Default value is 1.
*/
const TUint32 KTelVideoMbxKey                               = 0x00000001;


#endif      // TELVMBXSETTINGSCRKEYS_H

// End of file