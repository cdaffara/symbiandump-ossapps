/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Definition file for AgendaUtilPrivate class.
*
*/

// System includes
#include <hbextendedlocale.h>
#include <calsession.h>
#include <calinstance.h>
#include <CalenImporter>
#include <CalenInterimUtils2>
#include <calentry.h>
#include <calrrule.h>
#include <e32math.h>
#include <caluser.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <f32file.h>
#include <s32file.h>
#include <asshddefs.h>

// User includes
#include <agendaentry.h>
#include "agendautil_p.h"
#include "agendaentry_p.h"
#include "CleanupResetAndDestroy.h"

// Constants
//recognition string for ical
_LIT8(KVersionICal,		"VERSION:2.0");
//recognition string for vcal
_LIT8(KVersionVCal,		"VERSION:1.0");
const TInt KReadDataAmount = 256;
const TInt KNoOfDaysInWeek = 7;
static const int startDateArray[2] = { 1900, 1};
static const int endDateArray[2] = { 2100, 1};

AgendaUtilPrivate::AgendaUtilPrivate(AgendaUtil* parent)
:q(parent)
{
	mEntryViewCreated = false;
	mInstanceViewCreated = false;
	mIsDeleting = false;
	mIsEntryViewBeingDeleted = false;
	mIsInstanceViewBeingDeleted = false;
	prepareSession();
}

bool AgendaUtilPrivate::prepareSession()
{
	if (iCalSession && mEntryViewCreated && mInstanceViewCreated)
		{
		// Everything is ready, nothing to be done
		return true;
		}

	// Create the cal session if not already there.
	if (!iCalSession)
		{
		TRAP(iError, iCalSession = CCalSession::NewL(););
		if (!iCalSession)
			{
			return false;
			}

		const TDesC& file = iCalSession->DefaultFileNameL();
		TRAPD(iError, iCalSession->OpenL(file));
		if (iError == KErrNotFound)
			{
			iCalSession->CreateCalFileL(file);
			iCalSession->OpenL(file);
			}
		else if (iError != KErrNone)
			{
			return false;
			}

		// Subscribe for notification on changes in the opened session (file).
		TCalTime startDate;
		TCalTime endDate;
		TDateTime startTime = TDateTime(
				startDateArray[0], static_cast<TMonth>(startDateArray[1]),
				0, 0, 0, 0, 0);
		TDateTime endTime = TDateTime(
				endDateArray[0], static_cast<TMonth>(endDateArray[1]),
				0, 0, 0, 0, 0);

		startDate.SetTimeUtcL(startTime);
		endDate.SetTimeUtcL(endTime);
		CalCommon::TCalTimeRange searchTimeRange(startDate, endDate);

		CCalChangeNotificationFilter* filter = 0;
		filter = CCalChangeNotificationFilter::NewL(
				MCalChangeCallBack2::EChangeEntryAll, true, searchTimeRange);

		iCalSession->StartChangeNotification(*this, *filter);

		// Cleanup.
		delete filter;
		}

	// First construct the CCalInstanceView if not already available.
	// The CCalEntryView is constructed in CompletedL. Instance view
	// is created before entry view since entry view is required only
	// when editing/saving any entry. So we will construct it later
	if (!iCalInstanceView) {
	    TRAP (iError, iCalInstanceView = CCalInstanceView::NewL(*iCalSession,
	                                                           *this);)
	}
	
	// All the requests have been made
	return true;
}

AgendaUtilPrivate::~AgendaUtilPrivate()
{
    if (iCalEntryView && !mIsEntryViewBeingDeleted) {
        mIsEntryViewBeingDeleted = true;
        delete iCalEntryView;
        iCalEntryView = NULL;
    }
    
    if (iCalInstanceView && !mIsInstanceViewBeingDeleted) {
        mIsInstanceViewBeingDeleted = true;
        delete iCalInstanceView;
        iCalInstanceView = NULL;
    }
	
	if (iCalSession) {
		iCalSession->StopChangeNotification();
	}
	delete iCalSession;
}

void AgendaUtilPrivate::Completed(TInt aError)
{
	iError = aError;

	if (mIsDeleting) {
		// If deletion was in progress, then it is completed now
		// emit the signal to the clients.
		mIsDeleting = false;
		emit q->entriesDeleted(iError);
	}

	if (KErrNone != iError) {
		// Something has gone wrong, return
		if (iCalEntryView && !mIsEntryViewBeingDeleted) {
            mIsEntryViewBeingDeleted = true;
            delete iCalEntryView;
            iCalEntryView = NULL;
            mIsEntryViewBeingDeleted = false;
		}
		if (iCalInstanceView && !mIsInstanceViewBeingDeleted) {
            mIsInstanceViewBeingDeleted = true;
            delete iCalInstanceView;
            iCalInstanceView = NULL;
            mIsInstanceViewBeingDeleted = false;
		}
		return;
	}

	if (iCalInstanceView && !mInstanceViewCreated) {
	    // Instance view is now created.
	    mInstanceViewCreated = true;
	    emit q->instanceViewCreationCompleted(iError);
	    // Start with the construction of entry view
		if (!iCalEntryView) {
		    TRAP (iError,
		          iCalEntryView = CCalEntryView::NewL(*iCalSession, *this);
		    );
		}
	} else if(iCalEntryView && !mEntryViewCreated) {
	    // Entry view is now constructed
		mEntryViewCreated = true;
		emit q->entryViewCreationCompleted(iError);
	}
}

void AgendaUtilPrivate::CalChangeNotification(
		RArray<TCalChangeEntry>& aChangeItems)
{
	QList<ulong> ids;
	for (int i = 0; i < aChangeItems.Count(); i++) {
		ids.append(aChangeItems[i].iEntryId);
	}

	emit q->entriesChanged(ids);
}

/*!
	To store the new entry or update the entry in the Calendar db.

	\param entry The entry to be added/updated.
	\param range The recurrence range of entry.
	\param instanceOriginalDateTime The start time of the original instance..
	\return ulong The local uid of the entry added/updated in the db.
*/
ulong AgendaUtilPrivate::store(
		AgendaEntry &entry, AgendaUtil::RecurrenceRange range,
		QDateTime &instanceOriginalDateTime)
{
	// Will be filled with the lUID of the new entry created.
	TCalLocalUid localUid = 0;

	// First check if the session to the calendar database is prepared or not.
	if (!mInstanceViewCreated) {
		// Something went wrong
		return localUid;
	}
	CCalEntry *calEntry = 0;
	
	TRAP(
			iError,
			// Get the global uid.
			CCalenInterimUtils2* calenInterimUtils2 = CCalenInterimUtils2::NewL();
			bool isChild = !(entry.recurrenceId().isNull());
			
			//Flag to decide whether entry is added or updated
			bool entryAdded = false;
			
			// if the entry id is zero means need to create a new entry
			if ((AgendaUtil::ThisAndAll == range) && (0 == entry.id())) {
		
				entryAdded = true;
				
				HBufC8* globalUid = calenInterimUtils2->GlobalUidL();
				CleanupStack::PushL(globalUid);
				calEntry
						= CCalEntry::NewL(
										  static_cast<CCalEntry::TType> (entry.type()),
										  globalUid,
										  static_cast<CCalEntry::TMethod> (entry.method()),
										  0);
		
				CleanupStack::Pop(globalUid);
			} else if (((AgendaUtil::ThisOnly == range) && isChild)
					|| ((AgendaUtil::ThisAndAll == range) && (entry.id() > 0))) {
				
				// Updating the entry/Exceptional entry
				calEntry = iCalEntryView->FetchL(entry.id());
				
				CleanupStack::PushL(calEntry);
				// Repeat rule 
				TCalRRule rrule;
				TBool isRepeating = calEntry->GetRRuleL( rrule );
		
				// If the repeat rule is cleared then Clear the Repeat rule from CCalEntry
				if ((AgendaUtil::ThisAndAll == range) 
						&& isRepeating && !(entry.isRepeating())) {
					calEntry->ClearRepeatingPropertiesL();
				}
				CleanupStack::Pop(calEntry);	
			} else {
				// Creating a exceptional entry
				if ((AgendaUtil::ThisOnly == range) && !isChild) {
					// Get the entry corresponding to the id.
					CCalEntry *parentEntry = iCalEntryView->FetchL(entry.id());
					CleanupStack::PushL(parentEntry);
					// We are creating an exception, hence get the global Uid
					HBufC8* guid = parentEntry->UidL().AllocLC();
					
					
					
					// create new (child) entry
					// Use original instance time for recurrenceID as this entry hasn't got one.
					TCalTime originalCalTime;
					TDateTime originalDateTime(instanceOriginalDateTime.date().year(),
								TMonth(instanceOriginalDateTime.date().month() - 1),
								instanceOriginalDateTime.date().day() -1,
								0,
								0,
								0,
								0);
					
					TTime originalDateTimeTTime(originalDateTime);
					// Use floating time for non-timed entries so that
					// the time will be same regardless of the timezone
					if(entry.isTimedEntry()) {
					    originalCalTime.SetTimeLocalL(originalDateTimeTTime);					    					    
					}else {
					    originalCalTime.SetTimeLocalFloatingL(originalDateTimeTTime);
					}
					// create the new child now
					calEntry = CCalEntry::NewL(parentEntry->EntryTypeL(), 
											   guid,
											   parentEntry->MethodL(),
											   parentEntry->SequenceNumberL(),
											   originalCalTime,
											   CalCommon::EThisOnly);
		
					// reset local UID and clear the repeat rule for exceptional entry
					calEntry->SetLocalUidL(TCalLocalUid(0));
					calEntry->ClearRepeatingPropertiesL();
					
					CleanupStack::Pop(guid);
					CleanupStack::PopAndDestroy(parentEntry);
					
					// clear repeat rule properties
					AgendaRepeatRule repeatrule;
					entry.setRepeatRule(repeatrule);
				}
		
			}
			
			// Converting agenda entry to CCalEntry to store it to database
			createCCalEntryFromAgendaEntry(entry, *calEntry);
			
			calenInterimUtils2->StoreL(*iCalEntryView, *calEntry, true);
			localUid = calEntry->LocalUidL();
			
			// Emit signal upon successful creation of entry.
			if (0 < localUid) {
				// if creating new entry then emit signal entryAdded else entryUpdated
				if (entryAdded) {
				emit q->entryAdded(localUid);
				} else {
					q->entryUpdated(localUid);
				}
			}
			
			delete calenInterimUtils2;
			delete calEntry;
	)
	return localUid;

}

/*!
	Clones the `entry' passed in the argument and saves it as type `type'.

	\param entry Entry which should be used for cloning.
	\param type The new type of the entry.
	\return ulong The local UID of the new entry.

	\sa deleteEntry()
 */
