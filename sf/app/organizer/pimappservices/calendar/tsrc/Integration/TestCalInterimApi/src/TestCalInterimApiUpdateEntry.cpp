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

#include "TestCalInterimApiUpdateEntry.h"
#include <calsession.h>
#include <calentry.h>
#include <calentryview.h>
#include <calcategory.h>
#include <calattachment.h>

_LIT(KGUid,						"guid");
_LIT(KSummary,					"summary");
_LIT(KDescription,				"description");
_LIT(KStoreEntry,				"storeentry");
_LIT(KUpdateEntry,				"updateentry");
_LIT(KRecurrenceId,				"recurrenceid");
_LIT(KTimeMode,					"timemode");
_LIT(KCategories,				"categories");
_LIT(KAlarmOffset,				"alarmoffset");
_LIT(KGEOLat,					"GEOLat");
_LIT(KGEOLong,					"GEOLong");
_LIT(KTrans,					"trans");
_LIT(KPerformanceTest,			"performancetest");
_LIT(KTimeLimit,	 			"timelimit");
_LIT(KAllocTest,				"alloctest");
_LIT(KLocation,					"location");
_LIT(KAttendees,				"attendees");
_LIT(KCommonName,				"commonname");
_LIT(KAttendeeRole,				"role");
_LIT(KAttendeeStatus,			"status");
_LIT(KResponseRequested,		"rsvp");
_LIT(KFileName,	 				"attachmentname");
_LIT(KNewFileName,	 			"newlabel");
_LIT(KDrive,	 				"drive");
_LIT(KLastModifiedDate,	 		"lastmodifieddate");
_LIT(KAttributes,	 			"attributes");
_LIT(KClearAttribute, 			"clearattributes");
_LIT(KContentId,	 			"contentid");
_LIT(KMimeType,		 			"mimetype");
_LIT(KContentIdAttachment,		"createcontentid");

_LIT(KInfoLocalUid,				"Local UID %d");
_LIT(KInfoStoreMessage,			"Entry updated successfully using StoreL");
_LIT(KInfoUpdateMessage,		"Entry updated successfully using UpdateL");
_LIT(KInfoNoMatchInRecurrenceId,"Recurrence Id for entryindex%d does not matches");

_LIT(KErrInStoreEntry,			"Error:Entry Not stored");
_LIT(KErrUpdate,				"ERROR:Update of child entry using UpdateL() Api is not allowed");
_LIT(KErrNoAalarm,				"ERROR:This entry does NOT have an alarm");

CTestCalInterimApiUpdateEntry::CTestCalInterimApiUpdateEntry()
:	CTestCalInterimApiSuiteStepBase()
,	iStoreEntry(EFalse)
,	iUpdateEntry(EFalse)
,	iEntryView(NULL)
,	iAllocTestForUpdate(EFalse)

/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiUpdateEntry);
	}

