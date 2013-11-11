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

#ifndef __T_SMTP_UTC_STEP_H__
#define __T_SMTP_UTC_STEP_H__
 
#include <test/testexecutestepbase.h>
#include <stdlib.h>
#include "emailtestutils.h"
#include <cemailaccounts.h>

#include "t_smtp_utc_server.h"

class DummyObserver : public MMsvSessionObserver
	{
	public:
		virtual void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};


class CSmtpUtcEntryStep : public CTestStep
	{
public:
	CSmtpUtcEntryStep();
	~CSmtpUtcEntryStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	TMsvId iMessageId;
	CActiveScheduler* iScheduler;
	CEmailTestUtils* iTestUtils;
	CMsvSession* iSession;
	DummyObserver* iObserver;
	TSmtpAccount iSmtpAccountId;
	CTestActive* iTestActive;
private:
	void CreateServiceAndAccountL();
	void CreateMessageL();
	void SendMessageL();
	TVerdict TestDateL();
	TVerdict RunTestL();
	};
 	
_LIT(KSmtpUtcEntryStep,"SmtpUtcEntryStep");
	
#endif  //__T_SMTP_UTC_STEP_H__