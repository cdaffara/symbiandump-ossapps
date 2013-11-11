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

#ifndef __CIMSMTPMOBILITYSETTINGS_H__
#define __CIMSMTPMOBILITYSETTINGS_H__

#include <e32base.h>

class CImSmtpSettings;

/**
Configuration settings for an SMTP mobility account

A SMTP mobility account allows different send settings to be used by SMTP
accounts depending on what access point is being used by the SMTP connection.
When a connection is made for a SMTP account, its associated list of SMTP
mobility accounts is examined to see which, if any, is linked to the bearer
that will be used for the connection. If one is found, then the settings for
the SMTP mobility account are used for the connection and subsequent email
sending in preference to the settings for the SMTP account.

@publishedPartner
@released
*/
class CImSmtpMobilitySettings : public CBase
	{
public:
	IMPORT_C static CImSmtpMobilitySettings* NewL();
	IMPORT_C static CImSmtpMobilitySettings* NewLC();
	~CImSmtpMobilitySettings();

	IMPORT_C CImSmtpMobilitySettings* CopyL();
	IMPORT_C CImSmtpMobilitySettings* CopyLC();
	IMPORT_C void Reset();
	IMPORT_C const TPtrC ServerAddress() const;
	IMPORT_C void SetServerAddressL(const TDesC& aServerAddress);
	IMPORT_C TUint Port() const;
	IMPORT_C void SetPort(const TUint aPortNumber);
	IMPORT_C TBool SecureSockets() const;
	IMPORT_C void SetSecureSockets(TBool aFlag);
	IMPORT_C TBool SSLWrapper() const;
	IMPORT_C void SetSSLWrapper(TBool aFlag);
	IMPORT_C const TPtrC EmailAlias() const;
	IMPORT_C void SetEmailAliasL(const TDesC& aEmailAlias);
	IMPORT_C const TPtrC EmailAddress() const;
	IMPORT_C void SetEmailAddressL(const TDesC& aEmailAddress);
	IMPORT_C const TUid DefaultMsgCharSet() const;
	IMPORT_C void SetDefaultMsgCharSet(TUid aDefaultMsgCharSet);
	IMPORT_C const TPtrC8 LoginName() const;
	IMPORT_C void SetLoginNameL(const TDesC8& aLoginName);
	IMPORT_C const TPtrC8 Password() const;
	IMPORT_C void SetPasswordL(const TDesC8& aPassword);
	IMPORT_C TBool SMTPAuth() const;
	IMPORT_C void SetSMTPAuth(TBool aFlag);
	IMPORT_C void SetTlsSslDomainL(const TDesC8& aFolderPath);
	IMPORT_C TPtrC8 TlsSslDomain() const;

	void SetSmtpSettings(CImSmtpSettings* aSettings);
	CImSmtpSettings& SmtpSettings() const;

private:
	CImSmtpMobilitySettings();
	void ConstructL();

private:
	CImSmtpSettings* iSmtpSettings;
	};

#endif //__CIMSMTPMOBILITYSETTINGS_H__
