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

#ifndef __CALENTRY_H__
#define __CALENTRY_H__

#include <calcommon.h>

/** Minimum latitude in degrees.
@see CCalGeoValue::SetLatLongL
@publishedAll
@released
*/
const TReal KCalGEOMinLatitude = -90;

/** Maximum latitude in degrees.
@see CCalGeoValue::SetLatLongL
@publishedAll
@released
*/
const TReal KCalGEOMaxLatitude  = 90;

/** Minimum longitude in degrees.
@see CCalGeoValue::SetLatLongL
@publishedAll
@released
*/
const TReal KCalGEOMinLongitude = -180;

/** Maximum longitude in degrees.
@see CCalGeoValue::SetLatLongL
@publishedAll
@released
*/
const TReal KCalGEOMaxLongitude  = 180;

/** Maximum number of decimal places for longitude and latitude values.
The position specified to CCalGeoValue::SetLatLongL will be truncated 
to KGEOMaxDecimalPoint decimal places
@see CCalGeoValue::SetLatLongL
@publishedAll
@released
*/
const TUint KCalGEOMaxDecimalPlaces = 6;

class CCalAlarm;
class CCalAttachment;
class CCalAttendee;
class CCalCategory;
class CCalEntryImpl;
class CCalGeoValue;
class CCalUser;
class CTzRules;
class TCalRRule;
class CCalEntryIdImpl;

