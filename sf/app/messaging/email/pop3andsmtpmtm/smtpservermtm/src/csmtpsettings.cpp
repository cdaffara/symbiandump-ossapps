// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "csmtpsettings.h"
#include <cimsmtpmobilitysettings.h>
#include <iapprefs.h>
#include <msventry.h>

/**
Factory constructor
*/
CSmtpSettings* CSmtpSettings::NewL(const CMsvServerEntry& aServerEntry)
	{
	CSmtpSettings* self = new (ELeave) CSmtpSettings(aServerEntry);
	CleanupStack::PushL(self);
	self->ConstructL(aServerEntry);
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor
*/
CSmtpSettings::CSmtpSettings(const CMsvServerEntry& aServerEntry) :
 iSmtpAccountId(aServerEntry.Entry().MtmData2())
	{
	}

/**
Second phase constructor
*/
void CSmtpSettings::ConstructL(const CMsvServerEntry& aServerEntry)
	{
	CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();

	iDefaultSettings = new(ELeave) CImSmtpSettings;
	iIapPrefs= CImIAPPreferences::NewLC();
	CleanupStack::Pop();

	TSmtpAccount id;
	id.iSmtpAccountId = iSmtpAccountId;
	id.iSmtpAccountName = aServerEntry.Entry().iDetails;
	id.iSmtpService = aServerEntry.Entry().iServiceId;
	id.iRelatedService = aServerEntry.Entry().iRelatedId;

	emailAccounts->LoadSmtpSettingsL(id, *iDefaultSettings);
	emailAccounts->LoadSmtpIapSettingsL(id, *iIapPrefs);

 	CleanupStack::PopAndDestroy(emailAccounts);
	}

/**
Destructor
*/
CSmtpSettings::~CSmtpSettings()
	{
	delete iDefaultSettings;
	delete iIapSettings;
	delete iIapPrefs;
	}

/**
Sets the IAP currently in use.
The class will be updated to use the settings specific to that IAP

@param aIap IAP in use
*/
void CSmtpSettings::SetIapL(TUint32 aIap)
	{
	delete iIapSettings;
	iIapSettings = NULL;

	if (aIap != KDefaultSettingsIap)
		{
		CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();

		CImSmtpMobilitySettings* newSettings = CImSmtpMobilitySettings::NewLC();

		if (emailAccounts->LoadSmtpMobilitySettingsL(iSmtpAccountId, aIap, *newSettings))
			{
			CleanupStack::Pop(newSettings);
			iIapSettings = newSettings;
			}
		else
			{
			CleanupStack::PopAndDestroy(newSettings);
			}

		CleanupStack::PopAndDestroy(emailAccounts);
		}
	}

/**
Get the IAP preferences

@return IAP preferences
*/
CImIAPPreferences& CSmtpSettings::IapPrefs()
	{
	return *iIapPrefs;
	}

/**
Get server address

@see CImSmtpSettings::ServerAddress()
@see CImSmtpMobilitySettings::ServerAddress()

@return Server address
*/
const TPtrC CSmtpSettings::ServerAddress() const
	{
	if (iIapSettings)
		{
		return iIapSettings->ServerAddress();
		}
	else
		{
		return iDefaultSettings->ServerAddress();
		}
	}

/**
Get TCP port

@see CImSmtpSettings::Port()
@see CImSmtpMobilitySettings::Port()

@return TCP port
*/
TUint CSmtpSettings::Port() const
	{
	if (iIapSettings)
		{
		return iIapSettings->Port();
		}
	else
		{
		return iDefaultSettings->Port();
		}	
	}

/**
Get secure sockets flag

@see CImSmtpSettings::SecureSockets()
@see CImSmtpMobilitySettings::SecureSockets()

@return ETrue if secure sockets flag is set, EFalse if not
*/
TBool CSmtpSettings::SecureSockets() const
	{
	if (iIapSettings)
		{
		return iIapSettings->SecureSockets();
		}
	else
		{
		return iDefaultSettings->SecureSockets();
		}
	}

/**
Get SSL Wrapper flag

@see CImSmtpSettings::SSLWrapper()
@see CImSmtpMobilitySettings::SSLWrapper()

@return ETrue if SSL wrapper flag is set, EFalse if not
*/
TBool CSmtpSettings::SSLWrapper() const
	{
	if (iIapSettings)
		{
		return iIapSettings->SSLWrapper();
		}
	else
		{
		return iDefaultSettings->SSLWrapper();
		}
	}

/**
Get bearer mobility flag

@see CImSmtpSettings::BearerMobility()

@return ETrue if bearer mobility flag is set, EFalse if not
*/
TBool CSmtpSettings::BearerMobility() const
	{
	return iDefaultSettings->BearerMobility();
	}

/**
Get email alias

@see CImSmtpSettings::EmailAlias()
@see CImSmtpMobilitySettings::EmailAlias()

@return Email alias
*/
const TPtrC CSmtpSettings::EmailAlias() const
	{
	if (iIapSettings)
		{
		return iIapSettings->EmailAlias();
		}
	else
		{
		return iDefaultSettings->EmailAlias();
		}
	}

/**
Get email address

@see CImSmtpSettings::EmailAddress()
@see CImSmtpMobilitySettings::EmailAddress()

@return Email address
*/
const TPtrC CSmtpSettings::EmailAddress() const
	{
	if (iIapSettings)
		{
		return iIapSettings->EmailAddress();
		}
	else
		{
		return iDefaultSettings->EmailAddress();
		}
	}

/**
Get reply to address

@see CImSmtpSettings::ReplyToAddress()

@return Reply to address
*/
const TPtrC CSmtpSettings::ReplyToAddress() const
	{
	return iDefaultSettings->ReplyToAddress();
	}

/**
Get receipt address

@see CImSmtpSettings::ReceiptAddress()

@return Receipt address
*/
const TPtrC CSmtpSettings::ReceiptAddress() const
	{
	return iDefaultSettings->ReceiptAddress();
	}

/**
Get body encoding

@see CImSmtpSettings::BodyEncoding()

@return Body encoding
*/
TMsgOutboxBodyEncoding CSmtpSettings::BodyEncoding() const
	{
	return iDefaultSettings->BodyEncoding();
	}

/**
Get default message charset

@see CImSmtpSettings::DefaultMsgCharSet()
@see CImSmtpMobilitySettings::DefaultMsgCharSet()

@return Default message charset
*/
const TUid CSmtpSettings::DefaultMsgCharSet() const
	{
	if (iIapSettings)
		{
		return iIapSettings->DefaultMsgCharSet();
		}
	else
		{
		return iDefaultSettings->DefaultMsgCharSet();
		}
	}

/**
Get send copy to self option

@see CImSmtpSettings::SendCopyToSelf()

@return Send copy to self option
*/
TImSMTPSendCopyToSelf CSmtpSettings::SendCopyToSelf() const
	{
	return iDefaultSettings->SendCopyToSelf();
	}

/**
Get login name

@see CImSmtpSettings::LoginName()
@see CImSmtpMobilitySettings::LoginName()

@return Login name
*/
const TPtrC8 CSmtpSettings::LoginName() const
	{
	if (iIapSettings)
		{
		return iIapSettings->LoginName();
		}
	else
		{
		return iDefaultSettings->LoginName();
		}
	}

/**
Get password

@see CImSmtpSettings::Password()
@see CImSmtpMobilitySettings::Password()

@return Password
*/
const TPtrC8 CSmtpSettings::Password() const
	{
	if (iIapSettings)
		{
		return iIapSettings->Password();
		}
	else
		{
		return iDefaultSettings->Password();
		}
	}

/**
Get SMTP authentication flag

@see CImSmtpSettings::()
@see CImSmtpMobilitySettings::SMTPAuth()

@return ETrue if SMTP authentication flag is set, EFalse if not
*/
TBool CSmtpSettings::SMTPAuth() const
	{
	if (iIapSettings)
		{
		return iIapSettings->SMTPAuth();
		}
	else
		{
		return iDefaultSettings->SMTPAuth();
		}
	}

/**
Get TLS/SSL domain name

@see CImSmtpSettings::TlsSslDomain()
@see CImSmtpMobilitySettings::TlsSslDomain()

@return TLS/SSL domain name
*/
TPtrC8 CSmtpSettings::TlsSslDomain() const
	{
	if (iIapSettings)
		{
		return iIapSettings->TlsSslDomain();
		}
	else
		{
		return iDefaultSettings->TlsSslDomain();
		}
	}

const CImSmtpSettings& CSmtpSettings::SmtpSettings() const
	{
	return *iDefaultSettings;
	}
