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

#include "TestCalInterimApiRichAlarmStep.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <s32file.h>

_LIT8(KAlarmExportTestGUID0, "AlarmExportTestGUID0");
_LIT8(KAlarmExportTestGUID1, "AlarmExportTestGUID1");
_LIT8(KAlarmExportTestGUID2, "AlarmExportTestGUID2");
_LIT8(KAlarmExportTestGUID3, "AlarmExportTestGUID3");
_LIT8(KAlarmExportTestGUID4, "AlarmExportTestGUID4");
_LIT8(KAlarmExportTestGUID5, "AlarmExportTestGUID5");

_LIT (KCreateNewAgendaFile, "Creating New Agenda file %S");
_LIT(KAgendaFile,"C:CITVCal.dat");
_LIT(KEntryExportFile,"c:\\tcal_dataexchange_export.vcs");
// _LIT(KEntryExportFile2,"c:\\tcal_dataexchange_export2.vcs");  //For Extra Debugging
_LIT(KJPEGDataFile, "\\longtail2.jpg");
_LIT(KEntryNo, "Adding Entry With No Alarm");
_LIT(KContentBasic, "Setting and Getting Alarm Content");
_LIT(KEntryAdded, "Entry Added");
_LIT(KEntrySimple, "Adding Entry With Basic Alarm Sound");
_LIT(KEntryURLOnly, "Adding Entry with URL Alarm Action");
_LIT(KEntryMIMEOnly, "Adding Entry with inline Alarm Action");
_LIT(KEntryBoth, "Adding Entry with both simple alarm sound and URL alarm action");
_LIT(KExporting, "Exporting entries to file");
_LIT(KImporting, "Importing entries from file");
_LIT(KStoring, "Storing Entries in model");


CTestCalInterimApiRichAlarmStep::~CTestCalInterimApiRichAlarmStep()
/**
 * Destructor
 */
	{
	delete iEntryView;
	delete iSession;
	WaitForAgendaServerShutdown();
	}

CTestCalInterimApiRichAlarmStep::CTestCalInterimApiRichAlarmStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiRichAlarmStep);
	}

TVerdict CTestCalInterimApiRichAlarmStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Start Alarm Exporting/Importing Tests"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestCalInterimApiRichAlarmStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalInterimApiRichAlarmStep::doTestStepL()
	{
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	iSession  = CCalSession::NewL();

	CTestCalInterimApiCallbackForRichAlarms* alarmCallback = CTestCalInterimApiCallbackForRichAlarms::NewL(this);
	CleanupStack::PushL(alarmCallback);

	OpenSessionFileL();
	iEntryView = CCalEntryView::NewL(*iSession,*alarmCallback);

	SetTestStepResult(EFail);

	CActiveScheduler::Add(alarmCallback);
	CActiveScheduler::Start();

	//An array to store the Cal Alarm Data for comparison later
	RPointerArray<CCalAlarm> calAlarmArray;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayAlarms, &calAlarmArray));
	calAlarmArray.Reset();


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
	INFO_PRINTF1(KEntryNo);
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
	INFO_PRINTF1(KEntryAdded);

	//
	// Entry with basic alarm (sound) only
	//
	INFO_PRINTF1(KEntrySimple);
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
	calAlarmArray.AppendL(alarm2);
	CleanupStack::Pop(alarm2);
	alarm2->SetAlarmSoundNameL(_L("c:\\mysound"));
	alarm2->SetTimeOffset(100);
	entry2->SetAlarmL(alarm2);
	numEntriesToBeStored++;
	INFO_PRINTF1(KEntryAdded);

	//
	// Entry with URL alarm action and no sound name
	//
	INFO_PRINTF1(KEntryURLOnly);
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
	calAlarmArray.AppendL(alarm3);
	CleanupStack::Pop(alarm3);
	_LIT8(KAlarmURL1, "http://www.symbian.com");
	_LIT8(KAlarmURLType, "text/http");

	CCalContent* content3 = CCalContent::NewL();
	CleanupStack::PushL(content3);

	HBufC8* urlContentBuf3 = HBufC8::NewL(KAlarmURL1().Length());
	CleanupStack::PushL(urlContentBuf3);
	*urlContentBuf3 = KAlarmURL1();

	HBufC8* urlTypeBuf3 = HBufC8::NewL(KAlarmURLType().Length());
	CleanupStack::PushL(urlTypeBuf3);
	*urlTypeBuf3 = KAlarmURLType();

	content3->SetContentL(urlContentBuf3, urlTypeBuf3, CCalContent::EDispositionUrl);
	CleanupStack::Pop(2, urlContentBuf3); // SetContentL takes ownership

	alarm3->SetAlarmAction(content3); //takes ownership
	CleanupStack::Pop(content3);
	entry3->SetAlarmL(alarm3);
	numEntriesToBeStored++;
	INFO_PRINTF1(KEntryAdded);

	//
	// Entry with inline alarm action and no sound name
	//
	// read in a jpeg
	_LIT(KReadJpg, "Reading a jpg from a file");
	INFO_PRINTF1(KReadJpg);

	RFs   fs;
	fs.Connect();
	CleanupClosePushL(fs);
	RFile dataFile;
	CleanupClosePushL(dataFile);
	 
	TFileName jpegDataFile;
