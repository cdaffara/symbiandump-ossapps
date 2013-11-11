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

#ifndef __T_PARSE_UTCSTEP_H__
#define __T_PARSE_UTCSTEP_H__

#include <test/testexecutestepbase.h>
#include "emailtestutils.h"
#include "smstestutils.h"
#include <stdlib.h>
#include <msvapi.h>
#include <smuthdr.h> 
 
#include "T_PARSE_UTCServer.h"


class DummyObserver : public MMsvSessionObserver
	{
	public:
		void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};

class CTestUTCParse : public CTestStep
	{
public:
	CTestUTCParse();
	~CTestUTCParse();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void WriteStoreL();
	TVerdict TestParsing();
	void TraverseL(const TMsvId aTMsvId);
	void ReadEmailL();
	void TestReadL(const TDesC& aDir, TFileName aFilename);
	void ReadDataL(CImRecvConvert* aRecvConvert, TFileName aFileName);
	TVerdict TestTime(TInt aCount,TTime* aTime);
	void ConnectToMessageServerL();
	void PrintEntryTime(TTime* aTIme);
private:
	CEmailTestUtils* iTestUtils;
	CActiveScheduler* iScheduler;
	CMsvEntryArray* iEntryArray; 
	CMsvSession* iSession;
	DummyObserver* iObserver;
	TInt iMessageEntryCount;
	CArrayFixFlat<TTime>* iTimeList;		
	};

 		
_LIT(KTestUTCParse,"TestUTCParse");
	
#endif