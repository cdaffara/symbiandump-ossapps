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

#ifndef __SMSSSEND_H__
#define __SMSSSEND_H__

#include <msvstd.h> 
#include "smssactive.h"

class CSmsHeader;
class TSmsProgress;
class CSmsRecipientSend;
class CSmsEventLogger;

class CSmsSend : public CSmssActive
/**
@internalComponent
@released
*/
	{
public:
		
	static CSmsSend* NewL(TSmsProgress& aProgress, CMsvServerEntry& aServerEntry, RFs& aFs, CSmsHeader& aHeader, CRichText& aRichText, TMsvEntry& aEntry);
	~CSmsSend();

	void Start(TRequestStatus& aStatus, const TMsvId aMsvId, CSmsRecipientSend* aRecipientSend);

private:
	CSmsSend(TSmsProgress& aProgress,CMsvServerEntry& aServerEntry, RFs& aFs, CSmsHeader& aHeader, CRichText& aRichText, TMsvEntry& aEntry);
	void OpenEntryL(const TMsvId aMsvId);
	void SendNextRecipientL();
	void UpdateAfterRecipientSentL(const TInt aError);
	void UpdateEntryAfterAllSentL();

private:	// method from CSmssActive

	virtual void DoRunL();			
	virtual void DoSmssCancel();
	virtual void DoComplete(TInt& aStatus);

private:

	TMsvId iSmsServiceId;
	TSmsProgress& iProgress;
	TMsvEntry& iEntry;
	CSmsHeader& iSmsHeader;
	CRichText& iRichText;

	enum TSmsSendState
		{
		ESmsSendWaiting,
		ESmsSendOpeningEntry,
		ESmsSendSendingEntry,
		ESmsSendUpdatingRecipient
		} iState;

	CSmsRecipientSend* iRecipientSend;
	CSmsEventLogger* iLogger;
	TBool iDeliveryReports;	
	TInt iSentRecipients;
	TInt iLastSentIndex;
	// Contains Failed status of a Recipient in a list of recipients.
	TInt iRecipientFailedStatus ;

	};

#endif	// __SMSSSEND_H__