ulong AgendaUtilPrivate::cloneEntry(
		const AgendaEntry &entry, AgendaEntry::Type type)
{
	// First prepare the session with agenda server.
	if (!mInstanceViewCreated) {
		// Something went wrong.
		return 0;
	}

	if (entry.isNull()
			|| type == AgendaEntry::TypeUnknown) {
		return 0;
	}

	// Will be filled with the lUID of the new entry created.
	TCalLocalUid localUid = 0;
	int success = 0;
	CCalEntry *originalEntry = 0;
	HBufC8* globalUid = 0;

	// Get the stored entry first.
	TRAP(
			iError,

			originalEntry = iCalEntryView->FetchL(entry.id());
	)

	if (!originalEntry) {
		return 0;
	}

	// Now save the GUID of the saved entry.
	TRAP(
			iError,
			globalUid = originalEntry->UidL().AllocL();
	)

	delete originalEntry;

	// Now start cloning and create a new entry.
	if (AgendaEntry::TypeNote == type) {
		TRAP(
				iError,

				RPointerArray<CCalEntry> entryArray;
				CleanupClosePushL(entryArray);

				// Construct a CCalEntry object and start filling the details.
				CCalEntry* newEntry = 0;
				newEntry = CCalEntry::NewL(
						static_cast<CCalEntry::TType>(type),
						globalUid,
						static_cast<CCalEntry::TMethod>(entry.method()),
						0);

				// Add description.
				TPtrC description(reinterpret_cast<const TUint16*>(
						entry.description().utf16()));
				newEntry->SetDescriptionL(description);

				// Set the favourite property.
				newEntry->SetFavouriteL(entry.favourite());

				// Finally set the entry to the database using the entry view.
				entryArray.AppendL(newEntry);
				iCalEntryView->StoreL(entryArray, success);
				localUid = newEntry->LocalUidL();

				// Cleanup.
				CleanupStack::PopAndDestroy(&entryArray);
		)
	} else {
		TRAP(
				iError,

				RPointerArray<CCalEntry> entryArray;
				CleanupClosePushL(entryArray);

				// Construct a CCalEntry object and start filling the details.
				CCalEntry* newEntry = 0;
				newEntry = CCalEntry::NewL(
						static_cast<CCalEntry::TType>(type),
						globalUid,
						static_cast<CCalEntry::TMethod>(entry.method()),
						0);

				// Add the summary.
				if (!entry.summary().isNull()) {
					TPtrC summary(reinterpret_cast<const TUint16*>(
							entry.summary().utf16()));
					newEntry->SetSummaryL(summary);
				}

				// Set the entry Start/End Date and time.
				QDate date = entry.startTime().date();
				QTime time = entry.startTime().time();

				TDateTime startDateTime(
						date.year(), static_cast<TMonth>(date.month() - 1),
						date.day() - 1, time.hour(), time.minute(), 0, 0);
				TTime entryStartTime(startDateTime);
				TCalTime calStartTime;

				date = entry.endTime().date();
				time = entry.endTime().time();

				TDateTime endDateTime(
						date.year(), static_cast<TMonth>(date.month() - 1),
						date.day() - 1, time.hour(), time.minute(), 0, 0);
				TTime entryEndTime(endDateTime);
				TCalTime calEndTime;
				
				bool isNonFloating = false;
                // Use floating time for non-timed entries so that
                // the time will be same regardless of the timezone
				if(type == AgendaEntry::TypeAppoinment|| type == AgendaEntry::TypeReminder) {
				    calStartTime.SetTimeLocalL(entryStartTime); 
				    calEndTime.SetTimeLocalL(entryEndTime);
				    isNonFloating = true;
				}else {
				    calStartTime.SetTimeLocalFloatingL(entryStartTime);
				    calEndTime.SetTimeLocalFloatingL(entryEndTime); 
				    isNonFloating = false;
				}
				newEntry->SetStartAndEndTimeL(calStartTime, calEndTime);

				// Add attendees to the entry.
				addAttendeesToEntry(entry.d->m_attendees, *newEntry);

				// Add categories to the entry.
				addCategoriesToEntry(entry.d->m_categories, *newEntry);

				// Add description to the entry.
				TPtrC description(reinterpret_cast<const TUint16*>(
						entry.description().utf16()));
				newEntry->SetDescriptionL(description);

				// Set the favourite property.
				newEntry->SetFavouriteL(entry.favourite());

				// Add Alarm to the entry.
				AgendaAlarm alarm = entry.alarm();
				if (!alarm.isNull()) {
					setAlarmToEntry(alarm, *newEntry);
				}

				// Set the priority.
				int priority = entry.priority();
				if (entry.priority() != -1) {
					newEntry->SetPriorityL(priority);
				}

				// Set the location.
				if (!entry.location().isNull()) {
					TPtrC location(reinterpret_cast<const TUint16*>(
							entry.location().utf16()));
					newEntry->SetLocationL(location);
				}

				// Set the repeat type if applicable.
				if (AgendaRepeatRule::InvalidRule
						!= entry.repeatRule().type()) {
					AgendaRepeatRule agendaRepeatRule = entry.repeatRule();
					TCalRRule repeatRule = 
							createTCalRRuleFromAgendaRRule(agendaRepeatRule, isNonFloating);
					newEntry->SetRRuleL(repeatRule);
				}

				// Save the status of the entry.
				newEntry->SetStatusL((CCalEntry::TStatus) entry.status());
				newEntry->SetLastModifiedDateL();
				
				// Save the geo value if any
				AgendaGeoValue entryGeoValue = entry.geoValue();
				if (!entryGeoValue.isNull()) {
					CCalGeoValue* geoValue = CCalGeoValue::NewL();
					double latitude;
					double longitude;
					entryGeoValue.getLatLong(latitude, longitude);
					
					// set the values to symbian geo value
					geoValue->SetLatLongL(latitude, longitude);
					
					// set it to CCalentry
					newEntry->SetGeoValueL(*geoValue);
					delete geoValue;
				}
								
				// Finally set the entry to the database using the entry view.
				entryArray.AppendL(newEntry);
				iCalEntryView->StoreL(entryArray, success);
				localUid = newEntry->LocalUidL();

				// Cleanup.
				CleanupStack::PopAndDestroy(&entryArray);
		)
	}
	// Emit signal upon successful creation of entry.
	if (0 < localUid && 1 == success) {
		emit q->entryAdded(localUid);
	}
	return localUid;
}

/*!
	Deletes a given entry with `id'.

	\param id The id of the entry to be deleted.
	\return bool true if deletion successful, false otherwise.
 */
bool AgendaUtilPrivate::deleteEntry(ulong id)
{
	// First prepare the session with agenda server.
	if (!mInstanceViewCreated) {
		// Something went wrong.
		return false;
	}
	int success = 0;

	// Now delete the entry with agenda server.
	TRAP(
			iError,
			RArray<TCalLocalUid> localIds;
			CleanupClosePushL(localIds);

			localIds.AppendL(id);
			iCalEntryView->DeleteL(localIds, success);
			CleanupStack::PopAndDestroy(&localIds);
	)

	// Emit the signal to notify the deletion of entry.
	if (0 < success) {
		emit q->entryDeleted(id);
	}
	return (success != 0);
}

/*!
	Deletes a repeated entry given the recurrance range.

	\param entry
	\param range
	\return bool true if deletion successful, false otherwise.
 */
void AgendaUtilPrivate::deleteRepeatedEntry(
		AgendaEntry& entry,
		AgendaUtil::RecurrenceRange range)
{

	// First prepare the session with agenda server.
	if (!mInstanceViewCreated) {
		// Something went wrong.
		return;
	}

	TRAP(iError,

			if (range != AgendaUtil::ThisAndAll) {
				// Find the correct instance to be deleted from entry.
				CCalInstance* instance = findPossibleInstance(entry);
				if (instance) {
				    iCalInstanceView->DeleteL(instance,
				                              (CalCommon::TRecurrenceRange)range);
				}
			} else {
				// Unfortunately we can't pass the existing child instance
				// through to the InstanceView DeleteL function because even if
				// we pass in EThisAndAll, it only ever deletes the exception.
				// We'll have to fetch the parent then delete it via
				// the entry view.
				CCalEntry* calEntry = iCalEntryView->FetchL(entry.id());
				CleanupStack::PushL(calEntry);

				// Fetch the entries based on the global uid of the entry, as
				// the exceptional entries share the global uid with the parent
				// entry.
				RPointerArray<CCalEntry> entries;
				CleanupResetAndDestroyPushL(entries);
				iCalEntryView->FetchL(calEntry->UidL(), entries);
				if (entries.Count()) {
				    iCalEntryView->DeleteL(*entries[0]);
				}

				CleanupStack::PopAndDestroy(&entries);
				CleanupStack::PopAndDestroy();
			}
	)
	
	// Emit the signal to notify the deletion of entry.
	emit q->entryDeleted(entry.id());
}

/*!
	Fetches an AgendaEntry, given the id.

	\param id The (ulong) local uid the entry to be fetched.
	\return AgendaEntry The fetched entry.
 */
AgendaEntry AgendaUtilPrivate::fetchById(ulong id)
{
	AgendaEntry entry;

	// First check if the session with the calendar exists.
	if (!mInstanceViewCreated) {
		// Return empty AgendaEntry.
		return entry;
	}

	TRAP(
			iError,

			CCalEntry* calEntry = iCalEntryView->FetchL(id);

			// We fetch only if we have got a valid calendar entry.
			if (calEntry) {
				CleanupStack::PushL(calEntry);

				// We have a valid CCalEntry. We get the AgendaEntry
				// corresponding to that.
				entry = createAgendaEntryFromCalEntry(*calEntry);

				// Cleanup.
				CleanupStack::PopAndDestroy(calEntry);
			}
	)
	return entry;
}

/*!
	Returns a list of entry ids which match the given filter.

	\param filter The filter to be applied to get the entry ids.
	\return QList a list of entry ids.
 */
QList<ulong> AgendaUtilPrivate::entryIds(AgendaUtil::FilterFlags filter)
{
	QList<ulong> listOfIds;

	// First check if the session with agenda server exists.
	if (!mInstanceViewCreated) {
		// Return empty list.
		return listOfIds;
	}

	TRAP(
			iError,

			RPointerArray<CCalInstance> instanceList;
            CleanupResetAndDestroyPushL(instanceList);
			CalCommon::TCalViewFilter filters = filter;
			TCalTime startDateForInstanceSearch;
			TCalTime endDateForInstanceSearch;

			TDateTime startTime = TDateTime(
					startDateArray[0], static_cast<TMonth>(startDateArray[1]),
					0, 0, 0, 0, 0);

			TDateTime endTime = TDateTime(
					endDateArray[0], static_cast<TMonth>(endDateArray[1]),
					0, 0, 0, 0, 0);

			startDateForInstanceSearch.SetTimeLocalL(startTime);
			endDateForInstanceSearch.SetTimeLocalL(endTime);
			CalCommon::TCalTimeRange searchTimeRange(
					startDateForInstanceSearch, endDateForInstanceSearch);

			iCalInstanceView->FindInstanceL(
					instanceList, filters, searchTimeRange);

			TDateTime date;

			for (int iter = 0; iter < instanceList.Count(); iter++) {
				listOfIds.append(instanceList[iter]->Entry().LocalUidL());
			}

			// Cleanup.
			CleanupStack::PopAndDestroy(&instanceList);
	)

	return listOfIds;
}

/*!
	Retruns a list of entries matching a given filter.

	\param Filter to be applied to get the matching entries.
	\return QList a list of AgendaEntries matching the given filter.
 */
QList<AgendaEntry> AgendaUtilPrivate::fetchAllEntries(
		AgendaUtil::FilterFlags filter)
{
	QList<AgendaEntry> entryList;

	// First check if the session with agenda server exists.
	if (!mInstanceViewCreated) {
		// Return empty list.
		return entryList;
	}

	TRAP(
			iError,

			RPointerArray<CCalInstance> instanceList;
            CleanupResetAndDestroyPushL(instanceList);
			CalCommon::TCalViewFilter filters = filter;
			TCalTime startDateForInstanceSearch;
			TCalTime endDateForInstanceSearch;

			TDateTime startTime = TDateTime(
					startDateArray[0], static_cast<TMonth>(startDateArray[1]),
					0, 0, 0, 0, 0);

			TDateTime endTime = TDateTime(
					endDateArray[0], static_cast<TMonth>(endDateArray[1]),
					0, 0, 0, 0, 0);

			startDateForInstanceSearch.SetTimeLocalL(startTime);
			endDateForInstanceSearch.SetTimeLocalL(endTime);
			CalCommon::TCalTimeRange searchTimeRange(
					startDateForInstanceSearch,
					endDateForInstanceSearch);

			iCalInstanceView->FindInstanceL(
					instanceList, filters, searchTimeRange);

			TDateTime date;

			for (int iter = 0; iter < instanceList.Count(); iter++) {
				entryList.append(
						createAgendaEntryFromCalEntry(
								instanceList[iter]->Entry(),
								instanceList[iter]));
			}

			// Cleanup.
			CleanupStack::PopAndDestroy(&instanceList);
	)

	return entryList;
}

QList<AgendaEntry> AgendaUtilPrivate::fetchEntriesInRange(
		QDateTime rangeStart, QDateTime rangeEnd,
		AgendaUtil::FilterFlags filter)
{
    QList<AgendaEntry> entryList;
    if(!mInstanceViewCreated)
        {
            // return empty list
            return entryList;
        }

    TRAP(iError,

        RPointerArray<CCalInstance> instanceList;
        CleanupResetAndDestroyPushL(instanceList);
        CalCommon::TCalViewFilter filters = filter;
        TCalTime startDateForInstanceSearch;
        TCalTime endDateForInstanceSearch;

        TDateTime startTime(rangeStart.date().year(),
                TMonth(rangeStart.date().month() - 1),
                rangeStart.date().day() - 1,
                rangeStart.time().hour(),
                rangeStart.time().minute(),
                rangeStart.time().second(),
                rangeStart.time().msec());

        TDateTime endTime(rangeEnd.date().year(),
                TMonth(rangeEnd.date().month() - 1),
                rangeEnd.date().day() - 1,
                rangeEnd.time().hour(),
                rangeEnd.time().minute(),
                rangeEnd.time().second(),
                rangeEnd.time().msec());

        startDateForInstanceSearch.SetTimeLocalL(startTime);
        endDateForInstanceSearch.SetTimeLocalL(endTime);
        CalCommon::TCalTimeRange searchTimeRange(
                startDateForInstanceSearch,
                endDateForInstanceSearch);

        iCalInstanceView->FindInstanceL(instanceList, filters, searchTimeRange);

        for(TInt i = 0; i<instanceList.Count(); i++)
        {
            entryList.append(createAgendaEntryFromCalEntry(instanceList[i]->Entry(), instanceList[i]));
        }
        CleanupStack::PopAndDestroy(&instanceList);
    )

    return entryList;
}

