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

#ifndef __T_PARTIALDOWNLOADSTEP_H__
#define __T_PARTIALDOWNLOADSTEP_H__
 
#include <test\testexecutestepbase.h>
#include "emailtestutils.h"
#include "smstestutils.h"
#include <stdlib.h>
#include <msvapi.h>
#include <smuthdr.h> 
#include <iapprefs.h>
#include "t_partialdownloadspoofserver.h"
#include "t_partialdownloadimapclient.h"

/**
@SYMTestCaseID DEF06342
@SYMTestType UT
@SYMTestPriority High
@SYMDEF DEF06342
@SYMTestCaseDependencies SpoofServer.dll
@SYMTestCaseDesc IMAP Test of partial fetching of empty messages.
@SYMTestActions Sets partial download limits to 2 bytes. 
Downloads the first message which has no body and an attachment greater than 2 bytes.
Downloads a second message.
Verifies that the messages where successfully fetched.
@SYMTestExpectedResults Both messages should be fetched..
*/

class CPartialDownloadStep : public CTestStep , MImapTestEventHandler
	{
public:
	CPartialDownloadStep();
	~CPartialDownloadStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void GetDetailsFromIniFileL();
	void CreateImapAccountL();
	virtual void TestComplete(TInt aErrorCode);
	TInt GetEntryCountL();
private:
	CSpoofServer* iSpoofServer;
	CPartialDownLoadImapClient* iImapClient;
	CConsoleBase* iConsole;
	CActiveScheduler* iScheduler;
	TMsvId	iImapService;
	CEmailTestUtils* iTestUtils;
	CMsvSession* iSession;
	TInt iMessageEntryCount;
	TDummySessionObserver* iSessionObserver;
	TPtrC16 iScriptFile;
	TBool iFetchWholeMessage;
	};

 		
_LIT(KTestPartialDownLoad,"TestPartialDownLoad");
	
#endif  //__T_PARTIALDOWNLOADSTEP_H__