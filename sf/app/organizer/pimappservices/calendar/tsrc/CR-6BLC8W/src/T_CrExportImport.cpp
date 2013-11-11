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

#include <calinstanceview.h>
#include <calinstance.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calrrule.h> 
#include <fbs.h>
#include <s32file.h>
#include <calentry.h>
#include <calalarm.h>
#include "T_CrExportImport.h"

_LIT(KEntries, "entries");
_LIT(KEntries2, "entries2");

_LIT(KEntryExportFile,"c:\\test\\t_cal_dataexchange_export.vcs");
_LIT(KEntryExportFile2,"c:\\test\\t_cal_dataexchange_export2.vcs");
_LIT(KEntryExportFileRpt,"c:\\test\\t_cal_data_exportrpt.vcs");
_LIT(KEntryExportFileRpt2,"c:\\test\\t_cal_data_exportrpt2.vcs");
_LIT(KEntryExportFileExc,"c:\\test\\t_cal_data_exportexc.vcs");
_LIT(KEntryExportFileExc2,"c:\\test\\t_cal_data_exportexc2.vcs");
_LIT(KEntryExportFileSpor,"c:\\test\\t_cal_data_exportspor.vcs");
_LIT(KEntryExportFileSpor2,"c:\\test\\t_cal_data_exportspor2.vcs");
_LIT(KEntryExportFileNegOffset,"c:\\test\\t_cal_data_ExportImportNegOffset.vcs");

_LIT(KEntryImportFile,"c:\\cr6blc8w\\testdata\\t_cal_dataexchange_import.vcs");

CTestCalInterimApiExportImport::CTestCalInterimApiExportImport()
	{
	SetTestStepName(KTestCalInterimApiExportImport);
	}

CTestCalInterimApiExportImport::~CTestCalInterimApiExportImport()
	{
	}
	
	
TVerdict CTestCalInterimApiExportImport::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	
	TPtrC dataPtr;
	TPtrC dataPtr2;

	TESTL(GetStringFromConfig(ConfigSection(), KEntries, dataPtr));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries2, dataPtr2));

	RPointerArray<CCalEntry> entries;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries));

	CreateEntriesL(dataPtr, entries);
	CreateEntriesL(dataPtr2, entries);


	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
			
	// store the entries in the model & verify the count
	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	
	//build the data exchange object and Export the entries
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TInt err = fs.MkDirAll(KEntryExportFile);
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	CleanupClosePushL(fs);
	RFile outFile;
	CleanupClosePushL(outFile);
	err = outFile.Replace(fs, KEntryExportFile, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	
	dataExchange->ExportL(KUidVCalendar, writeStream, entries);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile);  // outFile.Close()
	
	// The entries have been EXPORTed. We need to delete them from
	// the model store so that when we IMPORT them, they are not
	// duplicates of what is already there
	TInt idx = 0;
	for (idx=0; idx < cnt; idx++)
		{
		view->DeleteL(*(entries[idx]));
		}

	// Try fetching entries, this should fail if they have been deleted.
	RPointerArray<CCalEntry> empty;
	CleanupRPtrArrayDeleteAndCloseL(empty);
	TRAP_IGNORE(view->FetchL(entries[0]->UidL(), empty));

	TESTL(0 == empty.Count());
	CleanupStack::PopAndDestroy(&empty);

	// try to IMPORT the entries previously EXPORTed
	RPointerArray<CCalEntry> entries2;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries2));

	RFile inFile;
	CleanupClosePushL(inFile);
	err = inFile.Open(fs, KEntryExportFile, EFileRead);
	User::LeaveIfError(err);
	RFileReadStream readStream(inFile);
	CleanupClosePushL(readStream);
	dataExchange->ImportL(KUidVCalendar, readStream, entries2);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&inFile);
	
	TInt cnt2 = entries2.Count();
	TESTL(cnt == cnt2);
	
	// store the imported entries to get all the internal fields
	// set properly
	TInt cnt3 = 0;
	view->StoreL(entries2, cnt3);
	
	TESTL(cnt == cnt3);
	
	
	RFile outFile2;
	CleanupClosePushL(outFile2);
	err = outFile2.Replace(fs, KEntryExportFile2, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream2(outFile2);
	CleanupClosePushL(writeStream2);
	
	dataExchange->ExportL(KUidVCalendar, writeStream2, entries2);
	writeStream2.CommitL();
	CleanupStack::PopAndDestroy(&writeStream2); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile2);  // outFile.Close()
	
	
	//now check the times and time modes
	TInt idx2 = 0;
	TInt numFixed = 0;
	TInt numFloating = 0;
	for (idx2 = 0; idx2 < cnt; idx2++)
		{
		CCalEntry* ex = entries[idx2];
		CCalEntry* im = entries2[idx2];
		TESTL(ex->EntryTypeL() == im->EntryTypeL());
		TESTL(ex->StartTimeL().TimeMode() == im->StartTimeL().TimeMode());
		TESTL(ex->EndTimeL().TimeMode() == im->EndTimeL().TimeMode());
		TESTL(ex->StartTimeL().TimeUtcL() == im->StartTimeL().TimeUtcL());
		TESTL(ex->EndTimeL().TimeUtcL() == im->EndTimeL().TimeUtcL());
		
		if (ex->EndTimeL().TimeUtcL() != Time::NullTTime())
			{
			TESTL(ex->StartTimeL().TimeMode() == ex->EndTimeL().TimeMode());
			}

		TCalTime::TTimeMode thisMode = ex->StartTimeL().TimeMode();
		thisMode == TCalTime::EFixedUtc ? ++numFixed : ++numFloating;
		}
		
	// we should have the same number of fixed and floating entries
	// if the data file has been set up properly
	TESTL(numFixed == numFloating);
			
	entries2.ResetAndDestroy();
	entries.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(6);
	
	return TestStepResult();
	}
//------------------------------------------------------------