void AgendaUtilPrivate::markDatesWithEvents(QDateTime rangeStart, 
	QDateTime rangeEnd,AgendaUtil::FilterFlags filter, QList<QDate>& dates)
{
	RPointerArray<CCalInstance> instanceList;
	
	if(!mInstanceViewCreated) {
	    // return empty list
	    return;
	}
	
	CleanupResetAndDestroyPushL(instanceList);
	CalCommon::TCalViewFilter filters = filter;
	TCalTime startDateForInstanceSearch;
	TCalTime endDateForInstanceSearch;

	TDateTime startTime(rangeStart.date().year(),
			TMonth(rangeStart.date().month() - 1),
			rangeStart.date().day() - 1,
			rangeStart.time().hour(),
			rangeStart.time().minute(),
			rangeStart.time().second(),
			rangeStart.time().msec());

	TDateTime endTime(rangeEnd.date().year(),
			TMonth(rangeEnd.date().month() - 1),
			rangeEnd.date().day() - 1,
			rangeEnd.time().hour(),
			rangeEnd.time().minute(),
			rangeEnd.time().second(),
			rangeEnd.time().msec());

	startDateForInstanceSearch.SetTimeLocalL(startTime);
	endDateForInstanceSearch.SetTimeLocalL(endTime);
	CalCommon::TCalTimeRange searchTimeRange(
			startDateForInstanceSearch,
			endDateForInstanceSearch);

	iCalInstanceView->FindInstanceL(instanceList, filters, searchTimeRange);

	// Parse thru the list and mark the dates which have events
	for (int i = 0; i < instanceList.Count(); i++) {
		CCalEntry::TType type = instanceList[i]->Entry().EntryTypeL();
		// Get the start time and end time of the events
		TCalTime startCalTime = instanceList[i]->StartTimeL();
		TCalTime endCalTime = instanceList[i]->EndTimeL();
		TDateTime startDateTime = startCalTime.TimeLocalL().DateTime();
		TDateTime endDateTime = endCalTime.TimeLocalL().DateTime();
		QDate startDate(startDateTime.Year(), startDateTime.Month()+1,
						startDateTime.Day() + 1);
        QDate endDate(endDateTime.Year(), endDateTime.Month()+1,
                        endDateTime.Day() + 1);
		if (type == CCalEntry::EEvent || type == CCalEntry::EAppt ||
				type == CCalEntry::EReminder) {
			if(endsAtStartOfDay(instanceList[i], endCalTime.TimeLocalL())) {
				TDateTime endDateTime = endCalTime.TimeLocalL().DateTime();
				// prevent problems with items ending tomorrow at 00:00
				endDateTime.SetMinute(endDateTime.Minute() - 1);
				TTime time(endDateTime);
				// If it is ending before the start of the grid 
				if (time <= startDateForInstanceSearch.TimeLocalL()) {
					continue;
				}
			}
        // Mark the required dates frm start date to end date
        int numOfDays = 0;
        //check if the start date of the entry is before the start day of the grid
        if(startDate < rangeStart.date()){
            if(endDate<=rangeEnd.date()){
                //if the end date of entry is lying in the grid ,
                //then mark the entry from start day of the grid to the end date of the entry
                numOfDays = rangeStart.date().daysTo(endDate);
            }
            else{
                //if end date of the entry is greater then the last date of grid, 
                //then mark all the date of the grid with the entry 
               numOfDays = rangeStart.daysTo(rangeEnd);
            }
            // Check if the event is all-day
            if (instanceList[i]->Entry().EntryTypeL() == CCalEntry::EEvent) {
                // no need to consider the date on which it ends
                // reduce days count by 1
                numOfDays--;
                }
            //start the entries from the first day of the grid
            for (int j = 0; j <= numOfDays; j++) {
                QDate date = rangeStart.date().addDays(j);
                if (date <= rangeEnd.date()) {
                    dates.append(date);
                } else {
                    break;
                }
            }
        }
        //if the start date of the entry is lying inside the grid
        else{
            if(endDate<=rangeEnd.date()){
                //if the end date of entry is lying in the grid ,
                //then mark the entry from start date of the entry to the end date of the entry
                numOfDays = startDate.daysTo(endDate);
            }
            else{
                //if end date of the entry is greater then the last date of grid, 
                //then mark all the date from start date of the entry to the end date of the grid 
                numOfDays = startDate.daysTo(rangeEnd.date()); 
            }
            // Check if the event is all-day
            if (instanceList[i]->Entry().EntryTypeL() == CCalEntry::EEvent) {
                // no need to consider the date on which it ends
                // reduce days count by 1
                numOfDays--;
                }
            for (int j = 0; j <= numOfDays; j++) {
                QDate date = startDate.addDays(j);
                if (date <= rangeEnd.date()) {
                    dates.append(date);
                } else {
                    break;
                }
            }   
        }
    } else if (type == CCalEntry::EAnniv) {
        if (startDate <= rangeEnd.date()) {
            dates.append(startDate);
        }
    } else if (type == CCalEntry::ETodo) {
            // if start time is less that today, then mark it for today
            if (startDate < QDate::currentDate()) {
                dates.append(QDate::currentDate());
            } else {
                dates.append(startDate);
            }
        }
    }
    CleanupStack::PopAndDestroy(&instanceList);
}

QList<AgendaEntry> AgendaUtilPrivate::createEntryIdListForDay( QDateTime day,
                            AgendaUtil::FilterFlags filter )
{
    QList<AgendaEntry> entryList;
    if(!mInstanceViewCreated) {
        // return empty list
        return entryList;
    }
    TCalTime dummy;
    CalCommon::TCalTimeRange dayRange( dummy, dummy );
    TRAP(iError,
        getDayRange(day, day, dayRange);
        RPointerArray<CCalInstance> instanceList;
        CleanupResetAndDestroyPushL(instanceList);
        CalCommon::TCalViewFilter filters = filter;
        // Check if the filter has todos also to be included
        if(filter & CalCommon::EIncludeIncompletedTodos)
            {
            // Show the incompleted todos till date only for the current date 
            if(QDate::currentDate() == day.date())
                {
                // Remove the todos from the filter and fetch it separately
                CalCommon::TCalViewFilter changedfilter = 
                                   filter ^ CalCommon::EIncludeIncompletedTodos;
                iCalInstanceView->FindInstanceL(
                                         instanceList, changedfilter, dayRange);
                // Get the range from minimum datetime till the today
                getDayRange(minTime(), day, dayRange);
                iCalInstanceView->FindInstanceL(
                        instanceList ,CalCommon::EIncludeIncompletedTodos | 
                        // only fetch the first instance for repeating to-dos!
                        CalCommon::EIncludeRptsNextInstanceOnly, 
                        dayRange);
                }
            else if(QDate::currentDate() < day.date())
                {
            	// Fetch all the instances including todos
            	// only if the day range is in future dates
                iCalInstanceView->FindInstanceL(
                                               instanceList, filters, dayRange);
                }
            else if(QDate::currentDate() > day.date())
				{
				// Remove todos from the filter
				CalCommon::TCalViewFilter changedfilter = 
								   filter ^ CalCommon::EIncludeIncompletedTodos;
            	// Fetch all the instances excluding todos
            	// only if the day range is in past dates
				iCalInstanceView->FindInstanceL(
										 instanceList, changedfilter, dayRange);
				}
            }
        else
            {
            iCalInstanceView->FindInstanceL(instanceList, filters, dayRange);
            }
        
        // In this list, check if any events are there that end at the start of the day
        // we should not show it again on next day
        TCalTime calTime;
        TDateTime dateTime(
                day.date().year(),
                static_cast<TMonth>(day.date().month() - 1),
                day.date().day() - 1, day.time().hour(),
                day.time().minute(), 0, 0);
        TTime time(dateTime);
        for (int i(0); i < instanceList.Count(); i++) {
            if ((endsAtStartOfDay(instanceList[i], time))) {
                // Previous day event has been found, we are not supposed to
                // show it on current day, hence remove it from the instance list
                CCalInstance *instance = instanceList[i];
                instanceList.Remove(i);
                delete instance;
                i--;
            }
        }
        
        // Sort the list
        sortInstanceList(instanceList);
        for(TInt i = 0; i<instanceList.Count(); i++)
            {
                entryList.append(createAgendaEntryFromCalEntry(
                                    instanceList[i]->Entry(), instanceList[i]));
            }
        CleanupStack::PopAndDestroy( &instanceList );
        )

    return entryList;
}

int AgendaUtilPrivate::importvCalendar(const QString& fileName, 
										AgendaEntry& entry)
{
	int success = -1 ;

	// First prepare the session with agenda server.
	if (!mInstanceViewCreated) {
		return success;
	}

	CCalenImporter* calImporter = CCalenImporter::NewL(*iCalSession);
	CleanupStack::PushL(calImporter);
	calImporter->SetImportMode(ECalenImportModeExtended);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TPtrC path(reinterpret_cast<const TUint16*> (fileName.utf16()));
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs, path, EFileRead));
	CleanupClosePushL(stream);

	TBuf8<KReadDataAmount> vcalData;
	stream.ReadL(vcalData, KReadDataAmount);

	//return the read stream back to the beginning for import
	stream.Source()->SeekL(MStreamBuf::ERead, EStreamBeginning, 0);

	TInt errImport(KErrNone);
	RPointerArray<CCalEntry> calEntryArray;
	CleanupClosePushL(calEntryArray);
	if (vcalData.FindF(KVersionVCal) == KErrNotFound) {
		//using the ICAl import API
		TRAP( errImport, calImporter->ImportICalendarL( stream, calEntryArray ) );
	} else if (vcalData.FindF(KVersionICal) == KErrNotFound) {
		//using the VCAL import API
		TRAP( errImport, calImporter->ImportVCalendarL( stream, calEntryArray ) );
	}
	success = errImport;
	if (errImport != KErrNone || calEntryArray.Count() == 0) {
		// the data was corrupt
		User::Leave(errImport);
	}

	//sets the local UID to 0 in imported entry
	calEntryArray[0]->SetLocalUidL(TCalLocalUid(0));
	AgendaEntry agendaEntry = createAgendaEntryFromCalEntry(*calEntryArray[0]);
	if (!agendaEntry.isNull()) {
		entry = agendaEntry;
	}

	CleanupStack::PopAndDestroy(4, calImporter);
	return success;
}

bool AgendaUtilPrivate::exportAsvCalendar(
		const QString& fileName, ulong calendarEntryId)
{
	// First prepare session with agenda server.
	if (!mInstanceViewCreated) {
		return false;
	}
	TRAP(
			iError,

			TPtrC path(reinterpret_cast<const TUint16*>(fileName.utf16()));
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			fs.MkDirAll(path);
			RFileWriteStream rs;
			User::LeaveIfError(rs.Replace(fs, path, EFileWrite));

			CCalEntry* entry = iCalEntryView->FetchL(calendarEntryId);
			CleanupStack::PushL(entry);

			RPointerArray<CCalEntry> calEntryArray;
			CleanupClosePushL(calEntryArray);
			CCalDataExchange* calDataExchange =
					CCalDataExchange::NewL(*iCalSession);
			CleanupStack::PushL(calDataExchange);
			calDataExchange->ExportL(KUidVCalendar, rs, calEntryArray);

			CleanupStack::PopAndDestroy(4, &fs);
	)

	return (iError == KErrNone);
}

/*!
	Returns error code.
 */
AgendaUtil::Error AgendaUtilPrivate::error() const
{
	switch (iError) {
		case KErrNone:
			return AgendaUtil::NoError;
		case KErrNoMemory:
			return AgendaUtil::OutOfMemoryError;
		case KErrInUse:
			return AgendaUtil::AlreadyInUse;
		default:
			return AgendaUtil::UnknownError;
	}
}

void AgendaUtilPrivate::setCompleted(
		AgendaEntry& entry, bool complete, QDateTime& dateTime)
{
	int success = 0;

	TRAP(
			iError,

			// Convert the dateTime to TCalTime.
			TCalTime calTime;
			TDateTime time(
					dateTime.date().year(), TMonth(dateTime.date().month() - 1),
					dateTime.date().day() - 1, dateTime.time().hour(),
					dateTime.time().minute(), dateTime.time().second(),
					dateTime.time().msec());

			calTime.SetTimeLocalL(time);

			// Fetch the calentry
			CCalEntry* calEntry = iCalEntryView->FetchL(entry.id());
			calEntry->SetCompletedL(complete, calTime);

			// Update the entry in the Database
			RPointerArray<CCalEntry> array;
			CleanupResetAndDestroyPushL(array);
			array.AppendL(calEntry);
			iCalEntryView->UpdateL(array, success);

			// Cleanup.
			CleanupStack::PopAndDestroy( &array );
	)

	if (0 < success) {
		emit q->entryUpdated(entry.id());
	}
}

