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
// t_parse_utc_imap_step.h
// 
//

#ifndef __T_PARSE_UTC_IMAP_STEP_H__
#define __T_PARSE_UTC_IMAP_STEP_H__
 
#include <test\testexecutestepbase.h>
#include "emailtestutils.h"
#include "smstestutils.h"
#include <stdlib.h>
#include <msvapi.h>
#include <smuthdr.h> 
#include <iapprefs.h>
#include "t_parse_utc_imapserver.h"
#include "imapspoofserver.h"
#include "imapclient.h"
#include "eventhandler.h"
//
// CTestUTCParse
//

class CTestUTCParse : public CTestStep , MImapTestEventHandler
	{
public:
	CTestUTCParse();
	~CTestUTCParse();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void GetDetailsFromIniFileL();
	void WriteStoreL();
	void CreateImapAccountL();
	void PrintEntryTime(TTime* aTIme);
	void TraverseL(const TMsvId aTMsvId);
	TVerdict CheckTimeStamps();
	virtual void HandleEvent(TInt aEventCode);
	
private:
	CImapSpoofServer* iImapServer;
	CActiveImapClient* iImapClient;
	CConsoleBase* iConsole;
	CActiveScheduler* iScheduler;
	TMsvId	iImapService;
	CEmailTestUtils* iTestUtils;
	CMsvSession* iSession;
	TInt iMessageEntryCount;
	TDummySessionObserver* iSessionObserver;
	TPtrC16 iScriptFile;
	TTime iExpectedTimeStamp;
	TTime iExpectedTimeStamp2;
	TTime iActualTimeStamp;
	TTime iActualTimeStamp2;
	TBool iExpectedTimeStampIsNow;
	TBool iExpectedTimeStampIsNow2;	
	TBool iFetchWholeMessage;
	};

 		
_LIT(KTestUTCParse,"TestUTCParse");
	
#endif  //__T_PARSE_UTC_IMAP_STEP_H__