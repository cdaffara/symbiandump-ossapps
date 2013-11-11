// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGMENTRY_H__
#define __AGMENTRY_H__

#include "agmsimpleentry.h"
#include "agmgsentry.h"

// ------------------------- Referenced clases ------------------------------

class CAgnContent;
class CAgnAttendee;
class CAgnAttachment;
class CAgnCategory;
// -------------------------------------------------------------------------

const TReal KGEODefaultValue = -999.999;

/**
 When an entry is added, this struct returns information from the server to the client
 about any changes/assignment of IDs related to the entry, so that the client-side entry 
 can be updated.
@internalComponent
*/
struct TAgnEntryParameters
	{
	TAgnEntryId iEntryId;
	TCalLocalUid iLocalId;
	TTime iRecurrenceId;
	TTime iLastModifiedDateUtc;
	CalCommon::TRecurrenceRange iRecurrenceRange;
	TStreamId iTzStreamId;
	TBool iSystemTzRule;
	};

NONSHARABLE_CLASS(CAgnEntry) : public CAgnSimpleEntry
/** Class representing a full agenda entry.

CAgnEntry adds common entry information including summary, description, location,
categories and attendees to CAgnSimpleEntry.

To find the type of an entry or instance, use CAgnSimpleEntry::Type().

On the client-side, a CAgnEntry is created whenever CCalEntry::NewL is called, and whenever a 
CCalEntry is fetched from the calendar store. When a CCalInstance is fetched from the calendar store, 
it will contain a 'light entry' (i.e. a CAgnSimpleEntry), but if any operation is made on that light
entry which cannot be handled by CAgnSimpleEntry, the full CAgnEntry will be loaded.

On the server-side, CAgnEntry is externalized and internalized to streams in the calendar file. 
CAgnEntry is not used in any of the indexes.

@internalComponent
@released
*/
	{
public:

	/** Used with summary, description and alarm action to indicate if the data has changed.
	 These items are all stored on file in separate streams to the entry that owns them.*/
	enum TAgnDataChange
		{
		EAgnDataNoChange, 
		EAgnDataDeleted, 
		EAgnDataAdded,
		EAgnDataUpdated
		};

	// construction / destruction
	IMPORT_C static CAgnEntry* NewL(CCalEntry::TType aType);
	IMPORT_C static CAgnEntry* NewL(RReadStream& aStream);
	IMPORT_C static CAgnEntry* NewL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum);
	IMPORT_C static CAgnEntry* NewL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum, 
									const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange);

	IMPORT_C ~CAgnEntry();

	// Alarm properties
	IMPORT_C const TDesC& AlarmSoundName() const;
	IMPORT_C void SetAlarmSoundNameL(const TDesC& aAlarmSoundName);
	IMPORT_C void ClearAlarm();

	IMPORT_C void SetAlarmAction(CAgnContent* aAlarmAction);
	IMPORT_C CAgnContent* AlarmAction() const;
	IMPORT_C TBool AlarmActionIsLoaded() const;
	IMPORT_C TAgnDataChange AlarmActionChange() const;	

	IMPORT_C void SetAlarmActionStreamId(const TStreamId& aStreamId);
	IMPORT_C const TStreamId& AlarmActionStreamId() const;

	// Synchronisation status
	IMPORT_C void SetReplicationStatusL(CCalEntry::TReplicationStatus aReplicationStatus);	
	IMPORT_C CCalEntry::TReplicationStatus ReplicationStatus() const;

	// Persistence
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);

	IMPORT_C void ExternalizeToBufferL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeFromBufferL(RReadStream& aStream);
	
	IMPORT_C void VerifyBeforeStoreL();

	// copy and compare
	IMPORT_C TBool CompareL(const CAgnEntry& aEntry) const;
	IMPORT_C void CopyFromL(const CAgnEntry& aSource, CCalEntry::TCopyType aCopyType);
	IMPORT_C CAgnEntry* CloneL() const;

	// summary, location, description
	IMPORT_C void SetSummary(HBufC* aSummary); // takes ownership
	IMPORT_C const TDesC& Summary() const;
	IMPORT_C TBool SummaryIsLoaded() const;
	IMPORT_C TAgnDataChange SummaryChange() const;
	IMPORT_C const TStreamId& SummaryStreamId() const;
	IMPORT_C void SetSummaryStreamId(const TStreamId& aStreamId);
	
	IMPORT_C void SetLocationL(const TDesC& aLocation);
	IMPORT_C const TDesC& Location() const;

	IMPORT_C void SetDescription(HBufC* aText); // takes ownership
	IMPORT_C const TDesC& Description() const;
	IMPORT_C TBool DescriptionIsLoaded() const;
	IMPORT_C const TStreamId& DescriptionStreamId() const;
	IMPORT_C void SetDescriptionStreamId(const TStreamId& aStreamId);
	IMPORT_C TAgnDataChange DescriptionChange() const;
	
	// dtstamp
	IMPORT_C void SetDTStampUtcL(const TTime& aDTStampUtc);
	IMPORT_C const TTime& DTStampUtcL() const;

	// busy status
	IMPORT_C void SetBusyStatusL(CCalEntry::TTransp aBusyStatus);
	IMPORT_C CCalEntry::TTransp BusyStatus() const;
	
	// GEO
	IMPORT_C void GeoValue(TReal& aLatitude, TReal& aLongitude) const;
	IMPORT_C void SetGeoValueL(const TReal& aLatitude, const TReal& aLongitude);
	
	// Categories
	IMPORT_C TInt CategoryCount() const;
	IMPORT_C void AddCategoryL(CAgnCategory* aCategory); // takes ownership
	IMPORT_C CAgnCategory& FetchCategory(TInt aIndex) const;
	IMPORT_C void DeleteCategory(TInt aIndex);
	TBool CategoryExist(CAgnCategory& aSearchCategory) const;

	// Attendees
	IMPORT_C TInt AttendeeCount() const;
	IMPORT_C void AddAttendeeL(CAgnAttendee* aAttendee); // takes ownership
	IMPORT_C CAgnAttendee& FetchAttendee(TInt aIndex) const;
	IMPORT_C void DeleteAttendee(TInt aIndex);

	IMPORT_C void SetOrganizerL(CAgnAttendee* aAttendee); // takes ownership
	IMPORT_C CAgnAttendee* Organizer() const;
	IMPORT_C void SetPhoneOwnerL(CAgnAttendee* aOwner); // takes ownership
	IMPORT_C CAgnAttendee* PhoneOwner() const;
	void CreateAttendeeListL();
	
	// attachments
	IMPORT_C void AddAttachmentL(CAgnAttachment* aAttachment); // takes ownership
	IMPORT_C TInt DeleteAttachmentL(const CAgnAttachment& aAttachment);
	IMPORT_C CAgnAttachment& Attachment(TInt aIndex) const;
	IMPORT_C TInt AttachmentCount() const;

	// Group Scheduling data
	TAgnCalendarTime GetChildRecurrenceId(TCalLocalUid aChildId) const;

	IMPORT_C CGsData::TGsDataType GsDataType() const;
	IMPORT_C void SetMethod(CCalEntry::TMethod aMethod);
	IMPORT_C CCalEntry::TMethod Method() const;
	IMPORT_C void SetGuid(HBufC8* aGuid); // takes ownership
	IMPORT_C const TDesC8& Guid() const;		
	IMPORT_C void SetSequenceNumber(TInt aSeqNum);
	IMPORT_C TInt SequenceNumber() const;
	IMPORT_C TAgnCalendarTime RecurrenceId() const;
	IMPORT_C CalCommon::TRecurrenceRange RecurrenceRange() const;

	IMPORT_C TBool HasChildren() const;
	
	// Child entry data
	IMPORT_C void SetRecurrenceIdFromParentL(const CAgnEntry& aParentEntry);
	IMPORT_C TCalLocalUid ParentId() const;
	IMPORT_C void SetParentId(TCalLocalUid aParentId);
	IMPORT_C void UpdateRecurrenceIdL(const TAgnCalendarTime& aRecId);
	IMPORT_C void SetRecurrenceRangeL(CalCommon::TRecurrenceRange aRange);
	//Parent entry data
	IMPORT_C const RArray<TGsChildRefData>& ChildIds() const;
	IMPORT_C void AddChildIdL(const TGsChildRefData& aChildRefData);
	IMPORT_C void RemoveChildId(TCalLocalUid aId);
	IMPORT_C void PruneRDatesL(const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange);

	// User integer.
	IMPORT_C void SetUserInt( TUint32 aUserInt );
	IMPORT_C TUint32 UserInt() const;
		