/*!
	Deletes entries between a given range.

	\param start The start datetime of the range.
	\param end The end datetime of the range.
	\param filter The filter to include types of entries.
 */
void AgendaUtilPrivate::deleteEntries(
		QDateTime& start, QDateTime& end, AgendaUtil::FilterFlags filter)
{
	TRAP(
			iError,

			TDateTime startDateTime(
					start.date().year(), TMonth(start.date().month() - 1),
					start.date().day() - 1, start.time().hour(),
					start.time().minute(), start.time().second(),
					start.time().msec());

			TDateTime endDateTime(
					end.date().year(), TMonth(end.date().month() - 1),
					end.date().day() - 1, end.time().hour(),
					end.time().minute(), end.time().second(),
					end.time().msec());

			TCalTime startCalTime;
			TCalTime endCalTime;
			startCalTime.SetTimeLocalL(startDateTime);
			endCalTime.SetTimeLocalL(endDateTime);

			CalCommon::TCalTimeRange calTimeRange(startCalTime, endCalTime);

			// Set the flag indicating that delete is in progress.
			mIsDeleting = true;
			iCalEntryView->DeleteL(
					calTimeRange,
					(CalCommon::TCalViewFilterFlags) filter, *this);
	)
}

/*!
	Returns the parent entry of the given entry.

	\param entry The entry for which parent entry is required.
	\return AgendaEntry The parent entry.
 */
AgendaEntry AgendaUtilPrivate::parentEntry(AgendaEntry& entry)
{
	AgendaEntry parentEntry;
	// First check if the session with the calendar exists.
	if (!mInstanceViewCreated) {
		// Return empty AgendaEntry.
		return entry;
	}

	TRAP(
			iError,

			// Get the CalEntry equivalent of the entry.
			CCalEntry* calEntry = iCalEntryView->FetchL(entry.id());

			if (calEntry) {
				CleanupStack::PushL(calEntry);
				// Get all the entries with same global Uid.
				RPointerArray<CCalEntry> entries;
				CleanupResetAndDestroyPushL(entries);
				iCalEntryView->FetchL(calEntry->UidL(), entries);
				parentEntry = createAgendaEntryFromCalEntry(*entries[0]);
				CleanupStack::PopAndDestroy(&entries);
				CleanupStack::PopAndDestroy(calEntry);
			}
	)

	// Return the parent entry.
	return parentEntry;
}

/*!
	Returns the start and end times of previous occurence of a particular
	instance

	\param entry The instance with which previous instance details are obtained
	\return None
 */
void AgendaUtilPrivate::getPreviousInstanceTimes(AgendaEntry& entry, 
												QDateTime& startTime, 
												QDateTime& endTime)
{
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	TCalTime previousStartTime;
	TCalTime previousEndTime;
	TTime zero(TInt64(0));
	// Fetch the parent entry 
	// Get the CalEntry equivalent of the entry.
	CCalEntry* calEntry = iCalEntryView->FetchL(entry.id());
	
	// Update the start and end dates as per the instance
	TCalTime instanceStartCalTime;
	TCalTime instanceEndCalTime;
	TDateTime instStartDateTime(entry.startTime().date().year(),
		static_cast<TMonth>(entry.startTime().date().month() - 1),
		entry.startTime().date().day() - 1, entry.startTime().time().hour(),
		entry.startTime().time().minute(), entry.startTime().time().second(), 0);
	
	TDateTime instEndDateTime(entry.endTime().date().year(),
		static_cast<TMonth>(entry.endTime().date().month() - 1),
		entry.endTime().date().day() - 1, entry.endTime().time().hour(),
		entry.endTime().time().minute(), entry.endTime().time().second(), 0);
	
	TTime instStartTime(instStartDateTime);
	TTime instEndTime(instEndDateTime);
	// For nontimed entries set the floating time
	if(entry.isTimedEntry()) {
		instanceStartCalTime.SetTimeLocalL(instStartTime);
		instanceEndCalTime.SetTimeLocalL(instEndTime);					    					    
	}else {
		instanceStartCalTime.SetTimeLocalFloatingL(instStartTime);
		instanceEndCalTime.SetTimeLocalFloatingL(instEndTime);
	}
	
	calEntry->SetStartAndEndTimeL(instanceStartCalTime,instanceEndCalTime);
	
	// Get the parent entry of this instance
	if (calEntry) {
		iCalEntryView->FetchL(calEntry->UidL(), entries);
	}
	
	TCalTime currentInstanceDate = calEntry->RecurrenceIdL();
	if (currentInstanceDate.TimeUtcL() == Time::NullTTime()) {
		// We must be creating a new exception. Calculate the recurrence id.
		TTime startTime =  entries[0]->StartTimeL().TimeLocalL();
		TTimeIntervalMinutes timeOfDay = 0;
		TTime beginningOfDay = zero + startTime.DaysFrom( zero );
		startTime.MinutesFrom(beginningOfDay,timeOfDay);
		beginningOfDay = zero + calEntry->StartTimeL().TimeLocalL().DaysFrom(zero);
		currentInstanceDate.SetTimeLocalL( beginningOfDay + timeOfDay );
	}
	TDateTime check = currentInstanceDate.TimeLocalL().DateTime();
	TCalRRule rrule;
	if (entries[0]->GetRRuleL(rrule)) {
		int repeatIndex = getRepeatIndex(*entries[0]);
		TBool keepLooking = ETrue;
		RArray<TCalTime> exdates;
		CleanupClosePushL( exdates );
		entries[0]->GetExceptionDatesL(exdates);
		
		// Needed for case ERepeatOther
		TCalRRule::TType type( rrule.Type() );
		TInt repeatInterval( rrule.Interval() );
		TCalTime start, end;
		TTime previousInstanceTime = Time::NullTTime(); 
		
		while (keepLooking) {
			// Subtract the repeat interval of the parent.
			switch (repeatIndex) {
				case repeatDaily:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(1) );
					break;
				case repeatWeekly:
				case repeatWorkdays:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(7) );
					break;
				case repeatBiWeekly:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(14) );
					break;
				case repeatMonthly:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalMonths(1) );
					break;
				case repeatYearly:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalYears(1) );
					break;
				case repeatOther:
					/* This case includes repeating events like: 3 days every week, 3rd weekday of everymonth
					   that does not fall in any cases above but still have repeating rule*/
					   
					// Check if the current entry being edited is child entry
					// If yes, then put back the child entry time to currentInstanceDate  
					if (calEntry->RecurrenceIdL().TimeUtcL() != Time::NullTTime()) {
						 currentInstanceDate.SetTimeLocalL(calEntry->StartTimeL().TimeLocalL());
					}
			 
					 switch (type) {
						case TCalRRule::EDaily:
							start.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(1 * repeatInterval));
							break;
						case TCalRRule::EWeekly:
							start.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(7 * repeatInterval));
							break;
						case TCalRRule::EMonthly: 
							// Add 7 days of buffer to cover the cases were gap b/w two instances of the event 
							// can go beuong 30 days. Ex: Every third wednesday of every month 
							start.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalMonths(repeatInterval)-TTimeIntervalDays(7 * repeatInterval));
							break;
						case TCalRRule::EYearly:  
							// Add 7 days of buffer to cover the cases were gap b/w two instances of the event 
							// can go beuong 365 days. Ex: Every third wednesday of September of every year
							start.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalYears(repeatInterval)-TTimeIntervalDays(7 * repeatInterval));
							break;
					 }
					 
					 end.SetTimeLocalL(zero + currentInstanceDate.TimeLocalL().DaysFrom( zero ));
					 previousInstanceTime = getPreviousInstanceForRepeatOther(*entries[0], CalCommon::TCalTimeRange( start, end));
					 currentInstanceDate.SetTimeLocalL( previousInstanceTime);
					 break;
				default:
				case notRepeated:
					keepLooking = EFalse;
					break;
			}
			// Is currentInstanceDate before parent dt start?
			if (currentInstanceDate.TimeLocalL() < 
					entries[0]->StartTimeL().TimeLocalL()) {
				TBool exceptionEntryBeforeParentStartPresent = EFalse;
				for (TInt i=0; i<exdates.Count(); ++i) {
					if( exdates[i].TimeLocalL() < currentInstanceDate.TimeLocalL() ) {
						exceptionEntryBeforeParentStartPresent = ETrue;
					}
				}
				if (!exceptionEntryBeforeParentStartPresent) {
					currentInstanceDate.SetTimeLocalL(Time::NullTTime());
					previousStartTime = currentInstanceDate;
				}
				// There are no instances before the exception
				keepLooking = EFalse;
			} else {
				// TODO: Put the rest of this function into a separate function, as it's identical
				// to GetNextInstanceTimeL().

				// Is there an exdate on currentInstanceDate?
				TBool isExdateOnDay = EFalse;
				for (TInt i=0; i<exdates.Count(); ++i) {
					if( exdates[i].TimeLocalL() == currentInstanceDate.TimeLocalL() )
						{
						isExdateOnDay = ETrue;
						// There is an exdate - is there a child associated with the exdate?
						for(TInt j=1; j<entries.Count(); ++j)
							{
							if( entries[j]->RecurrenceIdL().TimeLocalL() == currentInstanceDate.TimeLocalL() )
								{
								// This child is the previous instance.
								previousStartTime = entries[j]->StartTimeL();
								previousEndTime = entries[j]->EndTimeL();
								keepLooking = EFalse;
								}
							}
						break;
						}
				}
				if (!isExdateOnDay) {
					// The instance exists and hasn't been deleted or made into an exception.
					// Use information from the parent to set the start/end times.
					previousStartTime = currentInstanceDate;

					TTimeIntervalMinutes duration;
					TTime start = entries[0]->StartTimeL().TimeLocalL();
					TTime end = entries[0]->EndTimeL().TimeLocalL(); 
					end.MinutesFrom( start, duration );
					previousEndTime.SetTimeLocalL( currentInstanceDate.TimeLocalL() + duration );
					keepLooking = EFalse;
				}
			}
		}
		CleanupStack::PopAndDestroy(&exdates);
	}
	
	if(previousStartTime.TimeUtcL() != Time::NullTTime()) {
		// Convert TCalTimes to QDateTimes
		TDateTime prevStart = previousStartTime.TimeLocalL().DateTime();
		TDateTime prevEnd = previousEndTime.TimeLocalL().DateTime();
		startTime.setDate(QDate(prevStart.Year(), prevStart.Month()+1,
		                        prevStart.Day() + 1));
		startTime.setTime(QTime(prevStart.Hour(), prevStart.Minute(), 0, 0));
		endTime.setDate(QDate(prevEnd.Year(), prevEnd.Month()+1,
		                      prevEnd.Day() + 1));
		endTime.setTime(QTime(prevEnd.Hour(), prevEnd.Minute(), 0, 0));
	} else {
		QDateTime nullDateTime;
		startTime = nullDateTime;
		endTime = nullDateTime;
	}
	delete calEntry;
	CleanupStack::PopAndDestroy(&entries);
}

/*!
	Returns the start and end times of next occurence of a particular
	instance

	\param entry The instance with which next instance details are obtained
	\return None
 */