CTestCalInterimApiUpdateEntry::~CTestCalInterimApiUpdateEntry()
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
TVerdict CTestCalInterimApiUpdateEntry::doTestStepL()
	{
	TEntryProperties	entryProperties;
	TPtrC	guid;
	TPtrC	timeMode;

	TBool	performanceTest(EFalse);
	TReal32	maxTimeLimit(5.0);
	TPtrC 	timeLimitString;

	TESTL( GetStringFromConfig(ConfigSection(), KGUid, guid) );
	TESTL( GetBoolFromConfig(ConfigSection(), KStoreEntry, iStoreEntry) );
	TESTL( GetBoolFromConfig(ConfigSection(), KUpdateEntry, iUpdateEntry) );
	GetBoolFromConfig(ConfigSection(), KAllocTest,	iAllocTestForUpdate);

	// Dont mind if these are not given in the ini file.
	GetStringFromConfig(ConfigSection(), KSummary, entryProperties.iSummary);
	GetStringFromConfig(ConfigSection(), KDescription, entryProperties.iDescription);
	GetStringFromConfig(ConfigSection(), KRecurrenceId, entryProperties.iRecurrenceId);
	GetStringFromConfig(ConfigSection(), KCategories, entryProperties.iCategories);
	GetStringFromConfig(ConfigSection(), KTimeMode, timeMode);
	GetStringFromConfig(ConfigSection(), KGEOLat, entryProperties.iGeoLat);
	GetStringFromConfig(ConfigSection(), KGEOLong, entryProperties.iGeoLong);
	GetIntFromConfig(ConfigSection(), KTrans, entryProperties.iTrans);
	GetStringFromConfig(ConfigSection(), KLocation, entryProperties.iLocation);
	GetStringFromConfig(ConfigSection(), KAttendees, entryProperties.iAttendees);
	GetStringFromConfig(ConfigSection(), KCommonName, entryProperties.iCommonName);

	GetBoolFromConfig(ConfigSection(), KPerformanceTest, performanceTest);

	entryProperties.iHasAlarm =
	GetIntFromConfig   (ConfigSection(), KAlarmOffset, entryProperties.iAlarmOffset);
	entryProperties.iHasRole =
	GetIntFromConfig(ConfigSection(), KAttendeeRole, entryProperties.iCalRole);
	entryProperties.iHasStatus =
	GetIntFromConfig(ConfigSection(), KAttendeeStatus, entryProperties.iCalStatus);
	entryProperties.iHasRsvp =
	GetBoolFromConfig(ConfigSection(), KResponseRequested, entryProperties.iRsvp);
	GetStringFromConfig(ConfigSection(), KFileName, iAttachProperties.iName);
	GetStringFromConfig(ConfigSection(), KMimeType, iAttachProperties.iMimeType);
	GetStringFromConfig(ConfigSection(), KNewFileName, iAttachProperties.iNewLabel);
	GetStringFromConfig(ConfigSection(), KDrive, iAttachProperties.iDrive);
	GetStringFromConfig(ConfigSection(), KLastModifiedDate, iAttachProperties.iLastModifiedDate);
	iAttachProperties.iHasAttributes = GetIntFromConfig(ConfigSection(), KAttributes, iAttachProperties.iAttribute);
	iAttachProperties.iHasClearAttribute = GetIntFromConfig(ConfigSection(), KClearAttribute, iAttachProperties.iClearAttribute);
	GetStringFromConfig(ConfigSection(), KContentId, iAttachProperties.iContentId);
	GetStringFromConfig(ConfigSection(), KContentIdAttachment, iAttachProperties.iCreateContentId);

	if (!performanceTest)
		{
		if (!iEntryView)
			{
			iEntryView = CCalEntryView::NewL(GetSession(), *this);
			CActiveScheduler::Start();
			}

		FetchAndUpdateL(guid, entryProperties, timeMode);

		}
	else
		{

		// Performance test to determine the time taken for creating an entry view
		GetStringFromConfig(ConfigSection(), KTimeLimit, timeLimitString);
		maxTimeLimit = GetTReal32(timeLimitString);

		if 	(maxTimeLimit <= 0.0)
			{
			maxTimeLimit = 5.0;
			}

		PerformanceTestEntryViewL(maxTimeLimit);
		}

	return TestStepResult();
	}


/** Fetch the entry by passing the Uid of the entry and set the properties to be updated.
@param	aEntryView Pointer to CCalEntryView
@param	aGuid Guid of the entry to be fetched
@param	aEntryProperties Structure containing the entry properties
@param	aTimeMode timemode of the entry ie. Floating time/ Utc
*/
void CTestCalInterimApiUpdateEntry::FetchAndUpdateL(const TPtrC& aGuid, const TEntryProperties& aEntryProperties, const TPtrC& aTimeMode)
	{
	RPointerArray<CCalEntry>	entriesFetched;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	HBufC8*	entryGuid = HBufC8::NewLC(aGuid.Length());
	entryGuid->Des().Copy(aGuid);
	iEntryView->FetchL(entryGuid->Des(), entriesFetched);

	CheckRecurrenceIdAndSetDataL(entriesFetched, aEntryProperties, aTimeMode);

	CleanupStack::PopAndDestroy(entryGuid);
	CleanupStack::PopAndDestroy(&entriesFetched); //Takes the ownership of entriesToStore
	}


