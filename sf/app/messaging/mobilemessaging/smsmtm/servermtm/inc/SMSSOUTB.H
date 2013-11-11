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

#ifndef __SMSSOUTB_H__
#define __SMSSOUTB_H__

#include <e32base.h>

#include <msventry.h>
#include <msvsenderroraction.h>
#include <smutset.h>
#include <msvschedulepackage.h>
#include "smssactive.h"

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
#include <ups/upsclient.h>
using namespace UserPromptService;
#endif

class CParaFormatLayer;
class CCharFormatLayer;
class CSmsSendSession;
class CMsvScheduleSend;
class CSmsEventLogger;
class CSmsEditorBuffer;

class CSmsOutboxSend : public CSmssActive
/**
@internalComponent
@released
*/
	{
public:
	enum TSmsOutboxSendState
		{
		ESmsOutboxSendStateWaiting,
		ESmsOutboxSendStateFindingOtherMessages,
		ESmsOutboxSendStateSending,
		ESmsOutboxSendStateReScheduling,
		ESmsOutboxSendStateAddLogEvent,
		ESmsOutboxSendStateGetLogEvent,
		ESmsOutboxSendStateChangeLogEvent,
		ESmsOutboxSendStateLogEntryComplete,
		ESmsOutboxSendStateMovingEntry,
		ESmsOutboxSendStateComplete,
		ESmsOutboxSendAuthoriseState
		};

public:
	static CSmsOutboxSend* NewL(CMsvServerEntry& aServerEntry, CMsvScheduleSend& aScheduleSend, RFs& aFs);
	~CSmsOutboxSend();

	const TSmsProgress& Progress(); 
	
#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
	void StartL(TRequestStatus& aStatus,const CMsvEntrySelection& aSelection, const TBool aMove, const TDesC8& aParameter, TThreadId aClientThreadId, TBool aHasCapability);
#endif	

	void Start(TRequestStatus& aStatus,const CMsvEntrySelection& aSelection, const TBool aMove, const TDesC8& aParameter);
	
protected:
	void DoSmssCancel();

private:
	void DoRunL();	

private:
	CSmsOutboxSend(CMsvServerEntry& aServerEntry, CMsvScheduleSend& aScheduleSend, RFs& aFs);
	void ConstructL();
	void SendNextHeaderL();
	void DoComplete(TInt& aStatus); //from CSmssActive
	void FindOtherMessagesL(const CMsvEntrySelection& aSelection);

	void MoveEntryL();
	void LogEntry();
	void ReScheduleFailedMessageL();
	void DoReScheduleL(const TMsvSendErrorAction* aErrorAction = NULL);

	TBool ConditionsRightForSending(TMsvSendErrorAction& rErrorAction);

// Logging Functions
	void GetLogEvent(TLogId aId);
	void AddLogEvent();
	void ChangeLogEvent();

	TBool MessageSent() const;
	TBool CanLogRecipient(const CSmsNumber& aNumber) const;
	TInt GetLogStatus() const;

	void FailOutstandingMessages(TInt aError, TInt aSendingState);
	void SendHeader();
private:
	TSmsProgress 		iProgress;

	CMsvEntrySelection* iMsvEntrySelection;
	CSmsSendSession* 	iSendSession;
	CSmsEventLogger* 	iLogger;

	CRichText* 			iRichText;
	CParaFormatLayer* 	iParaLayer;
	CCharFormatLayer* 	iCharLayer;
	CSmsEditorBuffer* 	iText;
	CSmsHeader* 		iSmsHeader;

	CMsvScheduleSend& 	iScheduleSend;

	TMsvSchedulePackage iPackage;
	TMsvEntry 			iEntry;
	TMsvId 				iCurrentMessage;
	TBool 				iMove;
	TBool 				iSentFolderExists;
	TTime 				iStartTime;
	TBool 				iCondMet;
	TInt 				iErr;
		
#if (defined SYMBIAN_USER_PROMPT_SERVICE)
	/** For Interaction with UPS server */
	RUpsSession 		iUpsSession;
	RUpsSubsession 		iUpsSubsession;
	TBool 			iHasCapability;
	TUpsDecision 		iDecision;
#endif	
	};

#endif	// __SMSSOUTB_H__
