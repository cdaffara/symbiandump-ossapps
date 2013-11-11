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

#ifndef __CALENTRYIMPL_H__
#define __CALENTRYIMPL_H__

#include "calentry.h"
#include "agmids.h"

class CAgnContent;
class CAgnEntry;
class CAgnRptDef;
class CAgnSimpleEntry;
class CCalLiteEntry;
class CCalSession;
class CCalSessionImpl;
class TAgnAlarmInfo;
class TAgnRpt;
class TCalRRule;
class RAgendaServ;
/** Implementation class for a CCalEntry.
@internalComponent
*/
NONSHARABLE_CLASS(CCalEntryImpl) : public CBase
	{
public:
	static CCalEntryImpl* NewL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum, const TCalTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange);
	static CCalEntryImpl* NewL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum);
	static CCalEntryImpl* NewL(CAgnEntry& aEntry, CCalSessionImpl& aSessionImpl); // takes ownership of aEntry
	static CCalEntryImpl* NewL(CCalLiteEntry& aEntry, CCalSessionImpl& aSessionImpl);
	~CCalEntryImpl();
	
	void AddAttendeeL(CCalAttendee* aAttendee);
	RPointerArray<CCalAttendee>& AttendeesL();
	void DeleteAttendeeL(TInt aIndex);
	void SetOrganizerL(CCalUser* aAttendee);
	
	void SetPhoneOwnerL(const CCalUser* aOwner);
	CCalUser* OrganizerL();
	CCalUser* PhoneOwnerL();	
	
	void SetAlarmSoundNameL(const TDesC& aAlarmSoundName);
	CTzRules* GetTzRulesL();
	void SetTzRulesL(const CTzRules& aTzRules);
	void SetTzRulesL();
		
	void SetRDatesL(const RArray<TCalTime>& aRDates);
	void GetRDatesL(RArray<TCalTime>& aRDates);
	
	void SetExceptionDatesL(const RArray<TCalTime>& aExDates);
	void GetExceptionDatesL(RArray<TCalTime>& aExDates);

	const RPointerArray<CCalCategory>& CategoryListL();
	void AddCategoryL(CCalCategory* aCategory);
	void DeleteCategoryL(TInt aIndex);
	
	TCalTime RecurrenceIdL();
	CalCommon::TRecurrenceRange RecurrenceRangeL();
	
	void SetMethodL(CCalEntry::TMethod aMethod);
	
	TCalTime StartTimeL();
	TCalTime EndTimeL();
	void SetStartAndEndTimeL(const TCalTime& aStartTime, const TCalTime& aEndTime);
	
	TCalTime LastModifiedDateL();
	
	TCalTime DTStampL();
	
	TCalTime CompletedTimeL();
	void SetCompletedL(TBool aCompleted, const TCalTime& aTime);

	void SetRRuleL(const TCalRRule& aRule);
	TBool GetRRuleL(TCalRRule& aRule); 
	
	TCalTime NextInstanceForLocalUIDL(const TCalTime& aTime);
	TCalTime PreviousInstanceForLocalUIDL(const TCalTime& aTime);

	void ClearRepeatingPropertiesL();

	void SetAlarmL(CCalAlarm* aAlarm); 
	CCalAlarm* AlarmL();

	CCalEntry::TType EntryTypeL();
	void SetStatusL(CCalEntry::TStatus aStatus);
	CCalEntry::TStatus StatusL();

	void SetReplicationStatusL(CCalEntry::TReplicationStatus aReplicationStatus);
	CCalEntry::TReplicationStatus ReplicationStatusL();

	void SetPriorityL(TUint aPriority);
	TUint PriorityL();

	TBool CompareL(const CCalEntry& aEntry);

	void PopulateAttachmentListL();
	void AddAttachmentL(CCalAttachment* aAttachment); // takes ownership
	void DeleteAttachmentL(const CCalAttachment& aAttachment);
	CCalAttachment* AttachmentL(TInt aIndex);
	TInt AttachmentCountL();
	
	void SetSummaryL(const TDesC& aSummary);
	const TDesC& SummaryL();
	void SetDescriptionL(const TDesC& aDescription);
	const TDesC& DescriptionL(); 
	void SetLocationL(const TDesC& aLocation);
	const TDesC& LocationL();

	CCalEntry::TMethod MethodL();
	TInt SequenceNumberL();
	const TDesC8& UidL();
	
	void SetSequenceNumberL(TInt aSeq);
	void SetLastModifiedDateL();
	void SetLastModifiedDateL(const TCalTime& aModifiedTime);
	
	void SetDTStampL(const TCalTime& aDTStampTime);
	
	CAgnEntry* GetFullEntryL();
	TAgnEntryId AgnEntryIdL();

	void CopyFromL(CCalEntryImpl& aOther, CCalEntry::TCopyType aCopyType);
	
	void SetLocalUidL(TCalLocalUid aLocalId);
	TCalLocalUid LocalUidL();
	MAgnCalendarTimeMode::TTimeMode TimeMode() const;
	
	CCalEntry::TTransp BusyStatusL();
	void SetBusyStatusL(CCalEntry::TTransp aBusyStatus);
	
	CCalGeoValue* GeoValueL();
	void SetGeoValueL(const CCalGeoValue& aGeoValue);
	void ClearGeoValueL();
	
	TCalTime FindRptUntilTimeL(TInt aCount);
	
	void SetUserIntL( TUint32 aUserInt );
	TUint32 UserIntL();

	TUint8 ShortFileIdL();
	