/** Check if the recurrence Id matches with the recurrence Id of fetched entries and then Update the data
@param	aEntryView Pointer to CCalEntryView
@param	aEntriesFetched Array containing the fetched entries
@param	aEntryProperties Structure containing the entry properties
@param	aTimeMode timemode of the entry ie. Floating time/ Utc
*/
void CTestCalInterimApiUpdateEntry::CheckRecurrenceIdAndSetDataL(RPointerArray<CCalEntry> aEntriesFetched, const TEntryProperties& aEntryProperties, const TPtrC& aTimeMode)
	{
	for ( TInt i = 0; i < aEntriesFetched.Count(); i++ )
		{
		CCalEntry*	calEntry = aEntriesFetched[i];
		if ( aEntryProperties.iRecurrenceId != KNullDesC16() )
			{
			// To convert Tptrc to TTime
			TTime	recurrenceIdValue;
			recurrenceIdValue.Set(aEntryProperties.iRecurrenceId);
			TCalTime	recurrenceId;
			// Sets the time mode to floating or Utc based on the time mode given
			SetCalTimeL(recurrenceIdValue, recurrenceId, SetTimeMode(aTimeMode));
			if ( recurrenceId.TimeUtcL() == (calEntry->RecurrenceIdL().TimeUtcL()) )
				{
				SetDataL(calEntry, aEntryProperties);
				}
			else
				{
				INFO_PRINTF2(KInfoNoMatchInRecurrenceId, i);
				}
			}
		else
			{
			if ( calEntry->RecurrenceIdL().TimeUtcL() == Time::NullTTime() )
				{
				SetDataL(calEntry, aEntryProperties);
				}
			}
		}
	}


/** Sets the data received from the ini file and Update the Parent/ child entry
@param	aCalEntry Pointer to the CCalEntry
@param	aEntryProperties Structure containing the entry properties
*/
void CTestCalInterimApiUpdateEntry::SetDataL(CCalEntry* aCalEntry, const TEntryProperties& aEntryProperties)
	{
	if ( aEntryProperties.iSummary != KNullDesC16() )
		{
		aCalEntry->SetSummaryL(aEntryProperties.iSummary);
		}
	if ( aEntryProperties.iDescription != KNullDesC16() )
		{
		aCalEntry->SetDescriptionL(aEntryProperties.iDescription);
		}
	if(aEntryProperties.iCategories != KNullDesC16() )
		{
		AddCategoriesL(aCalEntry, aEntryProperties.iCategories);
		}
	if(aEntryProperties.iHasAlarm)
		{
		ModifyAlarmL(aCalEntry, aEntryProperties.iAlarmOffset);
		}
	if(aEntryProperties.iGeoLat != KNullDesC16() && aEntryProperties.iGeoLong != KNullDesC16())
		{
		ModifyGEOPropertyL(aCalEntry, aEntryProperties.iGeoLat, aEntryProperties.iGeoLong);
		}
	if(aEntryProperties.iTrans != KValueIntNotIni)
		{
		ModifyTransL(aCalEntry, aEntryProperties.iTrans);
		}
	if ( aEntryProperties.iAttendees != KNullDesC16() )
		{
		AddAttendeesL(aCalEntry, aEntryProperties);
		}
	if(iAttachProperties.iName != KNullDesC() )
		{
		GetAttachmentAndSetDataL(aCalEntry);
		}
	iLocalUid = aCalEntry->LocalUidL();
	INFO_PRINTF2(KInfoLocalUid, iLocalUid);
	if ( iStoreEntry )
		{
		StoreEntryL(aCalEntry);
		}
	if ( iUpdateEntry )
		{
		UpdateEntryL(aCalEntry);
		}
	}

/** Gets the attachment from the entry and check the attachment properties
@param	aCalEntry Pointer to the CCalEntry
*/
void CTestCalInterimApiUpdateEntry::GetAttachmentAndSetDataL(CCalEntry* aCalEntry)
	{
	for ( TInt i = 0 ; i < aCalEntry->AttachmentCountL(); i++ )
		{
		CCalAttachment*	calAttachment = aCalEntry->AttachmentL(i);

		// If the name of the attachment matches with the expected name, then fetch that particular attachment
		if ( calAttachment->Label() == iAttachProperties.iName )
			{
			// Modify the attachment properties
			SetAttachmentDataL(calAttachment,*aCalEntry);
			}
		}
	}

