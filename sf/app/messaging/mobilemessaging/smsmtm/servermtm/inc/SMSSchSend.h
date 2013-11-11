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

#ifndef SMS_SCH_SEND_H_
#define SMS_SCH_SEND_H_

#include <smuthdr.h>
#include <msvschedulesend.h>
#include <txtrich.h>

//
//
//	CSmsScheduleSend Declaration
//
//

class CSmsScheduleSend : public CMsvScheduleSend
/**
@internalComponent
@released
*/
	{
public:
	static CSmsScheduleSend* NewL(CMsvServerEntry& aServerEntry);

protected:
	CSmsScheduleSend(CMsvServerEntry& aServerEntry);
	CMsvScheduledEntry* GetMessageL(const TMsvId aId) const;
	};

//
//
//	CSmsScheduledEntry Declaration
//
//

class CSmsScheduledEntry : public CMsvScheduledEntry
/**
@internalComponent
@released
*/
	{
public:
	static CSmsScheduledEntry* NewL(const TMsvEntry& aEntry);
	~CSmsScheduledEntry();

	TBool CanSendToAnyRecipients(const CMsvSendErrorActions& aErrorActions, TMsvSendErrorAction& aAction);
	TBool CanSendToAnyRecipients(const TMsvSendErrorAction& aAction);
	void RecipientsResetRetries();
	void RecipientsIncreaseRetries();
	void RecipientsSetFailed();
	TBool RecipientsAllSent() const;

	void RecipientsStoreL(CMsvStore& aStore) const;
	void RecipientsRestoreL(CMsvStore& aStore);

private:
	CSmsScheduledEntry(const TMsvEntry& aEntry);
	void ConstructL();
	TBool CheckRecipient(CSmsNumber& aRcpt, const TMsvSendErrorAction& aAction);

	CRichText* iRichText;
	CParaFormatLayer* iParaLayer;
	CCharFormatLayer* iCharLayer;
	CSmsHeader* iHeader;
	CArrayPtrFlat<CSmsNumber>* iNumbers;
	};


#endif