void AgendaUtilPrivate::getNextInstanceTimes(AgendaEntry& entry, 
                                          QDateTime& startTime, 
                                          QDateTime& endTime)
{
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	TCalTime nextStartTime;
	TCalTime nextEndTime;
	TTime zero(TInt64(0));
	// Fetch the parent entry 
	// Get the CalEntry equivalent of the entry.
	CCalEntry* calEntry = iCalEntryView->FetchL(entry.id());
	
	// Update the start and end dates as per the instance
	TCalTime instanceStartCalTime;
	TCalTime instanceEndCalTime;
	TDateTime instStartDateTime(entry.startTime().date().year(),
		static_cast<TMonth>(entry.startTime().date().month() - 1),
		entry.startTime().date().day() - 1, entry.startTime().time().hour(),
		entry.startTime().time().minute(), entry.startTime().time().second(), 0);
	
	TDateTime instEndDateTime(entry.endTime().date().year(),
		static_cast<TMonth>(entry.endTime().date().month() - 1),
		entry.endTime().date().day() - 1, entry.endTime().time().hour(),
		entry.endTime().time().minute(), entry.endTime().time().second(), 0);
	
	TTime instStartTime(instStartDateTime);
	TTime instEndTime(instEndDateTime);
	// For nontimed entries set the floating time
	if (entry.isTimedEntry()) {
		instanceStartCalTime.SetTimeLocalL(instStartTime);
		instanceEndCalTime.SetTimeLocalL(instEndTime);
	}else {
		instanceStartCalTime.SetTimeLocalFloatingL(instStartTime);
		instanceEndCalTime.SetTimeLocalFloatingL(instEndTime);
	}
	calEntry->SetStartAndEndTimeL(instanceStartCalTime,instanceEndCalTime);
	
	// Get the parent entry of this instance
	if (calEntry) {
		iCalEntryView->FetchL(calEntry->UidL(), entries);
	}
	
	TCalTime currentInstanceDate = calEntry->RecurrenceIdL();
	if (currentInstanceDate.TimeUtcL() == Time::NullTTime()) {
		// We must be creating a new exception. Calculate the recurrence id.
		TTime startTime =  entries[0]->StartTimeL().TimeLocalL();
		TTimeIntervalMinutes timeOfDay = 0;
		TTime beginningOfDay = zero + startTime.DaysFrom( zero );
		startTime.MinutesFrom(beginningOfDay,timeOfDay);
		beginningOfDay = zero + calEntry->StartTimeL().TimeLocalL().DaysFrom(zero);
		currentInstanceDate.SetTimeLocalL( beginningOfDay + timeOfDay );
	}
	
	TCalRRule rrule;
	if (entries[0]->GetRRuleL(rrule)) {
		int repeatIndex = getRepeatIndex(*entries[0]);
		TBool keepLooking = ETrue;
		RArray<TCalTime> exdates;
		CleanupClosePushL( exdates );
		entries[0]->GetExceptionDatesL(exdates);
		
		// Needed for case ERepeatOther
		TCalRRule::TType type( rrule.Type() );
		TInt repeatInterval( rrule.Interval() );
		TCalTime start, end;
		TTime previousInstanceTime = Time::NullTTime(); 
		
		while (keepLooking) {
			// Subtract the repeat interval of the parent.
			switch (repeatIndex) {
				case repeatDaily:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(1) );
					break;
				case repeatWeekly:
				case repeatWorkdays:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(7) );
					break;
				case repeatBiWeekly:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(14) );
					break;
				case repeatMonthly:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalMonths(1) );
					break;
				case repeatYearly:
					currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalYears(1) );
					break;
				case repeatOther:
					/* This case includes repeating events like: 3 days every week, 3rd weekday of everymonth
					   that does not fall in any cases above but still have repeating rule*/
					   
					// Check if the current entry being edited is child entry
					// If yes, then put back the child entry time to currentInstanceDate  
					if (calEntry->RecurrenceIdL().TimeUtcL() != Time::NullTTime()) {
						 currentInstanceDate.SetTimeLocalL(calEntry->StartTimeL().TimeLocalL());
					}
			 
					 switch (type) {
						case TCalRRule::EDaily:
							start.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(1 * repeatInterval));
							break;
						case TCalRRule::EWeekly:
							start.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(7 * repeatInterval));
							break;
						case TCalRRule::EMonthly: 
							// Add 7 days of buffer to cover the cases were gap b/w two instances of the event 
							// can go beuong 30 days. Ex: Every third wednesday of every month 
							start.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalMonths(repeatInterval)-TTimeIntervalDays(7 * repeatInterval));
							break;
						case TCalRRule::EYearly:  
							// Add 7 days of buffer to cover the cases were gap b/w two instances of the event 
							// can go beuong 365 days. Ex: Every third wednesday of September of every year
							start.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalYears(repeatInterval)-TTimeIntervalDays(7 * repeatInterval));
							break;
					 }
					 
					 end.SetTimeLocalL(zero + currentInstanceDate.TimeLocalL().DaysFrom( zero ));
					 previousInstanceTime = getNextInstanceForRepeatOther(*entries[0], CalCommon::TCalTimeRange( start, end));
					 currentInstanceDate.SetTimeLocalL( previousInstanceTime);
					 break;
				default:
				case notRepeated:
					keepLooking = EFalse;
					break;
			}
			// Is currentInstanceDate after repeat until date?
			if (currentInstanceDate.TimeLocalL() >  
					rrule.Until().TimeLocalL()) {
				TBool exceptionEntryAfterRepeatUntilPresent = EFalse;
				for (TInt i=0; i<exdates.Count(); ++i) {
					if(exdates[i].TimeLocalL() > rrule.Until().TimeLocalL()) {
						exceptionEntryAfterRepeatUntilPresent = ETrue;
					}
				}
				if (!exceptionEntryAfterRepeatUntilPresent) {
					currentInstanceDate.SetTimeLocalL(Time::NullTTime());
					nextStartTime = currentInstanceDate;
				}
				// There are no instances before the exception
				keepLooking = EFalse;
			} else {
				// TODO: Put the rest of this function into a separate function, as it's identical
				// to GetNextInstanceTimeL().

				// Is there an exdate on currentInstanceDate?
				TBool isExdateOnDay = EFalse;
				for (TInt i=0; i<exdates.Count(); ++i) {
					if( exdates[i].TimeLocalL() == currentInstanceDate.TimeLocalL() )
						{
						isExdateOnDay = ETrue;
						// There is an exdate - is there a child associated with the exdate?
						for(TInt j=1; j<entries.Count(); ++j)
							{
							if( entries[j]->RecurrenceIdL().TimeLocalL() == currentInstanceDate.TimeLocalL() )
								{
								// This child is the previous instance.
								nextStartTime = entries[j]->StartTimeL();
								nextEndTime = entries[j]->EndTimeL();
								keepLooking = EFalse;
								}
							}
						break;
						}
				}
				if (!isExdateOnDay) {
					// The instance exists and hasn't been deleted or made into an exception.
					// Use information from the parent to set the start/end times.
					nextStartTime = currentInstanceDate;

					TTimeIntervalMinutes duration;
					TTime start = entries[0]->StartTimeL().TimeLocalL();
					TTime end = entries[0]->EndTimeL().TimeLocalL(); 
					end.MinutesFrom( start, duration );
					nextEndTime.SetTimeLocalL( currentInstanceDate.TimeLocalL() + duration );
					keepLooking = EFalse;
				}
			}
		CleanupStack::PopAndDestroy( &exdates );
		}
	}
	
	if (nextStartTime.TimeUtcL() != Time::NullTTime()) {
		// Convert TCalTimes to QDateTimes
		TDateTime nextStart = nextStartTime.TimeLocalL().DateTime();
		TDateTime nextEnd = nextEndTime.TimeLocalL().DateTime();
		startTime.setDate(QDate(nextStart.Year(), nextStart.Month()+1,
		                        nextStart.Day() + 1));
		startTime.setTime(QTime(nextStart.Hour(), nextStart.Minute(), 0, 0));
		endTime.setDate(QDate(nextEnd.Year(), nextEnd.Month()+1,
		                      nextEnd.Day() + 1));
		endTime.setTime(QTime(nextEnd.Hour(), nextEnd.Minute(), 0, 0));	
	} else {
		QDateTime nullDateTime;
		startTime = nullDateTime;
		endTime = nullDateTime;
	}
	delete calEntry;
	
	CleanupStack::PopAndDestroy(&entries);
}

/*!
	 Returns true if there are no entries in the database else returns false
	 
	 \return bool 
 */
bool AgendaUtilPrivate::areNoEntriesInCalendar()
{
	// First prepare the session with the agenda server.
	if (!mInstanceViewCreated) {
        // database is not ready, so, return that there are no entries
		return true;
	}
	
	bool isEmpty;
	// Query for the entries for entire range
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	
	// Create the filter
	CalCommon::TCalViewFilter filters = AgendaUtil::IncludeAnniversaries
										| AgendaUtil::IncludeAppointments
										| AgendaUtil::IncludeEvents
										| AgendaUtil::IncludeReminders
										| AgendaUtil::IncludeIncompletedTodos;
	
	// Set up the range
	TCalTime startDateForInstanceSearch;
	TCalTime endDateForInstanceSearch;

	TDateTime startTime = TDateTime(
			startDateArray[0], static_cast<TMonth>(startDateArray[1]),
			0, 0, 0, 0, 0);

	TDateTime endTime = TDateTime(
			endDateArray[0], static_cast<TMonth>(endDateArray[1]),
			0, 0, 0, 0, 0);

	startDateForInstanceSearch.SetTimeLocalL(startTime);
	endDateForInstanceSearch.SetTimeLocalL(endTime);
	CalCommon::TCalTimeRange searchTimeRange(
			startDateForInstanceSearch,
			endDateForInstanceSearch);
	
	// Fire a query
	iCalInstanceView->FindInstanceL(
			instanceList, filters, searchTimeRange);
	
	// Check the list count
	if (instanceList.Count()) {
		isEmpty = false;
	} else {
		isEmpty = true;
	}
	
	// Cleanup
	CleanupStack::PopAndDestroy( &instanceList );
	return isEmpty;
}

/*!
	Returns the minimum time supported.

	\return QDateTime holding the minimum supported time.
 */
QDateTime AgendaUtilPrivate::minTime()
{
	TTime minTime = TCalTime::MinTime();

	// Convert it to QT
	QDate date(
			minTime.DateTime().Year(), minTime.DateTime().Month() + 1,
			minTime.DateTime().Day() + 1);
	QTime time(
			minTime.DateTime().Hour(), minTime.DateTime().Minute(),
			minTime.DateTime().Second());

	QDateTime minimumDateTime(date, time);

	return minimumDateTime;
}

/*!
	Returns the maximum time supported.

	\return QDateTime holding the maximum supported time.
 */
QDateTime AgendaUtilPrivate::maxTime()
{
	// Returns maximum time allowed, 31.12.2100 0:00 is max so 30.12.2100 is
	// last actual date to be used.
	TTime maxTime = TCalTime::MaxTime() - TTimeIntervalMinutes( 1 );

	// Convert it to QT
	QDate date(
			maxTime.DateTime().Year(), maxTime.DateTime().Month() + 1,
			maxTime.DateTime().Day() + 1);
	QTime time(
			maxTime.DateTime().Hour(), maxTime.DateTime().Minute(),
			maxTime.DateTime().Second());

	QDateTime maximumDateTime(date, time);

	return maximumDateTime;
}

/*!
    Returns true if entry repeats on workdays else false

    \return true if entry repeats on workdays else false
 */
bool AgendaUtilPrivate::isWorkdaysRepeatingEntry(
											const AgendaRepeatRule& repeatRule)
{
	bool status = true;
	int fixedNum = 1;
	int ruleday = 0;
	HbExtendedLocale locale = HbExtendedLocale::system();

	QString workDaysString = locale.workDays();
	bool ok;
	uint workDays = workDaysString.toUInt(&ok, 2);
	if (!ok) {
		return false;
	}
	QList<AgendaRepeatRule::Day> weekDaysFromRule = repeatRule.byDay();

	QList<AgendaRepeatRule::Day> weekDaysFromLocale;

	// "workDays" is a bit mask of seven bits indicating (by being set) which days are workdays. 
	// The least significant bit corresponds to Monday, the next bit to Tuesday and so on. 
	// "workDays" is converted into weekDaysFromLocale for comparing with "weekDaysFromRule".
	for (TInt i = 0; i < KNoOfDaysInWeek; i++) {
		ruleday = fixedNum << i;
		if (workDays & ruleday) {
			weekDaysFromLocale.append((AgendaRepeatRule::Day) i);
		}
	}

	// Checks whether the device "workdays" are same as the event's repeat days.
	if (weekDaysFromRule.count() == weekDaysFromLocale.count()) {
		for (int i = 0; i < weekDaysFromLocale.count(); i++) {
			if ((int) weekDaysFromLocale[i] != (int) weekDaysFromRule[i]) {
				status = false;
				break;
			}
		}
	} else {
		status = false;
	}

	return status;

}
/*!
	Creates an AgendaEntry object from a given CCalEntry and CCalInstance.

	\param calEntry Reference to a CCalEntry.
	\param instance A CCalInstance.

	\return AgendaEntry.
 */