/** Updating the entry by calling StoreL() Api of CCalEntryView
@param	aEntryView Pointer to CCalEntryView
@param	aCalEntry Pointer to the CCalEntry
*/
void CTestCalInterimApiUpdateEntry::StoreEntryL(CCalEntry* aCalEntry)
	{
	RPointerArray<CCalEntry>	entriesToStore;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesToStore));
	entriesToStore.AppendL(aCalEntry);
	TInt	entriesStored(0);
	TRAPD( err, iEntryView->StoreL(entriesToStore, entriesStored) );
	CleanupStack::Pop(&entriesToStore); // The ownership of this is transferred to entriesFetched
	if ( err == KErrNone )
		{
		TESTL( entriesToStore.Count() == entriesStored );
		TESTL( iLocalUid == entriesToStore[0]->LocalUidL() );
		INFO_PRINTF1(KInfoStoreMessage);
		}
	else
		{
		ERR_PRINTF1(KErrInStoreEntry);
		SetTestStepError(err);
		SetTestStepResult(EFail);
		}
	}


/** Updating the entry by calling UpdateL() Api of CCalEntryView
@param	aEntryView Pointer to CCalEntryView
@param	aCalEntry Pointer to the CCalEntry
*/
void CTestCalInterimApiUpdateEntry::UpdateEntryL(CCalEntry* aCalEntry)
	{
	RPointerArray<CCalEntry>	entriesToStore;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesToStore));
	entriesToStore.AppendL(aCalEntry);
	TInt	entriesStored(0);
	if ( iAllocTestForUpdate ) // OOM testing for UpdateL() API
		{
		OOMTestForUpdateEntryL(entriesToStore, entriesStored);
		CleanupStack::Pop(&entriesToStore);
		}
	else
		{
		TRAPD(err, iEntryView->UpdateL(entriesToStore, entriesStored));
		CleanupStack::Pop(&entriesToStore); // The ownership of this is transferred to entriesFetched
		if ( err == KErrArgument )
			{
			ERR_PRINTF1(KErrUpdate);
			}
		else if ( err == KErrNone )
			{
			TESTL( entriesToStore.Count() == entriesStored );
			INFO_PRINTF1(KInfoUpdateMessage);
			TESTL( iLocalUid == entriesToStore[0]->LocalUidL() );
			}
		else
			{
			ERR_PRINTF1(KErrUpdate);
			SetTestStepError(err);
			SetTestStepResult(EFail);
			}
		}
	}

/** Add a category to the entry
@param	aCalEntry Pointer to the CCalEntry
@param	aCategories category to add
*/
void CTestCalInterimApiUpdateEntry::AddCategoriesL(CCalEntry* aCalEntry, const TDesC& aCategories)
	{
	RArray<TPtrC>	categoryList;
	CleanupClosePushL(categoryList);
	TokenizeStringL(aCategories, categoryList);
	for( TInt i = 0; i < categoryList.Count(); ++i )
		{
		CCalCategory*	category = CCalCategory::NewL(categoryList[i]);
		aCalEntry->AddCategoryL(category);
		}
	CleanupStack::PopAndDestroy(&categoryList);
	}

/**
To modify the alarm time
@param	aCalEntry Pointer to the CCalEntry
@param	aAlarmOffset offset of the alarm
*/
void CTestCalInterimApiUpdateEntry::ModifyAlarmL(CCalEntry* aCalEntry, const TInt& aAlarmOffset)
	{
	CCalAlarm*	alarm = aCalEntry->AlarmL();
	if( !alarm )
		{
		ERR_PRINTF1(KErrNoAalarm);
		SetTestStepResult(EFail);
		return;
		}
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(aAlarmOffset);
	aCalEntry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	}

