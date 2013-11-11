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

#include "TestCalInterimApiReadData.h"
#include <calsession.h>
#include <calcategory.h>
#include <calentryview.h>
#include <calalarm.h>
#include <caluser.h>
#include <calrrule.h>
#include <e32math.h>

#define	REPEATOVERYEARS	5
#define	INTERVALYEAR	365

_LIT(KDataFile,					"datafile");
_LIT(KColon,					":");
_LIT(KComment,					"//");
_LIT(KAllocTest,				"alloctest");

_LIT(KInfoAllocTest,			"Alloc Testing");
_LIT(KInfoAddingRecordToStore,	"Adding Record to Store");
_LIT(KInfoRecordAdded,			"Record Added");
_LIT(KInfoFetchingEntries, 		"Fetching entries");
_LIT(KInfoEntryFetched,			"Entry Fetched");
_LIT(KInfoDatafile,				"datafile=%S");
_LIT(KInfoCommandData,			"command:%S data:%S");
_LIT(KInfoCommand,				"command:%S");
_LIT(KInfoChildEntries,			"No. of Child Entries=%d");
_LIT(KInfoParentEntry,			"No. of Parent Entry=%d");

_LIT(KErrDatafileDataMissing,	"datafile data missing");
_LIT(KErrCommandNotProcessed,	"Error command not processed");
_LIT(KErrDataRead,				"Data Read finally returned: %d");
_LIT(KErrInEntryStore,			"Entry is not stored succesfuly. Error occured: %D");

_LIT(KWarnInvalidData,			"Warning: Invalid Data passed");

CTestCalInterimApiReadData::CTestCalInterimApiReadData()
:	CTestCalInterimApiSuiteStepBase()
,	iDataFileOpened(EFalse)
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalInterimApiReadData);
	}

CTestCalInterimApiReadData::~CTestCalInterimApiReadData()
/**
 * Destructor
 */
	{
	iParsers.Close();
	if ( iDataFileOpened )
		{
		iDataFile.Close();
		iDataFileOpened=EFalse;
		}
	delete iEntryView;
	delete iCurrentParser;
	}

void CTestCalInterimApiReadData::ParserCreatedL(CTestCalInterimApiParserBase* aNewParser)
	{
	iParsers.Append(aNewParser);
	iCurrentParser=aNewParser;
	}

void CTestCalInterimApiReadData::ProcessEntryL(CCalEntry* aEntry)
	{
	TInt						entriesStored(0);
	RPointerArray<CCalEntry>	entriesToStore;
	CleanupClosePushL(entriesToStore);
	entriesToStore.AppendL(aEntry);
	INFO_PRINTF1(KInfoAddingRecordToStore);
	TRAPD(err, iEntryView->StoreL(entriesToStore, entriesStored));
	if( err==KErrNone )
		{
		INFO_PRINTF1(KInfoRecordAdded);
		const TPtrC8 uid(aEntry->UidL());
		FetchEntryL(uid, aEntry);
		}
	else if ( err==KErrNotFound || err==KErrNotSupported 
			|| err==KErrArgument || err==KErrNotReady )
		{
		WARN_PRINTF1(KWarnInvalidData);
		}
	else
		{
		ERR_PRINTF2(KErrInEntryStore, err);
		SetTestStepResult(EFail);
		}
	// This only closes the array, but does not reset and destroy.
	// We must not, as ownership of aEntry lies with the function who called us
	CleanupStack::PopAndDestroy(&entriesToStore);
	delete aEntry;
	}

void CTestCalInterimApiReadData::FetchEntryL(const TPtrC8& aUid, CCalEntry* aEntry )
	{
	RPointerArray<CCalEntry>	entriesToFetch;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesToFetch));
	INFO_PRINTF1(KInfoFetchingEntries);
	iEntryView->FetchL(aUid, entriesToFetch);
	INFO_PRINTF1(KInfoEntryFetched);

	CheckTheNumOfParentChildEntryL(entriesToFetch);

	CCalEntry* calEntry = GetNewlyAddedEntryL(aEntry, entriesToFetch);

	TESTL(calEntry != NULL);
	INFO_PRINTF1(_L("Compare the entry fetched from the db and the entry after setting all the attributes"));
	// Compare the attributes of the entry before and after store
	CompareEntriesL(aEntry, calEntry);
	CleanupStack::PopAndDestroy(&entriesToFetch);
	}

