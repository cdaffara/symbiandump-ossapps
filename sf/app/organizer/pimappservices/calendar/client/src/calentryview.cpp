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

#include <calentryview.h>

#include "calentryviewimpl.h"

CCalEntryView::CCalEntryView()
	{
	}
	
/** The destructor frees all resources owned by the entry view, prior to its destruction.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalEntryView::~CCalEntryView()
	{
	delete iImpl;
	}

void CCalEntryView::ConstructL(CCalSession& aCalSession, MCalProgressCallBack* aProgressCallBack)
	{
	iImpl = CCalEntryViewImpl::NewL(aCalSession,aProgressCallBack);
	}
	
/** Allocates and constructs an entry view, with a reference to a state callback object.
@param aSession The handle to the calendar session.
@param aProgressCallBack Provides a callback function which notifies the user when the index has been built.
@return A pointer to the new entry view object.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalEntryView* CCalEntryView::NewL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack)	
	{
	CCalEntryView* self = new (ELeave) CCalEntryView();
	CleanupStack::PushL(self);
	self->ConstructL(aSession,&aProgressCallBack);
	CleanupStack::Pop(self);
	return self;
	}

/* Allocates and constructs an entry view.
@param aSession The handle to the calendar session.
@return A pointer to the new entry view object.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalEntryView* CCalEntryView::NewL(CCalSession& aSession)	
	{
	CCalEntryView* self = new (ELeave) CCalEntryView();
	CleanupStack::PushL(self);
	self->ConstructL(aSession, NULL);
	CleanupStack::Pop(self);
	return self;
	}

/** Fetches all CCalEntry objects related to the Uid.
The first entry in the array will always be the parent entry.
see CCalEntry for definition of parent entry.
@param aUid The Uid of the entries to fetch.
@param aCalEntryArray On return, this contains an array of related CCalEntry objects.
@see CCalEntry
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability ReadUserData
*/	
EXPORT_C void CCalEntryView::FetchL(const TDesC8& aUid, RPointerArray<CCalEntry>& aCalEntryArray) const
	{
	iImpl->FetchL(aUid, aCalEntryArray);
	}
	
/** Fetches a CCalEntry object related to the cal unique id.
@param aCalLocalId The cal id of the entry to fetch.
@return The entry that has been fetched, this will be NULL if there are no entries with the cal unique id
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability ReadUserData
*/	
EXPORT_C CCalEntry* CCalEntryView::FetchL(TCalLocalUid aCalLocalId) const
	{
	return iImpl->FetchL(aCalLocalId);
	}
	
/** Gets all the modified calendar local ids in the file since a specified date
@param aTime The time to get the ids since.
@param aIds On return will contain all the calendar local ids.
@publishedAll
@released
@capability ReadUserData
*/
EXPORT_C void CCalEntryView::GetIdsModifiedSinceDateL(const TCalTime& aTime, RArray<TCalLocalUid>& aIds) const
	{
	iImpl->GetIdsModifiedSinceDateL(aTime, aIds);
	}
	
/** Stores entries in the calendar file.
@param aCalEntryList The list of entries to be stored.
@param aNumSuccessfulEntry On return, this contains the number of entries which were successfully stored.
@leave KErrAccessDenied if one of these entries contains a file attachment with a file handle that does not 
have the EFileWrite mode.
@leave KErrBadHandle if one of these entries contains a file attachment with a file handle that does not belong
to a shared protected file server session.
@leave KErrArgument if one of these entries contains a file attachment which has a content ID but no binary data or file handle.
@leave KErrArgument if one of these entries has an invalid repeat rule. Possible causes are:
- the start time is later or equal to the until time;
- the interval is less than 1; 
- the repeat rule has no data to indicate when it should repeat, e.g. a weekly repeat with no weekdays set.
@leave KErrArgument if one of these entries is a modifying entry with a different time mode (fixed / floating) to the original entry.
@leave KErrArgument if one of these entries is a modifying entry with a different type (e.g. appt / todo) to the original entry.
@leave KErrArgument if one of these entries is a modifying entry but the original entry is not in the calendar store.
@leave KErrArgument if one of these entries is a repeating modifying entry with a recurrence range of CalCommon::EThisOnly.
@leave KErrNotFound if one of these entries is a modifying entry with a non-existent recurrence ID.
@leave KErrNotSupported if one of these entries is a repeating modifying entry, but the original entry already has a repeating modifying entry.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@leave KErrNotReady If an attachment file belonging to an entry is on a drive where the media has been removed.
Otherwise any of the system-wide error codes.
@publishedAll
@released
@capability WriteUserData
*/
EXPORT_C void CCalEntryView::StoreL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry)
	{
	iImpl->StoreL(aCalEntryList, aNumSuccessfulEntry);
	}
	
/** Updates entries in the calendar file.
@param aCalEntryList The list of entries to be updated.
@param aNumSuccessfulEntry On return, this contains the number of entries which were successfully updated.
@publishedAll
@released
@capability WriteUserData
@leave KErrArgument If aCalEntryList contains a CCalEntry that is not the parent. see CCalEntry for definition of a parent entry.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@leave KErrNotReady If an attachment file belonging to an entry is on a drive where the media has been removed.
@see CCalEntry
*/
EXPORT_C void CCalEntryView::UpdateL(const RPointerArray<CCalEntry>& aCalEntryList, TInt& aNumSuccessfulEntry)
	{
	iImpl->UpdateL(aCalEntryList, aNumSuccessfulEntry);
	}
		
/** Deletes an entry from the calender file.
If this a parent entry all it's children will also be deleted.
see CCalEntry for definition of parent entry.
@param aCalEntry The entry to delete.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability WriteUserData
*/
EXPORT_C void CCalEntryView::DeleteL(const CCalEntry& aCalEntry)
	{
	iImpl->DeleteL(aCalEntry);
	}
	
/** Deletes multiple entries from the calendar file.
If this function leaves, then no entries have been deleted.
@param aUidList A list of UIDs to delete.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability WriteUserData
*/
EXPORT_C void CCalEntryView::DeleteL(const CDesC8Array& aUidList)
	{
	iImpl->DeleteL(aUidList);
	}

/** Deletes a subset of the model's entries.
The entries affected are those which occur within the time range specified by aCalTimeRange.

This method will lock the calendar file.  Any request made, by any client, before this has completed will leave with KErrLocked.

@param aCalTimeRange The Time Range for the operation.
@param aFilter Identifies which entry types should be included in the operation.
@param aProgressCallBack Reference to a callback object which reports the progress of the operation.
@publishedAll
@released
*/	
EXPORT_C void CCalEntryView::DeleteL(const CalCommon::TCalTimeRange& aCalTimeRange, CalCommon::TCalViewFilter aFilter, MCalProgressCallBack& aProgressCallBack)
	{
	iImpl->DeleteL(aCalTimeRange,aFilter,aProgressCallBack);
	}
	
/** Deletes multiple entries from the calendar file.
@param aCalLocalIds A list of calendar local ids to delete.
@param aNumSuccessfulDeleted The number of local ids that have been successfully processed.
@leave KErrNotReady If the calendar file is on a drive where the media has been removed.
@publishedAll
@released
@capability WriteUserData
*/	
EXPORT_C void CCalEntryView::DeleteL(const RArray<TCalLocalUid>& aIds, TInt& aNumSuccessfulDeleted)
	{
	iImpl->DeleteL(aIds, aNumSuccessfulDeleted);
	}
