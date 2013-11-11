// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_SMSREPLYTOSTEP_H__
#define __T_SMSREPLYTOSTEP_H__
 
#include <test\testexecutestepbase.h>
#include <stdlib.h>
#include "smstestutils.h"
#include <es_sock.h>

#include "t_smsreplytoserver.h"


class EntryObserver : public MMsvEntryObserver
	{
	public:
		virtual void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);		
	};
	
class SessionObserver : public MMsvSessionObserver
	{
	public:
		virtual void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/){};
	};

class CSmsReplyToStep : public CTestStep
	{
public:
	CSmsReplyToStep();
	~CSmsReplyToStep();
	void InitialiseReplyToStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void InitialiseSimTsyL();
	void InitialiseTestL();
	void StartWatcherL();
	TBool WatchersAlreadyRunningL();
	void CreateMessageL();
	void SetRecipientsL(CSmsHeader& aHeader);
	void SendMessageL();
	TBool CheckMessageL();
private:
	CSmsTestUtils* iTestUtils;
	EntryObserver* iEntryObserver;
	CMsvSession* iSession;
	SessionObserver* iSessionObserver;
	CMsvEntry* iEntry;
	CActiveScheduler* iScheduler;
	CTestUtilsWatcherStarter* iWatchers;
	CMsvOperation* iOperation;
	TMsvId iMessageId;
	CTestActive* iTestActive;
	};
 	
_LIT(KSmsReplyToStep1,"SmsReplyToStep1");
	
#endif  // __T_SMSREPLYTOSTEP_H__