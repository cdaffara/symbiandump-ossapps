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

#ifndef __T_DEF059571_IMAP_MISSINGOFFSETSTEP_H__
#define __T_DEF059571_IMAP_MISSINGOFFSETSTEP_H__
 
#include <test\testexecutestepbase.h>
#include "emailtestutils.h"
#include "smstestutils.h"
#include <stdlib.h>
#include <msvapi.h>
#include <smuthdr.h> 
#include <iapprefs.h>
#include "t_def059571_imap_missingoffsetserver.h"
#include "t_def059571_imap_client.h"

/**
@SYMTestCaseID DEF059571
@SYMTestType UT
@SYMTestPriority High
@SYMDEF DEF059571
@SYMTestCaseDependencies SpoofServer.dll
@SYMTestCaseDesc IMAP Test of fetch response with no offset parameter and html content
@SYMTestActions 
Downloads an email message from spoof server simulating a fetch response with no offset 
parameter and HTML content bodytext, 1st character of which is "<"
Verifies that the messages where successfully fetched.
@SYMTestExpectedResults The message should be correctly fetched.
*/

//
// CTestImapMissingOffsetStep
//

class CTestImapMissingOffsetStep : public CTestStep , MImapTestEventHandler
	{
public:
	CTestImapMissingOffsetStep();
	~CTestImapMissingOffsetStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void GetDetailsFromIniFileL();
	void CreateImapAccountL();
	void PrintEntryTime(TTime* aTIme);
	TInt GetEntryCountL();
	virtual void TestComplete(TInt aErrorCode);
	
private:
	CSpoofServer* iSpoofServer;
	CActiveImapClient* iImapClient;
	CConsoleBase* iConsole;
	CActiveScheduler* iScheduler;
	TMsvId	iImapService;
	CEmailTestUtils* iTestUtils;
	CMsvSession* iSession;
	TInt iMessageEntryCount;
	TDummySessionObserver* iSessionObserver;
	TPtrC16 iScriptFile;
	TInt iExpectedEntryCount;
	};

 		
_LIT(KTestImapMissingOffset,"TestImapMissingOffset");
	
#endif  //__T_DEF059571_IMAP_MISSINGOFFSETSTEP_H__