private:
	// following enums for internal use only	
	enum TFlags
		{
		EAgnAlarmActionDeleted = 0x01,
		};
		
	void SetFlagOn(TFlags aFlag);
	void SetFlagOff(TFlags aFlag);
	TBool IsFlagSet(TFlags aFlag) const;
		
	CAgnEntry(CCalEntry::TType aType);
	void ConstructL(HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum);
	void ConstructL(HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum, 
					const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange);
	
 	void ExternalizeGsDataL(RWriteStream& aStream, TBool aToBuffer) const;
 	void InternalizeGsDataL(RReadStream& aStream, TBool aFromBuffer);
 	
 	void ExternalizeEntryL(RWriteStream& aStream, TBool aToBuffer=EFalse) const;
 	void InternalizeEntryL(RReadStream& aStream, TBool aFromBuffer=EFalse);


private:
	
	CGsData* 						iGsData;   // the group scheduling data

	HBufC*							iSummary;
	TStreamId						iSummaryStreamId;
	HBufC*							iDescription;
	TStreamId						iDescriptionStreamId;
	HBufC*							iLocation;
	TTime							iDTStampUtc;
	
	CAgnContent*					iAlarmAction;
	TStreamId						iAlarmActionStreamId;
	HBufC* 							iAlarmSoundName;// the alarm pre-time is stored in CAgnSimpleEntry
	RPointerArray<CAgnAttendee>*	iAttendeeList;
	CAgnAttendee*					iPhoneOwner;
	CAgnAttendee*	 				iMeetingOrganizer;
	RPointerArray<CAgnCategory>*	iCategoryList;
	RPointerArray<CAgnAttachment>*	iAttachments;
	
	TUint8							iBusyStatus;
	TUint8							iReplicationStatus;
	TUint16							iFlags;
	TReal							iLatitude;
	TReal							iLongitude;
	};

#endif
