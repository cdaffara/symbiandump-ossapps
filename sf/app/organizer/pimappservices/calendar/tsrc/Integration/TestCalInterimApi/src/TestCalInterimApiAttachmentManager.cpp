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

#include "TestCalInterimApiAttachmentManager.h"
#include <calsession.h>
#include <calentry.h>
#include <calentryview.h>
#include <calattachment.h>
#include <calattachmentmanager.h>

_LIT(KSortType,							"sorttype");
_LIT(KCheckEntries,						"checkentries");
_LIT(KFileName,							"filename");
_LIT(KGuid,								"guid%d");
_LIT(KInfoIndex,						"Index %d");
_LIT(KExpectedAttachmentCount,			"expectedattachmentcount");

_LIT(KInfoEntryRetrieved,				"Entry with the attachment is retrieved successfully");
_LIT(KInfoAttachmentSize,				"Attachment Size is %d");
_LIT(KInfoSortBySize,					"Entry sorted by their size");
_LIT(KInfoSortByLastModifiedNewestFirst,"Entry Sorted by their newest lastmodifieddate");
_LIT(KInfoSortByLastModifiedOldestFirst,"Entry Sorted by their oldest lastmodifieddate");

_LIT(KErrAttachmentIterator,			"Error while retrieving the attachments, No attachments found");
_LIT(KErrEntryNotRetrieved,				"Entry with the attachment is not retrieved");
_LIT(KErrSortBySize,					"Error: Entry not sorted by their size");
_LIT(KErrSortByLastModifiedNewestFirst,	"Error: Entry not sorted by their newest last modified date");
_LIT(KErrSortByLastModifiedOldestFirst,	"Error: Entry not sorted by their oldest last modified date");
_LIT(KErrInFetchAttachmentsL,			"Error in Fetching Attachments");

_LIT(KSortByDateModifiedNewestFirst,	"ESortByDateModifiedNewestFirst");
_LIT(KSortByDateModifiedOldestFirst,	"ESortByDateModifiedOldestFirst");

/**
Constructor
*/
CTestCalInterimApiAttachmentManager::CTestCalInterimApiAttachmentManager()
:	CTestCalInterimApiSuiteStepBase()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiAttachmentManager);
	}

