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

#include "calentryimpl.h"
#include "agmentry.h"
#include  <e32math.h>

/** Allocates and constructs a new entry of the specified type, Global UID
and sequence number.

The entry's data is all initialised to default values, e.g.
the start and end dates are reset to null.

The entry takes ownership of the Uid descriptor.

Note that this function will not work unless a CCalSession has been created
by calling CCalSession::NewL().

@param aType One of: EAppt, ETodo, EEvent, EReminder, EAnniv, ENote to create
an appointment, a todo, an event, a reminder or an anniversary entry.
@param aUid The Global UID of the entry. This field is mandatory. NewL takes ownership of the global UID.
@param aMethod The Method value describing the entry. Default value is EGSMethodNone.
@param aSeqNum The sequence number of the entry. Default value is 0.
@return Pointer to the new entry.
@capability None
@publishedAll
@released
*/
EXPORT_C CCalEntry* CCalEntry::NewL(TType aType, HBufC8* aUid, TMethod aMethod, TUint aSeqNum)
	{
	CCalEntry* entry = new (ELeave) CCalEntry();
	CleanupStack::PushL(entry);
	entry->ConstructL(aType, aUid, aMethod, aSeqNum);
	CleanupStack::Pop(entry);
	return entry;
	}

/** Allocates and constructs a new entry of the specified type, Global UID,
sequence number and recurrence ID.

The entry takes ownership of the Uid descriptor.

@param aType One of: EAppt, ETodo, EEvent, EReminder, EAnniv, ENote to create
an appointment, a todo, an event, a reminder or an anniversary entry.
@param aUid The Global UID of the entry. This field is mandatory. NewL takes ownership of the global UID.
@param aMethod The Method value describing the entry.
@param aSeqNum The sequence number of the entry.
@param aRecurrenceId The recurrence ID of the entry.
@param aRange The recurrence ID range of the entry.
@return Pointer to the new entry.
@leave KErrArgument If the UID passed is invalid
@panic CalInterimAPI 8 If the entry type passed is invalid
@capability None
@publishedAll
@released
 */
EXPORT_C CCalEntry* CCalEntry::NewL(TType aType, HBufC8* aUid, TMethod aMethod, TUint aSeqNum,
									const TCalTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange)
	{
	CCalEntry* entry = new (ELeave) CCalEntry();
	CleanupStack::PushL(entry);
	entry->ConstructL(aType, aUid, aMethod, aSeqNum, aRecurrenceId, aRange);
	CleanupStack::Pop(entry);
	return entry;
	}

// Takes ownership of the implementation class
CCalEntry* CCalEntry::NewL(CCalEntryImpl* aImpl)
	{
	CCalEntry* entry = new (ELeave) CCalEntry();
	CleanupStack::PushL(entry);
	entry->ConstructL(aImpl);
	CleanupStack::Pop(entry);
	return entry;
	}

void CCalEntry::ConstructL(TType aType, HBufC8* aUid, TMethod aMethod, TUint aSeqNum)
	{
	iImpl = CCalEntryImpl::NewL(aType, aUid, aMethod, aSeqNum);
	}

void CCalEntry::ConstructL(TType aType, HBufC8* aUid, TMethod aMethod, TUint aSeqNum, const TCalTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange)
	{
	iImpl = CCalEntryImpl::NewL(aType, aUid, aMethod, aSeqNum, aRecurrenceId, aRange);
	}

void CCalEntry::ConstructL(CCalEntryImpl* aImpl)
	{
	iImpl = aImpl;
	}

CCalEntryImpl* CCalEntry::Impl() const
	{
	return iImpl;
	}

/** The destructor frees all resources owned by the entry, prior to its destruction.

@publishedAll
@released
@capability None
 */
EXPORT_C CCalEntry::~CCalEntry()
	{
	delete iImpl;
	}

/** Compares another entry for equality with this one.

@param aEntry Pointer to an entry.
@return ETrue if aEntry is the same as this entry, EFalse if not.
@publishedAll
@released
@capability None
 */
EXPORT_C TBool CCalEntry::CompareL(const CCalEntry& aEntry) const
	{
	return iImpl->CompareL(aEntry);
	}

/** Copies the contents of another entry to this one.
All entry fields will be overwritten, including the UID and the local UID. This is identical to calling CopyFromL(aOther, ECopyAll).

Please note that this function cannot be called under the following conditions:
- if a file attachment has been added to this entry by its file handle, AND
- if that entry has not been stored in the calendar store yet.

 @param aOther Reference to source entry.
 @panic CalInterimAPI 7 If the entry type is invalid
 @publishedAll
 @released
 @capability None
  */

