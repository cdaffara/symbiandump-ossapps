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

#include "TestCalInterimApiSuiteStepBase.h"
#include <calsession.h>
#include <calentry.h>
#include <calinstance.h>
#include <calcontent.h>
#include <calcategory.h>
#include <calrrule.h>
#include <e32math.h>
#include <calattachment.h>
#include <bautils.h>

_LIT(KAllocTest,					"alloctest");
_LIT(KNotifyProgress,				"notifyprogress");
_LIT(KCalenderFile,					"calenderfile");
_LIT(KCreateCalenderFile,			"createcalenderfile");
_LIT(KDeleteCalenderFile,			"deletecalenderfile");
_LIT(KReCreateCalFile,              "recreatecalfile");

_LIT(KTimeModeFloating,				"EFloating");
_LIT(KFixedTimeZone,				"EFixedTimeZone");

_LIT(KOriginalDrive,				"z:");
_LIT(KCopyFileFromROMToRAM,			"copyfile");
_LIT(KInitialLocationOfFile,		"filelocation");

_LIT(KInfoPercentageCompleted,		"Percentage Completed %d");
_LIT(KInfoCreateCalenderFile,		"CreateCalender file %d");
_LIT(KInfoNotifyProgress,			"notifyprogress = %d");
_LIT(KInfoCalenderFile,				"calenderfile = %S");
_LIT(KInfoTimeElapsed, 				"Time required to convert the calendar file to 9.3 version file is %f");

_LIT(KErrPercentageCompleted,		"Error progress not required");
_LIT(KErrCompleted,					"Error Completed %d");
_LIT(KErrCalenderFileDataMissing,	"Error calenderfile data missing");
_LIT(KErrInCreateCalFileL,			"Error in CreateCalFileL %d");
_LIT(KErrInOpenL,					"Error in OpenL %d");
_LIT(KErrDeletingCalenderFile,		"Error %d deleting calender file");
_LIT(KErrInDefaultFileName, 		"Error FileNames are not same");
_LIT(KErrInDeleteCalFileL, 			"Error %d in DeleteCalFileL");
_LIT(KErrEntryNotStored,			"Error: Entry Not stored");
_LIT(KErrTimeElapsed,				"Error: Time taken to convert the calendar file to 9.3 version is more than %f seconds");
_LIT(KErrAgendaServerShutdownTimedOut, "Error: The agenda server shutdown request has timed out. Check for open sessions");
_LIT(KSetTimeLocal,			"settimelocal");

CTestCalInterimApiSuiteStepBase::CTestCalInterimApiSuiteStepBase()
:	CTestStep()
,	iActiveScheduler(NULL)
,	iFsSessionConnected(EFalse)
,	iHasCalenderFileName(EFalse)
,	iNotifyProgress(ETrue)
,	iSession(NULL)
,	iHeapCheck(EFalse)
	{
	}

CTestCalInterimApiSuiteStepBase::~CTestCalInterimApiSuiteStepBase()
	{
	if ( iSession != NULL )
		{
		delete iSession;
		iSession = NULL;
		WaitForAgendaServerShutdown();
		}

	if ( iActiveScheduler != NULL )
		{
		delete iActiveScheduler;
		CActiveScheduler::Install(NULL);
		iActiveScheduler = NULL;
		}

	if ( iFsSessionConnected )
		{
		iFsSession.Close();
		iFsSessionConnected = EFalse;
		}
		
	//check no client side memory leak has occured, if flag is enabled in derived class	
 	if( iHeapCheck )
 	    {	    
         __UHEAP_MARKEND;
 	    }
	}

void CTestCalInterimApiSuiteStepBase::Progress(TInt aPercentageCompleted)
	{
	if ( iNotifyProgress )
		{
		INFO_PRINTF2(KInfoPercentageCompleted, aPercentageCompleted);
		}
	else
		{
		ERR_PRINTF1(KErrPercentageCompleted);
		SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiSuiteStepBase::Completed(TInt aError)
	{
	if ( aError != KErrNone )
		{
		ERR_PRINTF2(KErrCompleted, aError);
		SetTestStepResult(EFail);
		SetTestStepError(aError);
		}
	CActiveScheduler::Stop();
	}

TBool CTestCalInterimApiSuiteStepBase::NotifyProgress()
	{
	return iNotifyProgress;
	}
	
TVerdict CTestCalInterimApiSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	TVerdict	ret = CTestStep::doTestStepPreambleL();

	GetBoolFromConfig(ConfigSection(), KNotifyProgress, iNotifyProgress);
	INFO_PRINTF2(KInfoNotifyProgress, iNotifyProgress);
	User::LeaveIfError(iFsSession.Connect());
	iFsSessionConnected=ETrue;

	GetPIMTestServerL();
	iActiveScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);

	INFO_PRINTF1(_L("BEFORE CCALSESSION::NEWL"));
	
	iSession = CCalSession::NewL();

	INFO_PRINTF1(_L("AFTER CCALSESSION::NEWL"));
	
	TBool	createCalenderFile = EFalse;
	GetBoolFromConfig(ConfigSection(), KCreateCalenderFile, createCalenderFile);
	INFO_PRINTF2(KInfoCreateCalenderFile, createCalenderFile);

	TPtrC	calenderFile;
	iHasCalenderFileName = GetStringFromConfig(ConfigSection(), KCalenderFile, calenderFile);
	if ( iHasCalenderFileName )
		{
		INFO_PRINTF2(KInfoCalenderFile, &calenderFile);
		CreateAndOpenCalenderFileL(calenderFile, createCalenderFile);
		DefaultFileNameL();
		}
	else
		{
		ERR_PRINTF1(KErrCalenderFileDataMissing);
		// Some test steps like system time change might not need
		// a calendar file. Hence we do not fail the step here, but just
		// log it.
		}

	TBool	copyFile = EFalse;
	GetBoolFromConfig(ConfigSection(), KCopyFileFromROMToRAM, copyFile);

	if ( copyFile )
		{
		CopyImportFileToWritableDriveL();
		}
	//If test chooses to set time before actual operation begins.
	//This is to nullify any delay caused by previous operations which might cause any time related tests to fail.
	TPtrC setTime;
	TBool result = GetStringFromConfig(ConfigSection(), KSetTimeLocal, setTime);
	if (result)
		{
		TTime time(setTime);
		User::SetHomeTime(time);
		}

	return ret;
	}