/** Add an attendee to the entry
@param	aCalEntry Pointer to the CCalEntry
@param	aEntryProperties Struct containing the entry properties
*/
void CTestCalInterimApiUpdateEntry::AddAttendeesL(CCalEntry* aCalEntry, const TEntryProperties& aEntryProperties)
	{
	RArray<TPtrC>	attendeeList;
	CleanupClosePushL(attendeeList);
	TokenizeStringL(aEntryProperties.iAttendees, attendeeList);
	for(TInt i = 0; i < attendeeList.Count(); ++i)
		{
		CCalAttendee* attendee = CCalAttendee::NewL(attendeeList[i]);
		CleanupStack::PushL(attendee);

		if( aEntryProperties.iCommonName != KNullDesC16() )
			{
			attendee->SetCommonNameL(aEntryProperties.iCommonName);
			}
		if( aEntryProperties.iHasRole )
			{
			attendee->SetRoleL((CCalAttendee::TCalRole)aEntryProperties.iCalRole);
			}
		if( aEntryProperties.iHasStatus )
			{
			attendee->SetStatusL((CCalAttendee::TCalStatus)aEntryProperties.iCalStatus);
			}
		if ( aEntryProperties.iHasRsvp )
			{
			attendee->SetResponseRequested(aEntryProperties.iRsvp);
			}

		aCalEntry->AddAttendeeL(attendee);
		CleanupStack::Pop(attendee);
		}
	CleanupStack::PopAndDestroy(&attendeeList);
	}

