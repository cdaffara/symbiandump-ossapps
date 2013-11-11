// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__SSMTPSET_H__)
#define __SSMTPSET_H__

#if !defined (__MSVUIDS_H__)
#include <MSVUIDS.H>
#endif

#include <miutset.h>

class CMsvStore;
class RMsvWriteStream;
class RMsvReadStream;


const TUint16 KImSmtpServerSettingsVersion = 2;	// identify which version of this class has been stored
const TUint32 KSSMTPDefaultPortNumber = 25;

class TSSmtpProgress
/**
@internalComponent
@deprecated
*/
	{
public:
	enum TSSmtpProgressType {
		EStarting,
		EListening,
		EAccepting,
		EAuthorising,
		EReceiving,
		EDisconnecting,
		EDisconnected,
		ESSmtpMaxProgressValue=EDisconnected
		};

	TSSmtpProgressType	iSmtpProgress;
	TInt				iNoBytesReceived;
	TInt				iTotalMsgs;
	TInt				iErrorCode; // client side needs to be informed of an error
	};

class CImSSmtpSettings : public CImBaseEmailSettings
/**
@internalComponent
@deprecated
*/
	{
public:
	IMPORT_C CImSSmtpSettings();
	IMPORT_C ~CImSSmtpSettings();
	IMPORT_C void Reset();

	// streaming operations
	IMPORT_C void InternalizeL( RMsvReadStream&  );
	IMPORT_C void ExternalizeL( RMsvWriteStream& ) const;
	IMPORT_C void RestoreL( CMsvStore& aMessageStore );
	IMPORT_C void StoreL  ( CMsvStore& aMessageStore ) const;

	// EmailAddress
	IMPORT_C const TPtrC EmailAddress() const;
	IMPORT_C void SetEmailAddressL( const TDesC& );

	// PhoneNumber
	IMPORT_C const TPtrC PhoneNumber() const;
	IMPORT_C void SetPhoneNumberL( const TDesC& );

	IMPORT_C CImSSmtpSettings& CopyL(const CImSSmtpSettings& aCImSSmtpSettings);
	IMPORT_C TBool operator==(const CImSSmtpSettings& aCImSSmtpSettings) const;

private:
	HBufC*					iEmailAddress;  // local device user Email Address
	HBufC*					iPhoneNumber;   // local Phone Number
	};

#endif

