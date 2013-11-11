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

#include "TestCalInterimApiRichAlarmFormatStep.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <s32file.h>


_LIT(KEntryImportFile,"\\tcal_dataexch_format.vcs");
_LIT(KEntryBadImportFile, "\\tcal_dataexch_Badformat.vcs");
#define KNumEntriesInFile 5  // we know there are five entries in the file

_LIT(KJPEGDataFile, "\\longtail2.jpg");
_LIT(KImporting, "Importing entries from file");
_LIT(KEntryAlarmNotFound, "No alarm found, entry %d");
_LIT(KUnexpectedAlarmAction, "Alarm action found from corrupt X-EPOCALARM in vcal file");		
_LIT (KCreateNewAgendaFile, "Creating New Agenda file %S");
_LIT(KAgendaFile,"C:CITVCal.dat");

TFileName GetFullFileName(const TDesC& aPath)
	{
	TFileName fileName;
#ifdef __WINS__
	fileName.Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	fileName.Append(parse.Drive());
#endif
	fileName.Append(aPath);
	return fileName;
	}

CTestCalInterimApiRichAlarmFormatStep::~CTestCalInterimApiRichAlarmFormatStep()
/**
 * Destructor
 */
	{
	delete iEntryView;
	delete iSession;
	WaitForAgendaServerShutdown();
	}

CTestCalInterimApiRichAlarmFormatStep::CTestCalInterimApiRichAlarmFormatStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiRichAlarmFormatStep);
	}

TVerdict CTestCalInterimApiRichAlarmFormatStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Start Alarm Exporting/Importing Format Tests"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestCalInterimApiRichAlarmFormatStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalInterimApiRichAlarmFormatStep::doTestStepL()
	{
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(scheduler);  
	CActiveScheduler::Install(scheduler);

	iSession  = CCalSession::NewL();
	
	CTestCalInterimApiCallbackForRichAlarms* alarmCallback = CTestCalInterimApiCallbackForRichAlarms::NewL(this);
	CleanupStack::PushL(alarmCallback); 

	OpenSessionFileL();
	iEntryView = CCalEntryView::NewL(*iSession,*alarmCallback);
	
	CActiveScheduler::Add(alarmCallback);
	CActiveScheduler::Start();
	
	//build the CCalDataExchange object and import
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(*iSession);
	CleanupStack::PushL(dataExchange); 

	//IMPORT the calendar info from a known file
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);
	RFile inFile;
	CleanupClosePushL(inFile);
	TInt errR = inFile.Open(fs, GetFullFileName(KEntryImportFile), EFileRead);
	RFileReadStream readStream(inFile);
	CleanupClosePushL(readStream);
	
	RPointerArray<CCalEntry> secondEntryArray;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &secondEntryArray));
	secondEntryArray.Reset();
	
	INFO_PRINTF1(KImporting);
	TInt index = 0;
	for (index = 0; index < KNumEntriesInFile; index++)
		{
		dataExchange->ImportL(KUidVCalendar, readStream, secondEntryArray);
		}
	
	CleanupStack::Pop(&secondEntryArray);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&inFile);
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &secondEntryArray));

	SetTestStepResult(CompareAlarmDataL(&secondEntryArray));
	
	//Test for importing from a badly formatted file
	RFile badInFile;
	CleanupClosePushL(badInFile);
	TInt baddErr = badInFile.Open(fs, GetFullFileName(KEntryBadImportFile), EFileRead);
	RFileReadStream badStream(badInFile);
	CleanupClosePushL(badStream);
	
	RPointerArray<CCalEntry> badEntryArray;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &badEntryArray));
	badEntryArray.Reset();
	
	INFO_PRINTF1(KImporting);
	TInt i = 0;
	for (i = 0; i < 1; i++)
		{
		dataExchange->ImportL(KUidVCalendar, badStream, badEntryArray);
		}
	CCalEntry* pEntry = badEntryArray[0];
	CCalAlarm* pAlarm = pEntry->AlarmL();
	if (pAlarm)
		{
		CleanupStack::PushL(pAlarm);
		CCalContent* pContent = pAlarm->AlarmAction();
		if (pContent != NULL)
			{
			ERR_PRINTF1(KUnexpectedAlarmAction);
			SetTestStepResult(EFail);
			}
		delete pContent;
		CleanupStack::PopAndDestroy(pAlarm);
		}
	CleanupStack::PopAndDestroy(&badEntryArray);
	CleanupStack::PopAndDestroy(&badStream);
	CleanupStack::PopAndDestroy(&badInFile);

	
	CleanupStack::PopAndDestroy(5 , scheduler);

	return TestStepResult();
	}

