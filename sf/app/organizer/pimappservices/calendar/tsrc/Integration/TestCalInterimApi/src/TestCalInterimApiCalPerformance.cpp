// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiCalPerformance.h"

#include <calattachment.h>

// Initializes to current time
#define NOW(time); TTime time; time.HomeTime();


_LIT(KDataFile,							"datafile");
_LIT(KStorePerformance,					"storeperformance");
_LIT(KFetchPerformance,					"fetchperformance");
_LIT(KFetchAttachment,					"fetchattachment");
_LIT(KMaxTimeLimit,						"maxtimelimit");
_LIT(KFetchAttachPerformance,			"Time Taken of Fetching Attachment %d is %f secs");

_LIT(KInfoTimeTakenForStore,			"Time taken for storing %d entries is %f secs");
_LIT(KInfoFetchEntry,					"Time taken for fetching the entry %d is: %f secs");
_LIT(KInfoTotalTimeTakenForFetch,		"Total time taken for fetching %d entries is %f secs");
_LIT(KInfoTimeTakenForAttachment,		"Total time taken for fetching %d attachments in entry%d is %f secs");

_LIT(KErrAddOperation,					"Error in StoreL %d");
_LIT(KErrFetchOperation,				"Error in FetchL %d");


CTestCalInterimApiCalPerformance::CTestCalInterimApiCalPerformance()
: CTestCalInterimApiReadData()
, iMaxTimeLimit(0.0)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiCalPerformance);
	}


CTestCalInterimApiCalPerformance::~CTestCalInterimApiCalPerformance()
/**
 * Destructor
 */
	{
	iEntryArray.Close();
	delete iEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiCalPerformance::doTestStepL()
	{
	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	// Retrieves the Data file and opens it
	RetreiveIniDataL();
	
	// Reads data from the text file and adds the entry to the calendar file
	ReadAddEntriesL();
	
	TPtrC	maxTimeTaken;
	GetStringFromConfig(ConfigSection(), KMaxTimeLimit, maxTimeTaken);
	
	/* Converting TPtrC to TReal32 type which is used for comparing the time taken
	   with the MaxTimeLimit */
	TLex lex(maxTimeTaken);
	lex.Val(iMaxTimeLimit);
	
	// Log the time taken for adding the entries with attachment/ without attachment
	PerformanceOfAddEntriesL();	
	// Log the time taken for fetching the entries with attachment/ without attachment
	PerformanceOfFetchEntriesL();		
	/* If this flag is set, then compute the time taken for fetching all the attachments
	   in the entry */
	PerformanceOfFetchingAttachmentL();

	iEntryArray.ResetAndDestroy();
		
	return TestStepResult();
	}

/** Retrieves the data file from the ini and opens the data file
*/	
void CTestCalInterimApiCalPerformance::RetreiveIniDataL()
	{
	// Data file
	TPtrC	dataFile;
	TESTL( GetStringFromConfig(ConfigSection(), KDataFile, dataFile) );
	OpenAgendaDataFileL(dataFile);
	}

/**
* Virtual function called while parsing the entries from the data file
* Takes ownership of aEntry
@param aEntry - current entry being parsed
*/
void CTestCalInterimApiCalPerformance::ProcessEntryL(CCalEntry* aEntry)
	{
	iEntryArray.AppendL(aEntry);
	}

/**
* Log the time taken for Adding entries with/ without attachment to the calendar file
*/
void CTestCalInterimApiCalPerformance::PerformanceOfAddEntriesL()
	{
	// If this flag is set, then compute the time taken for storing the entries	
	TBool	storePerformance = EFalse;
	TESTL(GetBoolFromConfig(ConfigSection(), KStorePerformance, storePerformance));
	// If it is not set, then return
	if ( storePerformance == EFalse )
		{
		return;
		}
	TInt  entriesStored(0);
	// Log the current time and perform the operation
	NOW(startTime);
	TRAPD(err, iEntryView->StoreL(iEntryArray, entriesStored));
	// Log the current time after the operation
	NOW(endTime);
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(KErrAddOperation, err);
		SetTestStepResult(EFail);
		}

	// Check the time difference
	TReal32 elapsedTime = CalculateTimeDifference(startTime, endTime, iMaxTimeLimit);
	INFO_PRINTF3(KInfoTimeTakenForStore, iEntryArray.Count(), elapsedTime);
	}