CTestCalInterimApiImportMixed::CTestCalInterimApiImportMixed()
	{
	SetTestStepName(KTestCalInterimApiImportMixed);
	}

CTestCalInterimApiImportMixed::~CTestCalInterimApiImportMixed()
	{
	}
	
/**
Creates cal entries from ini data. Store it. Change UTC offset. Search instances by time.
NOTE To make test simpler all instanses should have different start time (at least 3 minutes difference)

*/	
	
TVerdict CTestCalInterimApiImportMixed::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	
	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view); 
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
	
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange); 
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs); 
	
	RPointerArray<CCalEntry> entries;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries));

	RFile inFile;
	CleanupClosePushL(inFile); 
	TInt err = inFile.Open(fs, KEntryImportFile, EFileRead);
	User::LeaveIfError(err);
	RFileReadStream readStream(inFile);
	CleanupClosePushL(readStream); 
	dataExchange->ImportL(KUidVCalendar, readStream, entries);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&inFile);

	TInt cnt = entries.Count();
	TESTL(cnt == 2);  // we know there are only 2 events in the .vcs file
	
	// First record was written with local Start and Utc End time.
	// Second record was written with Utc Start and local End time.
	// Importing should convert so that the Start Time mode is used.
	// Therefore the mode of the first entry should be floating and the
	// the mode of the second one fixed.
	CCalEntry* ce1 = entries[0];
	CCalEntry* ce2 = entries[1];
	TESTL(ce1->StartTimeL().TimeMode() == TCalTime::EFloating);
	TESTL(ce1->EndTimeL().TimeMode() == TCalTime::EFloating);
	
	TESTL(ce2->StartTimeL().TimeMode() == TCalTime::EFixedUtc);	
	TESTL(ce2->EndTimeL().TimeMode() == TCalTime::EFixedUtc);	

	entries.ResetAndDestroy();
	CleanupStack::PopAndDestroy(5);
		
	return TestStepResult();
	}	

//-------------------------------------------------------------------------------
CTestCalInterimApiExportImportRpt::CTestCalInterimApiExportImportRpt()
	{
	SetTestStepName(KTestCalInterimApiExportImportRpt);
	}

CTestCalInterimApiExportImportRpt::~CTestCalInterimApiExportImportRpt()
	{
	}
	
	
TVerdict CTestCalInterimApiExportImportRpt::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	
	TPtrC dataPtr;
	TPtrC dataPtr2;

	TESTL(GetStringFromConfig(ConfigSection(), KEntries, dataPtr));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries2, dataPtr2));

	RPointerArray<CCalEntry> entries;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries));

	CreateEntriesL(dataPtr, entries);
	CreateEntriesL(dataPtr2, entries);


	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
			
	// store the entries in the model & verify the count
	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	
	//build the data exchange object and Export the entries
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile outFile;
	CleanupClosePushL(outFile);
	TInt err = outFile.Replace(fs, KEntryExportFileRpt, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	
	dataExchange->ExportL(KUidVCalendar, writeStream, entries);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile);  // outFile.Close()
	
	// The entries have been EXPORTed. We need to delete them from
	// the model store so that when we IMPORT them, they are not
	// duplicates of what is already there
	TInt idx = 0;
	for (idx=0; idx < cnt; idx++)
		{
		view->DeleteL(*(entries[idx]));
		}

	// Try fetching entries, this should fail if they have been deleted.
	RPointerArray<CCalEntry> empty;
	CleanupRPtrArrayDeleteAndCloseL(empty);
	TRAP_IGNORE(view->FetchL(entries[0]->UidL(), empty));

	TESTL(0 == empty.Count());
	CleanupStack::PopAndDestroy(&empty);

	// try to IMPORT the entries previously EXPORTed
	RPointerArray<CCalEntry> entries2;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries2));

	RFile inFile;
	CleanupClosePushL(inFile);
	err = inFile.Open(fs, KEntryExportFileRpt, EFileRead);
	User::LeaveIfError(err);
	RFileReadStream readStream(inFile);
	CleanupClosePushL(readStream);
	dataExchange->ImportL(KUidVCalendar, readStream, entries2);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&inFile);
	
	TInt cnt2 = entries2.Count();
	TESTL(cnt == cnt2);
	
	// store the imported entries to get all the internal fields
	// set properly
	TInt cnt3 = 0;
	view->StoreL(entries2, cnt3);
	
	TESTL(cnt == cnt3);
	
	
	RFile outFile2;
	CleanupClosePushL(outFile2);
	err = outFile2.Replace(fs, KEntryExportFileRpt2, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream2(outFile2);
	CleanupClosePushL(writeStream2);
	
	dataExchange->ExportL(KUidVCalendar, writeStream2, entries2);
	writeStream2.CommitL();
	CleanupStack::PopAndDestroy(&writeStream2); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile2);  // outFile.Close()
	
	
	//now check the times and time modes
	TInt idx2 = 0;
	for (idx2 = 0; idx2 < cnt; idx2++)
		{
		CCalEntry* ex = entries[idx2];
		CCalEntry* im = entries2[idx2];
		TESTL(ex->EntryTypeL() == im->EntryTypeL());

        // Don't compare the number of rules for the exported and imported 
        // entries, they may differ. When the entry is imported the HMS is
        // adjusted to the same as the start date. When the rrule date is
        // converted to local time, it may skip over to the next day.
		TCalRRule exRule;
		TCalRRule imRule;
		TESTL(ex->GetRRuleL(exRule));
		TESTL(im->GetRRuleL(imRule));
		TESTL(exRule.Type() == imRule.Type());

		TCalTime::TTimeMode thisMode = ex->StartTimeL().TimeMode();
		TESTL(ex->StartTimeL().TimeMode() == im->StartTimeL().TimeMode());
		if (thisMode == TCalTime::EFixedUtc)
			{
			TESTL(exRule.DtStart().TimeUtcL() == imRule.DtStart().TimeUtcL());
			TESTL(ex->StartTimeL().TimeUtcL() == im->StartTimeL().TimeUtcL());
			TESTL(ex->EndTimeL().TimeUtcL() == im->EndTimeL().TimeUtcL());
			}
		else
			{
			TESTL(exRule.DtStart().TimeLocalL() == imRule.DtStart().TimeLocalL());
			TESTL(ex->StartTimeL().TimeLocalL() == im->StartTimeL().TimeLocalL());
			TESTL(ex->EndTimeL().TimeLocalL() == im->EndTimeL().TimeLocalL());
			}
		}
			
			
	entries2.ResetAndDestroy();
	entries.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(6);
	
	return TestStepResult();
	}
	