TVerdict CTestCalInterimApiRichAlarmFormatStep::CompareAlarmDataL(RPointerArray<CCalEntry>* aEntryArray)
	{	
	// This method is based on the five entres known to be in the imported file.
	// There is a separate method for each one (except the first, which has no alarm)
	// in an effort to keep this method resonably short and readable. 
	// The methods themselves will take care of any failure printing.
	// This means that NEW METHODS will be needed should new entries be added to the file.
	
	// The first one should have no alarm data
	if ((*aEntryArray)[0]->AlarmL())
		{
		_LIT(KEntryFirstErr, "First Entry has unexpected alarm");
		INFO_PRINTF1(KEntryFirstErr);

		return EFail;
		}
		
	// now compare the rest of the alarms
	
	if (!CheckAlarm1L((*aEntryArray)[1]->AlarmL()))
		{
		return EFail;
		}
		
	if (!CheckAlarm2L((*aEntryArray)[2]->AlarmL()))
		{
		return EFail;
		}

	if (!CheckAlarm3L((*aEntryArray)[3]->AlarmL()))
		{
		return EFail;
		}

	if (!CheckAlarm4L((*aEntryArray)[4]->AlarmL()))
		{
		return EFail;
		}

	return EPass;
	}
	
_LIT(KEntryAlarmWrongSound, "Wrong sound found for entry %d");		
_LIT(KEntryAlarmActionNotFound, "Alarm action not found for entry %d");		
_LIT(KEntryAlarmActionContentWrong, "Wrong content for alarm action for entry %d");		
_LIT(KEntryAlarmActionMimeTypeWrong, "Wrong MIME type for alarm action for entry %d");		
_LIT(KEntryAlarmActionDispositionWrong, "Wrong disposition for alarm action for entry %d");		
	
TBool CTestCalInterimApiRichAlarmFormatStep::CheckAlarm1L(CCalAlarm* aAlarm)
	{
	TInt alarmNum = 1;
	if (NULL == aAlarm)
		{
		INFO_PRINTF2(KEntryAlarmNotFound, alarmNum);
		return EFalse;
		}
		
	// This should be a basic AALARM. We'll just check the sound name
	if (aAlarm->AlarmSoundNameL() != _L("c:\\mysound"))
		{
		INFO_PRINTF2(KEntryAlarmWrongSound, alarmNum);
		return EFalse;
		}
			
	return ETrue;
	}

TBool CTestCalInterimApiRichAlarmFormatStep::CheckAlarm2L(CCalAlarm* aAlarm)
	{
	TInt alarmNum = 2;
	if (NULL == aAlarm)
		{
		INFO_PRINTF2(KEntryAlarmNotFound, alarmNum);
		return EFalse;
		}
		
	// This should be an extended alarm, just using the AALARM for timing.
	// Make sure there is no sound name set
		if (aAlarm->AlarmSoundNameL() != _L(""))
		{
		INFO_PRINTF2(KEntryAlarmWrongSound, alarmNum);
		return EFalse;
		}
		
	const CCalContent* alarmAction = aAlarm->AlarmAction();
	if (NULL == alarmAction)
		{
		INFO_PRINTF2(KEntryAlarmActionNotFound, alarmNum);
		return EFalse;
		}
		
	_LIT8(KAlarmURL1, "http://www.symbian.com");
	if (alarmAction->Content() != KAlarmURL1)
		{
		INFO_PRINTF2(KEntryAlarmActionContentWrong, alarmNum);
		return EFalse;
		}
		
	if (alarmAction->MimeType() != _L8("text/http"))
		{
		INFO_PRINTF2(KEntryAlarmActionMimeTypeWrong, alarmNum);
		return EFalse;
		}

	if (alarmAction->Disposition() != CCalContent::EDispositionUrl)
		{
		INFO_PRINTF2(KEntryAlarmActionDispositionWrong, alarmNum);
		return EFalse;
		}
	
	return ETrue;
	}
	
