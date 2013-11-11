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

#ifndef SMS_RECIPIENT_SEND_H_
#define SMS_RECIPIENT_SEND_H_

#include <msvstd.h>
#include <es_sock.h>
#include <biodb.h>
#include <es_wsms.h>

#include "smssactive.h"

class CSmsMessage;
class TSmsProgress;
class CSmsEventLogger;
class CSmsHeader;

class CSmsRecipientSend : public CSmssActive
/**
@internalComponent
@released
*/
	{
public:
	enum TSmsRecipientSendState
		{
		ESmsRecipientSendWaiting,
		ESmsRecipientSendGetLogEvent,
		ESmsRecipientSendAddLogEvent,
		ESmsRecipientSendChangeLogEvent,
		ESmsRecipientSendSendingMessage
		};

public:
	~CSmsRecipientSend();
	void Start(TRequestStatus& aStatus, const TMsvEntry& aEntry, CSmsHeader& aHeader, CSmsNumber& aRcpt);
	virtual TBool AcceptMessage(const TMsvEntry& aEntry, const CSmsHeader& aHeader) const = 0;

protected:
	CSmsRecipientSend(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry);
	
protected:
	void ConstructL();
	virtual TBool AcceptBioMsgId(const TBioMsgId& aBiomsgId) const = 0;
	TInt AcceptBioType(TInt aBioType, TBioMsgIdType aBearer) const;
	void GetDefaultSendBearerL(TInt aBioType, TBioMsgIdType aBearer, TBioMsgId& rBioMsgId) const;

	void GetLogEvent();
	void AddLogEvent();

private:
	void DoRunL();
	void DoComplete(TInt& aStatus);
	void DoSmssCancel();

	virtual void SendMessageL() = 0;
	virtual void StartL() = 0;
	virtual void DoCancelSendingMessage()=0;

	void ChangeLogEvent();

protected:

	TSmsProgress& iProgress;
	TSmsRecipientSendState iState;
	TPckgBuf<TUint> iSendBuffer;

	CSmsHeader* iHeader;
	CSmsNumber* iRecipient;
	TMsvEntry	iEntry;

	RSocketServ iSession;
	RSocket iSocket;

private:
	CSmsEventLogger* iLogger;
	};

class CTextRecipientSend : public CSmsRecipientSend
/**
@internalComponent
@released
*/
	{
public:
	static CTextRecipientSend* NewL(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry);
	~CTextRecipientSend();

private:

	CTextRecipientSend(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry);
	TBool AcceptMessage(const TMsvEntry& aEntry, const CSmsHeader& aHeader) const;
	TBool AcceptBioMsgId(const TBioMsgId& aBiomsgId) const;

	void SendMessageL();
	void StartL();
	void DoCancelSendingMessage();

	void PrependNbsStringL(const TMsvEntry& aEntry, CSmsMessage& arMessage, HBufC*& rOldMessage);
	void RevertBackToOldMessageL(const TDesC& aOldMessage, CSmsMessage& arMessage);
	};

class CWapRecipientSend : public CSmsRecipientSend
/**
@internalComponent
@released
*/
	{
public:
	static CWapRecipientSend* NewL(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry);
	~CWapRecipientSend();

private:
	CWapRecipientSend(TSmsProgress& aProgress, RFs& aFs, CMsvServerEntry& aServerEntry);
	TBool AcceptMessage(const TMsvEntry& aEntry, const CSmsHeader& aHeader) const;
	TBool AcceptBioMsgId(const TBioMsgId& aBiomsgId) const;

	void SendMessageL();
	void StartL();
	void DoCancelSendingMessage();

private:
	HBufC8* iMessage8;
	TWapAddr iWapAddr;
	};

#endif