EXPORT_C void CCalEntry::CopyFromL(const CCalEntry& aOther)
	{
	iImpl->CopyFromL(*aOther.iImpl, ECopyAll);
	}


/** Copies the contents of another entry to this one.
The entry fields that are copied can be specified using the TCopyType parameter.

@param aOther Reference to source entry.
@param aCopyType Which entry details are to be copied.
 ECopyAll if original entry and new entry should be identical. This is identical to calling CopyFromL(aOther).
 EDontCopyId if UID and local UID should not be copied, this should be used when creating a modifying entry.
@panic CalInterimAPI 7 If the entry type is invalid
@publishedAll
@released
@capability None
 */

EXPORT_C void CCalEntry::CopyFromL(const CCalEntry& aOther, TCopyType aCopyType)
 	{
	iImpl->CopyFromL(*aOther.iImpl, aCopyType);
 	}

/** Set the alarm for the entry.

This doesn't take ownership of the parameter.
This function will leave with KErrNotSupported if the alarm is not on same day.
@param aAlarm Pointer to the alarm, or NULL if the alarm is to be cleared.
@leave KErrNotSupported if the alarm offset is less than KCalAlarmMinValidOffset.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetAlarmL(CCalAlarm* aAlarm)
	{
	iImpl->SetAlarmL(aAlarm);
	}

/** Creates an alarm class containing data for this entry's alarm.

@return Pointer to a new alarm object, or NULL if this entry doesn't have one. Returns ownership.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalAlarm* CCalEntry::AlarmL() const
	{
	return iImpl->AlarmL();
	}

/** Sets the entry's repeat definition.

The repeat definition includes daily, weekly, monthly and yearly repeat
details, and start and end details.
 
If the start time of the entry is different from the start time of the 
repeat rule, the entry's start time will be used.
Also, the start time of the entry may be adjusted if it does not match an 
instance of the repeat rule.

This does not take ownership of the parameter.
@leave KErrNotSupported If the repeat rule passed is not one that is currently supported
@leave KErrArgument if this is a todo entry and its end date has not been set or this is a non-todo entry and its start date has not been set.
@leave KErrArgument if the until date of the repeat definition is non-NULL and is earlier than the start date.
@param aRpt The new repeat definition.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetRRuleL(const TCalRRule& aRpt)
	{
	return iImpl->SetRRuleL(aRpt);
	}

/** Get the entry's repeat rule.

In the returned TCalRRule, both the Count and the Until values can be accessed. However, in
the case of a repeat rule which repeats forever, the Count will always be returned as 0.

@param aRule On return contains the entry's repeat rule data, if present.
@return ETrue if the entry has a repeat rule, EFalse if not.
@publishedAll
@released
@capability None
*/
EXPORT_C TBool CCalEntry::GetRRuleL(TCalRRule& aRule) const
	{
	return Impl()->GetRRuleL(aRule);
	}

/** Clears the repeat details including exceptions and unsets the entry's 'is repeating'
property.
@capability None
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::ClearRepeatingPropertiesL()
	{
	iImpl->ClearRepeatingPropertiesL();
	}

/**
Gets the last modified date/time for this entry.

The last modified date of an entry is updated when:

- an entry is created using CCalEntry::NewL()
- an entry is updated in the calendar store using CCalEntryView::UpdateL()
- an existing entry is updated in the calendar store using CCalEntryView::StoreL()
- the client calls CCalEntry::SetLastModifiedDateL() on the entry
- the time zone rules (if any) associated with the entry are updated

@return The time at which the entry was last modified.

@publishedAll
@released
*/
EXPORT_C TCalTime CCalEntry::LastModifiedDateL() const
	{
	return iImpl->LastModifiedDateL();
	}

/** Gets the entry's DTSTAMP.
@return The DTSTAMP of the entry.
@publishedAll
@released
*/
EXPORT_C TCalTime CCalEntry::DTStampL() const
	{
	return iImpl->DTStampL();
	}

/** Sets the contents of the entry's location field.

@param aLocation Descriptor containing a location, e.g. for a meeting.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetLocationL(const TDesC& aLocation)
	{
	iImpl->SetLocationL(aLocation);
	}

/** Gets the contents of the entry's location field.

@return The location field.
@publishedAll
@released
@capability None
*/
EXPORT_C const TDesC& CCalEntry::LocationL() const
	{
	return iImpl->LocationL();
	}

