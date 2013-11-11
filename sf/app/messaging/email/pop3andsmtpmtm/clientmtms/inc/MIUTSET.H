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

#if !defined (__MIUTSET_H__)
#define __MIUTSET_H__

 

#include <msvuids.h>
#include <miuthdr.h>

const TInt KMaxSettingStringLength = 1024;

/** UID of an SMTP message type. */
const TUid KUidMsgTypeSMTP			= {0x10001028};	// 268439592
/** UID of a POP3 message type. */
const TUid KUidMsgTypePOP3			= {0x10001029};	// 268439593
/** UID of an IMAP4 message type. */
const TUid KUidMsgTypeIMAP4			= {0x1000102A};	// 268439594

/**
@deprecated
*/
const TUid KUidMsgTypeSMTPServer	= {0x10003923};	// 268450083


class CImBaseEmailSettings : public CBase
/**
Parent class for CImPop3Settings, CImImap4Settings, CImSmtpSettings.

This class should not be instantiated directly. It implements common email
settings such as the hostname or IP address of the server, port number,
and whether or not to use SSL.

@see CImPop3Settings
@see CImImap4Settings
@see CImSmtpSettings

@publishedAll
@released
*/
	{
public:
	IMPORT_C CImBaseEmailSettings();
	IMPORT_C virtual ~CImBaseEmailSettings();
	IMPORT_C void Reset();
	IMPORT_C const TPtrC ServerAddress() const;
	IMPORT_C void SetServerAddressL(const TDesC& aServerAddress);
	IMPORT_C TUint Port() const;
	IMPORT_C void SetPort(const TUint aPortNumber);
	IMPORT_C TBool SecureSockets() const;
	IMPORT_C void SetSecureSockets(TBool aFlag);
	IMPORT_C TBool SSLWrapper() const;
	IMPORT_C void SetSSLWrapper(TBool aFlag);
	IMPORT_C CImBaseEmailSettings& CopyL(const CImBaseEmailSettings& aCImBaseEmailSettings);
	IMPORT_C TBool operator==(const CImBaseEmailSettings& aCImBaseEmailSettings) const;
	IMPORT_C void SetBearerMobility(TBool aFlag);
	IMPORT_C TBool BearerMobility() const;

	TUint32 SettingsFlags() const;
	void SetSettingsFlags(TUint32 aSetFlags);

protected:
	enum TImBaseEmailSettings
		{
		EBaseEmailSettingsClearFlag			= 0x00000000,
		EBaseEmailSettingsSecureSockets		= 0x00000001,
		EBaseEmailSettingsSSLWrappedSockets	= EBaseEmailSettingsSecureSockets << 1,  //0x00000002,
		EBaseEmailSettingsLastUsedFlag		= EBaseEmailSettingsSSLWrappedSockets,  //0x00000002
		EBaseEmailSettingsBearerMobFlag		= 0x80000000
		};
	TUint32 iPortNumber;	// the port the TCP/IP connection be made on

	TUint32 iFlags;			// space for flags info (now and for future)

private:
	HBufC* iServerAddress;	// remote server's IP address
	};

#endif