CTestCalInterimApiAttachmentManager::~CTestCalInterimApiAttachmentManager()
/**
 * Destructor
 */
	{
	iLocalIds.Close();
	iExpectedLocalIdArray.Close();
	delete iAttachmentIterator;
	delete iCalAttachmentManager;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiAttachmentManager::doTestStepL()
	{
	iCalAttachmentManager = CCalAttachmentManager::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	// Retrieve the file attachments from the calendar store
	GetFileAttachmentsBySortTypeL();

	TBool	checkEntries = EFalse;
	GetBoolFromConfig(ConfigSection(), KCheckEntries, checkEntries);

	// If CheckEntries is set to true, then retrieve the entries with the file attachment
	if ( checkEntries )
		{
		// Retrieve the entries for the file attachment
		GetEntriesForFileAttachmentL();
		VerifyEntriesWithFileAttachmentL();
		}

	return TestStepResult();
	}

/**
Get the file attachments present in the calendar store based on the sorttype given by the user
*/
void CTestCalInterimApiAttachmentManager::GetFileAttachmentsBySortTypeL()
	{
	TPtrC	sortType;
	GetStringFromConfig(ConfigSection(), KSortType, sortType);
	// Fetch the attachments based on the sorttype given by the user
	iAttachmentIterator = iCalAttachmentManager->FetchAttachmentsL(ConvertToSortType(sortType));
	CheckAttachmentCountL();
	// If attachmentIterator is not present, set the test step to fail
	if ( !iAttachmentIterator )
		{
		ERR_PRINTF1(KErrAttachmentIterator);
		SetTestStepResult(EFail);
		}
	else
		{
		// Iterate and fetch the attachment
		CCalAttachment*	attachment = iAttachmentIterator->NextL();
		CleanupStack::PushL(attachment);
		if ( attachment )
			{
			// Get the size of the file 
			iSize = attachment->FileAttachment()->Size();
			// Get the last modified date time of the file
			iLastModifiedDateOfFirstAttachment = attachment->FileAttachment()->LastModifiedTimeUtc();
			// Iterate if there are more attachments in the attachmentIterator
			while ( iAttachmentIterator->HasMore() )
				{
				// Iterate and get the next attachment
				CCalAttachment* attachment = iAttachmentIterator->NextL();
				CleanupStack::PushL(attachment);
				// Compares the current attachment's size and lastmodifieddate with the previous attachment's size & lastmodifieddate
				CompareAttachSizeAndLastModifiedDateL(attachment, sortType);
				// Get the current size of the attachment
				iSize = attachment->FileAttachment()->Size();
				// Get the last modified date time of the file attachment
				iLastModifiedDateOfFirstAttachment = attachment->FileAttachment()->LastModifiedTimeUtc();				
				CleanupStack::PopAndDestroy(attachment);
				}	
			}
		CleanupStack::PopAndDestroy(attachment);
		}
	}

/**	Compares the current attachment's size and last Modified date with the previous attachment's
* size and last modified date 
@param	aAttachment	Pointer to CCalAttachment
@param	aSortType	SortType of the attachments
*/	
void CTestCalInterimApiAttachmentManager::CompareAttachSizeAndLastModifiedDateL(CCalAttachment* aAttachment, const TPtrC& aSortType)
	{
	CCalAttachmentManager::TSortOrder	sortType = ConvertToSortType(aSortType);
	// Check if the current attachment's size is greater than the previous attachment's size for ESortBySize
	if ( sortType == CCalAttachmentManager::ESortBySizeLargestFirst )
		{
		if ( aAttachment->FileAttachment()->Size() <= iSize )
			{
			// Print the size of the attachments
			TUint	size(aAttachment->FileAttachment()->Size());
			INFO_PRINTF2(KInfoAttachmentSize, size);
			INFO_PRINTF1(KInfoSortBySize);
			}
		else 
			{
			ERR_PRINTF1(KErrSortBySize);
			SetTestStepResult(EFail);
			}
		}
	else if ( aSortType == KSortByDateModifiedNewestFirst )
		{
		/* Check if the current attachment's lastmodifieddate is lesser than the
		   previous attachment's lastmodifieddate for ESortByDateModifiedNewestFirst */
		if ( aAttachment->FileAttachment()->LastModifiedTimeUtc() <= iLastModifiedDateOfFirstAttachment )
			{
			TTime	lastModifiedDateOfNextAttachment(aAttachment->FileAttachment()->LastModifiedTimeUtc());
			// Print the last modified date of the attachments
			PrintTimeL(lastModifiedDateOfNextAttachment);
			PrintTimeL(iLastModifiedDateOfFirstAttachment);
			INFO_PRINTF1(KInfoSortByLastModifiedNewestFirst);
			}
		else
			{
			ERR_PRINTF1(KErrSortByLastModifiedNewestFirst);
			SetTestStepResult(EFail);
			}
		}
	else // Sort By Oldest First
		{
		/* Check if the current attachment's lastmodifieddate is greater than the
		   previous attachment's lastmodifieddate for ESortByDateModifiedOldestFirst */
		if ( aAttachment->FileAttachment()->LastModifiedTimeUtc() >= iLastModifiedDateOfFirstAttachment )
			{
			TTime	lastModifiedDateOfNextAttachment(aAttachment->FileAttachment()->LastModifiedTimeUtc());
			// Print the last modified date of the attachments
			PrintTimeL(lastModifiedDateOfNextAttachment);
			PrintTimeL(iLastModifiedDateOfFirstAttachment);
			INFO_PRINTF1(KInfoSortByLastModifiedOldestFirst);
			}
		else
			{
			ERR_PRINTF1(KErrSortByLastModifiedOldestFirst);
			SetTestStepResult(EFail);
			}
		}
	}

/** Gets the data from the ini file and converts it of type CCalAttachmentManager::TSortType
@param	aSortType Sort Type of the attachments
@return	CCalAttachmentManager::TSortType Sort type
*/	
CCalAttachmentManager::TSortOrder CTestCalInterimApiAttachmentManager::ConvertToSortType(const TPtrC& aSortType)
	{
	CCalAttachmentManager::TSortOrder	sortType = CCalAttachmentManager::ESortBySizeLargestFirst;
	if ( aSortType.Compare(KSortByDateModifiedNewestFirst) == 0 )
		{
		sortType = CCalAttachmentManager::ESortByDateModifiedNewestFirst;
		}
	else if ( aSortType.Compare(KSortByDateModifiedOldestFirst) == 0 )
		{
		sortType = CCalAttachmentManager::ESortByDateModifiedOldestFirst;
		}
	
	return sortType;
	}
	
/** Retrieve the entries for the file attachment
*/	
void CTestCalInterimApiAttachmentManager::GetEntriesForFileAttachmentL()
	{
	TPtrC	fileName;
	GetStringFromConfig(ConfigSection(), KFileName, fileName);
	
	TPtrC	sortType;
	GetStringFromConfig(ConfigSection(), KSortType, sortType);
	// Fetch the attachments based on the sorttype given by the user
	TRAPD( err, iAttachmentIterator = iCalAttachmentManager->FetchAttachmentsL(ConvertToSortType(sortType)) );
	
	if ( err != KErrNone || iAttachmentIterator == NULL )
		{
		ERR_PRINTF1(KErrInFetchAttachmentsL);
		SetTestStepError(err);
		SetTestStepResult(EFail);
		}
		
	// Iterate if there are more attachments in the attachmentIterator
	while ( iAttachmentIterator->HasMore() )
		{
		// Iterate and get the next attachment
		CCalAttachment* attachment = iAttachmentIterator->NextL();
		CleanupStack::PushL(attachment);
		/* If the attachment name matches with the expected attachment name, then
		   get the entries associated with that file attachment */
		if ( attachment->Label() == fileName )
			{
			iCalAttachmentManager->EntriesReferencingFileAttachmentL(iLocalIds, *attachment);
			}
		CleanupStack::PopAndDestroy(attachment);
		}
	}

/** Verify the entries with the file attachment
*/	
void CTestCalInterimApiAttachmentManager::VerifyEntriesWithFileAttachmentL()
	{
	CCalEntryView* entryView = CCalEntryView::NewL(GetSession());
	CleanupStack::PushL(entryView);
	
	// Get the expected localId and append it in the array iLocalIdArray
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	index = 0;
	TBool	moreData = ETrue;

	RArray<TPtrC> guidArray;
	CleanupClosePushL(guidArray);
	
	while ( moreData )
		{
		TPtrC	guid;
		tempStore.Format(KGuid(), ++index);
		INFO_PRINTF2(KInfoIndex, index);
		
		moreData = GetStringFromConfig(ConfigSection(), tempStore, guid);
		guidArray.AppendL(guid);
		}

	//Compare the retrieved and the expected localIds of the entries
	for ( TInt i = 0; i < iLocalIds.Count(); i++ )
		{
		INFO_PRINTF2(_L("LOCALID %d"), iLocalIds[i]);
		CCalEntry* entry = entryView->FetchL(iLocalIds[i]);
		CleanupStack::PushL(entry);
		TBuf<256> guidBuffer;
		guidBuffer.Copy(entry->UidL());
		if ( guidBuffer == guidArray[i] )
			{
			
			INFO_PRINTF1(KInfoEntryRetrieved);
			}
		else
			{
			ERR_PRINTF1(KErrEntryNotRetrieved);
			SetTestStepResult(EFail);
			}
		CleanupStack::PopAndDestroy(entry);
		}

	CleanupStack::PopAndDestroy(&guidArray);
	CleanupStack::PopAndDestroy(entryView);
	}

/*
Check the attachment count that is returned by the iterator
*/
void CTestCalInterimApiAttachmentManager::CheckAttachmentCountL()
	{
	TInt	expectedAttachmentCount;
	GetIntFromConfig(ConfigSection(), KExpectedAttachmentCount, expectedAttachmentCount);
	
	// Chcck the file attachment count is equal to the expected count
	TESTL( iAttachmentIterator->Count() == expectedAttachmentCount );
	}
	


