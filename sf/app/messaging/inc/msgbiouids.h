/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 */

#ifndef _MSGBIOUIDS_H_
#define _MSGBIOUIDS_H_

#include <e32uid.h>

//
/// These are the Bio Message Type UID's.
//

/// Business Card
const TUid KMsgBioUidVCard         = {0x10005534};

/// Calendar event
const TUid KMsgBioUidVCalendar     = {0x10005533};

/// Internet Access Configuration
const TUid KMsgBioUidIAC           = {0x1000552F};

/// WML Browser Settings
const TUid KMsgBioUidWmlBrSettings = {0x10005532};

/// Picture Message
const TUid KMsgBioUidPictureMsg    = {0x101F4CF4};

/// Email Notification
const TUid KMsgBioUidEmailNotif    = {0x10005530};

/// Ringing Tone
const TUid KMsgBioUidRingingTone   = {0x10005535};

/// Operator Logo
const TUid KMsgBioUidOperatorLogo  = {0x10005536};

/// Compact Business Card
const TUid KMsgBioUidCompBusCard   = {0x10005531};

/// SyncML
const TUid KMsgBioUidSyncML        = {0x1000092B};

/// WV settings
const TUid KMsgBioUidWVSettings    = {0x101F4D8A};

/// ICalendar event
const TUid KMsgBioUidICalendar	   = {0x101F8849};

/// UID for the Provisioning Message type for BIO database
const TUid KMsgBioProvisioningMessage = {0x101F84D1};

const TUid KMsgBioNokiaServiceMessage = {0x102832EA};

const TUid KMsgBioNokiaServiceSentMessage = {0x20022FE6};

//// PoC
const TUid KMsgBioUidPoC			= {0x101FD869};

// DEPRECATED! DO NOT USE!
// the following constants are preserved for backward compatibility

/// Business Card
_LIT( KMsgDllNameVCard, "vCardbc.dll" );

/// Calendar event
_LIT( KMsgDllNameVCalendar, "VCalbc.dll" );

/// WML Browser Settings
_LIT( KMsgDllNameWmlBrSettings, "WMLBC.dll" );

/// Picture Message
_LIT( KMsgDllNamePictureMsg, "gmsModel.dll" );

/// Email Notification
_LIT( KMsgDllNameEmailNotif, "ENotifBC.dll" );

/// Ringing Tone
_LIT( KMsgDllNameRingingTone, "RingBC.dll" );

/// Operator Logo
_LIT( KMsgDllNameOperatorLogo, "OpLogoBC.dll" );

/// Compact Business Card
_LIT( KMsgDllNameCompBusCard, "vCardbc.dll" );

/// ICalendar event
_LIT( KMsgDllNameICalendar, "ICalBC.dll" );

/// Provisioning Message
_LIT( KMsgDllbc, "ProvisioningBC.dll" );

/// PoC Message
_LIT( KMsgDllPoC, "PoCBioPlugin.dll" );

#endif //_MSGBIOUIDS_H_

//end of file