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

#ifndef __AGMSIMPLEENTRY_H__
#define __AGMSIMPLEENTRY_H__

#include "agmids.h"
#include "calentry.h"

class CAgnRptDef;
class CAgnSimpleEntryAllocator;

/** The maximum possible alarm offset is a constant which means there is no alarm. */
const TInt32 KAlarmNotSet = KMaxTInt32;

NONSHARABLE_CLASS(CAgnSimpleEntry) : public CBase
/** Base class for agenda entry types.

It provides an interface to the following entry attributes:
- type
- repeat information
- entry id
- last changed date
- alarm time
- priority
- status
- start and end time

CAgnSimpleEntry is the base class for the full entry CAgnEntry.

On the client side, CAgnSimpleEntry is used as the 'light entry' which is stored in an 
instance. Calling FindInstanceL will return CCalInstances. Each of these CCalInstances will 
reference a 'light entry', which is a CAgnSimpleEntry. If a function is called on the instance 
which cannot be handled by CAgnSimpleEntry, the full CAgnEntry will be loaded.

On the server side, CAgnSimpleEntry is used in the entry index - CAgnIndex.
There is one main index of CAgnSimpleEntrys, other indexes contain pointers to simple entries in that
index.

@internalComponent
@released
*/
	{
public:
	IMPORT_C CCalEntry::TType Type() const;

	// time
	IMPORT_C void SetStartAndEndTimeL(const TAgnCalendarTime& aStartTime, const TAgnCalendarTime& aEndTime);
	IMPORT_C const TAgnCalendarTime& StartTime() const;
	IMPORT_C const TAgnCalendarTime& EndTime() const;
	IMPORT_C const TAgnCalendarTime& EntryTime() const; // returns start time for non-todos, end (due) time for todos
	
	IMPORT_C TBool MoveStartTimeLocalL(const TTime& aNewStartTimeLocal);
	IMPORT_C void AdjustStartUntilTimeForRepEntryL();
	
	IMPORT_C MAgnCalendarTimeMode::TTimeMode TimeMode() const;

	// alarm date/time
	IMPORT_C TBool HasAlarm() const;
	IMPORT_C void SetAlarmOffset(const TTimeIntervalMinutes& aStartTimeOffset);
	IMPORT_C TTimeIntervalMinutes AlarmOffset() const;

	// ids
	IMPORT_C const TAgnEntryId& EntryId() const;
	IMPORT_C void SetEntryId(const TAgnEntryId& aId);
	IMPORT_C TCalLocalUid LocalUid() const;	
	IMPORT_C void SetLocalUid(TCalLocalUid aUId);
	IMPORT_C TUint32 GuidHash() const;	
	IMPORT_C void SetGuidHash(TUint32 aGuidHash);	
	
	// repeat rule
	IMPORT_C void SetRptDefL(const CAgnRptDef& aRptDef);
	IMPORT_C void ClearRepeat();
	IMPORT_C CAgnRptDef* RptDef();
	IMPORT_C const CAgnRptDef* RptDef() const;

	// status
	IMPORT_C void SetStatus(CCalEntry::TStatus aStatus);
	IMPORT_C CCalEntry::TStatus Status() const;

	// last changed date
	IMPORT_C void SetLastModifiedDate();
	IMPORT_C void SetLastModifiedDateUtc(const TTime& aLastChangedDateUtc);
	IMPORT_C const TTime& LastModifiedDateUtc() const;
	
	// priority
	IMPORT_C void SetPriority(TUint8 aPriority);
	IMPORT_C TUint8 Priority() const;
	
	// crossed out date - todos only
	IMPORT_C const TTime& CompletedDateUtc() const;
	IMPORT_C void SetCompletedDateUtcL(const TTime& aDateUtc);
	IMPORT_C void SetIncomplete();

	// used for extraction
	IMPORT_C TTime ValidFromTimeLocalL() const;
	IMPORT_C TTime ValidToTimeLocalL() const;
	IMPORT_C TTime DurationPlusL(const TTime& aTime) const;
	IMPORT_C TTime DurationMinusL(const TTime& aTime) const;
	
	// non-exported
	void CopySimpleEntryL(const CAgnSimpleEntry& aSimpleEntry, CCalEntry::TCopyType aCopyType);
	
	IMPORT_C virtual void ExternalizeL(RWriteStream& aStream, TBool aToBuffer=EFalse) const;
	IMPORT_C virtual void InternalizeL(RReadStream& aStream, TBool aFromBuffer=EFalse);

	void doExternalizeL(RWriteStream& aStream, TBool aToBuffer) const;
	void doInternalizeL(RReadStream& aStream, TBool aFromBuffer);
	
	TBool operator == (const CAgnSimpleEntry& aSimpleEntry) const;
	
	// We only need to internalize/externalize the GuidHash for the
	// persistent index file so it's separate from the other
	// internalize/externalize methods.
	IMPORT_C void InternalizeGuidHashL(RReadStream& aReadStream);
	IMPORT_C void ExternalizeGuidHashL(RWriteStream& aWriteStream) const;
	IMPORT_C void SetCollectionId(TCalCollectionId aFileId);
	IMPORT_C TCalCollectionId CollectionId()const;
	
	// User integer.
	IMPORT_C void SetUserInt( TUint32 aUserInt );
	IMPORT_C TUint32 UserInt() const;

protected:

	friend class CAgnSimpleEntryAllocator;

	// set constructor and destructor protected because only the CAgnSimpleEntryAllocator
	// can create or destroy CAgnSimpleEntry objects

	CAgnSimpleEntry(CCalEntry::TType aType);
	virtual ~CAgnSimpleEntry();
	
	// overload new operator to allocate CAgnSimpleEntry object with CAgnSimpleEntryAllocator
	
	TAny* operator new(TUint aSize, TLeave aType, CAgnSimpleEntryAllocator* aAllocator);
	
	void StartTimeChanged();

private:
	CAgnRptDef* 		iRptDef;		// contains the repeat definition plus exceptions
	TAgnEntryId 		iEntryId;		// the entry Id
	TCalLocalUid		iLocalUid;		// the unique Id
	TUint32				iGuidHash;		// the hash of Guid
	TAgnCalendarTime 	iStartTime;
	TAgnCalendarTime 	iEndTime;
	TTime 				iLastModifiedDateUtc;
	TTime				iCompletedDateUtc; // todos only
	TInt32				iAlarmOffset;
	TUint8				iType;
	TUint8				iPriority;
	TUint8				iStatus;
	TUint8				iReserved;		// This exists whether declared or not because of the 4-byte memory boundaries
	TCalCollectionId	iCollectionId;

	TUint32				iUserInt; //the field has been moved to simple entry since PREQ2049
	};

#endif