AgendaEntry AgendaUtilPrivate::createAgendaEntryFromCalEntry(
		CCalEntry& calEntry, CCalInstance* instance)
{
	AgendaEntry entry;

	// Type.
	AgendaEntry::Type entryType =
			static_cast<AgendaEntry::Type>(calEntry.EntryTypeL());
	entry.setType(entryType);

	// Method.
	AgendaEntry::Method method =
			static_cast<AgendaEntry::Method>(calEntry.MethodL());
	entry.setMethod(method);

	// Summary.
	TPtrC calSummary = calEntry.SummaryL();
	entry.setSummary(
			QString::fromUtf16(calSummary.Ptr(), calSummary.Length()));

	// Description.
	TPtrC calDescription = calEntry.DescriptionL();
	entry.setDescription(
			QString::fromUtf16(calDescription.Ptr(), calDescription.Length()));

	// Location.
	TPtrC calLocation = calEntry.LocationL();
	entry.setLocation(
			QString::fromUtf16(calLocation.Ptr(), calLocation.Length()));

	// Priority.
	entry.setPriority(calEntry.PriorityL());

	// Time.
	TDateTime calStartDateTime;
	TDateTime calEndDateTime;
	if (instance) {
		// Set start time and end time frm the instance instead from entry
		// to copy the correct time.
		calStartDateTime = instance->StartTimeL().TimeLocalL().DateTime();
		calEndDateTime = instance->EndTimeL().TimeLocalL().DateTime();
	} else {
		calStartDateTime = calEntry.StartTimeL().TimeLocalL().DateTime();
		calEndDateTime = calEntry.EndTimeL().TimeLocalL().DateTime();
	}
	QDateTime startDateTime(
			QDate(
					calStartDateTime.Year(), calStartDateTime.Month()+1,
					calStartDateTime.Day() + 1),
			QTime(
					calStartDateTime.Hour(), calStartDateTime.Minute(), 0, 0));
	QDateTime endDateTime(
			QDate(
					calEndDateTime.Year(), calEndDateTime.Month()+1,
					calEndDateTime.Day() + 1),
			QTime(
					calEndDateTime.Hour(), calEndDateTime.Minute(), 0, 0));
	entry.setStartAndEndTime(startDateTime, endDateTime);

	// Attendees.
	RPointerArray<CCalAttendee> calAttendees = calEntry.AttendeesL();

	for (int i = 0; i < calAttendees.Count(); i++) {
		AgendaAttendee attendee;
		// Address.
		TPtrC calAddress = calAttendees[i]->Address();
		attendee.setAddress(
				QString::fromUtf16(calAddress.Ptr(), calAddress.Length()));

		// Common name.
		TPtrC calCommonName = calAttendees[i]->CommonName();
		attendee.setCommonName(QString::fromUtf16(
				calCommonName.Ptr(), calCommonName.Length()));

		// Response requested.
		attendee.setResponseRequested(calAttendees[i]->ResponseRequested());

		// Participant role.
		attendee.setRole(static_cast<AgendaAttendee::ParticipantRole>(
				calAttendees[i]->RoleL()));

		// Status.
		attendee.setStatus(
				static_cast<AgendaAttendee::StatusType>(
						calAttendees[i]->StatusL()));
		entry.addAttendee(attendee);
	}

// TODO: Right now we are not adding category to agendaEntry.
// Will be handled later when we have some usecase with category.
/*
	// Categories.
	const RPointerArray<CCalCategory>& calCategories = calEntry.CategoryListL();

	for (int i = 0; i < calCategories.Count(); i++) {
		AgendaCategory category;
		CCalCategory::TCalCategoryType categoryType =
				calCategories[i]->Category();
		if (categoryType == CCalCategory::ECalExtended) {
			TPtrC categoryName = calCategories[i]->ExtendedCategoryName();
			category.setExtendedCategoryName(
					QString::fromUtf16(
							categoryName.Ptr(), categoryName.Length()));
		}
		category.setCategory(
				static_cast<AgendaCategory::CategoryType>(categoryType));
		entry.addCategory(category);
	}
*/
	// Id.
	entry.d->m_id = calEntry.LocalUidL();

	// Alarm.
	CCalAlarm* calAlarm = calEntry.AlarmL();
	if (calAlarm) {
		CleanupStack::PushL(calAlarm);
		AgendaAlarm alarm;
		alarm.setAlarmSoundName(QString::fromUtf16(
				calAlarm->AlarmSoundNameL().Ptr(),
				calAlarm->AlarmSoundNameL().Length()));
		alarm.setTimeOffset(calAlarm->TimeOffset().Int());
		entry.setAlarm(alarm);
		CleanupStack::PopAndDestroy(calAlarm);
	}

	// Repear rule.
	TCalRRule calRRule;
	calEntry.GetRRuleL(calRRule);
	if (calRRule.Type() != TCalRRule::EInvalid) {
		AgendaRepeatRule agendaRepeatRule =
		        createAgendaRRuleFromTCalRRule(calRRule);
		// Set the rule now.
		entry.setRepeatRule(agendaRepeatRule);
	}

	// Get the RDates.
	RArray<TCalTime> rDateList;
	calEntry.GetRDatesL(rDateList);
	QList<QDate> qRDates;
	for (int i = 0; i < rDateList.Count(); i++) {
		TTime rDate = rDateList[i].TimeUtcL();
		QDate qRdate(
				rDate.DateTime().Year(),
				rDate.DateTime().Month() + 1,
				rDate.DateTime().Day() + 1);
		qRDates.append(qRdate);
	}
	// Set the RDates.
	entry.setRDates(qRDates);

	// Recurrence Id.
	TCalTime recCalTime = calEntry.RecurrenceIdL();
	
	// Set the recurrence id only when it is not NULL
	// so that we can identify the child properly
	if (recCalTime.TimeUtcL() != Time::NullTTime()) {
		TTime recTime = recCalTime.TimeUtcL();
		QDateTime qRecTime(
				QDate(
						recTime.DateTime().Year(), recTime.DateTime().Month() + 1,
						recTime.DateTime().Day() + 1),
				QTime(
						recTime.DateTime().Hour(), recTime.DateTime().Minute(),
						recTime.DateTime().Second()));
		entry.setRecurrenceId(qRecTime);
	}
	
	// Set the last modification time for the entry.
	TCalTime lastModCalTime = calEntry.LastModifiedDateL();
	TTime localTime = lastModCalTime.TimeLocalL();
	TDateTime localDateTime = localTime.DateTime();
	QDateTime lastModifiedTime(
			QDate(
					localDateTime.Year(),
					localDateTime.Month() + 1,
					localDateTime.Day() + 1),
			QTime(
					localDateTime.Hour(), localDateTime.Minute(),
					localDateTime.Second(), 0));
	entry.setLastModifiedDateTime(lastModifiedTime);

	// Set the status for the entry.
	entry.setStatus((AgendaEntry::Status) calEntry.StatusL());

	// Get the favourite property.
	entry.setFavourite(calEntry.FavouriteL());

	// Set the to-do completion date and time for the entry
	TCalTime completionTime = calEntry.CompletedTimeL();
	TTime completionTimeInlocal = completionTime.TimeLocalL();
	QDateTime completionDateTime(
				QDate(
						completionTimeInlocal.DateTime().Year(),
						completionTimeInlocal.DateTime().Month() + 1,
						completionTimeInlocal.DateTime().Day() + 1),
				QTime(
						completionTimeInlocal.DateTime().Hour(),
						completionTimeInlocal.DateTime().Minute(),
						completionTimeInlocal.DateTime().Second(), 0));
	entry.setCompletedDateTime(completionDateTime);

	// Set the dtStamp time for agenda entry. Currently dtstamp is used as
	// creation time for the entry type ENote.
	TCalTime dtStampTime = calEntry.DTStampL();
	TTime dtStampTimeInLocal = dtStampTime.TimeLocalL();
	QDateTime dtStampDateTime;
	if (dtStampTimeInLocal != Time::NullTTime()) {
		dtStampDateTime.setDate(
				QDate(
					dtStampTimeInLocal.DateTime().Year(),
					dtStampTimeInLocal.DateTime().Month() + 1,
					dtStampTimeInLocal.DateTime().Day() + 1));

		dtStampDateTime.setTime(
				QTime(
					dtStampTimeInLocal.DateTime().Hour(),
					dtStampTimeInLocal.DateTime().Minute(),
					dtStampTimeInLocal.DateTime().Second()));
	}
	entry.setDTStamp(dtStampDateTime);
	
	// Copy the geo value if it has any
	CCalGeoValue* geoValue = calEntry.GeoValueL();
	if (geoValue) {
		AgendaGeoValue entryGeoValue;
		double latitude;
		double longitude;
		geoValue->GetLatLong(latitude, longitude);
		entryGeoValue.setLatLong(latitude, longitude);
		
		// Set it to entry
		entry.setGeoValue(entryGeoValue);
		delete geoValue;
	}
	
	// Return the entry.
	return entry;
}

/*!
	Copy all the data to CCalEntry from a given AgendaEntry.
	\param agendaEntry Reference to a AgendaEntry.
	\param calEntry Reference to a CCalEntry.
 */
void AgendaUtilPrivate::createCCalEntryFromAgendaEntry(AgendaEntry &agendaEntry, CCalEntry &calEntry)
{
	if (agendaEntry.isNull()) {
			// Invalid entry.
			return;
		}
	
	TRAP(
			iError,
			// Add description to the agendaEntry.
			TPtrC
					description(
								reinterpret_cast<const TUint16*> (agendaEntry.description().utf16()));
			calEntry.SetDescriptionL(description);
			
			if(AgendaEntry::MethodUnknown != agendaEntry.method()) {
								calEntry.SetMethodL(
									static_cast<CCalEntry::TMethod> (agendaEntry.method()));
			}
	
			// Set the favourite property.
			calEntry.SetFavouriteL(agendaEntry.favourite());
			
			
			if (AgendaEntry::TypeNote == agendaEntry.type()) {
				// Set the last modification time.
				TCalTime calTime;
				QDateTime dateTime = agendaEntry.lastModifiedDateTime();
				TDateTime tempDateTime(dateTime.date().year(),
						static_cast<TMonth> (dateTime.date().month() - 1),
						dateTime.date().day() - 1, dateTime.time().hour(),
						dateTime.time().minute(), 0, 0);
				TTime tempTime(tempDateTime);
				calTime.SetTimeLocalL(tempTime);
				calEntry.SetLastModifiedDateL(calTime);
				
				// Set the dtstamp time.It is used to set the creation time.
				TCalTime creationCalTime;
				QDateTime dtStamp = agendaEntry.dtStamp();
				TDateTime
				creationDateTime(dtStamp.date().year(),
						static_cast<TMonth> (dtStamp.date().month() - 1),
						dtStamp.date().day() - 1, dtStamp.time().hour(),
						dtStamp.time().minute(), 0, 0);
				TTime creationTTime(creationDateTime);
				creationCalTime.SetTimeLocalL(creationTTime);
				calEntry.SetDTStampL(creationCalTime);
			} else {
	
			// Add the summary.
			TPtrC
					summary(
							reinterpret_cast<const TUint16*> (agendaEntry.summary().utf16()));
			calEntry.SetSummaryL(summary);
	
			// Set the agendaEntry Start/End Date and time.
			QDate date = agendaEntry.startTime().date();
			QTime time = agendaEntry.startTime().time();
	
			TDateTime startDateTime(date.year(), static_cast<TMonth> (date.month()
					- 1), date.day() - 1, time.hour(), time.minute(), 0, 0);
			TTime entryStartTime(startDateTime);
			TCalTime calStartTime;

			date = agendaEntry.endTime().date();
			time = agendaEntry.endTime().time();
	
			TDateTime endDateTime(date.year(), static_cast<TMonth> (date.month()
					- 1), date.day() - 1, time.hour(), time.minute(), 0, 0);
			TTime entryEndTime(endDateTime);
			TCalTime calEndTime;
			
			// Use floating time for the nontimed entries.
			if(agendaEntry.isTimedEntry()) {
			    calStartTime.SetTimeLocalL(entryStartTime);
			    calEndTime.SetTimeLocalL(entryEndTime);    
			}else {
			    calStartTime.SetTimeLocalFloatingL(entryStartTime);
			    calEndTime.SetTimeLocalFloatingL(entryEndTime);
			}
			calEntry.SetStartAndEndTimeL(calStartTime, calEndTime);
	
			// Add attendees to the agendaEntry.
			addAttendeesToEntry(agendaEntry.d->m_attendees, calEntry);
	
			// Add categories to the agendaEntry.
			addCategoriesToEntry(agendaEntry.d->m_categories, calEntry);
	
			// Add Alarm to the agendaEntry.
			AgendaAlarm alarm = agendaEntry.alarm();
			setAlarmToEntry(alarm, calEntry);
	
			// Set the priority.
			int priority = agendaEntry.priority();
			if (agendaEntry.priority() != -1) {
				calEntry.SetPriorityL(priority);
			}
	
			// Set the location.
			TPtrC
					location(
							 reinterpret_cast<const TUint16*> (agendaEntry.location().utf16()));
			calEntry.SetLocationL(location);
	
			// Set the repeat type if applicable.
			if (AgendaRepeatRule::InvalidRule != agendaEntry.repeatRule().type()) {
				AgendaRepeatRule agendaRepeatRule = agendaEntry.repeatRule();
				TCalRRule repeatRule =
						createTCalRRuleFromAgendaRRule(agendaRepeatRule, agendaEntry.isTimedEntry());
				calEntry.SetRRuleL(repeatRule);
			}
	
			// Save the status of the agendaEntry.
			calEntry.SetStatusL((CCalEntry::TStatus) agendaEntry.status());
	
			// Save the geo value if any
			AgendaGeoValue entryGeoValue = agendaEntry.geoValue();
			if (!entryGeoValue.isNull()) {
				CCalGeoValue* geoValue = CCalGeoValue::NewL();
				double latitude;
				double longitude;
				entryGeoValue.getLatLong(latitude, longitude);
	
				// set the values to symbian geo value
				geoValue->SetLatLongL(latitude, longitude);
	
				// set it to CCalentry
				calEntry.SetGeoValueL(*geoValue);
				delete geoValue;
			} else {
				calEntry.ClearGeoValueL();
			}
		}
	)

}