//-------------------------------------------------------------------------------
CTestCalInterimApiExportImportExc::CTestCalInterimApiExportImportExc()
	{
	SetTestStepName(KTestCalInterimApiExportImportExc);
	}

CTestCalInterimApiExportImportExc::~CTestCalInterimApiExportImportExc()
	{
	}
	
	
TVerdict CTestCalInterimApiExportImportExc::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	
	TPtrC dataPtr;
	TPtrC dataPtr2;

	TESTL(GetStringFromConfig(ConfigSection(), KEntries, dataPtr));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries2, dataPtr2));

	RPointerArray<CCalEntry> entries;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries));

	CreateEntriesL(dataPtr, entries);
	CreateEntriesL(dataPtr2, entries);
	
	// Add exceptions to the entries
	TInt count = entries.Count();
	TInt j = 0;
	RArray<TCalTime> exceptions;
	for(j=0; j < count; j++)
		{
		exceptions.Reset();
		// For each entries there are ten repeats. We'll make exceptions
		// of numbers 3 and 7.
		CCalEntry* calEn = entries[j];
		TCalTime::TTimeMode beginMode = calEn->StartTimeL().TimeMode();
		if (beginMode == TCalTime::EFloating)
			{
			TTime begin = calEn->StartTimeL().TimeLocalL();
			TTime excpt1 = begin + TTimeIntervalDays(3);
			TTime excpt2 = begin + TTimeIntervalDays(7);
			TCalTime calExcpt1;
			TCalTime calExcpt2;
			calExcpt1.SetTimeLocalFloatingL(excpt1);
			calExcpt2.SetTimeLocalFloatingL(excpt2);
			exceptions.AppendL(calExcpt1);
			exceptions.AppendL(calExcpt2);
			}
		else
			{
			TTime begin = calEn->StartTimeL().TimeUtcL();
			TTime excpt1 = begin + TTimeIntervalDays(3);
			TTime excpt2 = begin + TTimeIntervalDays(7);
			TCalTime calExcpt1;
			TCalTime calExcpt2;
			calExcpt1.SetTimeUtcL(excpt1);
			calExcpt2.SetTimeUtcL(excpt2);
			exceptions.AppendL(calExcpt1);
			exceptions.AppendL(calExcpt2);
			}
		calEn->SetExceptionDatesL(exceptions);
		}


	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
			
	// store the entries in the model & verify the count
	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	
	//build the data exchange object and Export the entries
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile outFile;
	CleanupClosePushL(outFile);
	TInt err = outFile.Replace(fs, KEntryExportFileExc, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	
	dataExchange->ExportL(KUidVCalendar, writeStream, entries);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile);  // outFile.Close()
	
	// The entries have been EXPORTed. We need to delete them from
	// the model store so that when we IMPORT them, they are not
	// duplicates of what is already there
	TInt idx = 0;
	for (idx=0; idx < cnt; idx++)
		{
		view->DeleteL(*(entries[idx]));
		}

	// Try fetching entries, this should fail if they have been deleted.
	RPointerArray<CCalEntry> empty;
	CleanupRPtrArrayDeleteAndCloseL(empty);
	TRAP_IGNORE(view->FetchL(entries[0]->UidL(), empty));

	TESTL(0 == empty.Count());
	CleanupStack::PopAndDestroy(&empty);

	// try to IMPORT the entries previously EXPORTed
	RPointerArray<CCalEntry> entries2;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries2));

	RFile inFile;
	CleanupClosePushL(inFile);
	err = inFile.Open(fs, KEntryExportFileExc, EFileRead);
	User::LeaveIfError(err);
	RFileReadStream readStream(inFile);
	CleanupClosePushL(readStream);
	dataExchange->ImportL(KUidVCalendar, readStream, entries2);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&inFile);
	
	TInt cnt2 = entries2.Count();
	TESTL(cnt == cnt2);
	
	// store the imported entries to get all the internal fields
	// set properly
	TInt cnt3 = 0;
	view->StoreL(entries2, cnt3);
	
	TESTL(cnt == cnt3);
	
	
	RFile outFile2;
	CleanupClosePushL(outFile2);
	err = outFile2.Replace(fs, KEntryExportFileExc2, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream2(outFile2);
	CleanupClosePushL(writeStream2);
	
	dataExchange->ExportL(KUidVCalendar, writeStream2, entries2);
	writeStream2.CommitL();
	CleanupStack::PopAndDestroy(&writeStream2); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile2);  // outFile.Close()
	
	
	//now check the times and time modes
	TInt idx2 = 0;
	for (idx2 = 0; idx2 < cnt; idx2++)
		{
		CCalEntry* ex = entries[idx2];
		CCalEntry* im = entries2[idx2];
		TESTL(ex->EntryTypeL() == im->EntryTypeL());
		
        // Don't compare the number of rules for the exported and imported 
        // entries, they may differ. When the entry is imported the HMS is
        // adjusted to the same as the start date. When the rrule date is
        // converted to local time, it may skip over to the next day.
		TCalRRule exRule;
		TCalRRule imRule;
		TESTL(ex->GetRRuleL(exRule));
		TESTL(im->GetRRuleL(imRule));
		TESTL(exRule.Type() == imRule.Type());

		RArray<TCalTime> exExceptions;
		RArray<TCalTime> imExceptions;
		
		ex->GetExceptionDatesL(exExceptions);
		im->GetExceptionDatesL(imExceptions);
		TESTL(exExceptions.Count() == imExceptions.Count());
		TInt exceptCount = exExceptions.Count();

		TCalTime::TTimeMode thisMode = ex->StartTimeL().TimeMode();
		TESTL(ex->StartTimeL().TimeMode() == im->StartTimeL().TimeMode());
		if (thisMode == TCalTime::EFixedUtc)
			{
			TInt j2 = 0;
			for (j2=0; j2 < exceptCount; j2++)
				{
				TESTL(exExceptions[j2].TimeUtcL() == imExceptions[j2].TimeUtcL());
				}
			TESTL(exRule.DtStart().TimeUtcL() == imRule.DtStart().TimeUtcL());
			TESTL(ex->StartTimeL().TimeUtcL() == im->StartTimeL().TimeUtcL());
			TESTL(ex->EndTimeL().TimeUtcL() == im->EndTimeL().TimeUtcL());
			}
		else
			{
			TInt j3 = 0;
			for (j3=0; j3 < exceptCount; j3++)
				{
				TESTL(exExceptions[j3].TimeLocalL() == imExceptions[j3].TimeLocalL());
				}
			TESTL(exRule.DtStart().TimeLocalL() == imRule.DtStart().TimeLocalL());
			TESTL(ex->StartTimeL().TimeLocalL() == im->StartTimeL().TimeLocalL());
			TESTL(ex->EndTimeL().TimeLocalL() == im->EndTimeL().TimeLocalL());
			}
		}
		
	
			
	entries2.ResetAndDestroy();
	entries.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(6);
	
	return TestStepResult();
	}

