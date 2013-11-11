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

#ifndef __CSMTPSETTINGS_H__
#define __CSMTPSETTINGS_H__

#define _MSVAPI_DONT_INCLUDE_FLOGGER_

#include <cemailaccounts.h>
#include <smtpset.h>

class CMsvServerEntry;
class CImSmtpMobilitySettings;
class CImIAPPreferences;

/**
USed in calls to SetIapL to indicate that no IAP is set and the default
SMTP settings should be used.
*/
const TUint32 KDefaultSettingsIap = -1;

/**
Stores the settings for the SMTP server MTM
*/
NONSHARABLE_CLASS(CSmtpSettings) : public CBase
	{
public:
	static CSmtpSettings* NewL(const CMsvServerEntry& aServerEntry);
	~CSmtpSettings();

	void SetIapL(TUint32 aIap);

	CImIAPPreferences& IapPrefs();
	const TPtrC ServerAddress() const;
	TUint Port() const;
	TBool SecureSockets() const;
	TBool SSLWrapper() const;
	TBool BearerMobility() const;
	const TPtrC EmailAlias() const;
	const TPtrC EmailAddress() const;
	const TPtrC ReplyToAddress() const;
	const TPtrC ReceiptAddress() const;
	TMsgOutboxBodyEncoding BodyEncoding() const;
	const TUid DefaultMsgCharSet() const;
	TBool AddVCardToEmail() const;
	TBool AddSignatureToEmail() const;
	TBool RequestReceipts() const;
	TImSMTPSendCopyToSelf SendCopyToSelf() const;
	TImSMTPSendMessageOption SendMessageOption() const;
	const TPtrC8 LoginName() const;
	const TPtrC8 Password() const;
	TBool SMTPAuth() const;
	TBool InboxLoginDetails() const;
	TInt ToCcIncludeLimit() const;
	TPtrC8 TlsSslDomain() const;
	const CImSmtpSettings& SmtpSettings() const;

private:
	CSmtpSettings(const CMsvServerEntry& aServerEntry);
	void ConstructL(const CMsvServerEntry& aServerEntry);

private:
	CImSmtpSettings* iDefaultSettings;
	CImSmtpMobilitySettings* iIapSettings;
	CImIAPPreferences* iIapPrefs;

	TSmtpAccountId iSmtpAccountId;
	};

#endif //__CSMTPSETTINGS_H__
