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

#include "TestCalInterimApiDCCreateEntries.h"
#include "TestCalInterimApiDefines.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataformat.h>

_LIT8(KAlarmExportTestGUID1, "AlarmExportTestGUID1");
_LIT8(KAlarmExportTestGUID2, "AlarmExportTestGUID2");
_LIT8(KAlarmExportTestGUID3, "AlarmExportTestGUID3");

CTestCalInterimApiDCCreateEntries::~CTestCalInterimApiDCCreateEntries()
/**
 * Destructor
 */
	{
	delete iEntryView;
	delete iSession;
	}

CTestCalInterimApiDCCreateEntries::CTestCalInterimApiDCCreateEntries()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalInterimApiDCCreateEntries);
	}

TVerdict CTestCalInterimApiDCCreateEntries::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Start Alarm Exporting/Importing Tests"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestCalInterimApiDCCreateEntries::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalInterimApiDCCreateEntries::doTestStepL()
	{
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(scheduler);  
	CActiveScheduler::Install(scheduler);

	iSession  = CCalSession::NewL();
	
	CTestCalInterimApiDCCallbackForAlarms* alarmCallback = CTestCalInterimApiDCCallbackForAlarms::NewL(this);
	CleanupStack::PushL(alarmCallback); 

	OpenSessionFileL();
	iEntryView = CCalEntryView::NewL(*iSession,*alarmCallback);
	
	SetTestStepResult(EPass);
	
	CActiveScheduler::Add(alarmCallback);
	CActiveScheduler::Start();
	
	
	// We have an entry view now. We will create some entries, then export 
	// them.
	RPointerArray<CCalEntry> firstEntryArray;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &firstEntryArray));

	firstEntryArray.Reset();
	TUint seqNo = 1;
	TInt numEntriesToBeStored = 0;
	
	//
	// Entry with no alarm
	//
	HBufC8* pGUID1= KAlarmExportTestGUID1().AllocLC();
	CCalEntry* entry1 = CCalEntry::NewL(CCalEntry::EAppt, pGUID1,
	                                    CCalEntry::EMethodAdd, seqNo);
	CleanupStack::PushL(entry1);                                   
	firstEntryArray.AppendL(entry1);
	CleanupStack::Pop(entry1);

	CleanupStack::Pop(pGUID1); // entry takes ownership
	TDateTime entryStart1(2006, EMarch, 8, 10, 0, 0, 0);
	TCalTime  calStart1;
	calStart1.SetTimeLocalL(entryStart1);
	TDateTime entryStop1(2006, EMarch, 8, 11, 0, 0, 0);
	TCalTime calStop1;
	calStop1.SetTimeLocalL(entryStop1);
	entry1->SetStartAndEndTimeL(calStart1, calStop1);

	numEntriesToBeStored++;
	
	//									
	// Entry with basic alarm (sound) only
	//
	seqNo++;
	HBufC8* pGUID2= KAlarmExportTestGUID2().AllocLC();
	CCalEntry* entry2 = CCalEntry::NewL(CCalEntry::EAppt, pGUID2,
	                                    CCalEntry::EMethodAdd, seqNo);
	CleanupStack::PushL(entry2);
	firstEntryArray.AppendL(entry2); 
	CleanupStack::Pop(entry2);                                   
	CleanupStack::Pop(pGUID2); // entry takes ownership
	TDateTime entryStart2(2006, EMarch, 7, 10, 0, 0, 0);
	TCalTime  calStart2;
	calStart2.SetTimeLocalL(entryStart2);
	TDateTime entryStop2(2006, EMarch, 7, 11, 0, 0, 0);
	TCalTime calStop2;
	calStop2.SetTimeLocalL(entryStop2);
	entry2->SetStartAndEndTimeL(calStart2, calStop2);

	CCalAlarm* alarm2 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm2);
	alarm2->SetAlarmSoundNameL(_L("c:\\mysound"));
	alarm2->SetTimeOffset(100);
	entry2->SetAlarmL(alarm2);
	numEntriesToBeStored++;

	//
	// Entry with Rich alarm (URL) only
	//
	seqNo++;
	HBufC8* pGUID3= KAlarmExportTestGUID3().AllocLC();
	CCalEntry* entry3 = CCalEntry::NewL(CCalEntry::EAppt, pGUID3,
	                                    CCalEntry::EMethodAdd, seqNo);
	CleanupStack::PushL(entry3);                                   
	firstEntryArray.AppendL(entry3);
	CleanupStack::Pop(entry3);
	CleanupStack::Pop(pGUID3); // entry takes ownership
	TDateTime entryStart3(2006, EApril, 7, 10, 0, 0, 0);
	TCalTime  calStart3;
	calStart3.SetTimeLocalL(entryStart3);
	TDateTime entryStop3(2006, EApril, 7, 11, 0, 0, 0);
	TCalTime calStop3;
	calStop3.SetTimeLocalL(entryStop3);
	entry3->SetStartAndEndTimeL(calStart3, calStop3);
	                                    
	                                    
	CCalAlarm* alarm3 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm3);
	_LIT8(KAlarmURL1, "http://www.symbian.com");
	_LIT8(KAlarmURLType, "text/http");
	
	CCalContent* richData3 = CCalContent::NewL();
	CleanupStack::PushL(richData3);
	
	HBufC8* urlContentBuf3 = HBufC8::NewL(KAlarmURL1().Length());
	CleanupStack::PushL(urlContentBuf3);
	*urlContentBuf3 = KAlarmURL1();
	
	HBufC8* urlTypeBuf3 = HBufC8::NewL(KAlarmURLType().Length());
	CleanupStack::PushL(urlTypeBuf3);
	*urlTypeBuf3 = KAlarmURLType();
	
	richData3->SetContentL(urlContentBuf3, urlTypeBuf3, CCalContent::EDispositionUrl);
	CleanupStack::Pop(2, urlContentBuf3); // SetRichDataContentL takes ownership

	alarm3->SetAlarmAction(richData3); //takes ownership
	CleanupStack::Pop(richData3);
	entry3->SetAlarmL(alarm3);
	numEntriesToBeStored++;
	

	//store the entries and check
	TInt numEntriesStored = 0;
	iEntryView->StoreL(firstEntryArray, numEntriesStored);
	if (numEntriesStored != numEntriesToBeStored)
		{
		_LIT(KEntryMatchErr, "Num Entries Stored (%d) does not match num entries created (%d)");
		INFO_PRINTF3(KEntryMatchErr, numEntriesStored, numEntriesToBeStored);
		CleanupStack::PopAndDestroy(5, scheduler);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	

	CleanupStack::PopAndDestroy(5, scheduler);
	
	return TestStepResult();
	}

	