//-------------------------------------------------------------------------------
CTestCalInterimApiExportImportSpor::CTestCalInterimApiExportImportSpor()
	{
	SetTestStepName(KTestCalInterimApiExportImportSpor);
	}

CTestCalInterimApiExportImportSpor::~CTestCalInterimApiExportImportSpor()
	{
	}
	
	
TVerdict CTestCalInterimApiExportImportSpor::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	
	TPtrC dataPtr;
	TPtrC dataPtr2;

	TESTL(GetStringFromConfig(ConfigSection(), KEntries, dataPtr));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries2, dataPtr2));

	RPointerArray<CCalEntry> entries;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries));

	CreateEntriesL(dataPtr, entries);
	CreateEntriesL(dataPtr2, entries);
	
	// Add RDates to the entries
	TInt count = entries.Count();
	TInt j = 0;
	RArray<TCalTime> rDates;
	for(j=0; j < count; j++)
		{
		rDates.Reset();
		// We'll put repeats at +3 days and +7 days
		CCalEntry* calEn = entries[j];
		TCalTime::TTimeMode beginMode = calEn->StartTimeL().TimeMode();
		if (beginMode == TCalTime::EFloating)
			{
			TTime begin = calEn->StartTimeL().TimeLocalL();
			TTime r1 = begin + TTimeIntervalDays(3);
			TTime r2 = begin + TTimeIntervalDays(7);
			TCalTime calR1;
			TCalTime calR2;
			calR1.SetTimeLocalFloatingL(r1);
			calR2.SetTimeLocalFloatingL(r2);
			rDates.AppendL(calR1);
			rDates.AppendL(calR2);
			}
		else
			{
			TTime begin = calEn->StartTimeL().TimeUtcL();
			TTime r1 = begin + TTimeIntervalDays(3);
			TTime r2 = begin + TTimeIntervalDays(7);
			TCalTime calR1;
			TCalTime calR2;
			calR1.SetTimeUtcL(r1);
			calR2.SetTimeUtcL(r2);
			rDates.AppendL(calR1);
			rDates.AppendL(calR2);
			}
		calEn->SetRDatesL(rDates);
		}


	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
			
	// store the entries in the model & verify the count
	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	
	//build the data exchange object and Export the entries
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile outFile;
	CleanupClosePushL(outFile);
	TInt err = outFile.Replace(fs, KEntryExportFileSpor, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	
	dataExchange->ExportL(KUidVCalendar, writeStream, entries);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile);  // outFile.Close()
	
	// The entries have been EXPORTed. We need to delete them from
	// the model store so that when we IMPORT them, they are not
	// duplicates of what is already there
	TInt idx = 0;
	for (idx=0; idx < cnt; idx++)
		{
		view->DeleteL(*(entries[idx]));
		}
	
	// Try fetching entries, this should fail if they have been deleted.
	RPointerArray<CCalEntry> empty;
	CleanupRPtrArrayDeleteAndCloseL(empty);
	TRAP_IGNORE(view->FetchL(entries[0]->UidL(), empty));

	TESTL(0 == empty.Count());
	CleanupStack::PopAndDestroy(&empty);
	
	// try to IMPORT the entries previously EXPORTed
	RPointerArray<CCalEntry> entries2;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries2));

	RFile inFile;
	CleanupClosePushL(inFile);
	err = inFile.Open(fs, KEntryExportFileSpor, EFileRead);
	User::LeaveIfError(err);
	RFileReadStream readStream(inFile);
	CleanupClosePushL(readStream);
	dataExchange->ImportL(KUidVCalendar, readStream, entries2);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&inFile);
	
	TInt cnt2 = entries2.Count();
	TESTL(cnt == cnt2);
	
	// store the imported entries to get all the internal fields
	// set properly
	TInt cnt3 = 0;
	view->StoreL(entries2, cnt3);
	
	TESTL(cnt == cnt3);
	
	RFile outFile2;
	CleanupClosePushL(outFile2);
	err = outFile2.Replace(fs, KEntryExportFileSpor2, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream2(outFile2);
	CleanupClosePushL(writeStream2);
	
	dataExchange->ExportL(KUidVCalendar, writeStream2, entries2);
	writeStream2.CommitL();
	CleanupStack::PopAndDestroy(&writeStream2); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile2);  // outFile.Close()
	
	//now check the times and time modes
	TInt idx2 = 0;
	for (idx2 = 0; idx2 < cnt; idx2++)
		{
		CCalEntry* ex = entries[idx2];
		CCalEntry* im = entries2[idx2];
		TESTL(ex->EntryTypeL() == im->EntryTypeL());
		
		RArray<TCalTime> exRDates;
		RArray<TCalTime> imRDates;
		
		ex->GetRDatesL(exRDates);
		im->GetRDatesL(imRDates);
		TESTL(exRDates.Count() == imRDates.Count());
		TInt rCount = exRDates.Count();

		TCalTime::TTimeMode thisMode = ex->StartTimeL().TimeMode();
		TESTL(ex->StartTimeL().TimeMode() == im->StartTimeL().TimeMode());
		if (thisMode == TCalTime::EFixedUtc)
			{
			TInt j2 = 0;
			for (j2=0; j2 < rCount; j2++)
				{
				TESTL(exRDates[j2].TimeUtcL() == imRDates[j2].TimeUtcL());
				}
			// When exporting entries with RDates, the start date gets changed
			// to the earliest RDate. This is desired behaviour, so we check it.
			// The comparison between the 2 start times is still valid for TODOs
			// and EVENTs so we check that also.
			TESTL((ex->StartTimeL().TimeUtcL() == im->StartTimeL().TimeUtcL()) ||
					(im->StartTimeL().TimeUtcL() == exRDates[0].TimeUtcL()));
			}
		else
			{
			TInt j3 = 0;
			for (j3=0; j3 < rCount; j3++)
				{
				TESTL(exRDates[j3].TimeLocalL() == imRDates[j3].TimeLocalL());
				}
			// When exporting entries with RDates, the start date gets changed
			// to the earliest RDate. This is desired behaviour, so we check it.
			// The comparison between the 2 start times is still valid for TODOs
			// and EVENTs so we check that also.
			TESTL((ex->StartTimeL().TimeLocalL() == im->StartTimeL().TimeLocalL()) ||
					(im->StartTimeL().TimeLocalL() == imRDates[0].TimeLocalL()));
			}
		}
					
	entries2.ResetAndDestroy();
	entries.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(6);
	
	return TestStepResult();
	}
	