TVerdict CTestCalInterimApiReadData::doTestStepL()
	{
	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	TBool allocTest = EFalse;
	GetBoolFromConfig(ConfigSection(), KAllocTest, allocTest);
	INFO_PRINTF2(KInfoAllocTest, allocTest);
	TPtrC	dataFile;
	if ( GetStringFromConfig(ConfigSection(), KDataFile, dataFile) )
		{
		TFileName dataFileName;
#ifdef __WINS__
		dataFileName.Append(_L("c:"));
#else
		TFileName fullName = RProcess().FileName();
		TParsePtrC parse(fullName);
		dataFileName.Append(parse.Drive());
#endif
		dataFileName.Append(dataFile);
	
		INFO_PRINTF2(KInfoDatafile, &dataFileName);
		OpenAgendaDataFileL(dataFileName);
		if( allocTest )
			{
			doAllocTestL();
			}
		else
			{
			ReadAddEntriesL();
			}
		}
	else
		{
		ERR_PRINTF1(KErrDatafileDataMissing);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}


/**
@SYMTestCaseID 			PIM-APP-ENGINES-CALINTERIMAPI-SECTION 4
@SYMTestType 			CIT
@SYMTestPriority 		Medium
@SYMPREQ 				1098
@SYMFssID 				3.2.6
@SYMTestCaseDesc 		Opens the text file on C:\
@SYMTestActions 		Opens the text file on C:\ which contains the entries
@SYMTestExpectedResults	Open a new calendar  file
*/
void CTestCalInterimApiReadData::OpenAgendaDataFileL(const TDesC& aDataFile)
	{
	User::LeaveIfError(iDataFile.Open(iFsSession, aDataFile, EFileShareReadersOrWriters));
	iDataFileOpened=ETrue;
	iAgendaData.Set(iDataFile);
	}

/**
@SYMTestCaseID 			PIM-CIT-CallnterimAPI-1098AP-01
*/
/**
@SYMTestCaseID 			PIM-CIT-CallnterimAPI-1098AP-02
*/
/**
@SYMTestCaseID 			PIM-CIT-CallnterimAPI-1098AP-03
*/
/**
@SYMTestCaseID 			PIM-CIT-CallnterimAPI-1098AP-04
*/
/**
@SYMTestCaseID 			PIM-CIT-CallnterimAPI-1098AP-05
*/
/**
@SYMTestCaseID 			PIM-APP-ENGINES-CALINTERIMAPI-SECTION 4
@SYMTestType 			CIT
@SYMTestPriority 		Medium
@SYMPREQ 				1098
@SYMFssID 				3.2.6 001 to 023
@SYMTestCaseDesc 		Set and Get attributes of an Event, Todo, Reminder, Appt, Aniv
@SYMTestActions 		Reads the data from the text data file, and adds the entry to the calendar file
@SYMTestExpectedResults All entries and their attributes added to the calendar file,
						Returns ETrue or EFalse
*/
void CTestCalInterimApiReadData::ReadAddEntriesL()
	{
	TBuf<MAX_TEXT_MESSAGE>	buffer;

	iCurrentParser=new (ELeave) CTestCalInterimApiParserMain(*this, *this);
	iParsers.AppendL(iCurrentParser);
	TInt dataRead(0);
	for (dataRead=iAgendaData.Read(buffer); dataRead==KErrNone; dataRead=iAgendaData.Read(buffer) )
		{
		TInt	bufferLength=buffer.Length();
		TInt	colon=buffer.Find(KColon);
		TPtrC	command;
		TPtrC	data;

		if ( colon!=KErrNotFound )
			{
			command.Set(buffer.Left(colon));
			data.Set(buffer.Right(bufferLength-colon-1));
			INFO_PRINTF3(KInfoCommandData, &command, &data);
			}
		else
			{
			command.Set(buffer);
			INFO_PRINTF2(KInfoCommand, &command);
			}

		if ( (command.Find(KComment)==KErrNotFound) && (bufferLength!=0) )
			{
			TInt	parseErr=KErrNotFound;
			while ( (parseErr==KErrNotFound) && (iCurrentParser!=NULL) )
				{
				parseErr=iCurrentParser->ParseL(command, data);

				if ( parseErr==KErrNotFound )
					{
					delete iCurrentParser;
					iCurrentParser=NULL;

					TInt	count=iParsers.Count()-1;
					iParsers.Remove(count);
					if ( count>0 )
						{
						iCurrentParser=iParsers[count-1];
						}
					}
				}
			if ( iCurrentParser==NULL )
				{
				ERR_PRINTF1(KErrCommandNotProcessed);
				SetTestStepResult(EFail);
				}
			}
		}
	iAgendaData.Seek(ESeekStart);
	INFO_PRINTF2(KErrDataRead, dataRead);
	}

void CTestCalInterimApiReadData::doAllocTestL()
	{
	INFO_PRINTF1(_L("Alloc test"));
	ReadAddEntriesL();
	TInt failCount = 1;
	for( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD( err, ReadAddEntriesL());
		if ( err==KErrNone )
			{
			__UHEAP_RESET;
			break;
			}
		if ( err == KErrNoMemory )
			{
			INFO_PRINTF1(_L("Memory allocation testing for readentries is done"));
			INFO_PRINTF2(_L("FAILCOUNT %d"), failCount);
			SetTestStepResult(EPass);
			break;
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		failCount++;
		}
	}

void CTestCalInterimApiReadData::CheckTheNumOfParentChildEntryL(RPointerArray<CCalEntry> aEntriesToFetch)
	{
	if ( aEntriesToFetch.Count() )
		{
		TInt	countOfChildEntry = 0;
		TInt	countOfParentEntry = 0;
		for ( TInt i = 0; i < aEntriesToFetch.Count(); i++ )
			{
			CCalEntry*	entry = aEntriesToFetch[i];
			if ( entry->RecurrenceIdL().TimeUtcL() != Time::NullTTime() )
				{
				countOfChildEntry++;
				}
			else
				{
				countOfParentEntry++;
				}
			}
		INFO_PRINTF2(KInfoChildEntries, countOfChildEntry);
		INFO_PRINTF2(KInfoParentEntry, countOfParentEntry);
		}
	}

/**
When the fetch returns multiple entries with same GUID, this method iterates through them
and finds out the entry we are interested in, which can be the parent or one of the children
*/
CCalEntry* CTestCalInterimApiReadData::GetNewlyAddedEntryL(CCalEntry* aEntry, RPointerArray<CCalEntry>& aEntries)
	{
	for(TInt i = 0; i < aEntries.Count(); ++i)
		{
		if(aEntry->RecurrenceIdL().TimeUtcL() == aEntries[i]->RecurrenceIdL().TimeUtcL())
			{
			return aEntries[i];
			}
		}
	return NULL;
	}
