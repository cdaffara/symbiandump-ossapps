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

#include "TestCalInterimApiExceptionStep.h"
#include <fbs.h>
#include <e32math.h>
#include <e32std.h> 
#include <e32test.h>
#include <s32file.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <calalarm.h>
#include <calrrule.h>

_LIT8(KAlarmExportTestGUID1, "AlarmExportTestGUID1");
_LIT8(KAlarmExportTestGUID2, "AlarmExportTestGUID2");
_LIT (KCreateNewAgendaFile, "Creating New Agenda file %S");
_LIT(KAgendaFile,"C:CITVCal.dat");

_LIT(KEntryExportFile,"c:\\tcal_dataexchange_export.vcs");
_LIT(KExporting, "Exporting entries to file");
_LIT(KImporting, "Importing entries from file");


CTestCalInterimApiDEF064928Step::~CTestCalInterimApiDEF064928Step()
/**
 * Destructor
 */
	{
	delete iEntryView;
	delete iSession;
	WaitForAgendaServerShutdown();
	}

CTestCalInterimApiDEF064928Step::CTestCalInterimApiDEF064928Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDEF064928Step);
	}

 

/** Function from defect raiser for testing
* Sets a new exception date and updates the entry in the agenda model.
@param aNewAgnEntry Agenda entry containing the exception date
@param aExistingAgnEntry Agenda entry that will be updated.
@return True if aExistingAgnEntry was updated. EFalse if the
exception date already existed.*/
TBool CTestCalInterimApiDEF064928Step::CreateExceptionDateL(const CCalEntry* aNewAgnEntry, CCalEntry* aExistingAgnEntry)
{
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	aExistingAgnEntry->GetExceptionDatesL(exceptions);
	TInt ret=exceptions.Find(aNewAgnEntry->RecurrenceIdL());
	if(ret==KErrNotFound)
		{
		exceptions.Append(aNewAgnEntry->RecurrenceIdL());
		aExistingAgnEntry->SetExceptionDatesL(exceptions);
		SaveToAgendaL(aExistingAgnEntry,ETrue);
		}
	else
		{
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(1);//close exceptions

	return (ret==KErrNotFound);
}


/** Function from defect raiser for testing
Saves or updates a CCalEntry to the agenda model.
@param aAgnEntry the CCalEntry to store
@param aUpdate Set to True if this is an update to an existing
entry, Should only be small changes with no time changes according to Symbian.
@leave KErrUnknown if the agenda store functions don't leave but
have not saved the entry*/
void CTestCalInterimApiDEF064928Step::SaveToAgendaL(const CCalEntry* aAgnEntry,TBool aUpdate)
{
	//Save the entry in the model
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(aAgnEntry);
	TInt success=0;
	if (aUpdate)
		iEntryView->UpdateL(array,success);
	else
		iEntryView->StoreL(array,success);

	if (success!=1)
		User::Leave(KErrUnknown);

	CleanupStack::PopAndDestroy();// array.Close();
}


TVerdict CTestCalInterimApiDEF064928Step::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Start Alarm Exporting DEF064928 Tests"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestCalInterimApiDEF064928Step::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalInterimApiDEF064928Step::doTestStepL()
	{
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(scheduler);  
	CActiveScheduler::Install(scheduler);

	iSession  = CCalSession::NewL();
	
	CTestCalInterimApiCallbackForAlarms* alarmCallback = CTestCalInterimApiCallbackForAlarms::NewL(this);
	CleanupStack::PushL(alarmCallback); 

	OpenSessionFileL();
	iEntryView = CCalEntryView::NewL(*iSession,*alarmCallback);
	
	SetTestStepResult(EFail);
	
	CActiveScheduler::Add(alarmCallback);
	CActiveScheduler::Start();
	
	TUint seqNo = 1;
	HBufC8* pGUID1= KAlarmExportTestGUID1().AllocLC();
	CCalEntry* entry1 = CCalEntry::NewL(CCalEntry::EAppt, pGUID1,
	                                    CCalEntry::EMethodAdd, seqNo);
	CleanupStack::Pop(pGUID1); // entry takes ownership
	CleanupStack::PushL(entry1);                                   

	TDateTime entryStart1(2006, EJuly, 6, 15, 0, 0, 0);
	TCalTime  calStart1;
	calStart1.SetTimeLocalL(entryStart1);
	TDateTime entryStop1(2006, EJuly, 6, 16, 0, 0, 0);
	TCalTime calStop1;
	calStop1.SetTimeLocalL(entryStop1);
	entry1->SetStartAndEndTimeL(calStart1, calStop1);

	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EDaily);
	CleanupStack::PushL(rpt); 

	rpt->SetInterval(1);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2006, EJuly, 6, 0, 0, 0, 0));
	rpt->SetDtStart(startTime);
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2006, EJuly, 10, 0, 0, 0, 0));
	rpt->SetUntil(endTime);
	entry1->SetRRuleL(*rpt); // doesn't take ownership

	CleanupStack::PopAndDestroy(rpt); 
	
	SaveToAgendaL(entry1, EFalse);

	seqNo++;
	HBufC8* pGUID2= KAlarmExportTestGUID2().AllocLC();
	TDateTime entryTime2(2006, EJuly, 9, 15, 0, 0, 0);
	TCalTime  calTime2;
	calTime2.SetTimeLocalL(entryTime2);
	CCalEntry* entry2 = CCalEntry::NewL(CCalEntry::EAppt, pGUID2,
	                                    CCalEntry::EMethodAdd, seqNo, calTime2, 
	                                    CalCommon::EThisOnly);
	CleanupStack::Pop(pGUID2); // entry takes ownership
	CleanupStack::PushL(entry2);                                   

	CreateExceptionDateL(entry2, entry1);

	RArray<TCalTime> exDateList1;
	CleanupClosePushL(exDateList1);
	entry1->GetExceptionDatesL(exDateList1);
	TESTL(exDateList1[0].TimeLocalL() == calTime2.TimeLocalL());
	CleanupStack::PopAndDestroy();

	//build the CCalDataExchange object and Export
	RFs   fs;
	fs.Connect();
	CleanupClosePushL(fs);
	
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange); 
	RFile outFile;
	CleanupClosePushL(outFile);
	TInt err = outFile.Replace(fs, KEntryExportFile, EFileWrite);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	
	INFO_PRINTF1(KExporting);
	RPointerArray<CCalEntry> firstEntryArray;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &firstEntryArray));
	iEntryView->FetchL(entry1->UidL(), firstEntryArray);
		
	dataExchange->ExportL(KUidVCalendar, writeStream, firstEntryArray);
	
	writeStream.CommitL();
	
	// check that the exception date was stored correctly
	CCalEntry *fetchedEntry = firstEntryArray[0];
	RArray<TCalTime> exDateList2;
	fetchedEntry->GetExceptionDatesL(exDateList2);
	CleanupClosePushL(exDateList2);
	TESTL(exDateList2[0].TimeLocalL() == calTime2.TimeLocalL());
	
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile); // outFile.Close()
	
	//Internalize the entries from the file we just wrote
	RFile inFile;
	CleanupClosePushL(inFile);
	TInt errR = inFile.Open(fs, KEntryExportFile, EFileRead);
	RFileReadStream readStream(inFile);
	CleanupClosePushL(readStream);
	
	RPointerArray<CCalEntry> secondEntryArray;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &secondEntryArray));
	
	INFO_PRINTF1(KImporting);
	dataExchange->ImportL(KUidVCalendar, readStream, secondEntryArray);
	
	CleanupStack::Pop(&secondEntryArray);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&inFile);
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &secondEntryArray));
	
	// check the imported alarm
	TESTL(secondEntryArray.Count() == 1);
	CCalEntry* entry = secondEntryArray[0];
	RArray<TCalTime> importedExDate;
	entry->GetExceptionDatesL(importedExDate);
	TESTL(importedExDate.Count() == 1);
	TESTL(importedExDate[0].TimeLocalL() == calTime2.TimeLocalL());

	CleanupStack::PopAndDestroy(7, scheduler);
	SetTestStepResult(EPass);
	return TestStepResult();
	}


void CTestCalInterimApiDEF064928Step::OpenSessionFileL()
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


void CTestCalInterimApiDEF064928Step::CloseAndDeleteRPtrArrayEntries(TAny* aObject)	
	{
	RPointerArray<CCalEntry>* array=REINTERPRET_CAST(RPointerArray<CCalEntry>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

void CTestCalInterimApiDEF064928Step::CloseAndDeleteRPtrArrayAlarms(TAny* aObject)	
	{
	RPointerArray<CCalAlarm>* array=REINTERPRET_CAST(RPointerArray<CCalAlarm>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

CTestCalInterimApiCallbackForAlarms* CTestCalInterimApiCallbackForAlarms::NewL(CTestCalInterimApiSuiteStepBase* aParent)
	{
	CTestCalInterimApiCallbackForAlarms* self = new (ELeave) CTestCalInterimApiCallbackForAlarms(aParent);
	return self;
	}
	
CTestCalInterimApiCallbackForAlarms::~CTestCalInterimApiCallbackForAlarms()
	{
	Cancel();
	}

CTestCalInterimApiCallbackForAlarms::CTestCalInterimApiCallbackForAlarms(CTestCalInterimApiSuiteStepBase* aParent)
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