TVerdict CTestCalInterimApiSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	TBool	deleteCalenderFile = EFalse;
	GetBoolFromConfig(ConfigSection(), KDeleteCalenderFile, deleteCalenderFile);

	if ( (deleteCalenderFile) && (iHasCalenderFileName) )
		{
		DeleteCalenderFileL();
		}

	iPIMTestServer.Close();

	return CTestStep::doTestStepPostambleL();
	}

/**
@SYMTestCaseID 			PIM-APP-ENGINES-CALINTERIMAPI-CIT-0001-HP
*/
/**
@SYMTestCaseID 			PIM-APP-ENGINES-CALINTERIMAPI-CIT-0002-HP
*/
/**
@SYMTestCaseID 			PIM-APP-ENGINES-CALINTERIMAPI-CIT-0003-HP
*/
/**
@SYMTestCaseID 			PIM-APP-ENGINES-CALINTERIMAPI-SECTION_4
*/
/**
@SYMTestCaseID 			PIM-APP-ENGINES-CALINTERIMAPI-CIT-1098AP-13
@SYMTestType			CIT
@SYMTestPriority		Medium
@SYMPREQ				1098
@SYMFssID				3.2.1 002, 003, 006, 013
@SYMTestCaseDesc		Creates and opens an calendar file on C:\
@SYMTestActions			Conditionally create a new calendar  File, and then open it
@SYMTestExpectedResults	CCalSession will create and open a new calendar  file
*/
void CTestCalInterimApiSuiteStepBase::CreateAndOpenCalenderFileL(const TDesC& aFileName, TBool aCreateFile)
	{
	iCalenderFileName = aFileName;
	if ( (TestStepResult() == EPass) && (aCreateFile) )
		{
		TRAPD(err, iSession->CreateCalFileL(iCalenderFileName));
		switch ( err )
			{
		case KErrNone:
			break;
		case KErrAlreadyExists:
			// In concurrent execution tests, it might turn out that another
			// thread has already deleted, when we try to delete, or another
			// thread might try to create when we have already created. We
			// ignore these errors. Any other error is propogated
			TRAPD(deleteErr, iSession->DeleteCalFileL(iCalenderFileName));
			TRAPD(createErr, iSession->CreateCalFileL(iCalenderFileName));
			if(deleteErr != KErrNotFound && createErr != KErrAlreadyExists)
				{
				User::LeaveIfError(deleteErr);
				User::LeaveIfError(createErr);
				}
			break;
		default:
			ERR_PRINTF2(KErrInCreateCalFileL, err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}

	if ( TestStepResult() == EPass )
		{
		TRAPD(err, iSession->OpenL(iCalenderFileName));
		if( err != KErrNone )
			{
			ERR_PRINTF2(KErrInOpenL, err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}
	}

/**
@SYMTestCaseID			PIM-APP-ENGINES-CALINTERIMAPI-CIT-0005-HP
@SYMTestType			CIT
@SYMTestPriority		Medium
@SYMPREQ				1098
@SYMFssID				3.2.1 005
@SYMTestCaseDesc		delete the opened file
@SYMTestActions			deletes the opened file
@SYMTestExpectedResults	Aganda file is and deleted
*/
void CTestCalInterimApiSuiteStepBase::DeleteCalenderFileL()
	{
	INFO_PRINTF1(_L("Deleting calender file"));

	TRAPD(err, iSession->DeleteCalFileL(iCalenderFileName));
	if( err == KErrNone )
		{
		TBool reCreateCalFile = ETrue;
	    if (!GetBoolFromConfig(ConfigSection(), KReCreateCalFile, reCreateCalFile))
	        {
	    	reCreateCalFile = ETrue;
	        }

	    if (reCreateCalFile)
	        {
	    	INFO_PRINTF1(_L("Trying to create the file after deletion"));
		    TRAPD(err, iSession->CreateCalFileL(iCalenderFileName));
		    if( err != KErrNone )
			    {
			    ERR_PRINTF2(KErrInDeleteCalFileL, err);
			    SetTestStepResult(EFail);
			    }
	        }
		}
	else
		{
		ERR_PRINTF2(KErrDeletingCalenderFile, err);
		SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiSuiteStepBase::DefaultFileNameL()
	{
	INFO_PRINTF1(_L("Retrieving the Default File"));

	const TPtrC fileName = iSession->DefaultFileNameL();
	INFO_PRINTF2(_L("The Default file name is %S"), &fileName);

	if( fileName.Compare(_L("C:Calendar")) == 0)
		{
		INFO_PRINTF1(_L("The default file name and the retrieved file matches"));
		}
	else
		{
		ERR_PRINTF1(KErrInDefaultFileName);
		SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiSuiteStepBase::OpenSessionL()
	{
	iSession = CCalSession::NewL();
	}

void CTestCalInterimApiSuiteStepBase::CloseSessionL()
	{
	delete 	iSession;
	iSession = NULL;
	WaitForAgendaServerShutdown();
	}
/**
 * Prints the current local time
 */
void CTestCalInterimApiSuiteStepBase::PrintHomeTimeL()
	{
	TTime now;
	now.HomeTime();
	PrintTimeL(now);
	}

/**
 * Prints the current local time
 @param aTime Time to be printed in the specified format
 */
void CTestCalInterimApiSuiteStepBase::PrintTimeL(const TTime& aTime)
	{
	_LIT(KTimeFmt, 				"%F%Y%M%D-%H:%T:%S");
	_LIT(KTimeLocal, 			"Local time : %s");

	TBuf<32> buf;
	aTime.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeLocal, buf.PtrZ());
	}

/**
 * Deletes objects of an entry array. Can be used as TCleanupOperation
 */
void CTestCalInterimApiSuiteStepBase::ResetAndDestroyEntryArray(TAny* aObject)
	{
	RPointerArray<CCalEntry>* array = static_cast<RPointerArray<CCalEntry>*>(aObject);
	array->ResetAndDestroy();
	}

/**
 * Deletes objects of an instance array. Can be used as TCleanupOperation
 */
void CTestCalInterimApiSuiteStepBase::ResetAndDestroyInstanceArray(TAny* aObject)
	{
	RPointerArray<CCalInstance>* array = static_cast<RPointerArray<CCalInstance>*>(aObject);
	array->ResetAndDestroy();
	}

/**
 * Deletes objects of a category array. Can be used as TCleanupOperation
 */
void CTestCalInterimApiSuiteStepBase::ResetAndDestroyCategoryArray(TAny* aObject)
	{
	RPointerArray<CCalCategory>* array = static_cast<RPointerArray<CCalCategory>*>(aObject);
	array->ResetAndDestroy();
	}

/**
 * Deletes objects of a attendee array. Can be used as TCleanupOperation
 */
void CTestCalInterimApiSuiteStepBase::ResetAndDestroyAttendeeArray(TAny* aObject)
	{
	RPointerArray<CCalAttendee>* array = static_cast<RPointerArray<CCalAttendee>*>(aObject);
	array->ResetAndDestroy();
	}

/**
  * Converts the TTime to TCalTime based on the time mode given
  @param aTimeData TTime variable to be set in the specified time mode
  @param aCalTimeData time that is set in the specified time mode
  @param aTimeMode time mode of the entry
  */
void CTestCalInterimApiSuiteStepBase::SetCalTimeL(const TTime& aTimeData, TCalTime& aCalTimeData, TCalTime::TTimeMode aTimeMode)
	{
	if(aTimeMode == TCalTime::EFloating)
		{
		aCalTimeData.SetTimeLocalFloatingL(aTimeData);
		}
	else if(aTimeMode == TCalTime::EFixedTimeZone)
		{
		aCalTimeData.SetTimeLocalL(aTimeData);
		}
	else
		{
		aCalTimeData.SetTimeUtcL(aTimeData);
		}
	}

/**
  * Sets the time mode based on the input given
  @param aTimeMode time mode of the entry
  */
TCalTime::TTimeMode CTestCalInterimApiSuiteStepBase::SetTimeMode(const TPtrC& aTimeMode)
	{
	TCalTime::TTimeMode	timeMode = TCalTime::EFixedUtc;
	if (aTimeMode.Compare(KTimeModeFloating) == 0)
		{
		timeMode = TCalTime::EFloating;
		}
	else if (aTimeMode.Compare(KFixedTimeZone) == 0)
		{
		timeMode = TCalTime::EFixedTimeZone;
		}
	else
		{
		timeMode = TCalTime::EFixedUtc;
		}
	return	timeMode;
	}

/**
  * Parses a comma separated string and constructs a list out of the values
  */
void CTestCalInterimApiSuiteStepBase::TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator)
	{
	TLex lexer(aString);

	while(!lexer.Eos())
		{
		lexer.SkipSpaceAndMark();

		while(!lexer.Eos() && lexer.Get() != aSeparator)
			{
			}

		if(!lexer.Eos())
			{
			lexer.UnGet(); // Do not include trailing ','
			}
		aList.AppendL(lexer.MarkedToken());
		lexer.Inc();
		}
	}

/**
Gets binary data from the file whose name is passed as parameter
@param aFileName alarm attachment file to be opened
@param aDataBuf value of the file attachment
*/
void CTestCalInterimApiSuiteStepBase::GetDataFromFileL(const TFileName& aFileName, HBufC8*& aDataBuf)
	{
	RFs fs;
	RFile file;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(fs, aFileName, EFileRead | EFileShareReadersOnly));
	TInt size;
	User::LeaveIfError(file.Size(size));
	aDataBuf = HBufC8::NewLC(size);
	TPtr8 pointer(aDataBuf->Des());
	User::LeaveIfError(file.Read(pointer, size));
	CleanupStack::Pop(aDataBuf);
	CleanupStack::PopAndDestroy(2, &fs); // file, fs
	}

/**
Converts a string to corresponding TDisposition
@param aDisposition disposition of the alarm attachment
@param aData data that contains the disposition type
*/
TInt CTestCalInterimApiSuiteStepBase::ConvertToDisposition(CCalContent::TDisposition& aDisposition, const TDesC& aData)
	{
	_LIT(KDispositionUrl, 				"EDispositionUrl");
	_LIT(KDispositionInline, 			"EDispositionInline");
	_LIT(KDispositionUnknown,			"EDispositionUnknown");

	TInt ret = KErrNone;
	if(aData == KDispositionUrl)
		{
		aDisposition = CCalContent::EDispositionUrl;
		}
	else if(aData == KDispositionInline)
		{
		aDisposition = CCalContent::EDispositionInline;
		}
	else if(aData == KDispositionUnknown)
		{
		aDisposition = CCalContent::EDispositionUnknown;
		}
	else
		{
		ret = KErrNotFound;
		}
	return ret;
	}

/**
Creates a CCalContent object with the help of the data passed
If the disposition is Inline, this method assumes that a filename has been passed
and reads in the attachment content from this file, so as to store it inline
with the CCalContent
@param aContent content of the alarm attachment
@param aMimeType MimeType of the alarm attachment
@param aDisposition disposition type of the alarm attachment
*/
CCalContent* CTestCalInterimApiSuiteStepBase::CreateCalContentL(HBufC8* aContent, HBufC8* aMimeType, CCalContent::TDisposition aDisposition)
	{
	CCalContent* calContent = CCalContent::NewL();
	CleanupStack::PushL(calContent);

	RFs	fs;
	User::LeaveIfError(fs.Connect());
	TFileName fileName;
	fileName.Copy(aContent->Des());
	// Check if the file exists in the system
	TInt	ret = BaflUtils::FileExists(fs, fileName);
	// If file exists, then read the binary data from the file and pass it.
	if ( ret )
		{
		if(aDisposition == CCalContent::EDispositionInline)
			{
			// Read the binary data, from the file passed from test-data
			delete aContent;
			aContent = NULL;
			GetDataFromFileL(fileName, aContent);
			calContent->SetContentL(aContent, aMimeType, aDisposition);
			}
		}
	else
		{
		calContent->SetContentL(aContent, aMimeType, aDisposition);
		}

	CleanupStack::Pop(calContent);
	return calContent;
	}


/**
Compares the properties of two alarms
@param aAlarm1 alarm of the stored entry
@param aAlarm1 alarm of the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareAlarmsL(CCalAlarm* aAlarm1, CCalAlarm* aAlarm2)
	{
	if(aAlarm1)
		{
		CCalContent* content1 = aAlarm1->AlarmAction();
		CCalContent* content2 = aAlarm2->AlarmAction();

		TEST1(aAlarm1->TimeOffset() == aAlarm2->TimeOffset(), ETrue);
		TEST1(aAlarm1->AlarmSoundNameL() == aAlarm2->AlarmSoundNameL(), ETrue);

		if(content1)
			{
			TEST1(content1->Content() == content2->Content(), ETrue);
			TEST1(content1->Disposition() == content2->Disposition(), ETrue);
			TEST1(content1->MimeType() == content2->MimeType(), ETrue);
			}
		else
			{// content1 is NULL. Test that content2 is NULL too
			TEST1(!content2, ETrue);
			}
		}
	else
		{// alarm1 is NULL. Test that alarm2 is NULL too
		TEST1(!aAlarm2, ETrue);
		}
	}

/**
Format of buffer to construct a TTime is YYYYMMDD:HHMMSS (15 characters).
@param aParamString String that is to be converted to TTime format
*/
TTime CTestCalInterimApiSuiteStepBase::FormatDateTime(const TDesC& aParamString)
	{
	// Format of buffer to construct a TTime is YYYYMMDD:HHMMSS (15 characters).
	// TTime uses zero-based values for month and day - which is confusing for scripting.
	// In our script, we use actual month and day numbers to make things simpler so we
	// must modify the string here to take account of this.
	TBuf<32> buf;
	TInt m, d;

	buf.Zero();
	buf.Copy(aParamString.Left(4));		// The year
	TLex lexMonth = aParamString.Mid(4, 2);
	lexMonth.Val(m);
	TLex lexDay = aParamString.Mid(6, 2);
	lexDay.Val(d);
	buf.AppendFormat(_L("%02d%02d"), m - 1, d - 1);	// The month and day
	buf.Append(aParamString.Right(7));

	return TTime(buf);
	}

/**
Converts a descriptor containing TRUE or FALSE to its respective TBool
@param aString String which contains true or false.
*/
TBool CTestCalInterimApiSuiteStepBase::ConvertStringToBool(const TDesC& aString)
	{
	_LIT(KTrue, "TRUE");

	if(aString.CompareF(KTrue) == KErrNone)
		{
		return ETrue;
		}
	return EFalse;
	}


/**
Converts a string to TReal32
@return	The TReal32 equivalent
@param	aString Descriptor containing the string to be converted
*/
TReal32 CTestCalInterimApiSuiteStepBase::GetTReal32(const TPtrC& aString)
	{
	TReal32 real32;
	TLex lex(aString);
	lex.Val(real32);
	return real32;
	}


/**
Computes time difference between first two parameters. Compares the time
difference with the value of third parameter.
@return		EFalse if the time difference is greater than the value of aMaxTimeLimit(third parameter)
@param		aStartTime  	The start time of the operation
@param		aEndTime 		The end time of the operation
@param		aTimeElapsed	Time taken for the operation
*/
TBool CTestCalInterimApiSuiteStepBase::ComputeTimeDifference(const TTime& aStartTime, const TTime& aEndTime, const TReal32& aMaxTimeLimit)
	{
	TBool returnCode = ETrue;
	TTimeIntervalMicroSeconds timeElapsed = aEndTime.MicroSecondsFrom(aStartTime);

	TReal32 aTimeElapsed = (static_cast<TReal32>(timeElapsed.Int64())) / (1000000);

	INFO_PRINTF2(KInfoTimeElapsed, aTimeElapsed);

	if ((aTimeElapsed >= aMaxTimeLimit))
		{
		ERR_PRINTF2(KErrTimeElapsed, aMaxTimeLimit);
		returnCode = EFalse;
		}
	return returnCode;
	}

/**
Store the entry in the calendar file
@param aCalEntryView Pointer to the CCalEntryView
@param aCalEntry Pointer to CCalEntry
*/
void CTestCalInterimApiSuiteStepBase::StoreCalendarEntryL(CCalEntryView* aCalEntryView, CCalEntry* aCalEntry)
	{
	RPointerArray<CCalEntry>	entriesStored;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesStored));
	entriesStored.AppendL(aCalEntry);
	TInt	numOfEntries(0);
	aCalEntryView->StoreL(entriesStored, numOfEntries);

	if( entriesStored.Count() != numOfEntries )
		{
		ERR_PRINTF1(KErrEntryNotStored);
		SetTestStepResult(EFail);
		}
	CleanupStack::Pop(&entriesStored);
	entriesStored.Close();
	}

/**
Gets the matching entry by checking the recurrence ids
@param aEntries Array containing the entries
@param aRecurrenceId recurrence id of the entry
*/
CCalEntry* CTestCalInterimApiSuiteStepBase::GetMatchingEntryL(RPointerArray<CCalEntry>& aEntries, TCalTime aRecurrenceId)
	{
	for(TInt i = 0; i < aEntries.Count(); ++i)
		{
		if(aRecurrenceId.TimeUtcL() == aEntries[i]->RecurrenceIdL().TimeUtcL())
			{
			return aEntries[i];
			}
		}
	return NULL;
	}

/**
Compare the properties of two entries
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareEntriesL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	CompareNonRecurrencePropertiesL(aEntry1, aEntry2);
	CompareAlarmsL(aEntry1->AlarmL(), aEntry2->AlarmL());
	CompareRepeatRulePropertiesL(aEntry1, aEntry2);
	CompareOrganizerPropertiesL(aEntry1, aEntry2);
	CompareAttachmentPropertiesL(aEntry1, aEntry2);
	if ( aEntry1->GeoValueL() )
		{
		CompareGeoL(aEntry1, aEntry2);
		}		
	}

/* Compare the Non recurrence properties of the entries
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareNonRecurrencePropertiesL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	TEST1( aEntry1->StartTimeL().TimeUtcL() == aEntry2->StartTimeL().TimeUtcL(), ETrue );
	TEST1( aEntry1->EndTimeL().TimeUtcL() == aEntry2->EndTimeL().TimeUtcL(), ETrue );
	TEST1( aEntry1->DTStampL().TimeUtcL() == aEntry2->DTStampL().TimeUtcL(), ETrue );
	TEST1( aEntry1->CompletedTimeL().TimeUtcL() == aEntry2->CompletedTimeL().TimeUtcL(), ETrue );
	TEST1( aEntry1->SummaryL() == aEntry2->SummaryL(), ETrue );
	TEST1( aEntry1->DescriptionL() == aEntry2->DescriptionL(), ETrue );
	TEST1( aEntry1->PriorityL() == aEntry2->PriorityL(), ETrue );
	TEST1( aEntry1->LocationL() == aEntry2->LocationL(), ETrue );
	TEST1( aEntry1->StatusL() == aEntry2->StatusL(), ETrue );
	TEST1( aEntry1->ReplicationStatusL() == aEntry2->ReplicationStatusL(), ETrue );
	TEST1( aEntry1->MethodL() == aEntry2->MethodL(), ETrue );
	TEST1( aEntry1->SequenceNumberL() == aEntry2->SequenceNumberL(), ETrue );
	
	CompareTransL(aEntry1, aEntry2);
	
	CompareRDatesL(aEntry1, aEntry2);
	CompareExDatesL(aEntry1, aEntry2);
	}

/* Compare the repeat rule properties of the entries before and after storing
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareRepeatRulePropertiesL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	TCalRRule	rptDef1;
	TBool	rpt1 = aEntry1->GetRRuleL(rptDef1);
	TCalRRule	rptDef2;
	TBool	rpt2 = aEntry2->GetRRuleL(rptDef2);
	if ( rpt1 )
		{
		TEST1( rptDef1.Type() == rptDef2.Type(), ETrue );
		TEST1( rptDef1.DtStart().TimeUtcL() == rptDef2.DtStart().TimeUtcL(), ETrue );
		TEST1( rptDef1.Until().TimeUtcL() == rptDef2.Until().TimeUtcL(), ETrue );
		TEST1( rptDef1.Count() == rptDef2.Count(), ETrue );
		TEST1( rptDef1.Interval() == rptDef2.Interval(), ETrue );
		RArray<TDay>	days1;
		CleanupClosePushL(days1);
		rptDef1.GetByDayL(days1);
		RArray<TDay>	days2;
		CleanupClosePushL(days2);
		rptDef2.GetByDayL(days2);
		TEST1( days1.Count() == days2.Count(), ETrue );
		for ( TInt i = 0; i < days1.Count(); i++ )
			{
			TESTL( days1[i] == days2[i] );
			}
		CleanupStack::PopAndDestroy(2); //days2, days1
		RArray<TInt>	monthDays1;
		CleanupClosePushL(monthDays1);
		rptDef1.GetByMonthDayL(monthDays1);
		RArray<TInt>	monthDays2;
		CleanupClosePushL(monthDays2);
		rptDef2.GetByMonthDayL(monthDays2);
		TEST1( monthDays1.Count() == monthDays2.Count(), ETrue );
		for ( TInt j = 0; j < monthDays1.Count(); j++ )
			{
			TESTL( monthDays1[j] == monthDays1[j] );
			}
		CleanupStack::PopAndDestroy(2); // monthDays2, monthDays1
		RArray<TMonth>	months1;
		CleanupClosePushL(months1);
		rptDef1.GetByMonthL(months1);
		RArray<TMonth>	months2;
		CleanupClosePushL(months2);
		rptDef2.GetByMonthL(months2);
		TEST1( months1.Count() == months2.Count(), ETrue );
		TEST1( rptDef1.WkSt() == rptDef2.WkSt(), ETrue );
		for ( TInt k = 0; k < months1.Count(); k++ )
			{
			TESTL( months1[k] == months2[k] );
			}
		CleanupStack::PopAndDestroy(2); // months2, months1
		}
	else
		{ // rptDef1 is NULL. Test that rptDef2 is NULL too
		TEST1( !rpt2, ETrue );
		}
	}

/* Compare the organizer properties of the entries before and after storing
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareOrganizerPropertiesL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	CCalUser*	organizer1 = aEntry1->OrganizerL();
	CCalUser*	organizer2 = aEntry2->OrganizerL();
	if ( organizer1 )
		{
		TEST1( organizer1->Address() == organizer2->Address(), ETrue );
		TEST1( organizer1->CommonName() == organizer2->CommonName(), ETrue );
		TEST1( organizer1->SentBy() == organizer2->SentBy(), ETrue );
		}
	else
		{ // organizer1 is NULL. Test that organizer2 is NULL too
		TEST1( !organizer2, ETrue );
		}
	}

/* Compare the rDates of the entries before and after storing
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareRDatesL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	RArray<TCalTime>	rDateList1;
	CleanupClosePushL(rDateList1);
	aEntry1->GetRDatesL(rDateList1);
	RArray<TCalTime>	rDateList2;
	CleanupClosePushL(rDateList2);
	aEntry2->GetRDatesL(rDateList2);
	TESTL( rDateList1.Count() == rDateList2.Count() );
	for ( TInt i = 0; i < rDateList1.Count(); i++ )
		{
		TESTL( rDateList1[i].TimeUtcL() == rDateList2[i].TimeUtcL() );
		}
	CleanupStack::PopAndDestroy(2); // rDateList2, rDateList1
	}

/* Compare the ExDates of the entries before and after storing
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareExDatesL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	RArray<TCalTime>	exDateList1;
	CleanupClosePushL(exDateList1);
	aEntry1->GetRDatesL(exDateList1);
	RArray<TCalTime>	exDateList2;
	CleanupClosePushL(exDateList2);
	aEntry2->GetRDatesL(exDateList2);
	TESTL( exDateList1.Count() == exDateList2.Count() );
	for ( TInt i = 0; i < exDateList1.Count(); i++ )
		{
		TESTL( exDateList1[i].TimeUtcL() == exDateList2[i].TimeUtcL() );
		}
	CleanupStack::PopAndDestroy(2); // exDateList2, exDateList1
	}

/**
Compares the properties of two geo values
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareGeoL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	TBool allocTest = EFalse;
	GetBoolFromConfig(ConfigSection(), KAllocTest, allocTest);
	TInt err = KErrNone;
	TInt errCode = KErrNone;

	CCalGeoValue* geoValue = NULL;

	if (allocTest)
		{
		err = OOMGeoValueL(aEntry1, geoValue);
		}
	else
		{
		TRAP(err, geoValue = aEntry1->GeoValueL());	
		}

	CCalGeoValue* storedGeoValue = NULL;

	if (allocTest)
		{
		errCode = OOMGeoValueL(aEntry2, storedGeoValue);
		}
	else
		{
		TRAP(errCode, storedGeoValue = aEntry2->GeoValueL());
		}

	if (geoValue == NULL)
		{
		TESTL(storedGeoValue == NULL);
		INFO_PRINTF1(_L("GEO Property not set within entry"));
		}
	else
		{
		CleanupStack::PushL(geoValue);
		TESTL(storedGeoValue != NULL);
		CleanupStack::PushL(storedGeoValue);
		
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("GeoValuesL Error Code %d"), err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}

		TReal lat = 0.0;
		TReal longitude = 0.0;
		TBool hasGEO = geoValue->GetLatLong(lat,longitude);

		TReal latTruncated = 0.0;
		Math::Round(latTruncated, lat, KMaxGEODecimalPoint);
		if (latTruncated != lat)
			{
			lat = latTruncated;
			}

		TReal longTruncated = 0.0;
		Math::Round(longTruncated, longitude, KMaxGEODecimalPoint);

		if (longTruncated != longitude)
			{
			longitude = longTruncated;
			}
		
		if (errCode != KErrNone)
			{
			ERR_PRINTF2(_L("GeoValuesL Error Code %d"), err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}

		TReal storedLat = 0.0;
		TReal storedLong = 0.0;

		TBool storedHasGEO = storedGeoValue->GetLatLong(storedLat, storedLong);

		if ( KMinGEOLat > lat || lat > KMaxGEOLat || KMinGEOLong > longitude || longitude > KMaxGEOLong )
			{
			TESTL(!hasGEO);
			TESTL(!storedHasGEO);
			TESTL(KDefaultGEOLatLong == storedLat);
			TESTL(KDefaultGEOLatLong == storedLong);
			INFO_PRINTF1(_L("GEO Property is set correctly to default value"));
			}
		else
			{
			TESTL(hasGEO);
			TESTL(storedHasGEO);
			TESTL(lat == storedLat);
			TESTL(longitude == storedLong);
			INFO_PRINTF1(_L("GEO Property is set correctly"));
			}
		
		CleanupStack::Pop(storedGeoValue);	
		CleanupStack::Pop(geoValue);	
		}
	}

/* Compare the transparency status of the entries before and after storing 
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
@test
*/
void CTestCalInterimApiSuiteStepBase::CompareTransL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	TBool allocTest = EFalse;
	GetBoolFromConfig(ConfigSection(), KAllocTest, allocTest);
	
	if (allocTest)
		{
		TInt trans1 = 0;
		TInt trans2 = 0;
		TInt tryCount = 1;
 		TInt err = KErrNone;
 		TInt err1 = KErrNone;
 		for ( ;; )
 			{
 			__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
 			TRAP(err, trans1 = aEntry1->TimeTransparencyL());
 			TRAP(err1, trans2 = aEntry2->TimeTransparencyL());
 			if ( err==KErrNone && err1==KErrNone)
 				{
 				__UHEAP_RESET;
 				INFO_PRINTF1(_L("OOM testing of CCalEntry::TimeTransparencyL Api is done"));
 				break;
 				}
 				
 			if ( err != KErrNoMemory || err1 != KErrNoMemory)
 				{
 				__UHEAP_RESET;
 				SetTestStepResult(EFail);
 				break;
 				}
 			tryCount++;
 			__UHEAP_RESET;
 			}
 		TESTL(trans1 == trans2);
		}
	else
		{
		TInt trans(aEntry1->TimeTransparencyL());
		if (trans<0)
			{
  			TESTL(KDefaultTranspStatus == aEntry2->TimeTransparencyL());
  			INFO_PRINTF1(_L("Busy Status is set correctly to default value"));
  			}
  		else
  			{
 			TESTL(trans == (aEntry2->TimeTransparencyL()));
  			INFO_PRINTF1(_L("Busy Status is set correctly"));
  			}
		}
	}

/* Retrives the geo value within the entry in out of memory conditions
@param aEntry1 Pointer to the stored entry
@param aGeoValue 
@test
*/
TInt CTestCalInterimApiSuiteStepBase::OOMGeoValueL(CCalEntry* aEntry, CCalGeoValue*& aGeoValue)
	{
	TInt tryCount = 1;
 	TInt err = KErrNone;
 	for ( ;; )
 		{
 		__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
 		TRAP(err, aGeoValue = aEntry->GeoValueL());

 		if ( err==KErrNone )
 			{
 			__UHEAP_RESET;
 			INFO_PRINTF1(_L("OOM testing of CCalEntry::GeoValueL Api is done"));
 			break;
 			}
 				
 		if ( err != KErrNoMemory )
 			{
 			__UHEAP_RESET;
 			SetTestStepResult(EFail);
 			break;
 			}
 		tryCount++;
 		__UHEAP_RESET;
 		}
 	return err;
	}

/* Compare the attachments properties of the entries before and after store
@param aEntry1 Pointer to the stored entry
@param aEntry2 Pointer to the entry fetched from the database
*/
void CTestCalInterimApiSuiteStepBase::CompareAttachmentPropertiesL(CCalEntry* aEntry1, CCalEntry* aEntry2)
	{
	TESTL( aEntry1->AttachmentCountL() == aEntry2->AttachmentCountL() );
	// Iterate through all the attachments and compare the properties of the attachment
	for ( TInt i = 0; i < aEntry1->AttachmentCountL(); i++ )
		{
		CCalAttachment*	attachment1 = aEntry1->AttachmentL(i);
		CCalAttachment* attachment2 = aEntry2->AttachmentL(i);
		// check if both the attachment exists
		if ( attachment1 != NULL  && attachment2 != NULL )
			{
			// Proceed only if the attachment name matches
			if ( attachment1->Label() == attachment2->Label() )
				{
				TESTL( attachment1->Type() == attachment2->Type() );
				TESTL( attachment1->MimeType() == attachment2->MimeType() );
				/* If both the attachments has file attachments, then compare the properties of 
				 file attachment like size and last modified date */
				if( attachment1->FileAttachment() && attachment2->FileAttachment() )
					{
					TESTL( attachment1->FileAttachment()->Size() == attachment2->FileAttachment()->Size() );
					TESTL( attachment1->FileAttachment()->LastModifiedTimeUtc() == attachment2->FileAttachment()->LastModifiedTimeUtc() );
					}
				}		
			}
		}
	}


/**
  * Copy the exported file attachments from ROM to RAM drive
  */
void CTestCalInterimApiSuiteStepBase::CopyImportFileToWritableDriveL()
	{
	CFileMan* fileCopier = CFileMan::NewL(iFsSession);
	CleanupStack::PushL(fileCopier);

	TPtrC	file;
	GetStringFromConfig(ConfigSection(), KInitialLocationOfFile, file);

	RArray<TPtrC>	fileList;
	CleanupClosePushL(fileList);
	TokenizeStringL(file, fileList);

	// Iterate through the file list and copy them from z drive to c drive
	for ( TInt i = 0; i < fileList.Count(); i++ )
		{
		TFileName initialLocationOfFile = fileList[i];
		initialLocationOfFile.Insert(0, KOriginalDrive());
		// get current time
		TTime now;
		now.HomeTime();
		// clear any read only attribute if the file is present such we avoid errors at copy time
		fileCopier->Attribs(fileList[i], 0, KEntryAttReadOnly, now);
		User::LeaveIfError(fileCopier->Copy(initialLocationOfFile, fileList[i], CFileMan::EOverWrite|CFileMan::ERecurse));
		// clear any read only attribute such we avoid errors at open time (true on real hardware)
		User::LeaveIfError(fileCopier->Attribs(fileList[i], 0, KEntryAttReadOnly, now));
		}

	CleanupStack::PopAndDestroy(&fileList);
	CleanupStack::PopAndDestroy(fileCopier);
	}

/*
*Computes time difference between first two parameters and prints them in the log file.
*@param aStartTime 	The start time of the operation
*@param	aEndTime	The end time of the operation
*@param aMaxTimeLimit Time taken for the entries without attachment in v9.2
*@return timeTakenForOperation
*/
TReal32 CTestCalInterimApiSuiteStepBase::CalculateTimeDifference(const TTime& aStartTime, const TTime& aEndTime, const TReal32& aMaxTimeLimit)
	{
	TTimeIntervalMicroSeconds timeElapsed = aEndTime.MicroSecondsFrom(aStartTime);

	TReal32 timeTakenForOperation = (static_cast<TReal32>(timeElapsed.Int64())) / (1000000);

	if ((timeTakenForOperation > aMaxTimeLimit))
		{
		INFO_PRINTF1(_L("The time taken for entries with attachment is more"));
		INFO_PRINTF2(_L("Time Taken for entries without attachment in v9.2 is --> %fseconds"), aMaxTimeLimit);
		INFO_PRINTF2(_L("Time Taken for entries with attachment is --> %fseconds"), timeTakenForOperation);
		}

	return timeTakenForOperation;
	}

/**
* Requests and waits for the agenda server to shutdown with a time out. 
* If the request times out, a failure is flagged indicating that the server cannot be shutdown e.g. open sessions still exist
*/

void CTestCalInterimApiSuiteStepBase::WaitForAgendaServerShutdown()
	{
	// the name of the agenda server process includes its uid like this [10003a5b]
	_LIT(KAgendaServerUIDMatch, "*[10003a5b]*");
	TFindProcess findProcess(KAgendaServerUIDMatch);
	
	TFullName fullName;
	
	TInt findProcessResult(KErrNone);
	findProcessResult = findProcess.Next(fullName);
	
	if (findProcessResult == KErrNone)
		{
		// find the latest agenda server process
		while (findProcessResult == KErrNone)
			{
			findProcessResult = findProcess.Next(fullName);
			}
			
		// The agenda server process is running so wait 
		RProcess process;
		if (process.Open(fullName) == KErrNone)
			{
			TRequestStatus processStatus;
			process.Logon(processStatus); // ask for a callback when the process ends
			
			// Don't wait for the server to close for longer than 7 seconds
			RTimer timeOutTimer;
			timeOutTimer.CreateLocal();
			TRequestStatus timeOutStatus;
			timeOutTimer.After(timeOutStatus, 7000000);
			
			// Wait for either the agenda server to close
			// or the time out timer to time out.
			User::WaitForRequest(processStatus, timeOutStatus);
			
			if (timeOutStatus.Int() == KRequestPending)
				{
				timeOutTimer.Cancel();
				User::WaitForRequest(timeOutStatus);
				}
			else
				{
				if (timeOutStatus.Int() == KErrNone)
					{
					// Agenda server shutdown request has timed out
					ERR_PRINTF1(KErrAgendaServerShutdownTimedOut);
					SetTestStepResult(EFail);
					}
				}

			timeOutTimer.Close();		
			process.LogonCancel(processStatus);	
			process.Close();
			}
		}
	}
