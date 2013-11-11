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

#include "TestCalInterimApiDCStep.h"
#include "TestCalInterimApiDefines.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataformat.h>

_LIT8(KAlarmExportTestGUID1, "AlarmExportTestGUID1");
_LIT8(KAlarmExportTestGUID2, "AlarmExportTestGUID2");


CTestCalInterimApiDCTest::~CTestCalInterimApiDCTest()
/**
 * Destructor
 */
	{
	delete iEntryView;
	delete iSession;
	}

CTestCalInterimApiDCTest::CTestCalInterimApiDCTest()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalInterimApiDCTest);
	}

TVerdict CTestCalInterimApiDCTest::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestCalInterimApiDCTest::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalInterimApiDCTest::doTestStepL()
	{
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(scheduler);  // Push 1
	CActiveScheduler::Install(scheduler);

	iSession  = CCalSession::NewL();
	
	CTestCalInterimApiCallbackForAlarms* alarmCallback = CTestCalInterimApiCallbackForAlarms::NewL(this);
	CleanupStack::PushL(alarmCallback); // Push 2

	OpenSessionFileL();
	iEntryView = CCalEntryView::NewL(*iSession,*alarmCallback);
	
	SetTestStepResult(EPass);
	
	CActiveScheduler::Add(alarmCallback);
	CActiveScheduler::Start();
	
	// For 9.2 onwards, we are going to read two entries that were stored
	// using 9.1
	RPointerArray<CCalEntry> entryArray;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entryArray)); // Push 3
	
	//The first entry has GUID KAlarmExportTestGUID1
	iEntryView->FetchL(KAlarmExportTestGUID1, entryArray);

	//The second entry has GUID KAlarmExportTestGUID2
	iEntryView->FetchL(KAlarmExportTestGUID2, entryArray);
	
	// check the Entry count
	TInt entryCount = entryArray.Count();
	if (entryCount != 2)
		{
		_LIT(KErrReadingEntries, "Did not read all entries from file");
		INFO_PRINTF1(KErrReadingEntries);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// entry 1 should have no alarm
	CCalAlarm* alarm1 = entryArray[0]->AlarmL();
	if (NULL != alarm1)
		{
		_LIT(KUnexpectedAlarm, "Entry had alarm when not expected");
		INFO_PRINTF1(KUnexpectedAlarm);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		
	// entry 2 had a "normal" alarm set
	CCalAlarm* alarm2 = entryArray[1]->AlarmL();
	if (NULL == alarm2)
		{
		_LIT(KUnexpectedNOAlarm, "Entry had no alarm when one expected");
		INFO_PRINTF1(KUnexpectedNOAlarm);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else
		{
		if (alarm2->AlarmSoundNameL() != _L("c:\\mysound"))
			{
			_LIT(KWrongAlarm, "Entry had wrong alarm");
			INFO_PRINTF1(KWrongAlarm);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		}
		
	CleanupStack::PopAndDestroy(3);

	
	return TestStepResult();
	}

	

void CTestCalInterimApiDCTest::OpenSessionFileL()
	{
	TBuf<MAX_TEXT_MESSAGE> buffer;
	buffer.Format(KCreateNewAgendaFile, &KAgendaRestoreFile);
	INFO_PRINTF1(buffer);
	iSession->OpenL(KAgendaRestoreFile());
	}


void CTestCalInterimApiDCTest::CloseAndDeleteRPtrArrayEntries(TAny* aObject)	
	{
	RPointerArray<CCalEntry>* array=REINTERPRET_CAST(RPointerArray<CCalEntry>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

void CTestCalInterimApiDCTest::CloseAndDeleteRPtrArrayAlarms(TAny* aObject)	
	{
	RPointerArray<CCalAlarm>* array=REINTERPRET_CAST(RPointerArray<CCalAlarm>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

CTestCalInterimApiCallbackForAlarms* CTestCalInterimApiCallbackForAlarms::NewL(CTestCalInterimApiDCTest* aParent)
	{
	CTestCalInterimApiCallbackForAlarms* self = new (ELeave) CTestCalInterimApiCallbackForAlarms(aParent);
	return self;
	}
	
CTestCalInterimApiCallbackForAlarms::~CTestCalInterimApiCallbackForAlarms()
	{
	Cancel();
	}

CTestCalInterimApiCallbackForAlarms::CTestCalInterimApiCallbackForAlarms(CTestCalInterimApiDCTest* aParent)
	: CActive(EPriorityHigh), iTestCalInterimApiSuiteStep(aParent)
	{
	}
    
void CTestCalInterimApiCallbackForAlarms::Start()
	{
	if (!IsActive())
		{
		SetActive();
		}	

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrNone);
	}

void CTestCalInterimApiCallbackForAlarms::Progress(TInt /*aPercentageCompleted*/)
	{
	}
	
void CTestCalInterimApiCallbackForAlarms::Completed(TInt /*aComplete*/)
	{
	Start();
	}
	
TBool CTestCalInterimApiCallbackForAlarms::NotifyProgress()
	{
	return ETrue;
	}
	
 void CTestCalInterimApiCallbackForAlarms::DoCancel()
	{
	Close();
	}
	
void CTestCalInterimApiCallbackForAlarms::RunL()
	{
	Close();
	}
	
void CTestCalInterimApiCallbackForAlarms::Close()
	{
	CActiveScheduler::Stop();
	}