private:
	CCalEntryImpl();
	CCalEntryImpl(CAgnEntry& aEntry, CCalSessionImpl& aSessionImpl);
	CCalEntryImpl(CCalLiteEntry& aEntry, CCalSessionImpl& aSessionImpl);
	void ConstructL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum, const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange);
	void ConstructL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum);
	void LoadFullEntryL();
	void ClearSimpleEntry();
	const CAgnSimpleEntry* SimpleEntry() const;
	CAgnSimpleEntry* SimpleEntry();
	void PopulateAttendeeListL();
	void PopulateCategoryListL();
	void CopyBasicRRuleDataL(TAgnRpt& aAgnRpt, const TCalRRule& aRpt);
	void SetCommonRptDataL(const TCalRRule& aRpt, CAgnRptDef& aRptDef);
	void FetchDatesL(const RArray<TAgnCalendarTime>* aDateList, RArray<TCalTime>& aRDates);
	void SetDatesL(TBool aDateType, const RArray<TCalTime>& aDates);
	
private: // member data
	// Pointers to entry data - if iFullEntry is NULL then this has been created 
	// from a lite entry, and iLiteEntry must be non-NULL
	CAgnEntry* iFullEntry;
	CCalLiteEntry* iLiteEntry;
	
	CCalSessionImpl* iSessionImpl; // not owned
	
	// Pointers to 'shell' CCal- classes.
	// iFullEntry owns the actual data in CAgn- classes. When these are returned to the client, 
	// they are wrapped inside a CCal- class. That wrapper is owned here so that the 
	// client does not take ownership of the CCal- classes which are returned.
	RPointerArray<CCalCategory>* iCalCategoryList;
	RPointerArray<CCalAttendee>* iCalAttendeeList;
	RPointerArray<CCalAttachment>* iCalAttachmentList;
	CCalUser* iCalOrganizer;

    TBool iRptSetByCount;
	};

/** Implementation class of CCalEntryId.

This takes data from the alarm server when a calendar alarm expires, and extracts details
about the calendar event associated with the alarm.

@internalComponent
*/
NONSHARABLE_CLASS(CCalEntryIdImpl) : public CBase
	{
public:
	static CCalEntryIdImpl* NewL(TDesC8* aFlatData);
	~CCalEntryIdImpl();
	TPtrC8 IdL();
	TPtrC StoreFileNameL();
	TCalTime RecurrenceIdL();
	TCalTime InstanceTimeL();
	TCalLocalUid LocalUidL();
private:
	CCalEntryIdImpl();
	void ConstructL(TDesC8* aFlatData);
private:
	const TDesC8* iFlatData;
	const TAgnAlarmInfo* iUniqueId;
	};
	
/** A wrapper class for a TAgnLiteEntry which allows it to be shared between many CCalEntry objects.

@internalComponent	
*/
NONSHARABLE_CLASS(CCalLiteEntry) : public CBase
	{
public:
	static CCalLiteEntry* NewL(CAgnSimpleEntry& aLiteEntry, RAgendaServ& aAgendaServer);
	
	void IncrementRefCount();
	void DecrementRefCount();
	
	CAgnSimpleEntry& LiteEntry() const;

private:
	CCalLiteEntry(CAgnSimpleEntry& aLiteEntry, RAgendaServ& aAgendaServer);
	~CCalLiteEntry();

private:
	RAgendaServ&	 iAgendaServ;
	CAgnSimpleEntry& iLiteEntry;
	TInt iRefCount;
	};
	
#endif // __CALENTRYIMPL_H__