#ifdef __WINS__
	jpegDataFile.Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	jpegDataFile.Append(parse.Drive());
#endif
	jpegDataFile.Append(KJPEGDataFile);
	
	TInt errData = dataFile.Open(fs, jpegDataFile, EFileRead);
	if(errData != KErrNone)
		{
		_LIT(KReadJpgOpenErr, "Error Opening the jpg file, %d");
		INFO_PRINTF2(KReadJpgOpenErr, errData);
		CleanupStack::PopAndDestroy(6, scheduler);
		SetTestStepResult(ETestSuiteError);
		return (TestStepResult());
		}
	TInt dataSize;
	errData = dataFile.Size(dataSize);
	if(errData != KErrNone)
		{
		_LIT(KReadJpgSizeErr, "Error Reading the size from the jpg file, %d");
		INFO_PRINTF2(KReadJpgSizeErr, errData);
		CleanupStack::PopAndDestroy(6, scheduler);
		SetTestStepResult(ETestSuiteError);
		return (TestStepResult());
		}

	HBufC8* dataBuf = HBufC8::NewL(dataSize);
	CleanupStack::PushL(dataBuf);
	TPtr8 dataPtr(dataBuf->Des());
	errData = dataFile.Read(dataPtr);
	if(errData != KErrNone)
		{
		_LIT(KReadJpgReadErr, "Error Reading the jpg from file, %d");
		INFO_PRINTF2(KReadJpgReadErr, errData);
		CleanupStack::PopAndDestroy(7, scheduler);
		SetTestStepResult(ETestSuiteError);
		return (TestStepResult());
		}
	CleanupStack::Pop(dataBuf);
	CleanupStack::PopAndDestroy(&dataFile);

	// We now have the jpeg in a data buffer, do the alarm stuff
	INFO_PRINTF1(KEntryMIMEOnly);
	seqNo++;
	HBufC8* pGUID4= KAlarmExportTestGUID4().AllocLC();
	CCalEntry* entry4 = CCalEntry::NewL(CCalEntry::EAppt, pGUID4,
	                                    CCalEntry::EMethodAdd, seqNo);
	CleanupStack::PushL(entry4);
	firstEntryArray.AppendL(entry4);
	CleanupStack::Pop(entry4);
	CleanupStack::Pop(pGUID4); //entry takes ownership

	TDateTime entryStart4(2006, ESeptember, 7, 10, 0, 0, 0);
	TCalTime  calStart4;
	calStart4.SetTimeLocalL(entryStart4);
	TDateTime entryStop4(2006, ESeptember, 7, 11, 0, 0, 0);
	TCalTime calStop4;
	calStop4.SetTimeLocalL(entryStop4);
	entry4->SetStartAndEndTimeL(calStart4, calStop4);

	CCalAlarm* alarm4 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm4);
	calAlarmArray.AppendL(alarm4);
	CleanupStack::Pop(alarm4);
	CCalContent* content4 = CCalContent::NewL();
	CleanupStack::PushL(content4);
	_LIT8(KAlarmJpegType, "image/jpeg");

	HBufC8* jpegTypeBuf = HBufC8::NewL(KAlarmJpegType().Length());
	CleanupStack::PushL(jpegTypeBuf);
	*jpegTypeBuf = KAlarmJpegType();

	content4->SetContentL(dataBuf, jpegTypeBuf, CCalContent::EDispositionInline);
	CleanupStack::Pop(jpegTypeBuf); // SetContentL takes ownership
	alarm4->SetAlarmAction(content4); // takes ownership
	CleanupStack::Pop(content4);
	entry4->SetAlarmL(alarm4);
	numEntriesToBeStored++;
	INFO_PRINTF1(KEntryAdded);

	// The following commented-out code may help in debugging.
	// The code is preceded by EXTRA-DEBUG for help in recognizing it.Removing the
	// comments may help give more information as to what is going on when there is
	// a problem with this test step.
	// File weasel.jpg should not be recognized by a jpeg reader as it has stream
	// information at it's beginning.
	// File weasel2.jpg should be recognized by a jpeg reader and should be the same
	// image as longtail.jpg, the original jpg read in for this test
	// File weasel3.jpg should also be the same as longtail.jpg, but will not be
	// output until after the export and import of the entries is completed.
	//EXTRA-DEBUG _LIT(KWeaselFile1, "c:\\weasel.jpg");
	//EXTRA-DEBUG _LIT(KWeaselFile2, "c:\\weasel2.jpg");
	//EXTRA-DEBUG _LIT(KWeaselFile3, "c:\\weasel3.jpg");

	//EXTRA-DEBUG RFile wFile;
	//EXTRA-DEBUG TInt werr = wFile.Replace(fs, KWeaselFile1, EFileWrite);
	//EXTRA-DEBUG RFileWriteStream wStream(wFile);
	//EXTRA-DEBUG CleanupClosePushL(wStream);
	//EXTRA-DEBUG wStream << data16;
	//EXTRA-DEBUG wStream.CommitL();
	//EXTRA-DEBUG wFile.Close();
	//EXTRA-DEBUG CleanupStack::PopAndDestroy(&wStream);

	//EXTRA-DEBUG RFile bleah;
	//EXTRA-DEBUG werr = bleah.Open(fs, KWeaselFile1, EFileRead);
	//EXTRA-DEBUG RFileReadStream rStream(bleah);
	//EXTRA-DEBUG CleanupClosePushL(rStream);
	//EXTRA-DEBUG rStream >> data16;
	//EXTRA-DEBUG CleanupStack::PopAndDestroy(&rStream);

	//EXTRA-DEBUG werr = wFile.Replace(fs, KWeaselFile2, EFileWrite);
	//EXTRA-DEBUG TPtr8 ugh = data16.Collapse();
	//EXTRA-DEBUG wFile.Write(ugh);
	//EXTRA-DEBUG wFile.Close();



	//
	// Entry with both basic alarm (sound) and alarm action (URL)
	//
	INFO_PRINTF1(KEntryBoth);
	seqNo++;
	HBufC8* pGUID5= KAlarmExportTestGUID5().AllocLC();
	CCalEntry* entry5 = CCalEntry::NewL(CCalEntry::EAppt, pGUID5,
	                                    CCalEntry::EMethodAdd, seqNo);
	CleanupStack::PushL(entry5);
	firstEntryArray.AppendL(entry5);
	CleanupStack::Pop(entry5);
	CleanupStack::Pop(pGUID5); // entry takes ownership

	TDateTime entryStart5(2006, EMay, 7, 10, 0, 0, 0);
	TCalTime  calStart5;
	calStart5.SetTimeLocalL(entryStart5);
	TDateTime entryStop5(2006, EMay, 7, 11, 0, 0, 0);
	TCalTime calStop5;
	calStop5.SetTimeLocalL(entryStop5);
	entry5->SetStartAndEndTimeL(calStart5, calStop5);

	CCalAlarm* alarm5 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm5);
	calAlarmArray.AppendL(alarm5);
	CleanupStack::Pop(alarm5);
	alarm5->SetAlarmSoundNameL(_L("c:\\mysound2"));
	_LIT8(KAlarmURL2, "http://www.symbian2.com");
	CCalContent* content5 = CCalContent::NewL();
	CleanupStack::PushL(content5);

	HBufC8* urlContentBuf5 = HBufC8::NewL(KAlarmURL2().Length());
	CleanupStack::PushL(urlContentBuf5);
	*urlContentBuf5 = KAlarmURL2();

	HBufC8* urlTypeBuf5 = HBufC8::NewL(KAlarmURLType().Length());
	CleanupStack::PushL(urlTypeBuf5);
	*urlTypeBuf5 = KAlarmURLType();

	content5->SetContentL(urlContentBuf5, urlTypeBuf5, CCalContent::EDispositionUrl);
	CleanupStack::Pop(2, urlContentBuf5); // SetContentL takes ownership

	alarm5->SetAlarmAction(content5); // takes ownership
	CleanupStack::Pop(content5);
	entry5->SetAlarmL(alarm5);
	numEntriesToBeStored++;
	INFO_PRINTF1(KEntryAdded);

	//
	// CalInterimAPI basic set/get test cases://
	//

	INFO_PRINTF1(KContentBasic);

	HBufC8* pGUID0= KAlarmExportTestGUID0().AllocLC();
	CCalEntry* entry0 = CCalEntry::NewL(CCalEntry::EAppt, pGUID0,
	                                    CCalEntry::EMethodAdd, seqNo);
	CleanupStack::Pop(pGUID0); // entry takes ownership
	CleanupStack::PushL(entry0);

	TDateTime entryStart0(2006, EMay, 7, 10, 0, 0, 0);
	TCalTime  calStart0;
	calStart0.SetTimeLocalL(entryStart0);
	TDateTime entryStop0(2006, EMay, 7, 11, 0, 0, 0);
	TCalTime calStop0;
	calStop5.SetTimeLocalL(entryStop0);
	entry0->SetStartAndEndTimeL(calStart0, calStop0);

	CCalAlarm* alarm0 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm0);
	CCalContent* content0 = CCalContent::NewL();
	CleanupStack::PushL(content0);

	// set the associated data, perform a direct copy
	_LIT8(KAlarmURL0, "http://www.symbian2.com");
	HBufC8* urlBufContent0 = HBufC8::NewL(KAlarmURL0().Length());
	CleanupStack::PushL(urlBufContent0);
	*urlBufContent0 = KAlarmURL0();
	TPtrC8 origContentPtr(*urlBufContent0);
	HBufC8* urlTypeBuf0 = HBufC8::NewL(KAlarmURLType().Length());
	CleanupStack::PushL(urlTypeBuf0);
	*urlTypeBuf0 = KAlarmURLType();
	TPtrC8 origTypePtr(*urlTypeBuf0);
	content0->SetContentL(urlBufContent0, urlTypeBuf0, CCalContent::EDispositionUrl);
	CleanupStack::Pop(2, urlBufContent0); // SetContentL takes  ownership

	// retrieve the data we just set
	TPtrC8 dataContentTPtr(content0->Content());
	TPtrC8 dataMimeTypeTPtr(content0->MimeType());
	CCalContent::TDisposition disposition = content0->Disposition();

	if ((dataContentTPtr != origContentPtr) || (dataMimeTypeTPtr != origTypePtr) ||
			 disposition != CCalContent::EDispositionUrl )
		{
		_LIT(KContentMatchErr, "Alarm action content does not match values created");
		INFO_PRINTF1(KContentMatchErr);
		CleanupStack::PopAndDestroy(8, scheduler);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// everything matches so far, okay to continue.....

	alarm0->SetTimeOffset(100);

	alarm0->SetAlarmAction(content0); // takes ownership
	CleanupStack::Pop(content0);

	// set the alarm in the Calendar entry (create a copy in CAgnEntry)
	entry0->SetAlarmL(alarm0);

	// now retrieve the alarm from the calendar entry
	CCalAlarm* alarmRetrieved = entry0->AlarmL();

	if (alarmRetrieved)
		{
		const CCalContent* contentRetrieved = alarmRetrieved->AlarmAction();

		TPtrC8 dataContentRetrieved(contentRetrieved->Content());
		TPtrC8 dataMimeTypeRetrieved(contentRetrieved->MimeType());
		CCalContent::TDisposition dispositionRetrieved = contentRetrieved->Disposition();

		if ((dataContentRetrieved != KAlarmURL0()) || (dataMimeTypeRetrieved != KAlarmURLType()) ||
			 disposition != CCalContent::EDispositionUrl )
			{
			_LIT(KContentMatchErr, "Alarm action content does not match values created");
			INFO_PRINTF1(KContentMatchErr);
			CleanupStack::PopAndDestroy(7, scheduler);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		}

	// we are done with this entry, no need to save it
	CleanupStack::PopAndDestroy(alarm0);
	CleanupStack::PopAndDestroy(entry0);


	//store the entries and check
	INFO_PRINTF1(KStoring);
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

	//build the CCalDataExchange object and Export
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange);
	RFile outFile;
	CleanupClosePushL(outFile);
	TInt err = outFile.Replace(fs, KEntryExportFile, EFileWrite);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);

	INFO_PRINTF1(KExporting);
	TInt writeCount = firstEntryArray.Count();
	dataExchange->ExportL(KUidVCalendar, writeStream, firstEntryArray);

	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile); // outFile.Close()

	// The entries have been externalized. We need to delete them so that
	// when we internalize them, they are not copies and can be retrieved,
	// then re-externalized. The final test will be a comparison of the
	// externalized files.
	TInt idx = 0;
	for (idx=0; idx < writeCount; idx++)
		{
		iEntryView->DeleteL(*(firstEntryArray[idx]));
		}

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

	//check the results of the ImportL
	TInt readCount = secondEntryArray.Count();

	// Store the second group of entries & compare the alarms
	INFO_PRINTF1(KStoring);
	if (readCount == writeCount)
		{
		TInt numEntriesStored2 = 0;
		iEntryView->StoreL(secondEntryArray, numEntriesStored2);
		if (numEntriesStored2 != readCount)
			{
			_LIT(KEntryStore2, "Num Entries stored (%d) does not match num entries imported (%d)");
			INFO_PRINTF3(KEntryStore2, numEntriesStored2, readCount);
			CleanupStack::PopAndDestroy(7, scheduler);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		SetTestStepResult(CompareAlarmDataL(&calAlarmArray, &secondEntryArray));
		}
	else
		{
		_LIT(KEntryInOutErr, "Num Entries Imported (%d) does not match num entries Exported (%d)");
		INFO_PRINTF3(KEntryInOutErr, readCount, writeCount);
		SetTestStepResult(EFail);
		}

	// In addition to looking at the jpegs, it may be useful to see what the imported
	// entries look like when they are exported again. The only difference between the two
	// output files should be creation and modification times.
	// Export to a a second file the entries that we have imported
	//EXTRA-DEBUG RFile outF2;
	//EXTRA-DEBUG CleanupClosePushL(outF2);
	//EXTRA-DEBUG TInt smoosh = outF2.Replace(fs, KEntryExportFile2, EFileWrite);
	//EXTRA-DEBUG RFileWriteStream ws(outF2);
	//EXTRA-DEBUG CleanupClosePushL(ws);
	//EXTRA-DEBUG dataExchange->ExportL(KUidVCalendar, ws, secondEntryArray);
	//EXTRA-DEBUG ws.CommitL();
	//EXTRA-DEBUG CleanupStack::PopAndDestroy(&ws);
	//EXTRA-DEBUG CleanupStack::PopAndDestroy(&outF2);


	CleanupStack::PopAndDestroy(7, scheduler);

	return TestStepResult();
	}

