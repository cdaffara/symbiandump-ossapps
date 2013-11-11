/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Email mailbox setting key definitions for IMAP4/POP3
*
*/

#ifndef IPSSETTINGKEYS_H
#define IPSSETTINGKEYS_H

namespace IpsServices {
    
// Value from ipssosplugin
static const quint32 EmailMailboxSettingRepository = 0x2000E53D; 



// Key offsets are defined here:

// EmailMailboxOffset defines key offset between mailboxes in the repository.
// For example mailboxId key for each mailbox is calculated by the following formula:
// key = mailboxNumber (starting from 1) * EmailMailboxOffset + EmailKeyMailboxId
// (1st mailbox = 1 * 0x1000 + 0 = 0x1000)
// This allows key range 0-0x0FFF to contain mailbox independent keys.
static const quint32 EmailMailboxOffset = 0x1000;

// Offsets for sync profiles keys.
// Keep-Up-To-Date
static const quint32 EmailProfileOffsetKUTD = 0x0;

// Save Energy
static const quint32 EmailProfileOffsetSE = 0x100;

// Manual Fetch
static const quint32 EmailProfileOffsetMF = 0x200;

// User Defined profile (for future use)
static const quint32 EmailProfileOffsetUD = 0x300;


// Key definitions are defined here:

// Mailbox id. This is a 32-bit integer excluding the protocol plug-in id.
static const quint32 EmailKeyMailboxId = 0;

// Preferences keys:

// Is automatic picture downloading enabled (0=Off,1=On)
static const quint32 EmailKeyPreferenceDownloadPictures = 1;

// Defines if message divider is shown in message list.
// Values: 0=Divider not shown, 1=1ivider is shown
static const quint32 EmailKeyPreferenceMessageDivider = 2;

// sync profile identifiers
static const int EmailSyncProfileKeepUpToDate = 0;
static const int EmailSyncProfileSaveEnergy = 1;
static const int EmailSyncProfileManualFetch = 2;
static const int EmailSyncProfileUserDefined = 3;

// initial sync profile value when a mailbox is created (save energy)
static const int EmailInitialSyncProfile = 1;

// Currently used sync profile 
//  Keep Up-to-Date (=KUTD) / Save Energy (=SE) / Manual Fetch (=MF) / User defined (=UD)
//  Keep Up-to-Date  = 0 (EmailProfileOffsetKUTD)
//  Save Energy      = 1 (EmailProfileOffsetSE)
//  Manual Fetch     = 2 (EmailProfileOffsetMF)
//  User Defined     = 3 (EmailProfileOffsetUD)     (<== for future use) 
//  (Could EMN be one value when taken in use)
static const quint32 EmailKeyReceptionActiveProfile = 3;

// Reception schedule keys. To calculate the actual key, use EmailProfileOffset<profile>.
// For example to determine inbox sync window key for the 'Save energy' profile:
// key = mailbox number (e.g. 1) * EmailMailboxOffset (0x1000) +
//       EmailKeyReceptionInboxSyncWindow (5) +
//       EmailProfileOffsetSE (0x100)
//       = 0x1105

// This is reserved for future use!
// State of "User defined receive profile" (1=Enabled,0=Disabled) affects
// visibility of the user defined profile. The profile is enabled in a mailbox
// once user changes any 'receive setting'.
static const quint32 EmailKeyReceptionUserDefinedProfile = 4;

// Inbox sync window in number of messages. 0 means all messages are synched
// from the server. 
// Value for each profile: KUTP(500),SE(50),FM(50), UD
// This key follows the profile offset.
static const quint32 EmailKeyReceptionInboxSyncWindow = 5;

// default values for each profile
static const int DefaultValueKeepUpToDateInboxSyncWindow = 500;
static const int DefaultValueSaveEnergyInboxSyncWindow = 50;
static const int DefaultValueManualSyncInboxSyncWindow = 50;
static const int DefaultUserDefinedInboxSyncWindow = -1;

// Other folder sync window in number of messages. 0 means all messages are synched
// from the server.               
// This key follows the profile offset.
static const quint32 EmailKeyReceptionGenericSyncWindowInMessages = 6;
                
// Receiving weekdays: list of days as a bit flag (integer).
// 0x01 = Mon, 0x02 = Tue, 0x04 = Wed, 0x08 = Thu, 0x10 = Fri, 0x20 = Sat, 0x40 = Sun
// Value for each profile: KUTP(0x7F), SE(0x7F), MF(-1=N/A), UD
// This key follows the profile offset.
static const quint32 EmailKeyReceptionWeekDays = 7;

// default values for each profile
static const int DefaultValueKeepUpToDateReceptionDays = 0x7F;
static const int DefaultValueSaveEnergyReceptionDays = 0x7F;
static const int DefaultValueManualSyncReceptionDays = -1;
static const int DefaultUserDefinedReceptionDays = -1;

// Receive start time. Value 00:00 – 23:59 (integer 0-1439).
// Value for each profile: KUTP(480), SE(480), MF(-1=N/A), UD
// This key follows the profile offset.
static const quint32 EmailKeyReceptionDayStartTime = 8;

// default values for each profile
static const int DefaultValueKeepUpToDateReceptionStartTime = 480;
static const int DefaultValueSaveEnergyReceptionStartTime = 480;
static const int DefaultValueManualSyncReceptionStartTime = -1;
static const int DefaultUserDefinedReceptionStartTime = -1;

// Receive end time in minutes. Value 00:00 – 23:59 (integer 0-1439).
// Value for each profile: KUTP(1320), SE(1320), MF(-1=N/A), UD
// This key follows the profile offset.
static const quint32 EmailKeyReceptionDayEndTime = 9;

// default values for each profile
static const int DefaultValueKeepUpToDateReceptionEndTime = 1320;
static const int DefaultValueSaveEnergyReceptionEndTime = 1320;
static const int DefaultValueManualSyncReceptionEndTime = -1;
static const int DefaultUserDefinedReceptionEndTime = -1;

// Refresh mail, during daytime
// Integer x minutes [possible values are 5, 15, 60, 240, 0 = "when open mailbox"].
// Value for each profile: KUTP(5), SE(60), MF(-1=N/A), UD
// This key follows the profile offset.
static const quint32 EmailKeyReceptionRefreshPeriodDayTime = 10;

// default values for each profile
static const int DefaultValueKeepUpToDateReceptionRefreshPeriodDayTime = 5;
static const int DefaultValueSaveEnergyReceptionRefreshPeriodDayTime = 60;
static const int DefaultValueManualSyncReceptionRefreshPeriodDayTime = -1;
static const int DefaultUserDefinedReceptionRefreshPeriodDayTime = -1;

// Refresh during other times.
// Integer x minutes [possible values 5, 15, 60, 240, 0 = "when open mailbox"].
// This key follows the profile offset.
static const quint32 EmailKeyReceptionRefreshPeriodOther = 11;

// End of reception schedule keys


// Defines if user name for particular mailbox is hidden.
static const quint32 EmailKeyUserNameHidden = 20;

static const quint32 EmailKeyEMNState = 21;
static const quint32 EmailKeyFirstEmnReceived = 22;
static const quint32 EmailKeyEmnReceivedNotSynced = 23;
static const quint32 EmailKeyAoLastSuccessfulUpdateL = 24;
static const quint32 EmailKeyAoLastSuccessfulUpdateH = 25;
static const quint32 EmailKeyAoLastUpdateFailed = 26;
static const quint32 EmailKeyAoUpdateSuccessfulWithCurSettings = 27;
} // namespace

#endif //IPSSETTINGKEYS_H
