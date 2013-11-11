// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_MMSSETTINGS_H__
#define __T_MMSSETTINGS_H__

#include <e32base.h>
#include "msvtestutilsbase.h"
#include <mtclreg.h>
#include <msvids.h>
#include <msvuids.h>
#include <msvapi.h>
#include <e32uid.h>
#include <msvruids.h>


// to be defined by test harness
GLREF_D RTest test; 
GLREF_D void doTestsL();

// forward declarations
class CMMSTestUtils;

// globals
TInt SchedulerError;
CMMSTestUtils* theUtils = NULL;
CTrapCleanup* theCleanup = NULL;


TInt theErr;
#define REPORT_IF_ERROR(a) {theErr=a; if (theErr) test(0);}

//**********************************
// CTestScheduler
//**********************************

class CTestScheduler : public CActiveScheduler
	{
public:
	void Error (TInt aError) const;
	};

void CTestScheduler::Error( TInt anError ) const
	{
	SchedulerError = anError;
	}

//**********************************
// CSendTestUtils
//**********************************

class CMMSTestUtils : public CTestUtils
	{
public:
	static CMMSTestUtils* NewL(RTest& aTest);
protected:
	CMMSTestUtils(RTest& aTest);
public:
	void Panic(TInt aPanic);
	void Start(const TDesC& aDes);
	void Complete();

private:
	TInt iTestNum;
	};

CMMSTestUtils* CMMSTestUtils::NewL(RTest& aTest)
	{
	CMMSTestUtils* self = new(ELeave)CMMSTestUtils(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMSTestUtils::CMMSTestUtils(RTest& aTest) :
	CTestUtils(aTest)
	{
	}

void CMMSTestUtils::Panic(TInt)
	{
	test(EFalse);
	}

void CMMSTestUtils::Start(const TDesC& aDes)
	{
	if (iTestNum == 0)
		{
		test.Start(aDes);
		}
	else
		{
		test.Next(aDes);
		}

	TestStart(++iTestNum, aDes);
	}

void CMMSTestUtils::Complete()
	{
	TestFinish(iTestNum, KErrNone);
	}

//**********************************
// Global
//**********************************

void SetupSchedulerL()
	{
	CActiveScheduler::Install(new(ELeave)CTestScheduler);
	}

void CloseScheduler(TInt aRet)
	{
	if (theUtils)
		{
		if (!aRet)
			{
			theUtils->TestHarnessCompleted();
			}
		else
			{
			theUtils->TestHarnessFailed(aRet);
			}
		}
	test(aRet == KErrNone);

	delete CActiveScheduler::Current();
	}

void doMainL()
	{
	SetupSchedulerL();

	// MMS Settings test utils
	theUtils = CMMSTestUtils::NewL(test);

  	// do the tests
	doTestsL();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret == KErrNone);
	CloseScheduler(ret);
	delete theUtils;
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

#endif