//-------------------------------------------------------------------------------
CTestCalInterimApiExportRecur::CTestCalInterimApiExportRecur()
	{
	SetTestStepName(KTestCalInterimApiExportRecur);
	}

CTestCalInterimApiExportRecur::~CTestCalInterimApiExportRecur()
	{
	}
	
void CTestCalInterimApiExportRecur::setUpEntriesL(RPointerArray<CCalEntry>& aEntries)
	{
		//Parent Entry
		//BEGIN:VTODO
		//TIMEMODE:EFloating
		//UID:15
		//DTSTART:19990101:010000.000000
		//DTEND:19990101:113000.000000
		//RRULE:DAILY
		//INTERVAL:1
		//COUNT:4
		//END:VTODO
	HBufC8* parentGuid = _L8("15").AllocLC();
	CCalEntry* parentEntry = CCalEntry::NewL(CCalEntry::ETodo,parentGuid,CCalEntry::EMethodNone, 0 );
	CleanupStack::Pop(parentGuid);
	CleanupStack::PushL(parentEntry);
	
	TTime startTime(TDateTime(1999, EJanuary, 0, 1, 0, 0, 0)); // 01:00 on Jan 1
	TCalTime calStartTime;
	calStartTime.SetTimeLocalFloatingL(startTime);
	TTime endTime(TDateTime(1999, EJanuary, 0, 11, 30, 0, 0)); // 11:30 on Jan 1
	TCalTime calEndTime;
	calEndTime.SetTimeLocalFloatingL(endTime);
	parentEntry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	TCalRRule rule1(TCalRRule::EDaily);
	rule1.SetInterval(1);
	rule1.SetDtStart(calStartTime); // every days from Jan 1 for 4 days
	rule1.SetCount(4);
	parentEntry->SetRRuleL(rule1);
	
	// set up Rec ID for the child entries
	// they are both going to use the same recurrenceIDs
	TTime recId(TDateTime(1999, EJanuary, 1, 1, 0, 0, 0)); // 01:00 on Jan 2
	TCalTime calRecId;
	calRecId.SetTimeLocalFloatingL(recId);
	
		// Child entry
		//BEGIN:VTODO
		//TIMEMODE:EFloating
		//RECURR-ID:19990102:113000.000000
		//RECURRENCERANGE:ThisOnly
		//UID:15
		//DTSTART:19990611:010000.000000
		//DTEND:19990611:110000.000000
		//END:VTODO
	HBufC8* childGuid = _L8("15").AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::ETodo, childGuid, CCalEntry::EMethodNone, 0,
		calRecId, CalCommon::EThisOnly);
	CleanupStack::Pop(childGuid);
	CleanupStack::PushL(childEntry);
	
	TTime childStartTime(TDateTime(1999, EJune, 10, 1, 0, 0, 0)); // 01:00 on June 11
	TCalTime childCalStartTime;
	childCalStartTime.SetTimeLocalFloatingL(childStartTime);
	TTime childEndTime(TDateTime(1999, EJune, 10, 11, 30, 0, 0)); // 11:30 on June 11
	TCalTime childCalEndTime;
	childCalEndTime.SetTimeLocalFloatingL(childEndTime);
	childEntry->SetStartAndEndTimeL(childCalStartTime, childCalEndTime);
	
		//Second Child entry
		//BEGIN:VTODO
		//TIMEMODE:EFloating
		//RECURR-ID:19990102:113000.000000
		//RECURRENCERANGE:ThisAndFuture
		//UID:15
		//DTSTART:19990511:010000.000000
		//DTEND:19990511:110000.000000
		//RRULE:DAILY
		//INTERVAL:1
		//COUNT:2
		//END:VTODO
	HBufC8* childGuid2 = _L8("15").AllocLC();
	CCalEntry* childEntry2 = CCalEntry::NewL(CCalEntry::ETodo, childGuid2, CCalEntry::EMethodNone, 0,
		calRecId, CalCommon::EThisAndFuture);
	CleanupStack::Pop(childGuid2);
	CleanupStack::PushL(childEntry2);
	
	TTime childStartTime2(TDateTime(1999, EMay, 10, 1, 0, 0, 0)); // 01:00 on May 11
	TCalTime childCalStartTime2;
	childCalStartTime2.SetTimeLocalFloatingL(childStartTime2);
	TTime childEndTime2(TDateTime(1999, EMay, 10, 11, 30, 0, 0)); // 11:30 on May 11
	TCalTime childCalEndTime2;
	childCalEndTime2.SetTimeLocalFloatingL(childEndTime2);
	childEntry2->SetStartAndEndTimeL(childCalStartTime2, childCalEndTime2);

	TCalRRule childRule(TCalRRule::EDaily);
	childRule.SetInterval(1);
	childRule.SetDtStart(calStartTime); // every days from Jan 1 for 4 days
	childRule.SetCount(2);
	childEntry2->SetRRuleL(childRule);
	
	aEntries.Append(parentEntry);
	aEntries.Append(childEntry);
	aEntries.Append(childEntry2);

	CleanupStack::Pop(3);//childEntry2, childEntry, parentEntry
	}