bool AgendaUtilPrivate::addAttendeesToEntry(
		const QList<AgendaAttendee>& attendees, CCalEntry& entry)
{
	// First prepare the session with the agenda server.
	if (!mInstanceViewCreated) {
		return false;
	}

	TRAP(
			iError,

			for (int i = 0; i < attendees.count(); i++) {
				CCalAttendee* attendee = CCalAttendee::NewL(
						TPtrC(reinterpret_cast<const TUint16*>(
								attendees.at(i).address().utf16())));
				attendee->SetCommonNameL(
						TPtrC(reinterpret_cast<const TUint16*>(
								attendees.at(i).commonName().utf16())));
				attendee->SetResponseRequested(
						attendees.at(i).responseRequested());
				attendee->SetRoleL(static_cast<CCalAttendee::TCalRole>(
						attendees.at(i).role()));
				attendee->SetStatusL(static_cast<CCalAttendee::TCalStatus>(
						attendees.at(i).status()));
				entry.AddAttendeeL(attendee);
			}
	)
	return (iError == KErrNone);
}

bool AgendaUtilPrivate::addCategoriesToEntry(
		const QList<AgendaCategory>& categories, CCalEntry& entry)
{
	// First prepare the session with the agenda server.
	if (!mInstanceViewCreated) {
		return false;
	}
	TRAP(
			iError,

			for (int i = 0; i < categories.count(); i++) {
				AgendaCategory::CategoryType type = categories.at(i).category();
				if (type != AgendaCategory::ExtendedCategory) {
					CCalCategory* category = CCalCategory::NewL(
							static_cast<CCalCategory::TCalCategoryType>(type));
					entry.AddCategoryL(category);
				} else {
					TPtrC categoryName = TPtrC(reinterpret_cast<const TUint16*>(
							categories.at(i).extendedCategoryName().utf16()));
					CCalCategory* category = CCalCategory::NewL(categoryName);
					entry.AddCategoryL(category);
				}
			}
	)
	return (iError == KErrNone);
}

bool AgendaUtilPrivate::setAlarmToEntry(
		const AgendaAlarm& alarm, CCalEntry& entry)
{
	TRAP(
			iError,

			if (alarm.isNull()) {
				entry.SetAlarmL(0);
			} else {
				CCalAlarm* calAlarm = CCalAlarm::NewL();
				CleanupStack::PushL(calAlarm);
				calAlarm->SetTimeOffset(alarm.timeOffset());
				if (!alarm.alarmSoundName().isNull()) {
					TPtrC alarmName(reinterpret_cast<const TUint16*>(
							alarm.alarmSoundName().utf16()));
					calAlarm->SetAlarmSoundNameL(alarmName);
				}
				entry.SetAlarmL(calAlarm);
				CleanupStack::PopAndDestroy(calAlarm);
			}
	)
	return (iError == KErrNone);
}

void AgendaUtilPrivate::sortInstanceList(RPointerArray<CCalInstance>& list)
{
	TLinearOrder<CCalInstance> instanceListOrder(
			AgendaUtilPrivate::entryCompare );
	list.Sort( instanceListOrder );
}

void AgendaUtilPrivate::getDayRange(
		const QDateTime& aStartDay, const QDateTime& aEndDay,
		CalCommon::TCalTimeRange& aRange )
{
	TDateTime start(
			aStartDay.date().year(), TMonth(aStartDay.date().month() - 1),
			aStartDay.date().day() - 1, aStartDay.time().hour(),
			aStartDay.time().minute(), aStartDay.time().second(),
			aStartDay.time().msec());
	TDateTime end(
			aEndDay.date().year(), TMonth(aEndDay.date().month() - 1),
			aEndDay.date().day() - 1, aEndDay.time().hour(),
			aEndDay.time().minute(), aEndDay.time().second(),
			aEndDay.time().msec());

	start.SetHour(0);
	start.SetMinute(0);
	start.SetSecond(0);
	start.SetMicroSecond(0);

	end.SetHour(23);
	end.SetMinute(59);
	end.SetSecond(59);
	end.SetMicroSecond(0);

	// Prevent overflow.
	TCalTime endDate;
	endDate.SetTimeLocalL(LimitToValidTime(TTime(end)));
	TCalTime startDate;
	startDate.SetTimeLocalL(LimitToValidTime(TTime(start)));

	CalCommon::TCalTimeRange dayrange(startDate, endDate);
	aRange = dayrange;
}

TTime AgendaUtilPrivate::LimitToValidTime( const TTime& aTime )
{

	TTime valid = aTime;
	valid = valid > (
			TCalTime::MaxTime() - TTimeIntervalMinutes( 1 ))
			? (TCalTime::MaxTime() - TTimeIntervalMinutes( 1 ))
			: valid;
	valid = valid < (TCalTime::MinTime()) ? (TCalTime::MinTime()) : valid;

	return valid;
}

CCalInstance* AgendaUtilPrivate::findPossibleInstance(AgendaEntry& entry)
{
    if(!mInstanceViewCreated) {
        // return empty list
        return NULL;
    }
	TCalTime dummy;
	CalCommon::TCalTimeRange dayRange(dummy, dummy);
	getDayRange(entry.startTime(), entry.startTime(), dayRange);
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	CalCommon::TCalViewFilter filter = 
				CalCommon::TCalViewFilter(CalCommon::EIncludeAnnivs |
										CalCommon::EIncludeAppts | 
										CalCommon::EIncludeEvents |
										CalCommon::EIncludeReminder |
										CalCommon::EIncludeIncompletedTodos);
	iCalInstanceView->FindInstanceL(instances, filter, dayRange);
	TTime entryStartTime(dayRange.StartTime().TimeLocalL());

	CCalInstance* result = 0;

	// For instances finishing the next day (now possible with unified
	// DateTime editor), we have to do our best to match the instance time
	// exactly - otherwise we could match the LocalUid to the incorrect
	// instance in a series.
	for(int i = 0; i < instances.Count() && !result; ++i)
	{
		if(instances[i]->Entry().LocalUidL() == entry.id())
		{
			// Check the instance time matches.
			if(instances[i]->StartTimeL().TimeLocalL() == entryStartTime)
			{
				result = instances[i];
				instances.Remove(i);
			}
		}
	}

	if(!result)
	{
		// Couldn't match the instance time exactly - just use the instance
		// with the same LocalUid as the one we're looking for.
		for(TInt i=0; i < instances.Count() && !result; ++i)
		{
			if(instances[i]->Entry().LocalUidL() == entry.id())
			{
				result = instances[i];
				instances.Remove(i);
			}
		}
	}

	// Cleanup.
	CleanupStack::PopAndDestroy(&instances);
	return result;
}

int AgendaUtilPrivate::entryCompare(
		const CCalInstance& aInstance1, const CCalInstance& aInstance2)
{
	int ret(Equal);

	const CCalEntry& entry1 = aInstance1.Entry();
	const CCalEntry& entry2 = aInstance2.Entry();
	const CCalEntry::TType type1 = entry1.EntryTypeL();
	const CCalEntry::TType type2 = entry2.EntryTypeL();

	// types are equal (reminders are handled as meetings)
	if(type1 == type2
			|| ((type1 == CCalEntry::EAppt && type2 == CCalEntry::EReminder)
			|| (type2 == CCalEntry::EAppt && type1 == CCalEntry::EReminder)))
	{
		switch(type1)
		{
			case CCalEntry::ETodo:
				ret = compareToDos(entry1, entry2);
				break;

			case CCalEntry::EAnniv:
			case CCalEntry::EEvent:
				ret = compareNonTimedNotes(aInstance1, aInstance2);
				break;

			case CCalEntry::EReminder:
			case CCalEntry::EAppt:
				ret = compareTimedNotes(aInstance1, aInstance2);
				break;

			default:
				ASSERT(EFalse);
		}
	}
	else // Types are different.
	{
		switch(type1)
		{
			case CCalEntry::ETodo:
				// To-dos come always first...
				ret = LessThan;
				break;

			case CCalEntry::EEvent:
			{
				// ...then day notes...
				if(type2 == CCalEntry::ETodo)
				{
					ret = GreaterThan;
				}
				else
				{
					ret = LessThan;
				}
			}
			break;

			case CCalEntry::EAnniv:
			{
				// ...and anniversaries...
				if((type2 == CCalEntry::ETodo) || (type2 == CCalEntry::EEvent))
				{
					ret = GreaterThan;
				}
				else
				{
					ret = LessThan;
				}
			}
			break;

			case CCalEntry::EReminder:
			case CCalEntry::EAppt:
				// ...and finally timed notes.
				ret = GreaterThan;
				break;

			default:
				ASSERT(EFalse);
		}
	}

	return ret;
}

int AgendaUtilPrivate::compareToDos(
		const CCalEntry& aEntry1, const CCalEntry& aEntry2)
{
    int ret( Equal );
    CCalEntry::TStatus status1 = aEntry1.StatusL();
    CCalEntry::TStatus status2 = aEntry2.StatusL();

    if( status1 == CCalEntry::ENullStatus )
        {
        status1 = CCalEntry::ETodoNeedsAction;
        }

    if( status2 == CCalEntry::ENullStatus )
        {
        status2 = CCalEntry::ETodoNeedsAction;
        }

    if( status1 == status2 )
        {
        TTime time1 = aEntry1.EndTimeL().TimeUtcL();
        TTime time2 = aEntry2.EndTimeL().TimeUtcL();

        if( time1 == time2 )
            {
            const TUint pri1( aEntry1.PriorityL() );
            const TUint pri2( aEntry2.PriorityL() );

            if( pri1 == pri2 )
                {
                time1 = aEntry1.LastModifiedDateL().TimeUtcL();
                time2 = aEntry2.LastModifiedDateL().TimeUtcL();

                if( time1 == time2 )
                    {
                    ret = Equal;
                    }
                else if( time1 > time2 )
                    {
                    ret = GreaterThan; // oldest first
                    }
                else
                    {
                    ret = LessThan;
                    }
                }
            else
                {
                if( pri1 > pri2 )
                    {
                    ret = GreaterThan;
                    }
                else
                    {
                    ret = LessThan;
                    }
                }
            }
        else
            {
            if( time1 > time2 )
                {
                ret = GreaterThan;
                }
            else
                {
                ret = LessThan;
                }
            }
        }
    else
        {
        if( status1 == CCalEntry::ETodoCompleted )
            {
            ret = GreaterThan;
            }
        else
            {
            ret = LessThan;
            }
        }

    return ret;
}

int AgendaUtilPrivate::compareNonTimedNotes( const CCalInstance& aInstance1,
                                              const CCalInstance& aInstance2 )
{
    int ret( Equal );
    TTime time1 = aInstance1.Time().TimeUtcL();
    TTime time2 = aInstance2.Time().TimeUtcL();

    if( time1 == time2 )
        {
        time1 = aInstance1.Entry().LastModifiedDateL().TimeUtcL();
        time2 = aInstance2.Entry().LastModifiedDateL().TimeUtcL();

        if( time1 == time2 )
            {
            ret = Equal;
            }
        else if( time1 > time2 )
            {
            ret = GreaterThan; // oldest first
            }
        else
            {
            ret = LessThan;
            }
        }
    else
        {
        if( time1 < time2 )
            {
            ret = LessThan;
            }
        else
            {
            ret = GreaterThan;
            }
        }

    return ret;
}

int AgendaUtilPrivate::compareTimedNotes( const CCalInstance& aInstance1,
                                           const CCalInstance& aInstance2 )
{
    int ret( Equal );
    TTime time1 = aInstance1.Time().TimeUtcL();
    TTime time2 = aInstance2.Time().TimeUtcL();

    if( time1 == time2 )
        {
        TTimeIntervalMinutes duration1;
        TTimeIntervalMinutes duration2;
        aInstance1.EndTimeL().TimeUtcL().MinutesFrom( aInstance1.StartTimeL().TimeUtcL(), duration1 );
        aInstance2.EndTimeL().TimeUtcL().MinutesFrom( aInstance2.StartTimeL().TimeUtcL(), duration2 );

        if( duration1 == duration2 )
            {
            time1 = aInstance1.Entry().LastModifiedDateL().TimeUtcL();
            time2 = aInstance2.Entry().LastModifiedDateL().TimeUtcL();

            if( time1 == time2 )
                {
                ret = Equal;
                }
            else if( time1 > time2 )
                {
                ret = GreaterThan; // oldest first
                }
            else
                {
                ret = LessThan;
                }
            }
        else
            {
            if( duration1 < duration2 )
                {
                ret = LessThan;
                }
            else
                {
                ret = GreaterThan;
                }
            }
        }
    else
        {
        if( time1 < time2 )
            {
            ret = LessThan;
            }
        else
            {
            ret = GreaterThan;
            }
        }

    return ret;
}

