// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SMSSENDSESSION_H__
#define __SMSSENDSESSION_H__

#include <e32base.h>
#include <msvstd.h>

#include "smssactive.h"

class TSmsProgress;
class CRichText;
class CSmsSend;
class CSmsRecipientSend;

class CSmsSendSession : public CSmssActive
/**
@internalComponent
@released
*/
	{
public:
	
	static CSmsSendSession* NewL(TSmsProgress& aProgress, CMsvServerEntry& aServerEntry, RFs& aFs, CSmsHeader& aHeader, CRichText& aRichText, TMsvEntry& aEntry);
	~CSmsSendSession();

	void SendSms(TRequestStatus& aStatus);
	TMsvId IncSms();
	TMsvId Id() {return iId;}
	void DivideMessagesL(CMsvEntrySelection& aSelection);

private:

	CSmsSendSession(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry, CSmsHeader& aHeader);
	void ConstructL(CRichText& aRichText, TMsvEntry& aEntry);
	void CreateRecipientsL();

	void DoSmssCancel();
	void DoRunL();
	void DoComplete(TInt& aStatus); //from CSmssActive

	enum TSmsSendSessionState
		{
		ESessionSending,
		ESessionNoMoreMessages,
		ESessionNoSendTypes
		} iState;

	class CSmsSendType : public CBase
		{
	public:
		static CSmsSendType* NewL(CSmsRecipientSend* aRecipientSend);
		~CSmsSendType();

		CSmsRecipientSend* iRecipientSend;
		CMsvEntrySelection* iSelection;
		TInt iCurrentMessage;

	private:
		CSmsSendType(CSmsRecipientSend* aRecipient);
		void ConstructL();
		};

	TMsvId GetNextSms(TInt& aCurType, CSmsSendType* aSendType, TInt& aCurMsg);

	CArrayPtrFlat<CSmsSendType>* iSendTypes;
	CSmsSend* iSmsSend;
	TSmsProgress& iProgress;
	TInt iCurrentSendType;
	TMsvId iId;
	CSmsHeader& iSmsHeader;
	};

#endif	// __SMSSENDSESSION_H__