void CTestCalInterimApiExportRecur::setUpEntries2L(RPointerArray<CCalEntry>& aEntries)
	{
		// Parent Entry
		//BEGIN:VTODO
		//TIMEMODE:EFloating
		//UID:16
		//DTSTART:19990101:010000.000000
		//DTEND:19990101:113000.000000
		//RRULE:DAILY
		//INTERVAL:1
		//COUNT:4
		//END:VTODO
	HBufC8* parentGuid = _L8("16").AllocLC();
	CCalEntry* parentEntry = CCalEntry::NewL(CCalEntry::ETodo,parentGuid,CCalEntry::EMethodNone, 0 );
	CleanupStack::Pop(parentGuid);
	CleanupStack::PushL(parentEntry);
	
	TTime startTime(TDateTime(1999, EJanuary, 0, 1, 0, 0, 0)); // 01:00 on Jan 1
	TCalTime calStartTime;
	calStartTime.SetTimeLocalFloatingL(startTime);
	TTime endTime(TDateTime(1999, EJanuary, 0, 11, 30, 0, 0)); // 11:30 on Jan 1
	TCalTime calEndTime;
	calEndTime.SetTimeLocalFloatingL(endTime);
	parentEntry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	TCalRRule rule1(TCalRRule::EDaily);
	rule1.SetInterval(1);
	rule1.SetDtStart(calStartTime); // every days from Jan 1 for 4 days
	rule1.SetCount(4);
	parentEntry->SetRRuleL(rule1);
	
	// set up Rec ID for the child entries
	// they are both going to use the same recurrenceIDs
	TTime recId(TDateTime(1999, EJanuary, 1, 1, 0, 0, 0)); // 01:00 on Jan 2
	TCalTime calRecId;
	calRecId.SetTimeLocalFloatingL(recId);
	
		//Child entry
		//BEGIN:VTODO
		//TIMEMODE:EFloating
		//RECURR-ID:19990102:113000.000000
		//RECURRENCERANGE:ThisOnly
		//UID:16
		//DTSTART:19990611:010000.000000
		//DTEND:19990611:110000.000000
		//END:VTODO
	HBufC8* childGuid = _L8("16").AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::ETodo, childGuid, CCalEntry::EMethodNone, 0,
		calRecId, CalCommon::EThisOnly);
	CleanupStack::Pop(childGuid);
	CleanupStack::PushL(childEntry);
	
	TTime childStartTime(TDateTime(1999, EJune, 10, 10, 0, 0, 0)); // 10:00 on June 11
	TCalTime childCalStartTime;
	childCalStartTime.SetTimeLocalFloatingL(childStartTime);
	TTime childEndTime(TDateTime(1999, EJune, 10, 11, 00, 0, 0)); // 11:00 on June 11
	TCalTime childCalEndTime;
	childCalEndTime.SetTimeLocalFloatingL(childEndTime);
	childEntry->SetStartAndEndTimeL(childCalStartTime, childCalEndTime);
	
		//Second Child entry
		//BEGIN:VTODO
		//TIMEMODE:EFloating
		//RECURR-ID:19990102:113000.000000
		//RECURRENCERANGE:ThisOnly
		//UID:16
		//DTSTART:19990511:010000.000000
		//DTEND:19990511:110000.000000
		//END:VTODO
	HBufC8* childGuid2 = _L8("16").AllocLC();
	CCalEntry* childEntry2 = CCalEntry::NewL(CCalEntry::ETodo, childGuid2, CCalEntry::EMethodNone, 0,
		calRecId, CalCommon::EThisOnly);
	CleanupStack::Pop(childGuid2);
	CleanupStack::PushL(childEntry2);
	
	TTime childStartTime2(TDateTime(1999, EMay, 10, 1, 0, 0, 0)); // 01:00 on May 11
	TCalTime childCalStartTime2;
	childCalStartTime2.SetTimeLocalFloatingL(childStartTime2);
	TTime childEndTime2(TDateTime(1999, EMay, 10, 11, 00, 0, 0)); // 11:00 on May 11
	TCalTime childCalEndTime2;
	childCalEndTime2.SetTimeLocalFloatingL(childEndTime2);
	childEntry2->SetStartAndEndTimeL(childCalStartTime2, childCalEndTime2);
	
	aEntries.Append(parentEntry);
	aEntries.Append(childEntry);
	aEntries.Append(childEntry2);

	CleanupStack::Pop(3);//childEntry2, childEntry, parentEntry
	}		