/** To Modify the GEO Property
@param	aCalEntry Pointer to the CCalEntry
@param	aGeoLat GEO Latitude to change
@param	aGeoLong GEO Longitude to change
*/
void CTestCalInterimApiUpdateEntry::ModifyGEOPropertyL(CCalEntry* aCalEntry, const TDesC& aGeoLat, const TDesC& aGeoLong)
	{
	TLex	lex(aGeoLat);
	TReal	geoLat;
	User::LeaveIfError(lex.Val(geoLat));

	TLex	lex1(aGeoLong);
	TReal	geoLong;
	User::LeaveIfError(lex1.Val(geoLong));

	CCalGeoValue* geoValue = CCalGeoValue::NewL();
	CleanupStack::PushL(geoValue);

	TRAPD(err, geoValue->SetLatLongL(geoLat, geoLong));

	if ( KMinGEOLat > geoLat || geoLat > KMaxGEOLat || KMinGEOLong > geoLong || geoLong > KMaxGEOLong )
		{
		INFO_PRINTF2(_L("SetLatLongL Error Code %d"), err);
		TESTL( err == KErrArgument );
		INFO_PRINTF1(_L("Geo Property Not Modified, due to invalid data"));
		}
	else
		{
		INFO_PRINTF2(_L("SetLatLongL Error Code %d"), err);
		TESTL( err == KErrNone );
		INFO_PRINTF1(_L("Geo Property Modified"));
		}

	if ( err == KErrNone)
		{
		TRAP( err, aCalEntry->SetGeoValueL(*geoValue) );

		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetGeoValueL Error Code %d"), err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}

	CleanupStack::PopAndDestroy(geoValue);
	}

/** To Modify the time transparency Status
@param	aCalEntry Pointer to the CCalEntry
@param	aTrans The time transparency status to change to
*/
void CTestCalInterimApiUpdateEntry::ModifyTransL(CCalEntry* aCalEntry, const TInt& aTrans)
	{
	TRAPD( err, aCalEntry->SetTimeTransparencyL(CCalEntry::TTransp(aTrans)) );

	// if aTrans is less than 0 (0 is the lowest possible transparency value)
	// then SetTimeTransparency should return KErrArgument
	if ( aTrans < 0 )
		{
		INFO_PRINTF2(_L("SetTimeTransparencyL Error Code %d"), err);
		TESTL( err == KErrArgument );
		INFO_PRINTF1(_L("Time Transparency Status Not Modified, due to invalid data"));
		}
	else
		{
		INFO_PRINTF2(_L("SetTimeTransparencyL Error Code %d"), err);
		TESTL( err == KErrNone );
		INFO_PRINTF1(_L("Time Transparency Status Modified"));
		}
	}

/**
Creates an entry view from a version 9.1 or version 9.2 calendar file.
Sets test step to EFail if the time taken is more than the maximum time
limit provided in the .ini file.
@param aMaxTimeLimit Maximum time limit allowed for this operation.
*/
void CTestCalInterimApiUpdateEntry::PerformanceTestEntryViewL(TReal32& aMaxTimeLimit)
	{

	// Starts recording time
	TTime	startTime;
	TTime	endTime;
	startTime.HomeTime();

	// Creates an entry view for v9.1 or v9.2 calendar file.
	if ( iEntryView )
		{
		delete iEntryView;
		iEntryView = NULL;
		}

	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	// Stops recording time
	endTime.HomeTime();

	if ( TestStepResult() == EPass )
		{
		if ( !ComputeTimeDifference(startTime, endTime, aMaxTimeLimit) )
			{
			SetTestStepResult(EFail);
			}
		}
	}

/** Set the datas received from the ini file and Update the Parent/ child entry's attachment
@param	aCalAttachment Pointer to the CCalAttachment
@param	aEntry Pointer to the CCalEntry
*/
void CTestCalInterimApiUpdateEntry::SetAttachmentDataL(CCalAttachment* aCalAttachment, CCalEntry& aEntry)
	{
	// Update the MIME type of the attachment
	if ( iAttachProperties.iMimeType != KNullDesC16() )
		{
		HBufC8*	mimeType = HBufC8::NewLC((iAttachProperties.iMimeType).Length());
		mimeType->Des().Copy(iAttachProperties.iMimeType);
		aCalAttachment->SetMimeTypeL(mimeType->Des());
		CleanupStack::PopAndDestroy(mimeType);
		}

	// Update the name of the attachment
	if ( iAttachProperties.iNewLabel != KNullDesC16() )
		{
		aCalAttachment->SetLabelL(iAttachProperties.iNewLabel);
		}

	if ( iAttachProperties.iHasClearAttribute )
		{
		aCalAttachment->ClearAttribute((CCalAttachment::TAttributes)iAttachProperties.iClearAttribute);
		}

	// Update the attribute of the attachment
	if ( iAttachProperties.iHasAttributes )
		{
		aCalAttachment->SetAttribute((CCalAttachment::TAttributes)iAttachProperties.iAttribute);
		}

	// Create CID attachment and add it to the entry
	if ( iAttachProperties.iCreateContentId != KNullDesC16() )
		{
		HBufC8*	contentData = HBufC8::NewLC(iAttachProperties.iCreateContentId.Length());
		contentData->Des().Copy(iAttachProperties.iCreateContentId);
		CCalAttachment*	attachment = CCalAttachment::NewFileByContentIdL(contentData->Des());
		CleanupStack::PushL(attachment);
		aEntry.AddAttachmentL(*attachment);
		CleanupStack::Pop(attachment);
		CleanupStack::PopAndDestroy(contentData);
		}

	CCalAttachmentFile*	attachmentFile = aCalAttachment->FileAttachment();
	CleanupStack::PushL(attachmentFile);

	if ( attachmentFile )
		{
		// Update the drive to store the attachment
		if( iAttachProperties.iDrive != KNullDesC16() )
			{
			attachmentFile->SetDriveL(iAttachProperties.iDrive);
			}

		// Update the last modified date and time for the attachment
		if ( iAttachProperties.iLastModifiedDate != KNullDesC16() )
			{
			attachmentFile->SetLastModifiedTimeUtc(iAttachProperties.iLastModifiedDate);
			}

		if ( iAttachProperties.iContentId != KNullDesC16() )
			{
			HBufC8*	contentId = HBufC8::NewLC(iAttachProperties.iContentId.Length());
			contentId->Des().Copy(iAttachProperties.iContentId);
			attachmentFile->SetContentIdL(contentId->Des());
			CleanupStack::PopAndDestroy(contentId);
			}
		}
	CleanupStack::Pop(attachmentFile);
	}

/**
OOM tests for Update Entry
@param aEntriesToStore Array containing the entries to be updated.
@param aEntriesStored Variable to store the number of updated entries
*/
void CTestCalInterimApiUpdateEntry::OOMTestForUpdateEntryL(RPointerArray<CCalEntry> aEntriesToStore, TInt& aEntriesStored)
	{
	TInt tryCount = 1;
	FOREVER
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);

		TRAPD(err, iEntryView->UpdateL(aEntriesToStore, aEntriesStored));

		if ( err == KErrNone )
			{
			__UHEAP_RESET;
			INFO_PRINTF1(_L("Memory allocation testing for UpdateEntryL is done"));
			INFO_PRINTF1(KInfoUpdateMessage);
			TESTL( iLocalUid == aEntriesToStore[0]->LocalUidL() );
			break;
			}
		if ( err != KErrNoMemory )
			{
			SetTestStepResult(EFail);
			break;
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		tryCount++;
		}
	INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
	}