/** Class representing a calendar entry. This can be an appointment, to-do
item, reminder, event, anniversary, note.

CCalEntry contains information about a calendar entry, including
a repeat definition, synchronisation information, and group scheduling data.

A repeating calendar entry has more than one occurrence. Instances are
represented by the CCalInstance class.

Synchronisation information includes a record of the date/time the entry was
last modified. This can be found by calling the LastModifiedDateL() function.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalEntry) : public CBase
	{
public:
	/** Defines the type of the calendar entry.
	@publishedAll
	@released
	*/
	enum TType
		{
		/** An appointment, which has a start time and end time. */
		EAppt,
		/** A to-do, which can have a start time and end time (the end time is the due date), or can be undated. */
		ETodo,
		/** An event, which has a start time and end time. */
		EEvent,
		/** A reminder, which has a start time only. */
		EReminder,
		/** An anniversary, which has a start time and end time. */
		EAnniv,
		/** A text note which has description */
		ENote
		};

	/** Defines the possible status values of the entry.
	There are values here to support both iCalendar (RFC 2445) and vCalendar v1.0 STATUS properties.
	Enumerated values for vCalendar v1.0 contain the prefix 'EVCal'.
	@publishedAll
	@released
	*/
	enum TStatus
		{
		/** Used to support iCalendar (RFC 2445) and vCalendar 1.0. The event is tentative. */
		ETentative,
		/** Used to support iCalendar (RFC 2445) and vCalendar 1.0. The event has been confirmed. */
		EConfirmed,
		/** Used to support iCalendar (RFC 2445). The event has been cancelled. */
		ECancelled,
		/** Used to support iCalendar (RFC 2445). The to-do entry needs action. */
		ETodoNeedsAction,
		/** Used to support iCalendar (RFC 2445) and vCalendar 1.0. The to-do entry has been completed. */
		ETodoCompleted,
		/** Used to support iCalendar (RFC 2445). The to-do entry is in progress. */
		ETodoInProcess,
		/** No status is set. */
		ENullStatus,
		
		/** Used to support vCalendar 1.0. The entry has been accepted. */
		EVCalAccepted,
		/** Used to support vCalendar 1.0. The entry needs action. */
		EVCalNeedsAction, 
		/** Used to support vCalendar 1.0. The entry has been sent. */
		EVCalSent, 
		/** Used to support vCalendar 1.0. The entry has been declined. */
		EVCalDeclined,
		/** Used to support vCalendar 1.0. The entry has been delegated. */
		EVCalDelegated,
		};

	/** Replication status.
	@publishedAll
	@released
	*/
	enum TReplicationStatus
		{
		/** No restriction on access. */
		EOpen,
		/** Data is private (no access). */
		EPrivate,
		/** Data is confidential (restricted access). */
		ERestricted
		};

	/** The method property of a Group Scheduling entry.
	@publishedAll
	@released
	*/
	enum TMethod
		{
		/** The RFC2445-defined method value 'None'. */
		EMethodNone,
		/** The RFC2445-defined method value 'Publish'. */
		EMethodPublish,
		/** The RFC2445-defined method value 'Request'. */
		EMethodRequest,
		/** The RFC2445-defined method value 'Reply'. */
		EMethodReply,
		/** The RFC2445-defined method value 'Add'. */
		EMethodAdd,
		/** The RFC2445-defined method value 'Cancel'. */
		EMethodCancel,
		/** The RFC2445-defined method value 'Refresh'. */
		EMethodRefresh,
		/** The RFC2445-defined method value 'Counter'. */
		EMethodCounter,
		/** The RFC2445-defined method value 'DeclineCounter'. */
		EMethodDeclineCounter
		};

	/** Specify which entry details are to be copied in CCalEntry::CopyFromL functions.
	@publishedAll
	@released
	*/
	enum TCopyType
		{
		/** Copy the ID of the entry */
		ECopyAll,
		/** Dont copy ID of the entry */
		EDontCopyId
		};

	/** The busy status of an entry and corresponds to the TRANSP property in the iCalendar (RFC 2445) and vCalendar specifications.
	In the vCalendar specification, the TRANSP property can be any positive integer value. 0 is opaque and blocks time, 1 
	is transparent and doesn't block time. Any higher values have an implementation-specific meaning.
	In the iCalendar specification (RFC 2445), the TRANSP property can be either OPAQUE (blocks time) or TRANSPARENT (doesn't block time).
	@publishedPartner
	@released
	*/
	enum TTransp
		{
		/** The entry blocks time. */
		ETranspBusy,
		/** The entry doesn't block time. */
		ETranspFree,
		/** The entry is tentative and blocks time. */
		ETranspTentative,
		/** The entry is an out-of-office calendar event and blocks time. */
		ETranspOutOfOffice,
		};

	// Construction / Destruction
	IMPORT_C static CCalEntry* NewL(TType aType, HBufC8* aUid, TMethod aMethod, TUint aSeqNum);
	IMPORT_C static CCalEntry* NewL(TType aType, HBufC8* aUid, TMethod aMethod, TUint aSeqNum,
					const TCalTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange);

	IMPORT_C ~CCalEntry();

	// Date and Time
	IMPORT_C TCalTime StartTimeL() const;
	IMPORT_C TCalTime EndTimeL() const;
	IMPORT_C void SetStartAndEndTimeL(const TCalTime& aStartTime, const TCalTime& aEndTime);

	IMPORT_C void SetLastModifiedDateL();
	IMPORT_C void SetLastModifiedDateL(const TCalTime& aModifiedTime);
	IMPORT_C TCalTime LastModifiedDateL() const;

	IMPORT_C void SetDTStampL(const TCalTime& aTime);
	IMPORT_C TCalTime DTStampL() const;

	IMPORT_C TCalTime CompletedTimeL() const;
	IMPORT_C void SetCompletedL(TBool aCompleted, const TCalTime& aTime);

	// Repeat rules / RDate / ExDate Methods
	IMPORT_C void SetRRuleL(const TCalRRule& aRule);
	IMPORT_C TBool GetRRuleL(TCalRRule& aRule) const;

	IMPORT_C void SetRDatesL(const RArray<TCalTime>& aRDateList);
	IMPORT_C void GetRDatesL(RArray<TCalTime>& aRDateList) const;

	IMPORT_C void SetExceptionDatesL(const RArray<TCalTime>& aExDateList);
	IMPORT_C void GetExceptionDatesL(RArray<TCalTime>& aExDateList) const;

	IMPORT_C void ClearRepeatingPropertiesL();
	
	// Entry alarm
	IMPORT_C void SetAlarmL(CCalAlarm* aAlarm);
	IMPORT_C CCalAlarm* AlarmL() const;

	// Comparison
	IMPORT_C TBool CompareL(const CCalEntry& aEntry) const;

 	// Copy
 	IMPORT_C void CopyFromL(const CCalEntry& aOther);
	IMPORT_C void CopyFromL(const CCalEntry& aOther, TCopyType aCopyType);

	// Text Fields
	IMPORT_C void SetSummaryL(const TDesC& aSummary);
	IMPORT_C const TDesC& SummaryL() const;
	IMPORT_C void SetDescriptionL(const TDesC& aDescription);
	IMPORT_C const TDesC& DescriptionL() const;
	IMPORT_C void SetLocationL(const TDesC& aLocation);
	IMPORT_C const TDesC& LocationL() const;

	// Category list
	IMPORT_C void AddCategoryL(CCalCategory* aCategory);
	IMPORT_C void DeleteCategoryL(TInt aIndex);
	IMPORT_C const RPointerArray<CCalCategory>& CategoryListL();

	//	Attendee Methods
	IMPORT_C void AddAttendeeL(CCalAttendee* aAttendee);
	IMPORT_C void DeleteAttendeeL(TInt aIndex);
	IMPORT_C RPointerArray<CCalAttendee>& AttendeesL() const;
	IMPORT_C void SetOrganizerL(CCalUser* aUser);
	IMPORT_C CCalUser* OrganizerL() const;
	IMPORT_C void SetPhoneOwnerL(const CCalUser* aOwner);
	IMPORT_C CCalUser* PhoneOwnerL() const;

	// Attachments
	IMPORT_C void AddAttachmentL(CCalAttachment& aAttachment);
	IMPORT_C void DeleteAttachmentL(const CCalAttachment& aAttachment);
	IMPORT_C CCalAttachment* AttachmentL(TInt aIndex) const;
	IMPORT_C TInt AttachmentCountL() const;
	// Other Attributes
	IMPORT_C TType EntryTypeL() const;
	IMPORT_C void SetStatusL(TStatus aStatus);
	IMPORT_C TStatus StatusL() const;

	IMPORT_C void SetReplicationStatusL(TReplicationStatus aReplicationStatus);
	IMPORT_C TReplicationStatus ReplicationStatusL() const;

	IMPORT_C void SetPriorityL(TUint aPriority);
	IMPORT_C TUint PriorityL() const;

	IMPORT_C void SetMethodL(TMethod aMethod);
	IMPORT_C TMethod MethodL() const;
	IMPORT_C void SetSequenceNumberL(TInt aSeq);
	IMPORT_C TInt SequenceNumberL() const;
	IMPORT_C const TDesC8& UidL() const;
	IMPORT_C TCalTime RecurrenceIdL() const;
	IMPORT_C CalCommon::TRecurrenceRange RecurrenceRangeL() const;

	IMPORT_C CTzRules* TzRulesL() const;
	IMPORT_C void SetTzRulesL(const CTzRules& aTzRule);
	IMPORT_C void SetTzRulesL();

	IMPORT_C void SetLocalUidL(TCalLocalUid aLocalId);
	IMPORT_C TCalLocalUid LocalUidL() const;

	IMPORT_C TTransp TimeTransparencyL() const;
	IMPORT_C void SetTimeTransparencyL(TTransp aBusyStatus);

	IMPORT_C CCalGeoValue* GeoValueL() const;
	IMPORT_C void SetGeoValueL(const CCalGeoValue& aGeoValue);
	IMPORT_C void ClearGeoValueL();

	IMPORT_C TCalTime NextInstanceForLocalUIDL(const TCalTime& aTime) const;
	IMPORT_C TCalTime PreviousInstanceForLocalUIDL(const TCalTime& aTime) const;

	IMPORT_C TCalTime FindRptUntilTimeL(TInt aCount);

	IMPORT_C void SetFavouriteL(TUint32 aFavourite);
	IMPORT_C TUint32 FavouriteL();
	
