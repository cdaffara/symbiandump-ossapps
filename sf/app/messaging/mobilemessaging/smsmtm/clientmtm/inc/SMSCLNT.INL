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
//

#include <msvuids.h>
#include <smcmmain.h>

/** 
Gets the message header for a message context.

The current context must be set to a message entry with type KUidMsvMessageEntryValue.
If the current context is not set, or is set to an entry not of type
KUidMsvMessageEntryValuethen a panic will occur.

The message header object is the SMS MTM encapsulation of an SMS message. The
contents of the message header has been extracted from the current context.

@return
The (non-const) message header object.

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	2
The current context was not of type KUidMsvMessageEntry (debug only).

@see	CSmsHeader
*/
inline CSmsHeader& CSmsClientMtm::SmsHeader()
	{
	__ASSERT_DEBUG(iMsvEntry, Panic(ESmscEntryNotSet));
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvMessageEntry, Panic(ESmscWrongContextType));
	return *iSmsHeader;
	}

/** 
Gets the (const) message header for a message context.

The current context must be set to a message entry with type KUidMsvMessageEntryValue.
If the current context is not set, or is set to an entry not of type
KUidMsvMessageEntryValuethen a panic will occur.

The message header object is the SMS MTM encapsulation of an SMS message. The
contents of the message header has been extracted from the current context.

@return
The (const) message header object.

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	2
The current context was not of type KUidMsvMessageEntry (debug only).

@see	CSmsHeader
*/
inline const CSmsHeader& CSmsClientMtm::SmsHeader() const
	{
	__ASSERT_DEBUG(iMsvEntry, Panic(ESmscEntryNotSet));
	__ASSERT_DEBUG(iMsvEntry->Entry().iType==KUidMsvMessageEntry, Panic(ESmscWrongContextType));
	return *iSmsHeader;
	}

/** 
Gets the current SMS service settings.

The current context must be set. If the current context is not set then a panic
will occur.

The SMS service settings must have been set or a panic will occur. The 
CSmsClientMtm::RestoreServiceAndSettingsL API can restore the SMS service 
settings. This also sets the SMS service ID.

@return
The current (non-const) SMS service settings.

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	11
The SMS Service settings have not been set (debug only).

@see	CSmsClientMtm::RestoreServiceAndSettingsL
*/
inline CSmsSettings& CSmsClientMtm::ServiceSettings()
	{
	__ASSERT_DEBUG(iMsvEntry, Panic(ESmscEntryNotSet));
	__ASSERT_DEBUG(iServiceSettings, Panic(ESmscSettingsNotSet));
	return *iServiceSettings;
	}

/**
Gets the current (const) SMS service settings.

The current context must be set. If the current context is not set then a panic
will occur.

The SMS service settings must have been set or a panic will occur. The 
CSmsClientMtm::RestoreServiceAndSettingsL API can restore the SMS service 
settings. This also sets the SMS service ID.

@return
The current (const) SMS service settings.

@panic	SMCM	1
The current context has not been set (debug only).

@panic	SMCM	11
The SMS Service settings have not been set (debug only).

@see	CSmsClientMtm::RestoreServiceAndSettingsL
*/
inline const CSmsSettings& CSmsClientMtm::ServiceSettings() const
	{	
	__ASSERT_DEBUG(iMsvEntry, Panic(ESmscEntryNotSet));
	__ASSERT_DEBUG(iServiceSettings, Panic(ESmscSettingsNotSet));
	return *iServiceSettings;
	}

/**
Gets the ID of the current SMS service.

If there is no current SMS service, the ID will be a value of 0. The 
CSmsClientMtm::RestoreServiceAndSettingsL API can set the SMS service IS. This 
also restores the SMS service settings.

@return
The ID for the current SMS service

@see	CSmsClientMtm::RestoreServiceAndSettingsL
*/
inline TInt CSmsClientMtm::ServiceId() const
	{
	return iServiceId;
	}
