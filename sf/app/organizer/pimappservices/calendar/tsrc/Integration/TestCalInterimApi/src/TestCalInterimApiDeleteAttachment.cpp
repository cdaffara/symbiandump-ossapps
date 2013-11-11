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

#include "TestCalInterimApiDeleteAttachment.h"
#include <calsession.h>
#include <calentry.h>
#include <calentryview.h>
#include <calattachment.h>

_LIT(KGUid,								"guid");
_LIT(KAttachment,						"attachments");
_LIT(KRecurrenceId,						"recurrenceid");
_LIT(KAllocTest,						"alloctest");

_LIT(KErrDeleteAttachment,				"Error while deleting the attachment");
_LIT(KErrAttachmentNotFound,			"Error: Attachment Not found");


CTestCalInterimApiDeleteAttachment::CTestCalInterimApiDeleteAttachment()
: CTestCalInterimApiSuiteStepBase(), iAllocTest(EFalse)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDeleteAttachment);
	}

CTestCalInterimApiDeleteAttachment::~CTestCalInterimApiDeleteAttachment()
/**
 * Destructor
 */
	{
	delete iCalEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiDeleteAttachment::doTestStepL()
	{
	iCalEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	
	TPtrC	gUid;
	TESTL(GetStringFromConfig(ConfigSection(), KGUid, gUid));			
	TPtrC	recurrenceId;
	GetStringFromConfig(ConfigSection(), KRecurrenceId, recurrenceId);
	GetBoolFromConfig(ConfigSection(), KAllocTest, iAllocTest);

	// Fetch the entry
	RPointerArray<CCalEntry>	entriesFetched;	
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	HBufC8*		entryId = HBufC8::NewLC(gUid.Length());
	entryId->Des().Copy(gUid);
	iCalEntryView->FetchL(entryId->Des(), entriesFetched);
	CleanupStack::PopAndDestroy(entryId);

	// Get the entry we are interested in
	TCalTime	recurrenceIdTime;
	if ( recurrenceId == KNullDesC )
		{
		recurrenceIdTime.SetTimeUtcL(Time::NullTTime());
		}
	else
		{
		recurrenceIdTime.SetTimeUtcL(TTime(recurrenceId));
		}

	// Get the matching entry for the recurrence id
	CCalEntry*	entry = GetMatchingEntryL(entriesFetched, recurrenceIdTime);
	
	TESTL(entry != NULL);

	// Delete the attachment and store the entry in the calendar file
	DeleteAttachmentAndStoreEntryL(entry);
			
	CleanupStack::PopAndDestroy(&entriesFetched);

	return TestStepResult();
	}

/** Delete the attachment present in the parent/child entry and store the entry in the file.
@param aCalEntry Pointer to the CCalEntry
*/		
void CTestCalInterimApiDeleteAttachment::DeleteAttachmentAndStoreEntryL(CCalEntry* aCalEntry)
	{	
	TPtrC	attachments;
	GetStringFromConfig(ConfigSection(), KAttachment, attachments);
	RArray<TPtrC>	attachmentList;
	CleanupClosePushL(attachmentList);
	TokenizeStringL(attachments, attachmentList);	
	// Get the number of attachments present in the entry
	TInt	attachmentCount = aCalEntry->AttachmentCountL();
	for ( TInt j = 0; j < attachmentList.Count(); j++ )
		{
		CCalAttachment*	calAttachment = NULL;
		// Fetch the attachment from the entry
		TRAPD( err, calAttachment = aCalEntry->AttachmentL(j));	
		if ( err != KErrNone )
			{
			ERR_PRINTF1(KErrAttachmentNotFound);
			SetTestStepError(err);
			SetTestStepResult(EFail);
			}
		// If attachment is found, then delete the particular attachment based on the requirement from the user
		if ( calAttachment )
			{
			// If the expected and the retrieved attachment name matches, then delete that attachment
			if ( attachmentList[j] == calAttachment->Label() )
				{
				if ( iAllocTest) // OOM testing
					{
					OOMTestsForDeleteAttachmentL(aCalEntry, calAttachment);
					StoreCalendarEntryL(iCalEntryView, aCalEntry);
					}
				else
					{
					TRAPD( err, aCalEntry->DeleteAttachmentL(*calAttachment));
					// Store the entry after deleting the attachment
					StoreCalendarEntryL(iCalEntryView, aCalEntry);

					if ( err != KErrNone )
						{
						ERR_PRINTF1(KErrDeleteAttachment);
						SetTestStepError(err);
						SetTestStepResult(EFail);
						}
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(&attachmentList);
	}
	
/** OOM tests for Fetch the entry from the file and delete the Parent / child entry's attachment
@param aCalEntry Pointer to the CCalEntry
@param aCalAttachment Pointer to the CCalAttachment
*/
void CTestCalInterimApiDeleteAttachment::OOMTestsForDeleteAttachmentL(CCalEntry* aCalEntry, CCalAttachment* aCalAttachment)
	{
	TInt tryCount = 1;
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		
		TRAPD( err, aCalEntry->DeleteAttachmentL(*aCalAttachment));
				
		if ( err == KErrNone ) 
			{
			__UHEAP_RESET;
			INFO_PRINTF1(_L("Memory allocation testing for Delete Attachment is done"));
			break;
			}
		if ( err != KErrNoMemory )
			{	
			SetTestStepResult(EFail);
			User::Leave(err);
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		tryCount++;
		}	
	INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
	}	