TVerdict CTestCalInterimApiExportRecur::doTestStepL()	
	{
	_LIT(KCalcutta, "Asia/Calcutta");
	TBufC<14> timeZone(KCalcutta);
	SetTZL(timeZone);
	TESTL(TestStepResult() == EPass);
	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
	
	// set up the first set of entries (expect three entries)
	RPointerArray<CCalEntry> entries;
	CleanupRPtrArrayDeleteAndCloseL(entries);
	RPointerArray<CCalEntry> fetched;
	CleanupRPtrArrayDeleteAndCloseL(fetched);
	setUpEntriesL(entries);
	TESTL(3 == entries.Count());
	
	// set up the second set of entries (expect three more entries)
	setUpEntries2L(entries);
	TESTL(6 == entries.Count());
	
	// store the entries in the model & verify the count
	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	
	// fetch the entries back from the model
	view->FetchL(entries[0]->UidL(), fetched);	
	
	// test the fetched entries for the correct data
	TESTL(2 == fetched.Count()); //expect only two entries to be fetched
	TESTL(CalCommon::EThisAndFuture == fetched[1]->RecurrenceRangeL()); //expect EThisAndFuture for second recurrence range
	
	// cleanup the fetched array
	fetched.ResetAndDestroy();
	
	// fetch the second set of entries back from the model
	view->FetchL(entries[3]->UidL(), fetched); 
	
	// test the fetched entries for the correct data
	TESTL(2 == fetched.Count()); //expect only two entries to be fetched
	TTime childEndTime2(TDateTime(1999, EMay, 10, 11, 00, 0, 0)); // 11:00 on May 11
	TCalTime childCalEndTime2;
	childCalEndTime2.SetTimeLocalFloatingL(childEndTime2);
	TESTL( childCalEndTime2.TimeLocalL() == fetched[1]->EndTimeL().TimeLocalL() ); //expect end times of the second child

	// cleanup the arrays
	fetched.ResetAndDestroy();
	entries.ResetAndDestroy();
	
	CleanupStack::PopAndDestroy(4);//fetched, entries, view, progress
	
	RestoreTZL();

	return TestStepResult();
	}
	
//-------------------------------------------------------------------------------
CTestCalInterimApiExportImportNegOffset::CTestCalInterimApiExportImportNegOffset()
	{
	SetTestStepName(KTestCalInterimApiExportImportNegOffset);
	}

CTestCalInterimApiExportImportNegOffset::~CTestCalInterimApiExportImportNegOffset()
	{
	}
	
void CTestCalInterimApiExportImportNegOffset::setUpEntriesWithAlarmOnNextDayL(RPointerArray<CCalEntry>& aEntries)
	{
		//BEGIN:VAPPT
		//TIMEMODE:EFloatingTimeZone
		//UID:6
		//DTSTART:19990316:230000.000000
		//DTEND:19990317:000000.000000
		//RRULE:DAILY
		//INTERVAL:1
		//COUNT:3
		//RDATE:19990105:090000.000000
		//ALARM:-60
		//END:VAPPT	
	HBufC8* firstGuid = _L8("6").AllocLC();
	CCalEntry* firstEntry = CCalEntry::NewL(CCalEntry::EAppt,firstGuid,CCalEntry::EMethodNone, 0 );
	CleanupStack::Pop(firstGuid);
	CleanupStack::PushL(firstEntry);
	
	TTime startTime(TDateTime(1999, EMarch, 15, 23, 00, 0, 0)); // 23:00:00 on March 16 1999
	TCalTime calStartTime;
	calStartTime.SetTimeLocalFloatingL(startTime);
	TTime endTime(TDateTime(1999, EMarch, 16, 0, 0, 0, 0)); // 12:00 on March 17 1999
	TCalTime calEndTime;
	calEndTime.SetTimeLocalFloatingL(endTime);
	firstEntry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	TCalRRule rule1(TCalRRule::EDaily);
	rule1.SetInterval(1);
	rule1.SetDtStart(calStartTime); // every day from start time for 3 days
	rule1.SetCount(3);
	firstEntry->SetRRuleL(rule1);
	
	TTime firstRTime(TDateTime(1999, EJanuary, 4, 9, 0, 0, 0)); // 09:00 on January 5 1999
	TCalTime calFirstRTime;
	calFirstRTime.SetTimeLocalFloatingL(firstRTime);
	RArray<TCalTime> firstRDates;
	firstRDates.Append(calFirstRTime);
	firstEntry->SetRDatesL(firstRDates);
	
	CCalAlarm *firstAlarm = CCalAlarm::NewL();
	firstAlarm->SetTimeOffset(-60); // this offset pushes the alarm to the next day
	CleanupStack::PushL(firstAlarm);
	firstEntry->SetAlarmL(firstAlarm); //EXPECT THIS ALARM TO LEAVE
	CleanupStack::Pop(firstAlarm);
	
	aEntries.Append(firstEntry);
	
	CleanupStack::Pop(firstEntry);
	}	