void CTestCalInterimApiDCCreateEntries::OpenSessionFileL()
	{
	TBuf<MAX_TEXT_MESSAGE> buffer;
	buffer.Format(KCreateNewAgendaFile, &KAgendaFile);
	INFO_PRINTF1(buffer);
	TRAPD(err, iSession->CreateCalFileL(KAgendaFile));
	if (KErrAlreadyExists==err)
		{
		iSession->DeleteCalFileL(KAgendaFile);
		iSession->CreateCalFileL(KAgendaFile);	
		}
	iSession->OpenL(KAgendaFile());
	}


void CTestCalInterimApiDCCreateEntries::CloseAndDeleteRPtrArrayEntries(TAny* aObject)	
	{
	RPointerArray<CCalEntry>* array=REINTERPRET_CAST(RPointerArray<CCalEntry>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

void CTestCalInterimApiDCCreateEntries::CloseAndDeleteRPtrArrayAlarms(TAny* aObject)	
	{
	RPointerArray<CCalAlarm>* array=REINTERPRET_CAST(RPointerArray<CCalAlarm>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

CTestCalInterimApiDCCallbackForAlarms* CTestCalInterimApiDCCallbackForAlarms::NewL(CTestCalInterimApiDCCreateEntries* aParent)
	{
	CTestCalInterimApiDCCallbackForAlarms* self = new (ELeave) CTestCalInterimApiDCCallbackForAlarms(aParent);
	return self;
	}
	
CTestCalInterimApiDCCallbackForAlarms::~CTestCalInterimApiDCCallbackForAlarms()
	{
	Cancel();
	}

CTestCalInterimApiDCCallbackForAlarms::CTestCalInterimApiDCCallbackForAlarms(CTestCalInterimApiDCCreateEntries* aParent)
	: CActive(EPriorityHigh), iTestCalInterimApiSuiteStep(aParent)
	{
	}
    
void CTestCalInterimApiDCCallbackForAlarms::Start()
	{
	if (!IsActive())
		{
		SetActive();
		}	

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrNone);
	}

void CTestCalInterimApiDCCallbackForAlarms::Progress(TInt /*aPercentageCompleted*/)
	{
	}
	
void CTestCalInterimApiDCCallbackForAlarms::Completed(TInt /*aComplete*/)
	{
	Start();
	}
	
TBool CTestCalInterimApiDCCallbackForAlarms::NotifyProgress()
	{
	return ETrue;
	}
	
 void CTestCalInterimApiDCCallbackForAlarms::DoCancel()
	{
	Close();
	}
	
void CTestCalInterimApiDCCallbackForAlarms::RunL()
	{
	Close();
	}
	
void CTestCalInterimApiDCCallbackForAlarms::Close()
	{
	CActiveScheduler::Stop();
	}


