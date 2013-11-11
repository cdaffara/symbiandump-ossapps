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

#include "TestCalInterimApiCheckAttendee.h"

#include <calentry.h>
#include <calentryview.h>
#include <caluser.h>

_LIT(KGuid,										"guid");
_LIT(KRecurrenceId,								"recurrenceid");
_LIT(KAttendees,								"attendees%d");
_LIT(KRole,										"role%d");
_LIT(KStatus,									"status%d");
_LIT(KCommonName,								"commonname%d");
_LIT(KExpectedAttendeeCount,					"expectedattendeecount");


_LIT(KExpectedAndRetrievedAttendeesCount,		"Expected Attendees Count : %d, Retrieved Attendees Count : %d");
_LIT(KInfoIndex,								"Index %d");


CTestCalInterimApiCheckAttendee::CTestCalInterimApiCheckAttendee()
:	CTestCalInterimApiSuiteStepBase()

/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiCheckAttendee);
	}

CTestCalInterimApiCheckAttendee::~CTestCalInterimApiCheckAttendee()
/**
 * Destructor
 */
	{
	delete iCalEntryView;
	}


/**
Base class pure virtual.
*/
TVerdict CTestCalInterimApiCheckAttendee::doTestStepL()
	{
	iCalEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	// Get the data from the ini and check the attendees properties of the entry
	GetDataAndCheckAttendeeL();
	
	return TestStepResult();
	}
	
/** Get the expected values from the ini.Fetch the entry and check the
attendee properties like count, role, status and common name
@test
*/
void CTestCalInterimApiCheckAttendee::GetDataAndCheckAttendeeL()														   
	{ 
	TPtrC	gUid;
	TPtrC	recurrenceId;

	TESTL(GetStringFromConfig(ConfigSection(), KGuid, gUid));
	GetStringFromConfig(ConfigSection(), KRecurrenceId, recurrenceId);

	FetchAndCheckAttendeesL(gUid, recurrenceId);
	}

/** Fetch the entry and check the attendess properties
@param	aGuid Data for fetching the entry
@param	aRecurrenceId Id to fetch the child entry
*/
void CTestCalInterimApiCheckAttendee::FetchAndCheckAttendeesL(const TPtrC& aGuid, const TPtrC& aRecurrenceId)
	{
	RPointerArray<CCalEntry>	entriesFetched;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	HBufC8*	entryId	= HBufC8::NewLC(aGuid.Length());
	entryId->Des().Copy(aGuid);
	iCalEntryView->FetchL(entryId->Des(), entriesFetched);
	CleanupStack::PopAndDestroy(entryId);

	// Get the entry we are interested in
	TCalTime	recurrenceIdTime;
	if ( aRecurrenceId == KNullDesC )
		{
		recurrenceIdTime.SetTimeUtcL(Time::NullTTime());
		}
	else
		{
		recurrenceIdTime.SetTimeUtcL(TTime(aRecurrenceId));
		}

	CCalEntry*	entry = GetMatchingEntryL(entriesFetched, recurrenceIdTime);
	
	TESTL( entry != NULL );
	// Check the properties of the attendees
	CheckAttendeePropertiesL(entry);

	CleanupStack::PopAndDestroy(&entriesFetched);
	}

/** Get the attendee and Check the attendess properties like role,status,address
and common name of the attendees based on the requirement from the user
@param	aEntry Pointer to CCalEntry
*/
void CTestCalInterimApiCheckAttendee::CheckAttendeePropertiesL(CCalEntry* aEntry)
	{
	RPointerArray<CCalAttendee>	entryAttendeeList;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entryAttendeeList));
	entryAttendeeList = aEntry->AttendeesL();

	TInt	expectedAttendeeCount;
	GetIntFromConfig(ConfigSection(), KExpectedAttendeeCount, expectedAttendeeCount);
	TInt	attendeeCount = entryAttendeeList.Count();
	// Check the expected and retrieved attendees count
	if ( expectedAttendeeCount == attendeeCount )
		{
		INFO_PRINTF3(KExpectedAndRetrievedAttendeesCount, expectedAttendeeCount, attendeeCount);
		TPtrC	attendees;
		TBool	moreData = ETrue;
		while ( moreData )
			{
			iTempStore.Format(KAttendees(), ++iIndex);
			INFO_PRINTF2(KInfoIndex, iIndex);
			moreData = GetStringFromConfig(ConfigSection(), iTempStore, attendees);		
			if ( moreData )
				{
				TESTL( entryAttendeeList[iAttendeeCount]->Address() == attendees );
				GetDataAndCheckAttendeePropertiesL(entryAttendeeList);
				iAttendeeCount++;
				}
			}
		}
	else
		{
		ERR_PRINTF3(KExpectedAndRetrievedAttendeesCount, expectedAttendeeCount, attendeeCount);
		SetTestStepResult(EFail);
		}
		
	CleanupStack::Pop(&entryAttendeeList); // entriesFetched owns the ownership
	}

/** get the input from the ini and Check the attendess properties like role,status,address
and common name of the attendees and compare it with the expected result.
*/
void CTestCalInterimApiCheckAttendee::GetDataAndCheckAttendeePropertiesL(RPointerArray<CCalAttendee> aEntryAttendeeList)
	{
	TInt	role;	
	iTempStore.Format(KRole(), iIndex);
	TBool	hasRole = EFalse;
	hasRole = GetIntFromConfig(ConfigSection(), iTempStore, role);
	TInt	status;
	iTempStore.Format(KStatus(), iIndex);
	TBool	hasStatus = EFalse;
	hasStatus = GetIntFromConfig(ConfigSection(), iTempStore, status);
	TPtrC	commonName;
	iTempStore.Format(KCommonName(), iIndex);
	GetStringFromConfig(ConfigSection(), iTempStore, commonName);
	
	if ( hasRole )
		{
		TESTL( aEntryAttendeeList[iAttendeeCount]->RoleL() == role );	
		}
	if ( hasStatus )
		{
		TESTL( aEntryAttendeeList[iAttendeeCount]->StatusL() == status );	
		}
	if ( commonName != KNullDesC() )
		{
		TESTL( aEntryAttendeeList[iAttendeeCount]->CommonName() == commonName );
		}
	}