void CTestCalInterimApiExportImportNegOffset::setUpEntriesL(RPointerArray<CCalEntry>& aEntries)
	{
		//BEGIN:VAPPT
		//TIMEMODE:EFloatingTimeZone
		//UID:6
		//DTSTART:19990316:230000.000000
		//DTEND:19990317:000000.000000
		//RRULE:DAILY
		//INTERVAL:1
		//COUNT:3
		//RDATE:19990105:090000.000000
		//ALARM:-59
		//END:VAPPT	
	HBufC8* firstGuid = _L8("6").AllocLC();
	CCalEntry* firstEntry = CCalEntry::NewL(CCalEntry::EAppt,firstGuid,CCalEntry::EMethodNone, 0 );
	CleanupStack::Pop(firstGuid);
	CleanupStack::PushL(firstEntry);
	
	TTime startTime(TDateTime(1999, EMarch, 15, 23, 00, 0, 0)); // 23:00:00 on March 16 1999
	TCalTime calStartTime;
	calStartTime.SetTimeLocalFloatingL(startTime);
	TTime endTime(TDateTime(1999, EMarch, 16, 0, 0, 0, 0)); // 12:00 on March 17 1999
	TCalTime calEndTime;
	calEndTime.SetTimeLocalFloatingL(endTime);
	firstEntry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(1);
	rule.SetDtStart(calStartTime); // every day from start time for 3 days
	rule.SetCount(3);
	firstEntry->SetRRuleL(rule);
	
	TTime firstRTime(TDateTime(1999, EJanuary, 4, 9, 0, 0, 0)); // 09:00 on January 5 1999
	TCalTime calFirstRTime;
	calFirstRTime.SetTimeLocalFloatingL(firstRTime);
	RArray<TCalTime> firstRDates;
	firstRDates.Append(calFirstRTime);
	firstEntry->SetRDatesL(firstRDates);
	
	CCalAlarm *firstAlarm = CCalAlarm::NewL();
	firstAlarm->SetTimeOffset(-59); //this offset keeps the alarm on the same day
	CleanupStack::PushL(firstAlarm);
	firstEntry->SetAlarmL(firstAlarm);
	CleanupStack::Pop(firstAlarm);
	
	aEntries.Append(firstEntry);
	
		//BEGIN:VTODO
		//TIMEMODE:EFloatingTimeZone
		//UID:5
		//DTSTART:19990316:230000.000000
		//DUE:19990317:000000.000000
		//RRULE:DAILY
		//INTERVAL:1
		//COUNT:3
		//RDATE:19990105:090000.000000
		//ALARM:-60
		//END:VTODO
		
	HBufC8* secondGuid = _L8("5").AllocLC();
	CCalEntry* secondEntry = CCalEntry::NewL(CCalEntry::ETodo,secondGuid,CCalEntry::EMethodNone, 0 );
	CleanupStack::Pop(secondGuid);
	CleanupStack::PushL(secondEntry);
	
	TTime secondStartTime(TDateTime(1999, EMarch, 15, 23, 0, 0, 0)); // 23:00:00 on March 16 1999
	TCalTime secondCalStartTime;
	secondCalStartTime.SetTimeLocalFloatingL(secondStartTime);
	TTime secondEndTime(TDateTime(1999, EMarch, 16, 0, 0, 0, 0)); // 12:00 on March 17 1999
	TCalTime secondCalEndTime;
	secondCalEndTime.SetTimeLocalFloatingL(secondEndTime);
	secondEntry->SetStartAndEndTimeL(secondCalStartTime, secondCalEndTime);
	
	TCalRRule secondRule(TCalRRule::EDaily);
	secondRule.SetInterval(1);
	secondRule.SetDtStart(secondCalStartTime); // every day from start time for 3 days
	secondRule.SetCount(3);
	secondEntry->SetRRuleL(secondRule);
	
	TTime secondRTime(TDateTime(1999, EJanuary, 4, 9, 0, 0, 0)); // 09:00 on January 5 1999
	TCalTime secondCalRTime;
	secondCalRTime.SetTimeLocalFloatingL(secondRTime);
	RArray<TCalTime> secondRDates;
	secondRDates.Append(secondCalRTime);
	secondEntry->SetRDatesL(secondRDates);
	
	CCalAlarm *secondAlarm = CCalAlarm::NewL();
	secondAlarm->SetTimeOffset(-60);
	CleanupStack::PushL(secondAlarm);
	secondEntry->SetAlarmL(secondAlarm);
	CleanupStack::Pop(secondAlarm);
	
	aEntries.Append(secondEntry);
	
	CleanupStack::Pop(2);//firstEntry, secondEntry

	}	

TVerdict CTestCalInterimApiExportImportNegOffset::doTestStepL()	
	{
	_LIT(KLondon, "Europe/London");
	TBufC<14> timeZone(KLondon);
	SetTZL(timeZone);
	TESTL(TestStepResult() == EPass);
	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
	
	// set up the first set of entries (expect three entries)
	RPointerArray<CCalEntry> entries;
	CleanupRPtrArrayDeleteAndCloseL(entries);
	RPointerArray<CCalEntry> fetched;
	CleanupRPtrArrayDeleteAndCloseL(fetched);
	TInt err;
	TRAP(err,setUpEntriesWithAlarmOnNextDayL(entries)); //next day alarms are not supported
	if(KErrNotSupported != err)
		{
			User::Leave(err);
		}
	setUpEntriesL(entries);
	TESTL(2 == entries.Count());

	// store the entries in the model & verify the count
	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	
	// fetch the entries back from the model
	view->FetchL(entries[0]->UidL(), fetched);	
	view->FetchL(entries[1]->UidL(), fetched);
	
	//build the data exchange object and Export the entries
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile outFile;
	CleanupClosePushL(outFile);
	err = outFile.Replace(fs, KEntryExportFileNegOffset, EFileWrite);
	User::LeaveIfError(err);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	
	dataExchange->ExportL(KUidVCalendar, writeStream, fetched);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile);  // outFile.Close()
	
	//clear the entries array it is ready for import
	entries.ResetAndDestroy();
	
	//import the entries back
	RFile inFile;
	CleanupClosePushL(inFile); 
	err = inFile.Open(fs, KEntryExportFileNegOffset, EFileRead);
	User::LeaveIfError(err);
	RFileReadStream readStream(inFile);
	CleanupClosePushL(readStream); 
	dataExchange->ImportL(KUidVCalendar, readStream, entries);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&inFile);

	//test imported data for alarm offset.
	TESTL(entries.Count() == 2);  // we know there are only 2 events in the .vcs file
	TTimeIntervalMinutes firstExpectedOffset = -59;
	TTimeIntervalMinutes secondExpectedOffset = -60;
	TESTL( entries[0]->AlarmL()->TimeOffset() == firstExpectedOffset);
	TESTL( entries[1]->AlarmL()->TimeOffset() == secondExpectedOffset);
	
	//cleanup the arrays
	fetched.ResetAndDestroy();
	entries.ResetAndDestroy();
	
	RestoreTZL();
	
	CleanupStack::PopAndDestroy(6);
	
	return TestStepResult();
	}
	
