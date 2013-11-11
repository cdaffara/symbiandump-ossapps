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

#include "TestCalInterimApiDeleteAttendee.h"
#include <calsession.h>
#include <calentry.h>

_LIT(KGuid,						"guid");
_LIT(KRecurrenceId,				"recurrenceid");
_LIT(KAttendees,				"attendees");

_LIT(KErrDeleteAttendee,		"Error:Error while Deleting the attendee");

CTestCalInterimApiDeleteAttendee::CTestCalInterimApiDeleteAttendee()
:	CTestCalInterimApiSuiteStepBase()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDeleteAttendee);
	}

CTestCalInterimApiDeleteAttendee::~CTestCalInterimApiDeleteAttendee()
/**
 * Destructor
 */
	{
	delete iEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiDeleteAttendee::doTestStepL()
	{
	TPtrC	guid;
	TPtrC	recurrenceId;

	TESTL(GetStringFromConfig(ConfigSection(), KGuid, guid));
	GetStringFromConfig(ConfigSection(), KRecurrenceId, recurrenceId);
	
	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	RPointerArray<CCalEntry>	entriesFetched;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	HBufC8*	entryId = HBufC8::NewLC(guid.Length());
	entryId->Des().Copy(guid);
	iEntryView->FetchL(entryId->Des(), entriesFetched);
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

	CCalEntry*	entry = GetMatchingEntryL(entriesFetched, recurrenceIdTime);
	
	TESTL( entry != NULL );
	
	// Delete the attendees present in the parent/child entry
	DeleteAttendeeL(entry);
	
	CleanupStack::PopAndDestroy(&entriesFetched);

	return TestStepResult();
	}

/** Delete the attendee present in the entry
@param	aCalEntry Pointer to CCalEntry
*/
void CTestCalInterimApiDeleteAttendee::DeleteAttendeeL(CCalEntry* aCalEntry)
	{
	TPtrC	attendees;
	GetStringFromConfig(ConfigSection(), KAttendees, attendees);
	RArray<TPtrC>	attendeeList;
	CleanupClosePushL(attendeeList);
	TokenizeStringL(attendees, attendeeList);	
	// Get the number of attendees present in the entry
	RPointerArray<CCalAttendee>	attendeeCount = aCalEntry->AttendeesL();
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyAttendeeArray, &attendeeCount));
	
	for ( TInt j = 0; j < attendeeList.Count(); j++ )
		{
		TRAPD(err, aCalEntry->DeleteAttendeeL(j));
		if ( err != KErrNone )
			{
			ERR_PRINTF1(KErrDeleteAttendee);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		// Store the entry after deleting the attendee
		StoreCalendarEntryL(iEntryView, aCalEntry);
		}
	CleanupStack::Pop(&attendeeCount); // entriesFetched owns the ownership
	CleanupStack::PopAndDestroy(&attendeeList);
	}