/** Gets a list of the entry's categories.

@return An array of categories.
@publishedAll
@released
@capability None
*/
EXPORT_C const RPointerArray<CCalCategory>& CCalEntry::CategoryListL()
	{
	return iImpl->CategoryListL();
	}

/** Appends a category to the entry's list of categories.

The entry takes ownership of the category specified.

@param aCategory The category to be added. This function takes ownership of the CCalCategory immediately so it should not be 
put on the cleanup stack before calling this function.
@leave KErrArgument if aCategory is NULL.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::AddCategoryL(CCalCategory* aCategory)
	{
	iImpl->AddCategoryL(aCategory);
	}

/** Deletes a category from the entry's list of categories.

@param aIndex The category to be deleted.
@leave KErrArgument If the index is out of range. Check CCalEntry::CategoryListL first to find out the range of allowed indexes.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::DeleteCategoryL(TInt aIndex)
	{
	iImpl->DeleteCategoryL(aIndex);
	}

/** Sets the priority for the entry.

The default value is zero.

@param aPriority Priority value between 0 and 255. The behaviour is undefined if the priority is greater than 255.
@leave KErrArgument if the priority value is not between 0 and 255.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetPriorityL(TUint aPriority)
	{
	__ASSERT_ALWAYS(aPriority <= 255, User::Leave(KErrArgument));
	iImpl->SetPriorityL(aPriority);
	}

/** Gets the priority of the entry.

@return Priority value between 0 and 255.
@publishedAll
@released
@capability None
*/
EXPORT_C TUint CCalEntry::PriorityL() const
	{
	return iImpl->PriorityL();
	}

/** Gets the start date/time for an entry.

@return The entry's start date/time.
@publishedAll
@released
@capability None
*/
EXPORT_C TCalTime CCalEntry::StartTimeL() const
	{
	return iImpl->StartTimeL();
	}

/** Gets the entry's end date/time.

@return The entry's end date/time.
@publishedAll
@released
@capability None
*/
EXPORT_C TCalTime CCalEntry::EndTimeL() const
	{
	return iImpl->EndTimeL();
	}

/** Sets the start and end date/times for an entry.

For an appointment, event or anniversary, the start time must be earlier than or equal to the end time.
For a reminder, the end time will be ignored.
For a todo entry, the end time is the 'due date'. If the end time is Time::NullTTime in a todo, then
the todo is undated - in this case both start and end date are set to Time::NullTTime. 
Only todos can be undated.

@param aStartDateTime The entry's new start date/time.
@param aEndDateTime The entry's new end date/time.
@leave KErrArgument If the start and end times are invalid.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetStartAndEndTimeL(const TCalTime& aStartDateTime, const TCalTime& aEndDateTime)
	{
	iImpl->SetStartAndEndTimeL(aStartDateTime, aEndDateTime);
	}


/** Gets a to-do entry's completed date.

This is the date on which the required action was carried out.
If the entry has not been crossed out, or if this is not a to-do entry, then
this function returns a null value.

@return The to-do entry's crossed out date.

@publishedAll
@released
@capability None
*/
EXPORT_C TCalTime CCalEntry::CompletedTimeL() const
	{
	return iImpl->CompletedTimeL();
	}

/** Sets the crossed out date for a to-do entry.
This is the date on which the required action was carried out.

If this function is called on an entry which is not a to-do, it will leave
with KErrNotSupported.

@param aCompleted ETrue if the to-do entry is complete - the entry's status is
set to ETodoCompleted; EFalse if the to-do entry is not complete - the entry's status is
set to ETodoNeedsAction
@param aDate The to-do entry's crossed out date. The time mode should not be
floating, it will leave with KErrArgument if it is.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetCompletedL(TBool aCompleted, const TCalTime& aDate)
	{
	// It doesn't make sense for the completed time to be floating.
	if (aDate.TimeMode() == TCalTime::EFloating)
	    {
	    User::Leave( KErrArgument );
	    }

	iImpl->SetCompletedL(aCompleted, aDate);
	}

/** Sets the summary text for this entry.

@param aSummary The summary text.
@leave KErrNotFound If a handle to the entry's summary could not be obtained
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetSummaryL(const TDesC& aSummary)
	{
	iImpl->SetSummaryL(aSummary);
	}

/** Gets the summary for the entry.

@return Descriptor pointer to the summary text.
@leave KErrNotFound If a handle to the entry's summary could not be obtained
@publishedAll
@released
@capability None
*/
EXPORT_C const TDesC& CCalEntry::SummaryL() const
	{
	return iImpl->SummaryL();
	}