/**
 * Constructor initializes the data members. Also sets the Test Step Name.
 *
 */
CTestCalInterimApiUpdateAllInstances::CTestCalInterimApiUpdateAllInstances()
:	CTestCalInterimApiSuiteStepBase()
	{
	SetTestStepName(KTestCalInterimApiUpdateAllInstances);
	}

/**
 * Destructor
 */
CTestCalInterimApiUpdateAllInstances::~CTestCalInterimApiUpdateAllInstances()
	{
	delete iEntryView;
	delete iEntryIterator;
	delete iGuidList;
	}

/**
 * Allocates necessary entities like Instance View, An Entry Iterator etc...
 */
TVerdict CTestCalInterimApiUpdateAllInstances::doTestStepPreambleL()
	{
	CTestCalInterimApiSuiteStepBase::doTestStepPreambleL();
	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	iEntryIterator=CCalIter::NewL(GetSession());
	iGuidList = new (ELeave) CDesC8ArrayFlat(4);
	return TestStepResult();
	}


TVerdict CTestCalInterimApiUpdateAllInstances::doTestStepL()
	{
	UpdateAllInstancesL();
	return TestStepResult();
	}


/**
* Update all calendar instances
* Any further iterator operations should result in KErrNotFound
*/
void CTestCalInterimApiUpdateAllInstances::UpdateAllInstancesL()
	{
	PopulateEntriesGuidListL();

	const TUint KGuidCount =  iGuidList->Count();
	for (TInt j(0); j < KGuidCount; ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
		iEntryView->FetchL(iGuidList->MdcaPoint(j), calEntries);

		const TUint KCalendarEntriesSpecificToGuid 	= calEntries.Count();
		if(KCalendarEntriesSpecificToGuid != 0)
			{
			RPointerArray<CCalEntry> updateArray;
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &updateArray));
			TInt success = 0;
			TInt err = KErrNone;
			CCalEntry* calEntry = calEntries[0];
			CleanupStack::PushL(calEntry);
			updateArray.Append(calEntry);
			calEntry->SetLastModifiedDateL();

			TBool updateUsingStoreApi = EFalse;
			GetBoolFromConfig(ConfigSection(), KUpdateInstancesUsingStoreApi, updateUsingStoreApi);

			if(updateUsingStoreApi)
				{
				TRAP(err, iEntryView->StoreL(updateArray, success)); // Parent Entry update using StoreL Api
				}
			else
				{
				TRAP(err, iEntryView->UpdateL(updateArray, success)); // Parent Entry update using UpdateL Api
				}

			if(err != KErrNone)
				{
				ERR_PRINTF2(KErrDuringUpdateInstancesOperation,err);
				SetTestStepResult(EFail);
				}

			if(KCalendarEntriesSpecificToGuid > 1)
				{
				for(TInt i(1); i < (KCalendarEntriesSpecificToGuid); ++i)
					{
					updateArray.Reset();
					calEntry = calEntries[i];
					updateArray.Append(calEntry);
					TRAP(err, iEntryView->StoreL(updateArray, success)); // Child Entries update
					if(err != KErrNone)
						{
						ERR_PRINTF2(KErrDuringUpdateInstancesOperation,err);
						SetTestStepResult(EFail);
						}
					}
				}
			CleanupStack::Pop(2, &updateArray); // calEntry, updateArray
			updateArray.Reset();
			}
		CleanupStack::PopAndDestroy(); // calEntries
		}
	}

/**
* Populates a CDescArray with guids of all the entries in the Calendar file
*/
void CTestCalInterimApiUpdateAllInstances::PopulateEntriesGuidListL()
	{
	TPtrC8	uid(iEntryIterator->FirstL());

	while(KNullDesC8()!=uid)
		{
		iGuidList->AppendL(uid);
		uid.Set(iEntryIterator->NextL());
		}
	}
