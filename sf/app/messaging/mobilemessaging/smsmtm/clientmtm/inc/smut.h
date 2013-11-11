// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// smut.h 
//
#ifndef __SMUT_H__
#define __SMUT_H__

#include <msvstd.h>
#include <msvids.h>
#include <barsc.h>

class CSmsMessage;
class CContactItemField;
class TResourceReader;
class CMsvSession;
class CMsvServerEntry;
class CMsvEntry;
class CSmsSettings;

/**
The SMS MTM UID.

Used to identify the MTMs used for handling SMS messages.

@publishedAll
@released
*/
const TUid KUidMsgTypeSMS			= {0x1000102C};

/**
The default maximum length used when extracting a description for an SMS message.

Used as default value of aMaxLength in TSmsUtilities::GetDescription. This
function is used to extract a string to used as a suitable description field in
the message index for a particular SMS message.

The CSmsSettings provides accessors to get and set the preferred maximum
description length.

@deprecated
This should not really be used outside of the SMS client MTM. The value provided
by the CSmsSettings class should be used.

@see	CSmsSettings::GetDescriptionLength
*/
const TInt KSmsDescriptionLength	= 32;

/**
The default maximum length used when extracting the sender/recipiant details for
an SMS message.

Used as default value of aMaxLength in TSmsUtilities::GetDetails. This function
is used to extract a string to used as the details field in the message index
for a particular SMS message.

@deprecated
This should really be used at all.
*/
const TInt KSmsDetailsLength		= 32;

_LIT(KSmsResourceFile, "\\resource\\messaging\\SMSS.RSC");

/**
SMS Messaging utility functions.

@publishedAll
@released
*/
class TSmsUtilities
	{
public:

/**
Special SMS Message Indication Type.

A special SMS message indication is used to notify of waiting services, for
instance a voicemail message.

The TSmsUtilitiesSpecialMessageType enum represents the flags for the known
indication types.
*/
	enum TSmsUtilitiesSpecialMessageType
		{
/**
Voice message waiting.
*/
		EVoiceMessageWaiting = 0,
/**
Fax message waiting.
*/
		EFaxMessageWaiting,
/**
E-mail message waiting.
*/
		EEmailMessageWaiting,
/**
Other message-type waiting (see 3GPP TS 23.038 [9] for definition of "other").
*/
		EOtherMessageWaiting,
/**
Mask used to obtain the mesasge indication type from the SMS data.
*/
		ESpecialMessageTypeMask = 0x7F
		};

	IMPORT_C static void ServiceIdL(CMsvServerEntry& aEntry, TMsvId& aFirstId, TUid aMtm = KUidMsgTypeSMS, CMsvEntrySelection* aServiceIds = NULL);
	IMPORT_C static void ServiceIdL(CMsvEntry& aEntry, TMsvId& aFirstId, TUid aMtm = KUidMsgTypeSMS, CMsvEntrySelection* aServiceIds = NULL);
	IMPORT_C static void ServiceIdL(CMsvSession& aSession, TMsvId& aFirstId, TUid aMtm = KUidMsgTypeSMS, CMsvEntrySelection* aServiceIds = NULL); //more efficient version of the previous one

	IMPORT_C static void PopulateMsgEntry(TMsvEntry& aEntry, const CSmsMessage& aMessage, TMsvId aServiceId, TUid aMtm = KUidMsgTypeSMS);
	IMPORT_C static void PopulateMsgEntry(TMsvEntry& aEntry, const CSmsMessage& aMessage, TMsvId aServiceId, const CSmsSettings& aSettings, TUid aMtm = KUidMsgTypeSMS);

	IMPORT_C static TInt GetDetails(RFs& aFs, const CSmsMessage& aMessage, TDes& aDetails, TInt aMaxLength = KSmsDetailsLength);
	IMPORT_C static TInt GetDetails(RFs& aFs, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength = KSmsDetailsLength);
	IMPORT_C static TInt GetDescription(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength = KSmsDescriptionLength);

	IMPORT_C static RResourceFile OpenResourceFileL(RFs& aFs);
	IMPORT_C static void ReadResourceStringL(RResourceFile aResourceFile, TInt aResourceId, TDes& aString);

private:
	static void CompareEntryL(const TMsvEntry& aEntry, TUid aMtm, TMsvId& aFirstId, CMsvEntrySelection* aServiceIds);
	static void Replace(const TDesC& aOld, const TDesC& aNew, TDes& aString);

	static TBool DoGetDescriptionL(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength);
	static void ExtractDescriptionFromMessage(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength);

	static TBool ValidGsmNumber(const TDesC& aTelephone);
	};

#endif	// __SMUT_H__