/** Sets the description text for this entry.

@param aDescription The summary text.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetDescriptionL(const TDesC& aDescription)
	{
	iImpl->SetDescriptionL(aDescription);
	}

/** Gets the description text for the entry.

@return Descriptor pointer to the description text.

@publishedAll
@released
*/
EXPORT_C const TDesC& CCalEntry::DescriptionL() const
	{
	return iImpl->DescriptionL();
	}

/** Gets the type of entry.

@return The entry type. One of: EAppt, ETodo, EEvent, EReminder, EAnniv, ENote.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalEntry::TType CCalEntry::EntryTypeL() const
	{
	return iImpl->EntryTypeL();
	}

/** Sets the entry status.

For a to-do entry, this is one of: ECancelled, ETodoNeedsAction,
ETodoCompleted, ETodoInProgress.

For any other entry types, this is one of: ECancelled, ETentative,
EConfirmed.

Passing a value in which does not match the entry type will result
in a panic.

@param aStatus The status.
@leave KErrArgument If the status passed in is invalid
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetStatusL(TStatus aStatus)
	{
	iImpl->SetStatusL(aStatus);
	}


/** Sets the entry replication status.
@param aReplicationStatus the replication status.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetReplicationStatusL(TReplicationStatus aReplicationStatus)
	{
	iImpl->SetReplicationStatusL(aReplicationStatus);
	}

/** Gets the entry replication status.

This is defined by TReplicationStatus. Default is TReplicationStatus::EOpen

@return The replication status.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalEntry::TReplicationStatus CCalEntry::ReplicationStatusL() const
	{
	return iImpl->ReplicationStatusL();
	}

/** Gets the entry status.

For a to-do entry, this is one of: ECancelled, ETodoNeedsAction,
ETodoCompleted, ETodoInProgress.

For any other entry types, this is one of: ECancelled, ETentative,
EConfirmed.

@return The status.
@panic CalInterimAPI 4 The status of the entry is invalid
@publishedAll
@released
@capability None
*/
EXPORT_C CCalEntry::TStatus CCalEntry::StatusL() const
	{
	return iImpl->StatusL();
	}


/** Sets the entry's method property for group scheduling.

@param aMethod The method, this is one of: EMethodNone, EMethodPublish,
EMethodRequest, EMethodReply, EMethodAdd, EMethodCancel, EMethodRefresh,
EMethodCounter, EMethodDeclineCounter.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetMethodL(TMethod aMethod)
	{
	if(aMethod<EMethodNone || aMethod>EMethodDeclineCounter)
		{
		User::Leave(KErrArgument);
		}
	iImpl->SetMethodL(aMethod);
	}

/** Sets the entry's sequence number for group scheduling.

@param aSeqNum The sequence number.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetSequenceNumberL(TInt aSeqNum)
	{
	iImpl->SetSequenceNumberL(aSeqNum);
	}

/** Sets the entry's last modified date to the current time.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetLastModifiedDateL()
	{
	iImpl->SetLastModifiedDateL();
	}
	
/** Sets the entry's last modified date to the time provided.

@publishedAll
@released
@param aModifiedTime The time used to set the last modified date.
@capability None
*/
EXPORT_C void CCalEntry::SetLastModifiedDateL(const TCalTime& aModifiedTime)
	{
	iImpl->SetLastModifiedDateL(aModifiedTime);
	}

/** Sets the entry's DTSTAMP to the time specified.

This method may be called by clients to record the date/time that the entry
was prepared for sending to a recipient as a scheduling object, as specified
in  RFC 2445, the Internet Calendaring and Scheduling Core Object Specification
iCalendar).  
This value will be Time::NullTTime by default unless the client sets it otherwise.

@param aDTStampTime The time that the DTSTAMP will be set to
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetDTStampL(const TCalTime& aDTStampTime)
	{
	iImpl->SetDTStampL(aDTStampTime);
	}

/** Returns the entry's method property for group scheduling.

@return The method, this is one of: EMethodNone, EMethodPublish,
EMethodRequest, EMethodReply, EMethodAdd, EMethodCancel, EMethodRefresh,
EMethodCounter, EMethodDeclineCounter.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalEntry::TMethod CCalEntry::MethodL() const
	{
	return iImpl->MethodL();
	}

/** Returns the entry's recurrence ID for group scheduling.

@return The recurrence ID.

@publishedAll
@released
@capability None
*/
EXPORT_C TCalTime CCalEntry::RecurrenceIdL() const
	{
	return iImpl->RecurrenceIdL();
	}