TBool CTestCalInterimApiRichAlarmFormatStep::CheckAlarm3L(CCalAlarm* aAlarm)
	{
	TInt alarmNum = 3;
	if (NULL == aAlarm)
		{
		INFO_PRINTF2(KEntryAlarmNotFound, alarmNum);
		return EFalse;
		}
		
	// This should be an extended alarm, just using the AALARM for timing.
	// Make sure there is no sound name set
		if (aAlarm->AlarmSoundNameL() != _L(""))
		{
		INFO_PRINTF2(KEntryAlarmWrongSound, alarmNum);
		return EFalse;
		}
		
	// Now check the alarm action. The content of the alarm action in
	// this alarm is a jpeg, so we'll read the original jpeg file and compare
	// the buffers
	const CCalContent* alarmAction = aAlarm->AlarmAction();
	if (NULL == alarmAction)
		{
		INFO_PRINTF2(KEntryAlarmActionNotFound, alarmNum);
		return EFalse;
		}
				
	if (alarmAction->MimeType() != _L8("image/jpeg"))
		{
		INFO_PRINTF2(KEntryAlarmActionMimeTypeWrong, alarmNum);
		return EFalse;
		}

	if (alarmAction->Disposition() != CCalContent::EDispositionInline)
		{
		INFO_PRINTF2(KEntryAlarmActionDispositionWrong, alarmNum);
		return EFalse;
		}
		
	// Now we get the original jpeg data from KJPEGDataFile
	RFs   fs;
	fs.Connect();
	CleanupClosePushL(fs);
	RFile dataFile;
	CleanupClosePushL(dataFile);
	
	TInt errData = dataFile.Open(fs, GetFullFileName(KJPEGDataFile), EFileRead);
	if(errData != KErrNone)
		{
		_LIT(KReadJpgOpenErr, "Error Opening the jpg file, %d");
		INFO_PRINTF2(KReadJpgOpenErr, errData);
		CleanupStack::PopAndDestroy(2);
		SetTestStepResult(ETestSuiteError);
		return (TestStepResult());
		}
	TInt dataSize;
	errData = dataFile.Size(dataSize);
	if(errData != KErrNone)
		{
		_LIT(KReadJpgSizeErr, "Error Reading the size from the jpg file, %d");
		INFO_PRINTF2(KReadJpgSizeErr, errData);
		CleanupStack::PopAndDestroy(2);
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
		CleanupStack::PopAndDestroy(3);
		SetTestStepResult(ETestSuiteError);
		return (TestStepResult());
		}
	CleanupStack::Pop(dataBuf);
	CleanupStack::PopAndDestroy(&dataFile);
	CleanupStack::PushL(dataBuf);
	
	
	if (dataPtr != alarmAction->Content())
		{
		INFO_PRINTF2(KEntryAlarmActionContentWrong, alarmNum);
		CleanupStack::PopAndDestroy(2, &fs);
		return EFalse;
		}

	CleanupStack::PopAndDestroy(2, &fs);
	return ETrue;
	}
	
TBool CTestCalInterimApiRichAlarmFormatStep::CheckAlarm4L(CCalAlarm* aAlarm)
	{
	TInt alarmNum = 4;
	if (NULL == aAlarm)
		{
		INFO_PRINTF2(KEntryAlarmNotFound, alarmNum);
		return EFalse;
		}
		
	// This should be an extended alarm, and an AALARM for timing.
	// Make sure there is a sound name set
		if (aAlarm->AlarmSoundNameL() != _L("c:\\mysound2"))
		{
		INFO_PRINTF2(KEntryAlarmWrongSound, alarmNum);
		return EFalse;
		}
		
	const CCalContent* alarmAction = aAlarm->AlarmAction();
	if (NULL == alarmAction)
		{
		INFO_PRINTF2(KEntryAlarmActionNotFound, alarmNum);
		return EFalse;
		}
		
	_LIT8(KAlarmURL, "http://www.symbian2.com");
	if (alarmAction->Content() != KAlarmURL)
		{
		INFO_PRINTF2(KEntryAlarmActionContentWrong, alarmNum);
		return EFalse;
		}
		
	if (alarmAction->MimeType() != _L8("text/http"))
		{
		INFO_PRINTF2(KEntryAlarmActionMimeTypeWrong, alarmNum);
		return EFalse;
		}

	if (alarmAction->Disposition() != CCalContent::EDispositionUrl)
		{
		INFO_PRINTF2(KEntryAlarmActionDispositionWrong, alarmNum);
		return EFalse;
		}
	return ETrue;
	}


void CTestCalInterimApiRichAlarmFormatStep::OpenSessionFileL()
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


void CTestCalInterimApiRichAlarmFormatStep::CloseAndDeleteRPtrArrayEntries(TAny* aObject)	
	{
	RPointerArray<CCalEntry>* array=REINTERPRET_CAST(RPointerArray<CCalEntry>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}

void CTestCalInterimApiRichAlarmFormatStep::CloseAndDeleteRPtrArrayAlarms(TAny* aObject)	
	{
	RPointerArray<CCalAlarm>* array=REINTERPRET_CAST(RPointerArray<CCalAlarm>*,aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}