RepeatIndex AgendaUtilPrivate::getRepeatIndex(const CCalEntry& aEntry)
{
	RepeatIndex repeatIndex(notRepeated);

	TCalRRule rrule;

	if (aEntry.GetRRuleL(rrule)) {
		TCalRRule::TType type(rrule.Type());
		TInt repeatInterval(rrule.Interval());

		// If repeat type of current note is not supported in Calendar,
		// default repeat value is "Other".
		repeatIndex = repeatOther;

		switch (type) {
			case TCalRRule::EDaily: {
				switch (repeatInterval) {
					case 1:
						repeatIndex = repeatDaily;
						break;
					case 7:
						repeatIndex = repeatWeekly;
						break;
					case 14:
						repeatIndex = repeatBiWeekly;
						break;
					default:
						break;
				}
				break;
			}

			case TCalRRule::EWeekly: {
				AgendaRepeatRule agendaRepeatRule =
				        createAgendaRRuleFromTCalRRule(rrule);
				bool isWorkdaysRepeating =
				        isWorkdaysRepeatingEntry(agendaRepeatRule);

				if (isWorkdaysRepeating) {
					repeatIndex = repeatWorkdays;
				} else {
					RArray<TDay> weekDays(7);
					rrule.GetByDayL(weekDays);
					if (weekDays.Count() == 1) // FIXME: AL - is this necessary?
					{
						switch (repeatInterval) {
							case 1:
								repeatIndex = repeatWeekly;
								break;
							case 2:
								repeatIndex = repeatBiWeekly;
								break;
							default:
								break;
						}
					}

					weekDays.Close();
				}
				break;
			}

			case TCalRRule::EMonthly: {
				RArray<TInt> monthDays(31);
				rrule.GetByMonthDayL(monthDays);

				if (monthDays.Count() == 1) // FIXME: AL - is this necessary?
				{
					switch (repeatInterval) {
						case 1:
							repeatIndex = repeatMonthly;
							break;
							// If interval of repeat is 12 months, 
							// every year is shown in Note Editor, 
							// because it means yearly repeat.
						case 12:
							repeatIndex = repeatYearly;
							break;
						default:
							break;
					}
				}

				monthDays.Close();

				break;
			}
			case TCalRRule::EYearly: {
				if (repeatInterval == 1) {
					repeatIndex = repeatYearly;
				}
				break;
			}

			default: {
				// If repeat type of current note is not supported in Calendar,
				// default repeat value is "Other".
				repeatIndex = repeatOther;
				break;
			}
		}
	}
	return repeatIndex;
}

TTime AgendaUtilPrivate::getPreviousInstanceForRepeatOther(CCalEntry& entry, 
									const CalCommon::TCalTimeRange& timeRange)
{
	RPointerArray<CCalInstance> allInstances;
	CleanupResetAndDestroyPushL(allInstances);
	
	TInt filter;
	// Get the entry type to be filtered
	switch(entry.EntryTypeL())
		{
		case CCalEntry::EAppt:
			filter = CalCommon::EIncludeAppts;
			break;
		case CCalEntry::ETodo:
			filter = CalCommon::EIncludeCompletedTodos | CalCommon::EIncludeIncompletedTodos;
			break;
		case CCalEntry::EEvent:
			filter = CalCommon::EIncludeEvents;
			break;
		case CCalEntry::EReminder:
			filter = CalCommon::EIncludeReminder;
			break;
		case CCalEntry::EAnniv:
			filter = CalCommon::EIncludeAnnivs;
			break;
		default:
			filter = CalCommon::EIncludeAll;
			break;
		};

	iCalInstanceView->FindInstanceL( allInstances, 
									 (CalCommon::TCalViewFilterFlags)filter,
									 timeRange);

	TTime previousTime = Time::NullTTime();
	
	for( TInt i = allInstances.Count() - 1; i >= 0; i-- )
		{
		if( allInstances[i]->Entry().UidL() == entry.UidL() )
			{
			previousTime = allInstances[i]->Time().TimeLocalL();
			break;
			}
		}

	CleanupStack::PopAndDestroy( &allInstances );  
	return previousTime;
}

TTime AgendaUtilPrivate::getNextInstanceForRepeatOther(CCalEntry& aEntry, 
								   const CalCommon::TCalTimeRange& timeRange)
{
	RPointerArray<CCalInstance> allInstances;
	CleanupResetAndDestroyPushL(allInstances);
    
    TInt filter;
    // Get the entry type to be filtered
    switch(aEntry.EntryTypeL())
        {
    	case CCalEntry::EAppt:
    		filter = CalCommon::EIncludeAppts;
    		break;
    	case CCalEntry::ETodo:
    		filter = CalCommon::EIncludeCompletedTodos | CalCommon::EIncludeIncompletedTodos;
    		break;
    	case CCalEntry::EEvent:
    		filter = CalCommon::EIncludeEvents;
    		break;
    	case CCalEntry::EReminder:
    		filter = CalCommon::EIncludeReminder;
    		break;
    	case CCalEntry::EAnniv:
    		filter = CalCommon::EIncludeAnnivs;
    		break;
    	default:
    		filter = CalCommon::EIncludeAll;
    		break;
        };
    
    iCalInstanceView->FindInstanceL( allInstances, 
                                     (CalCommon::TCalViewFilterFlags)filter,
                                     timeRange);
                                     
    TTime nextTime = Time::NullTTime();
    
	TInt i( 0 );
    for( ; i < allInstances.Count(); i++ )
        {
        if( allInstances[i]->Entry().UidL() == aEntry.UidL() )
            {
            nextTime = allInstances[i]->Time().TimeLocalL();
            break;
            }
        }
  
    CleanupStack::PopAndDestroy( &allInstances );  
    return nextTime;
}

TCalTime AgendaUtilPrivate::generateRecurrenceIdFromEntry( CCalEntry& entry, 
														TCalTime instanceDate )
{    
    TDateTime theTime = entry.StartTimeL().TimeUtcL().DateTime();
    TDateTime theDate = instanceDate.TimeUtcL().DateTime();

    theTime.SetYear(theDate.Year());
    theTime.SetMonth(theDate.Month());
    theTime.SetDay(theDate.Day());

    TCalTime toRet;
    toRet.SetTimeUtcL(theTime);
    
    return toRet;
}

bool AgendaUtilPrivate::endsAtStartOfDay( CCalInstance* instance,
                                          const TTime& day )
{
	TTime zero(TInt64(0));
	TTime dayStart = zero + day.DaysFrom( zero );
    const TTime startTime( instance->StartTimeL().TimeLocalL());
    const TTime endTime( instance->EndTimeL().TimeLocalL());

    const bool result( endTime > startTime && endTime == dayStart );

    return result;
}
	
AgendaRepeatRule AgendaUtilPrivate::createAgendaRRuleFromTCalRRule(
															TCalRRule &calRRule)
{
	AgendaRepeatRule agendaRepeatRule;
	RArray<TDay> days;
	calRRule.GetByDayL(days);
	QList<AgendaRepeatRule::Day> qDays;
	for (int i = 0; i < days.Count(); i++) {
		qDays.append(AgendaRepeatRule::Day(days[i]));
	}
	agendaRepeatRule.setByDay(qDays);

	RArray<TMonth> months;
	calRRule.GetByMonthL(months);
	QList<AgendaRepeatRule::Month> qMonths;
	for (int i = 0; i < months.Count(); i++) {
		qMonths.append(AgendaRepeatRule::Month(months[i]));
	}
	agendaRepeatRule.setByMonth(qMonths);

	RArray<TInt> monthDays;
	calRRule.GetByMonthDayL(monthDays);
	QList<int> qMonthDays;
	for (int i = 0; i < monthDays.Count(); i++) {
		qMonthDays.append(monthDays[i] + 1);
	}
	agendaRepeatRule.setByMonthDay(qMonthDays);

	agendaRepeatRule.setType((AgendaRepeatRule::RuleType) (calRRule.Type()));
	agendaRepeatRule.setInterval(calRRule.Interval());
	TCalTime time = calRRule.Until();
	TTime untilTime = time.TimeLocalL();
	QDateTime repeatUntil(QDate(untilTime.DateTime().Year(), 
	                     untilTime.DateTime().Month() + 1, 
	                     untilTime.DateTime().Day() + 1),
	                     QTime(untilTime.DateTime().Hour(),
	                     untilTime.DateTime().Minute()));
	agendaRepeatRule.setUntil(repeatUntil);

	TCalTime dayStart = calRRule.DtStart();
	TDateTime ruleStart = dayStart.TimeLocalL().DateTime();
	QDateTime qRuleStart(QDate(ruleStart.Year(), ruleStart.Month() + 1, 
	          ruleStart.Day() + 1),QTime(ruleStart.Hour(), ruleStart.Minute()));
	agendaRepeatRule.setRepeatRuleStart(qRuleStart);

	TDay wkDay = calRRule.WkSt();
	AgendaRepeatRule::Day qWkDay = (AgendaRepeatRule::Day) wkDay;
	agendaRepeatRule.setWeekStart(qWkDay);
	
	return agendaRepeatRule;

}

TCalRRule AgendaUtilPrivate::createTCalRRuleFromAgendaRRule(
												AgendaRepeatRule &agendaRRule, bool isNonFloating )
{
	TCalRRule
	        repeatRule(
	                   static_cast<TCalRRule::TType> (agendaRRule.type()));
	QDateTime ruleStartDateTime = agendaRRule.repeatRuleStart();
	TDateTime ruleStartCalendarDateTime(ruleStartDateTime.date().year(), 
	                 static_cast<TMonth> (ruleStartDateTime.date().month() - 1), 
	                 ruleStartDateTime.date().day() - 1,
	                 ruleStartDateTime.time().hour(),
	                 ruleStartDateTime.time().minute(),
	                 0, 
	                 0);
	
	QDateTime repeatUntilDate = agendaRRule.until();
	TDateTime repeatTill(repeatUntilDate.date().year(), 
	                   static_cast<TMonth> (repeatUntilDate.date().month() - 1),
	                   repeatUntilDate.date().day() - 1, 
	                   repeatUntilDate.time().hour(), 
	                   repeatUntilDate.time().minute(), 
	                   0, 
	                   0);
	
	TCalTime ruleStartCalTime;
	TCalTime ruleRepeatTillTime;
	if(isNonFloating) {
	    // Use Fixed time for timed entries.
	    ruleStartCalTime.SetTimeLocalL(TTime(ruleStartCalendarDateTime));
	    ruleRepeatTillTime.SetTimeLocalL(TTime(repeatTill));
	}else {
	    // Use Floating time for Non timed entries
	    ruleStartCalTime.SetTimeLocalFloatingL(TTime(ruleStartCalendarDateTime));
	    ruleRepeatTillTime.SetTimeLocalFloatingL(TTime(repeatTill));
	}
	repeatRule.SetDtStart(ruleStartCalTime);
	repeatRule.SetInterval(agendaRRule.interval());
	repeatRule.SetUntil(ruleRepeatTillTime);
	repeatRule.SetWkSt((TDay)agendaRRule.weekStart());

	QList<AgendaRepeatRule::Day> qDays = agendaRRule.byDay();
	RArray<TDay> days;
	for (int i = 0; i < qDays.count(); i++) {
		days.Append(TDay(qDays[i]));
	}
	if (days.Count() > 0) {
		repeatRule.SetByDay(days);
	}

	QList<int> qmonthDay = agendaRRule.byMonthDay();
	RArray<TInt> monthDay;
	for (int i = 0; i < qmonthDay.count(); i++) {
		monthDay.Append(qmonthDay[i] - 1);
	}
	if (monthDay.Count() > 0) {
		repeatRule.SetByMonthDay(monthDay);
	}

	QList<AgendaRepeatRule::Month> qMonth = agendaRRule.byMonth();
	RArray<TMonth> month;
	for (int i = 0; i < qMonth.count(); i++) {
		month.Append(TMonth(qMonth[i]));
	}
	if (month.Count() > 0) {
		repeatRule.SetByMonth(month);
	}
	
	return repeatRule;
}

// End of file  --Don't remove this.