/** Returns the recurrence range for the entry's recurrence ID.

@return The recurrence range.

@publishedAll
@released
@capability None
*/
EXPORT_C CalCommon::TRecurrenceRange CCalEntry::RecurrenceRangeL() const
	{
	return iImpl->RecurrenceRangeL();
	}

/** Returns the entry's sequence number for group scheduling.

@return The sequence number.

@publishedAll
@released
@capability None
*/
EXPORT_C TInt CCalEntry::SequenceNumberL() const
	{
	return iImpl->SequenceNumberL();
	}

/** Returns the entry's Global UID for group scheduling.

@return The Global UID.

@publishedAll
@released
@capability None
*/
EXPORT_C const TDesC8& CCalEntry::UidL() const
	{
	return iImpl->UidL();
	}

/** Gets the timezone rules for this entry's repeat rule.

@return A copy of the entry's timezone rules.

@publishedAll
@released
@capability None
*/
EXPORT_C CTzRules* CCalEntry::TzRulesL() const
	{
	return iImpl->GetTzRulesL();
	}

/** Sets the timezone rules for this entry's repeat rule.

@param aTzRules The entry's timezone rules. This function does not take ownership.
@leave KErrNotFound If there is not a repeat definition in the entry
@leave KErrArgument If the entry uses floating time mode
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetTzRulesL(const CTzRules& aTzRules)
	{
	iImpl->SetTzRulesL(aTzRules);
	}

/** Creates the timezone rules for this entry's repeat rule from the local time zone settings.
@leave KErrNotFound If there is not a repeat definition in the entry
@leave KErrArgument If the entry uses floating time mode
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetTzRulesL()
	{
	iImpl->SetTzRulesL();
	}

/** Replaces this group scheduling entry's list of RDATEs.

@param aRDates List of RDATEs to replace current RDATEs.

@publishedAll
@released
@capability None
@leave KErrArgument If the time mode of the RDATEs don't match the time mode of the entry
*/
EXPORT_C void CCalEntry::SetRDatesL(const RArray<TCalTime>& aRDates)
	{
	iImpl->SetRDatesL(aRDates);
	}

/** Gets a list of all RDATEs in this group scheduling entry.

@param aRDates On return, an array of RDATEs for this entry.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::GetRDatesL(RArray<TCalTime>& aRDates) const
	{
	iImpl->GetRDatesL(aRDates);
	}

/** Adds the given exception dates to this group scheduling entry.

@param aExDates The exception dates to be added.

@publishedAll
@released
@capability None
@leave KErrArgument If the time mode of the exception dates don't match the time mode of the entry
*/
EXPORT_C void CCalEntry::SetExceptionDatesL(const RArray<TCalTime>& aExDates)
	{
	iImpl->SetExceptionDatesL(aExDates);
	}

/** Gets a list of all exception dates on this group scheduling entry.

@param aExDateList On return, an array of exception dates for this entry.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::GetExceptionDatesL(RArray<TCalTime>& aExDateList) const
	{
	iImpl->GetExceptionDatesL(aExDateList);
	}

/** Gets the attendees for this entry.

@return An array of attendees. Note that this array should only be used to fetch attendee details. 
Any modifications to this array will not be reflected in the entry's actual attendee list.

@publishedAll
@released
@capability None
*/
EXPORT_C RPointerArray<CCalAttendee>& CCalEntry::AttendeesL() const
	{
	return iImpl->AttendeesL();
	}

/** Adds an attendee for this entry.

@param aAttendee The attendee to be added. This takes ownership of the CCalAttendee immediately so it should 
not be pushed onto the cleanup stack before calling this function.
@leave KErrArgument If the pointer is NULL.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::AddAttendeeL(CCalAttendee* aAttendee)
	{
	iImpl->AddAttendeeL(aAttendee);
	}

/** Deletes an attendee from this entry.

@param aIndex The index of the attendee (found in FetchAttendeesL) to be removed.
@leave KErrArgument If the Index passed in is out of bounds
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::DeleteAttendeeL(TInt aIndex)
	{
	iImpl->DeleteAttendeeL(aIndex);
	}

/** Sets the organizer for this entry.

@param aUser The organizer. This takes ownership of the CCalUser immediately so it should 
not be pushed onto the cleanup stack before calling this function.
@leave KErrArgument If the pointer is NULL.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetOrganizerL(CCalUser* aUser)
	{
	iImpl->SetOrganizerL(aUser);
	}

/** Sets the phone owner for this entry.

@param aOwner The phone owner (entry takes ownership).
@leave KErrArgument If the pointer is NULL.
@publishedAll
@released
@capability None
*/
EXPORT_C void CCalEntry::SetPhoneOwnerL(const CCalUser* aOwner)
	{
	iImpl->SetPhoneOwnerL(aOwner);
	}