TVerdict CTestCalInterimApiRichAlarmStep::CompareAlarmDataL(RPointerArray<CCalAlarm>* aAlarmArray, RPointerArray<CCalEntry>* aEntryArray)
	{
	const TInt KEntryCount = aEntryArray->Count();
	const TInt KAlarmCount = aAlarmArray->Count();
	TInt i = 0;

	// First entry should have no alarm data
	if ((*aEntryArray)[0]->AlarmL())
		{
		_LIT(KEntryFirstErr, "First Entry has unexpected alarm");
		INFO_PRINTF1(KEntryFirstErr);

		return EFail;
		}

	_LIT(KEntryAlarmNotFound, "No alarm found, entry %d");
	_LIT(KEntryAlarmMatchErr, "Alarms do not match, entry %d");

	// now compare the rest of the alarms
	for (i = 1; i < KEntryCount; ++i)
		{
		CCalAlarm* alarm2 = (*aEntryArray)[i]->AlarmL();
		// Make sure the alarms are there
		if (!alarm2)
			{
			INFO_PRINTF2(KEntryAlarmNotFound, i+1);
			return EFail;
			}

		TBool alarmFound = EFalse;
		for (TInt j = 0; j < KAlarmCount; ++j)
			{
			CCalAlarm* alarm1 = (*aAlarmArray)[j];

			// check the basic alarm data
			if ((alarm1->TimeOffset() == alarm2->TimeOffset()) ||
				(alarm1->AlarmSoundNameL() == alarm2->AlarmSoundNameL()))
				{
				//check the alarm action
				const CCalContent* content1 = alarm1->AlarmAction();
				const CCalContent* content2 = alarm2->AlarmAction();
				if (NULL == content1 && NULL == content2)
					{
					alarmFound = ETrue;
					break;
					}
				if (NULL != content1 && NULL != content2)
					{
					if (content1->Content() == content2->Content() &&
						content1->MimeType() == content2->MimeType() &&
						content1->Disposition() == content2->Disposition())
						{
						alarmFound = ETrue;
						break;
						}
					}
				}
			}

		if (!alarmFound)
			{
			INFO_PRINTF2(KEntryAlarmMatchErr, i);
			return EFail;
			}
		}
	return EPass;
	}