/**
* Log the time taken for Fetching Entries with/ without attachment
*/
void CTestCalInterimApiCalPerformance::PerformanceOfFetchEntriesL()
	{
	TBool	fetchPerformance = EFalse;
	GetBoolFromConfig(ConfigSection(), KFetchPerformance, fetchPerformance);
	// If it is not set, then return
	if ( fetchPerformance == EFalse )
		{
		return;
		}		

	RPointerArray<CCalEntry> calEntriesArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntriesArray));

	TReal32	totalTimeTaken = 0.0;
	// Iterate through the iEntryArray
	for (TInt j(0) ; j < iEntryArray.Count(); ++j)
		{
		TPtrC8	guid(iEntryArray[j]->UidL());
		
		// Log the current time and perform the operation
		NOW(startTime);
		TRAPD(err, iEntryView->FetchL(guid, calEntriesArray));
		// Log the current time after the operation
		NOW(endTime);
		
		// checks the time difference and returns the time taken for FetchL
		TReal32 elapsedTime = CalculateTimeDifference(startTime, endTime, iMaxTimeLimit);
		INFO_PRINTF3(KInfoFetchEntry, j, elapsedTime);

		/*Adds the elapsedtime with the total time taken for calculating the total 
		  time taken for fetching the entries in iEntryArray */
		totalTimeTaken += elapsedTime;

		if(err != KErrNone)
			{
			ERR_PRINTF2(KErrFetchOperation, err);
			SetTestStepResult(EFail);
			}
		}
	// Prints the total time taken for fetching the entries in the iEntryArray
	INFO_PRINTF3(KInfoTotalTimeTakenForFetch, iEntryArray.Count(), totalTimeTaken);
	CleanupStack::PopAndDestroy(&calEntriesArray); //calEntriesArray.ResetAndDestroy();
	}

// comment this block to run on Pre-1090 code
/**
* Log the time taken for Fetching the attachments
*/	
void CTestCalInterimApiCalPerformance::PerformanceOfFetchingAttachmentL()
	{
	TBool	fetchAttachPerformance = EFalse;
	GetBoolFromConfig(ConfigSection(), KFetchAttachment, fetchAttachPerformance);
	// If it is not set, then return
	if ( fetchAttachPerformance == EFalse )
		{
		return;
		}		
	RPointerArray<CCalEntry> calEntriesArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntriesArray));

	for ( TInt i = 0; i < iEntryArray.Count(); i++ )
		{
		TPtrC8	guid = iEntryArray[i]->UidL();
		TRAPD( err, iEntryView->FetchL(guid, calEntriesArray));
		if ( err != KErrNone )
			{
			ERR_PRINTF2(KErrFetchOperation, err);
			SetTestStepResult(EFail);
			}
		for ( TInt j = 0; j < calEntriesArray.Count(); j++ )
			{
			CCalEntry*	entry = calEntriesArray[j];
			CleanupStack::PushL(entry);

			TReal32	attachmentTimeTaken = 0.0;
			// Iterate through the attachments
			for ( TInt k = 0; k < entry->AttachmentCountL(); k++ )
				{
				// Log the current time and perform the operation
				NOW(startTime);
				CCalAttachment*	attachment = entry->AttachmentL(k);							
				// Log the current time after the operation
				NOW(endTime);
				// check the time difference
				TReal32 elapsedTime = CalculateTimeDifference(startTime, endTime, iMaxTimeLimit);
				INFO_PRINTF3(KFetchAttachPerformance, k, elapsedTime);
				attachmentTimeTaken += elapsedTime;
				}
				
			//Prints the total time taken for fetching all the attachments in the entry
			INFO_PRINTF4(KInfoTimeTakenForAttachment, entry->AttachmentCountL(), i, attachmentTimeTaken);
			CleanupStack::PopAndDestroy(entry);
			calEntriesArray.Reset();
			}
		}
	CleanupStack::PopAndDestroy(&calEntriesArray);//calEntriesArray.ResetAndDestroy();
	}