/** Gets the organizer for this entry.
This function creates a new CCalUser object containing the organizer data.

@return The organizer if present, NULL if not (Caller takes ownership).

@publishedAll
@released
@capability None
*/
EXPORT_C CCalUser* CCalEntry::OrganizerL() const
	{
	return iImpl->OrganizerL();
	}

/** Sets the calendar local id for this entry.
This should be only used to set the same unique id on a new entry which is the replacement of
an existing one in the Calendar store.
@publishedAll
@released 
@param aLocalId The calendar local id for this entry.
@capability None
*/
EXPORT_C void CCalEntry::SetLocalUidL(TCalLocalUid aLocalId)
	{
	iImpl->SetLocalUidL(aLocalId);
	}

/** Gets the calendar local id of this entry.
Note that this is the unique (for each Calendar file) Id of the entry allocated by
the Calendar server internally and will remain unchanged for lifetime of the entry.
@return The calendar local id.
@publishedAll
@released 
@capability None
*/
EXPORT_C TCalLocalUid CCalEntry::LocalUidL() const
	{
	return iImpl->LocalUidL();
	}

/** Gets the phone owner for this entry.
This function creates a new CCalUser object containing the phone owner data.

@return The phone owner if present, NULL if not (Caller takes ownership).

@publishedAll
@released
@capability None
*/
EXPORT_C CCalUser* CCalEntry::PhoneOwnerL() const
	{
	return iImpl->PhoneOwnerL();
	}

/** Gets the busy status for this entry.
The default value for this property is busy.
@see ETranspBusy
@return The busy status, as defined in CCalEntry::TTransp enumeration.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalEntry::TTransp CCalEntry::TimeTransparencyL() const
	{
	return iImpl->BusyStatusL();
	}

/** Sets the busy status for this entry.
@param aBusyStatus The busy status, as defined in CCalEntry::TTransp enumeration.

@pre None
@post The entry's busy status will be set to the given value.
@leave KErrArgument if aBusyStatus is a value less than 0.
Otherwise one of the system wide error codes.

@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalEntry::SetTimeTransparencyL(CCalEntry::TTransp aBusyStatus)
	{
	iImpl->SetBusyStatusL(aBusyStatus);
	}

/** Gets the geographic location for this entry, known as the GEO property in vCalendar and iCalendar (RFC 2445).
@return A new CCalGeoValue object containing the geographic location. Ownership is returned.

@pre None.
@post A geographical location object (CCalGeoValue) has been created containing the entry's GEO values if they are present. 
If they are not present, nothing happens and NULL is returned.
@publishedPartner
@released
@capability None
*/
EXPORT_C  CCalGeoValue* CCalEntry::GeoValueL() const
	{
	return iImpl->GeoValueL();
	}