void CTestCalInterimApiRichAlarmStep::OpenSessionFileL()
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


void CTestCalInterimApiRichAlarmStep::CloseAndDeleteRPtrArrayEntries(TAny* aObject)
	{
	RPointerArray<CCalEntry>* array=REINTERPRET_CAST(RPointerArray<CCalEntry>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

void CTestCalInterimApiRichAlarmStep::CloseAndDeleteRPtrArrayAlarms(TAny* aObject)
	{
	RPointerArray<CCalAlarm>* array=REINTERPRET_CAST(RPointerArray<CCalAlarm>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

CTestCalInterimApiCallbackForRichAlarms* CTestCalInterimApiCallbackForRichAlarms::NewL(CTestCalInterimApiSuiteStepBase* aParent)
	{
	CTestCalInterimApiCallbackForRichAlarms* self = new (ELeave) CTestCalInterimApiCallbackForRichAlarms(aParent);
	return self;
	}

CTestCalInterimApiCallbackForRichAlarms::~CTestCalInterimApiCallbackForRichAlarms()
	{
	Cancel();
	}

CTestCalInterimApiCallbackForRichAlarms::CTestCalInterimApiCallbackForRichAlarms(CTestCalInterimApiSuiteStepBase* aParent)
	: CActive(EPriorityHigh), iTestCalInterimApiSuiteStep(aParent)
	{
	}

void CTestCalInterimApiCallbackForRichAlarms::Start()
	{
	if (!IsActive())
		{
		SetActive();
		}

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrNone);
	}

void CTestCalInterimApiCallbackForRichAlarms::Progress(TInt /*aPercentageCompleted*/)
	{
	}

void CTestCalInterimApiCallbackForRichAlarms::Completed(TInt /*aComplete*/)
	{
	Start();
	}

TBool CTestCalInterimApiCallbackForRichAlarms::NotifyProgress()
	{
	return ETrue;
	}

 void CTestCalInterimApiCallbackForRichAlarms::DoCancel()
	{
	Close();
	}

void CTestCalInterimApiCallbackForRichAlarms::RunL()
	{
	Close();
	}

void CTestCalInterimApiCallbackForRichAlarms::Close()
	{
	CActiveScheduler::Stop();
	}