public:
	// Internal APIs
	static CCalEntry* NewL(CCalEntryImpl* aImpl);
	CCalEntryImpl* Impl() const;
	TUint8 ShortFileIdL();

private:
	void ConstructL(TType aType, HBufC8* aUid, TMethod aMethod, TUint aSeqNum, const TCalTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange);
	void ConstructL(TType aType, HBufC8* aUid, TMethod aMethod, TUint aSeqNum);
	void ConstructL(CCalEntryImpl* aImpl);

private: // member data
	CCalEntryImpl* iImpl;
	};


/** Class representing a uniquely identifiable calendar entry in a calendar file.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalEntryId) : public CBase
	{
public:
	IMPORT_C static CCalEntryId* NewL(TDesC8* aFlatData);
	IMPORT_C ~CCalEntryId();
	IMPORT_C TPtrC8 IdL();
	IMPORT_C TPtrC StoreFileNameL();
	IMPORT_C TCalTime RecurrenceIdL();
	IMPORT_C TCalTime InstanceTimeL() const;
	IMPORT_C TCalLocalUid LocalUidL() const;
private:
	CCalEntryId();
private:
	CCalEntryIdImpl* iImpl;
	};


/** Class representing the geographical location of a calendar entry.
This corresponds to the GEO property as defined in vCalendar / iCalendar (RFC 2445). It contains a latitude and a longitude.
@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalGeoValue) : public CBase
	{
public:
	IMPORT_C static CCalGeoValue* NewL();
	IMPORT_C ~CCalGeoValue();
	IMPORT_C void SetLatLongL(const TReal& aLatitude, const TReal& aLongitude);
	IMPORT_C TBool GetLatLong(TReal& aLatitude, TReal& aLongitude) const;
private:
	CCalGeoValue();
private:
	TReal iLatitude;
	TReal iLongitude;
	};

#endif // __CALENTRY_H__