/** Sets the geographic location for this entry, known as the GEO property in vCalendar and iCalendar (RFC 2445).
@param aGeoValue The geographic location. 
Note that if the latitude and longitude are not set, then the GEO property will be reset on this entry.
@pre A geographical location object (CCalGeoValue) has been created.
@post The entry's GEO property is set to the given value.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalEntry::SetGeoValueL(const CCalGeoValue& aGeoValue)
	{
	iImpl->SetGeoValueL(aGeoValue);
	}
	
/** Clears the geographic location for this entry, known as the GEO property in vCalendar and iCalendar (RFC 2445).
@pre None.
@post The entry's GEO property is cleared.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalEntry::ClearGeoValueL()
	{
	iImpl->ClearGeoValueL();
	}

/** Adds an attachment to this entry.
If the attachment contains a file handle, that file will be moved to the calendar store when the entry is stored.
@param aAttachment The attachment to add. CCalEntry takes ownership of the attachment.
@pre None
@post The attachment has been added to the entry's list of attachments.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalEntry::AddAttachmentL(CCalAttachment& aAttachment)
	{
	iImpl->AddAttachmentL(&aAttachment);
	}

/** Removes an attachment from an entry.
If the entry does not contain this attachment, nothing happens and the entry's attachment count will stay the same.
@param aAttachment The attachment to remove.

@pre None
@post The attachment has been removed from the entry's list of attachments if it was present.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalEntry::DeleteAttachmentL(const CCalAttachment& aAttachment)
	{
	iImpl->DeleteAttachmentL(aAttachment);
	}

/** Returns an attachment on the entry.
@param aIndex The index of the attachment to be returned.
@return The attachment. Ownership is not returned. NULL is returned if aIndex is negative 
or greater than the number of attachments on this entry.

@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalAttachment* CCalEntry::AttachmentL(TInt aIndex) const
	{
	return iImpl->AttachmentL(aIndex);
	}

/** Returns the number of attachments on the entry.
@return The number of attachments.

@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TInt CCalEntry::AttachmentCountL() const
	{
	return iImpl->AttachmentCountL();
	}

/** Find the next instance of this entry identified by its local UID after the specified time.
If the entry is not repeating and aTime is before the entry start time, then this function returns 
the entry start time. (Or end time for Todos)
If the entry has a repeat rule or sporadic dates, this function returns the next instance.
Instances with exceptions will not be returned. 
Instances of other entries with the same GUID will not be returned - this means modifying instance will
not be returned.
If aTime is an instance, then the next instance after that is returned.
If no instance exists after aTime, then TCalTime will contain Time::NullTTime().

@return The time of the next instance of this entry. This is Time::NullTTime() if no instance is found.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TCalTime CCalEntry::NextInstanceForLocalUIDL(const TCalTime& aTime) const
	{
	return iImpl->NextInstanceForLocalUIDL(aTime);
	}

/** Find the previous instance of this entry identified by its local UID before the specified time.
If the entry is not repeating and aTime is after the entry start time, then this function returns 
the entry start time. (Or end time for Todos)
If the entry has a repeat rule or sporadic dates, this function returns the previous instance.
Instances with exceptions will not be returned.
Instances of other entries with the same GUID will not be returned - this means modifying instance will
not be returned.
If aTime is an instance, then the previous instance is returned.
If no instance exists before aTime, then TCalTime will contain Time::NullTTime().

@return The time of the previous instance of this entry. This is Time::NullTTime() if no instance is found.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TCalTime CCalEntry::PreviousInstanceForLocalUIDL(const TCalTime& aTime) const
	{
	return iImpl->PreviousInstanceForLocalUIDL(aTime);
	}

/** Find the until time of an entry repeat rule using the given number of repeat instances.
   
@return The until time of this entry calculated from the number of repeat instances expected. This will return Time::NullTTime() if it cannot calculate it.
@pre None
@post None
@internalTechnology
@capability None
 */
EXPORT_C TCalTime CCalEntry::FindRptUntilTimeL(TInt aCount) 
	{
	return iImpl->FindRptUntilTimeL(aCount);
	}

TUint8 CCalEntry::ShortFileIdL()
	{
	return iImpl->ShortFileIdL();
	}

/** Set the entry as favourite
   
@param aFavourite value for favourite
	positive integer for setting entry as favourite
	zero for unmarking as favourite
	
@pre None
@post None
@capability None
 */
EXPORT_C void CCalEntry::SetFavouriteL(TUint32 aFavourite)
	{
	iImpl->SetUserIntL(aFavourite);
	}

/** Get favourite property from the entry

@return TUint32 return value for favourite
	positive integer if entry is favourite
	zero if entry is not favourite 
@capability None
 */
EXPORT_C TUint32 CCalEntry::FavouriteL()
	{
	return iImpl->UserIntL();
	}

// CCalEntryId //


/** Allocates and constructs IDs for new entry of the specified type.

@param aFlatData Pointer to the alarm data
@return Pointer to the new entry ID.
@leave KErrArgument if aFlatData is a NULL pointer.
@capability None
@publishedAll
@released
*/
EXPORT_C CCalEntryId* CCalEntryId::NewL(TDesC8* aFlatData)
	{
	CCalEntryId* self = new(ELeave) CCalEntryId();
	CleanupStack::PushL(self);
	self->iImpl = CCalEntryIdImpl::NewL(aFlatData);
	CleanupStack::Pop();
	return self;
	}
/**
Constructor
@internalTechnology
*/
CCalEntryId::CCalEntryId()
	{
	}
/**
Destructor
@internalTechnology
*/
EXPORT_C CCalEntryId::~CCalEntryId()
	{
	delete iImpl;
	}
/** Fetches the entry's ID

@publishedAll
@released
@capability None
@return Id of the entry
*/
EXPORT_C TPtrC8 CCalEntryId::IdL()
	{
	return iImpl->IdL();
	}

