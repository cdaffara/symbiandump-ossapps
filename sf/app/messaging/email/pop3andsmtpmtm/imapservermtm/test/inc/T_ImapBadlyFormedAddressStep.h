// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_IMAPADLYFORMEDADDRESSSTEP_H__
#define __T_IMAPADLYFORMEDADDRESSSTEP_H__

#include <testexecutestepbase.h>
#include "cspoofserver.h"
#include <msvapi.h>

class CImapBadlyFormedAddressClient;
class CEmailTestUtils;
class TDummySessionObserver;

class CImapBadlyFormedAddressStep : public CTestStep, MImapTestEventHandler
	{
public:
	CImapBadlyFormedAddressStep();
	~CImapBadlyFormedAddressStep();
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
	CImapBadlyFormedAddressClient* iImapClient;
	CConsoleBase* iConsole;
	CActiveScheduler* iScheduler;
	TMsvId iImapService;
	CEmailTestUtils* iTestUtils;
	CMsvSession* iSession;
	TInt iMessageEntryCount;
	TDummySessionObserver* iSessionObserver;
	TPtrC16 iScriptFile;
	TInt iExpectedEntryCount;
	};

_LIT(KTestImapBadlyFormedAddress,"TestImapBadlyFormedAddress");

#endif  //__T_IMAPADLYFORMEDADDRESSSTEP_H__