/** Stores the name of the file

@publishedAll
@released
@capability None
@return Pointer to the name of the file
*/
EXPORT_C TPtrC CCalEntryId::StoreFileNameL()
	{
	return iImpl->StoreFileNameL();
	}

/** Fetches the recurrence ID of the entry

@publishedAll
@released
@capability None
@return Pointer to the recurrence ID
*/
EXPORT_C TCalTime CCalEntryId::RecurrenceIdL()
	{
	return iImpl->RecurrenceIdL();
	}

/** Fetches the instance time of the entry

@publishedAll
@released
@capability None
@return Pointer to the instance time of the entry
*/
EXPORT_C TCalTime CCalEntryId::InstanceTimeL() const
	{
	return iImpl->InstanceTimeL();
	}

/** Fetches the local ID of the entry

@publishedAll
@released
@capability None
@return Pointer to the local ID of the entry
*/
EXPORT_C TCalLocalUid CCalEntryId::LocalUidL() const
	{
	return iImpl->LocalUidL();
	}

// CCalGeoValue //

/** Creates a new object to store geographic location.
@return A pointer to the new object.
@pre None
@post A new geographic location object is constructed
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalGeoValue* CCalGeoValue::NewL()
	{
	CCalGeoValue* self = new (ELeave) CCalGeoValue();
	return self;
	}

/** Constructor.
@pre None.
@post First phase of construction
@publishedPartner
@released
@capability None
*/
CCalGeoValue::CCalGeoValue():iLatitude(KGEODefaultValue),iLongitude(KGEODefaultValue)
	{
	}

/** Destructor.
@pre Construction of a geographic location object
@post The object is destroyed
@publishedPartner
@released
@capability None
*/
EXPORT_C CCalGeoValue::~CCalGeoValue()
	{
	}

/** Sets the latitude and longitude positions for the geographical location.
@param aLatitude The latitude in degrees. Minutes are represented as a decimal fraction, these will be rounded to 6 
decimal places. Latitude may be from -90 to 90. A positive latitude indicates a position north of the equator. 
A negative latitude is south of it.
@param aLongitude The longitude in degrees. Minutes are represented as a decimal fraction, these will be rounded to 6 
decimal places. Longitude may be from -180 to 180. A positive longitude indicates a position east of the prime meridian. 
A negative longitude is west of it.
@leave KErrArgument if the latitude or longitude are out of range
@pre None
@post The latitude and longitude of this object have been updated.
@publishedPartner
@released
@capability None
*/
EXPORT_C void CCalGeoValue::SetLatLongL(const TReal& aLatitude, const TReal& aLongitude)
	{
	// Bounds checking
	__ASSERT_ALWAYS(
	((aLatitude >= KCalGEOMinLatitude && aLatitude <= KCalGEOMaxLatitude) || aLatitude == KGEODefaultValue) 
	&& ((aLongitude >= KCalGEOMinLongitude && aLongitude <= KCalGEOMaxLongitude) || aLongitude == KGEODefaultValue), 
	User::Leave(KErrArgument));
	
	// Truncate to KGEOMaxDecimalPoint decimal places
	TReal truncatedLatitude;
	TReal truncatedLongitude;
	Math::Round(truncatedLatitude,aLatitude,KCalGEOMaxDecimalPlaces);
	Math::Round(truncatedLongitude,aLongitude,KCalGEOMaxDecimalPlaces);
		
	iLatitude = truncatedLatitude;
	iLongitude = truncatedLongitude;
	}

/** Gets the latitude and longitude positions for the geographical location.
@param aLatitude On return, this contains the value of the latitude in degrees. Minutes are represented as a decimal fraction.
Latitude may be from -90 to 90. A positive latitude indicates a position north of the equator. 
@param aLongitude On return, this contains the value of the longitude in degrees. Minutes are represented as a decimal fraction.
Longitude may be from -180 to 180. A positive longitude indicates a position east of the prime meridian. 
@return EFalse if the latitude and longitude have not been set on this object. ETrue if they have been set and are both valid.
@pre None
@post None
@publishedPartner
@released
@capability None
*/
EXPORT_C TBool CCalGeoValue::GetLatLong(TReal& aLatitude, TReal& aLongitude) const
	{
	if(iLongitude != KGEODefaultValue && iLongitude != KGEODefaultValue)
		{
		aLatitude = iLatitude;
		aLongitude = iLongitude;
		return ETrue;
		}
	return EFalse;
	}
