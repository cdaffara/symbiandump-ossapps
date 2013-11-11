// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <vtzrules.h>
#include <tz.h>
#include "agmentry.h"
#include "agmtlsproxy.h"
#include "agmtzrules.h"
#include "agstzruleindex.h"
#include "agmpanic.h"
#include "agmrptdef.h"
#include "agsfilemanager.h"

const TUid KUidAgnSystemTzIndex = {0x10285b96};
const TUid KUidAgnImportedTzIndex = {0x10285b97};
const TUid KUidAgnTimeStamp = {0x102863fb};

const TInt KMinYear = 0;
const TInt KMaxYear = 9999;

/* ------------------- CAgnTzRuleIndexItem -------------------*/
CAgnTzRuleIndexItem::CAgnTzRuleIndexItem(TStreamId aStreamId, CTzRules* aRule) :
	iRefCount(1), iTzRules(aRule)
	{
	iStreamId = aStreamId;
	}

CAgnTzRuleIndexItem::CAgnTzRuleIndexItem() :
	iRefCount(1), iTzRules(NULL)
	{
	}

CAgnTzRuleIndexItem* CAgnTzRuleIndexItem::NewL(RReadStream& aStream)
	{
	CAgnTzRuleIndexItem* self = new (ELeave) CAgnTzRuleIndexItem();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

CAgnTzRuleIndexItem* CAgnTzRuleIndexItem::NewL(TStreamId aStreamId, CTzRules* aTzRule)
	{
	CAgnTzRuleIndexItem* self = new (ELeave) CAgnTzRuleIndexItem(aStreamId, aTzRule);
	return self;
	}

CAgnTzRuleIndexItem::~CAgnTzRuleIndexItem()
	{
	delete iTzRules;
	}

void CAgnTzRuleIndexItem::IncrementRef()
	{
	++iRefCount;
	}

TInt CAgnTzRuleIndexItem::DecrementRef()
	{
	__ASSERT_DEBUG(iRefCount>0, User::Invariant());
	return --iRefCount;
	}

void CAgnTzRuleIndexItem::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt16L(iRefCount);
	aStream << iStreamId;
	}

void CAgnTzRuleIndexItem::InternalizeL(RReadStream& aStream)
	{
	iRefCount = aStream.ReadInt16L();
	aStream >> iStreamId;
	}

void CAgnTzRuleIndexItem::SetRule(CTzRules* aTzRules)
	{
	if (iTzRules)
		{
		delete iTzRules;
		}
	iTzRules = aTzRules;
	}

CTzRules* CAgnTzRuleIndexItem::TzRule() const
	{
	return iTzRules;
	}

const TStreamId& CAgnTzRuleIndexItem::StreamId() const
	{
	return iStreamId;
	}

TBool CAgnTzRuleIndexItem::IsSystemTzRule() const
	{
	return EFalse;
	}

/* ------------------- CAgnSystemTzRuleIndexItem -------------------*/
void CAgnSystemTzRuleIndexItem::ConstructL(const TDesC8& aTzId)
	{
	iTzId = aTzId.AllocL();
	}

CAgnSystemTzRuleIndexItem* CAgnSystemTzRuleIndexItem::NewL(TStreamId aStreamId,	const TDesC8& aTzId, CTzRules* aRule)
	{
	CAgnSystemTzRuleIndexItem* self = new (ELeave) CAgnSystemTzRuleIndexItem(aStreamId, aRule);
	CleanupStack::PushL(self);
	self->ConstructL(aTzId);
	CleanupStack::Pop(self);
	return self;
	}

CAgnSystemTzRuleIndexItem::CAgnSystemTzRuleIndexItem() :
	CAgnTzRuleIndexItem(), iTzId(NULL)
	{
	}

CAgnSystemTzRuleIndexItem::CAgnSystemTzRuleIndexItem(TStreamId aStreamId,
		CTzRules* aRules) :
	CAgnTzRuleIndexItem(aStreamId, aRules), iTzId(NULL)
	{
	}

CAgnSystemTzRuleIndexItem::~CAgnSystemTzRuleIndexItem()
	{
	delete iTzId;
	}

CAgnSystemTzRuleIndexItem* CAgnSystemTzRuleIndexItem::NewL(RReadStream& aStream)
	{
	CAgnSystemTzRuleIndexItem* self = new (ELeave) CAgnSystemTzRuleIndexItem();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

const TDesC8& CAgnSystemTzRuleIndexItem::TzId() const
	{
	return *iTzId;
	}

void CAgnSystemTzRuleIndexItem::ExternalizeL(RWriteStream& aStream) const
	{
	CAgnTzRuleIndexItem::ExternalizeL(aStream);
	aStream << *iTzId;
	}

void CAgnSystemTzRuleIndexItem::InternalizeL(RReadStream& aStream)
	{
	CAgnTzRuleIndexItem::InternalizeL(aStream);
	iTzId = HBufC8::NewL(aStream, KMaxTInt);
	}

TBool CAgnSystemTzRuleIndexItem::IsSystemTzRule() const
	{
	return ETrue;
	}

/* ------------------- CAgnTzRuleIndex -------------------*/
CAgnTzRuleIndex::CAgnTzRuleIndex(CStreamDictionary& aDictionary, CFileStore& aStore) :
	iDictionary(&aDictionary),
	iStore(&aStore),
	iPosCurSystemTzRule(KErrNotFound),
	iPosFetchedLastTime(0),
	iTzDbChangeTimeStamp(Time::NullTTime()),
	iTzRulesLastModifiedTime(Time::NullTTime())
	{
	}

CAgnTzRuleIndex* CAgnTzRuleIndex::NewL(CStreamDictionary& aDictionary, CFileStore& aStore)
	{
	CAgnTzRuleIndex* self = new(ELeave) CAgnTzRuleIndex(aDictionary, aStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return (self);
	}

void CAgnTzRuleIndex::ConstructL()
	{
	iTimeZoneAccessor = CAgnTlsProxy::CreateL(CAgnTlsProxy::TAgnTlsTzRulesType_Server);

	iSystemRuleStreamId = ReadItemsFromStoreL(KUidAgnSystemTzIndex);
	iUserDefinedRuleStreamId = ReadItemsFromStoreL(KUidAgnImportedTzIndex);

	RTz& tzServer = iTimeZoneAccessor->TzServer();
	CTzId* tzid = tzServer.GetTimeZoneIdL();
	CleanupStack::PushL(tzid);
	HandleCurrentTimeZoneChangeL(*tzid);
	CleanupStack::PopAndDestroy(tzid);
	}

/* Read the time stamp of time zone database has been modified last time.

- If the file is newly created, the time stamp will be set to current time
- If the file is re-opened, read the time stamp from the file.
- If the file is re-opened, but there isn't a time stamp, the time stamp will be set to current time
 */
void CAgnTzRuleIndex::ReadTimeStampL()
	{
	TStreamId streamId = iDictionary->At(KUidAgnTimeStamp);

	if (streamId == KNullStreamId)
		{//There is not a time stamp in the file either it is newly created or an existing one.
		TTime currentTime;
		currentTime.UniversalTime();
		iTzDbChangeTimeStamp = currentTime;

		if (RefreshTzRulesL())
			{
			iTzRulesLastModifiedTime = currentTime;
			}
		PersistToStoreL(KUidAgnTimeStamp);
		}
	else
		{//iTzChangTimeStamp is read from file
		ReadFromStoreL(KUidAgnTimeStamp, EFalse);
		}
	}

/** Check if the time zone server has been modified since last time the
 * Calendar server shut down. If so, the time zones of entries cached in the Calendar
 * will be updated. Calendar clients will be also notified with the change if
 * any of time zones associated with Calendar entries have been modified.
 *
 * This function is called when a Calendar file is opened.
 *
 */
void CAgnTzRuleIndex::CheckTzDbModificationL(CAgnServFile& aAgnServFile)
	{
	iAgnFile = &aAgnServFile;
	ReadTimeStampL();
	//This will result in HandleTzRulesChangeL to be called immediatly if a tz change has been published.
	iTimeZoneAccessor->AddSystemTimeObserverL(this);
	}
/*
 Called when the file is closed. It will write the current time to the Calendar file.
 */
void CAgnTzRuleIndex::WriteTimeStampL()
	{
	TStreamId streamId = iDictionary->At(KUidAgnTimeStamp);
	RStoreWriteStream writeStream;
	writeStream.ReplaceLC(*iStore, streamId);
	ExternalizeTimesL(writeStream);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	iStore->CommitL();

	}
void CAgnTzRuleIndex::ExternalizeTimesL(RStoreWriteStream& aWriteStream) const
	{
	aWriteStream << I64LOW(iTzDbChangeTimeStamp.Int64());
	aWriteStream << I64HIGH(iTzDbChangeTimeStamp.Int64());
	aWriteStream << I64LOW(iTzRulesLastModifiedTime.Int64());
	aWriteStream << I64HIGH(iTzRulesLastModifiedTime.Int64());
	}

void CAgnTzRuleIndex::InternalizeTimesL(RReadStream& aReadStream)
	{
	TUint32 low = aReadStream.ReadUint32L();
	TUint32 high = aReadStream.ReadUint32L();
	iTzDbChangeTimeStamp = TTime(MAKE_TINT64(high, low));

	low = aReadStream.ReadUint32L();
	high = aReadStream.ReadUint32L();
	iTzRulesLastModifiedTime = TTime(MAKE_TINT64(high, low));
	}

/* Externalise an array of tz rules or a time stamp to first time Calendar file.

 @param aUid refer to uid of object to write to the file.
 @return Stream Id refer to the object being written to the file
 */
TStreamId CAgnTzRuleIndex::PersistToStoreL(const TUid& aUid)
	{
	RStoreWriteStream writeStream;
	TStreamId streamId = writeStream.CreateLC(*iStore);
	if (aUid == KUidAgnSystemTzIndex)
		{
		ExternalizeRulesL(writeStream, iSystemTzRules);
		}
	else
		if (aUid == KUidAgnImportedTzIndex)
			{
			ExternalizeRulesL(writeStream, iUserDefinedTzRules);
			}
		else
			{
			__ASSERT_ALWAYS(aUid == KUidAgnTimeStamp,User::Invariant());
			ExternalizeTimesL(writeStream);
			}

	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);

	iDictionary->AssignL(aUid, streamId);

	// Re-write the dictionary stream
	RStoreWriteStream stream;
	TStreamId rootStreamId = iStore->Root();
	stream.ReplaceLC(*iStore, rootStreamId);
	stream<< *iDictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // dictionary stream
	iStore->CommitL();
	return streamId;
	}

void CAgnTzRuleIndex::ReadFromStoreL(const TUid& aUid, TBool aRollback)
	{
	TStreamId streamId = iDictionary->At(aUid);
	__ASSERT_ALWAYS(streamId != KNullStreamId, User::Leave(KErrNotFound));
	RStoreReadStream readStream;
	readStream.OpenLC(*iStore, streamId);
	if (aUid == KUidAgnSystemTzIndex)
		{
		InternalizeRulesL(readStream, ETrue, aRollback);
		}
	else
		if (aUid == KUidAgnImportedTzIndex)
			{
			InternalizeRulesL(readStream, EFalse, aRollback);
			}
		else
			{
			__ASSERT_ALWAYS(aUid==KUidAgnTimeStamp,User::Invariant());
			InternalizeTimesL(readStream);
			}

	CleanupStack::PopAndDestroy(&readStream);
	}

TStreamId CAgnTzRuleIndex::ReadItemsFromStoreL(const TUid& aUid, TBool aRollback)
	{
	TStreamId streamId = iDictionary->At(aUid);

	if (streamId == KNullStreamId)
		{
		streamId = PersistToStoreL(aUid);
		}
	else
		{
		ReadFromStoreL(aUid, aRollback);
		}

	return streamId;
	}

void CAgnTzRuleIndex::InternalizeRulesL(RReadStream& aStream,
		TBool aIsSystemRule, TBool aRollback)
	{
	TInt count = aStream.ReadInt16L();
	CAgnTzRuleIndexItem* item= NULL;
	for (TInt ii=0; ii<count; ++ii)
		{
		if (aIsSystemRule)
			{
			item = CAgnSystemTzRuleIndexItem::NewL(aStream);
			}
		else
			{
			item = CAgnTzRuleIndexItem::NewL(aStream);
			}

		CleanupStack::PushL(item);
		RStoreReadStream readStream;

		readStream.OpenLC(*iStore, item->StreamId());
		CTzRules* tzRules = CTzRules::NewL(readStream);
		item->SetRule(tzRules);
		CleanupStack::PopAndDestroy(&readStream);

		if (aIsSystemRule)
			{
			if (aRollback)
				{
				CleanupStack::Pop(item);
				RollbackItemL(iSystemTzRules, item);
				}
			else
				{
				iSystemTzRules.AppendL(item);
				CleanupStack::Pop(item);
				}
			}
		else
			{
			if (aRollback)
				{
				CleanupStack::Pop(item);
				RollbackItemL(iUserDefinedTzRules, item);
				}
			else
				{
				iUserDefinedTzRules.AppendL(item);
				CleanupStack::Pop(item);
				}
			}
		}
	}

void CAgnTzRuleIndex::ExternalizeRulesL(RWriteStream& aStream, const RPointerArray<CAgnTzRuleIndexItem>& aArray) const
	{
	TInt count = aArray.Count();
	aStream.WriteInt16L(count);

	for (TInt ii=0; ii<count; ++ii)
		{
		aArray[ii]->ExternalizeL(aStream);
		}
	}

CAgnTzRuleIndex::~CAgnTzRuleIndex()
	{
	iSystemTzRules.ResetAndDestroy();
	iUserDefinedTzRules.ResetAndDestroy();
	iTimeZoneAccessor->RemoveSystemTimeObserver(this);
	CAgnTlsProxy::Release(iTimeZoneAccessor);
	}

/**
 It is called when:

 Creating a light entry during index building.
 Fetch an entry

 @param reference to the entry fetching tz rules
 */
void CAgnTzRuleIndex::FetchTzRuleL(CAgnSimpleEntry& aEntry)
	{
	CAgnTzRules* tzZone = GetTzRulesFromEntry(aEntry);
	if (tzZone == NULL)
		{
		return;
		}

	TInt pos = KErrNotFound;
	if (tzZone->SystemTzRule())
		{
		pos = FindTzRuleByStreamIdL(tzZone->TzZoneStreamId(), iSystemTzRules,
				ETrue);
		if (pos>=0)
			{
			tzZone->SetTzRules(iSystemTzRules[pos]->TzRule());
			tzZone->SetUseCurrentSystemTzRules((pos == iPosCurSystemTzRule));
			}
		}
	else
		{
		__ASSERT_DEBUG(!tzZone->UseCurrentSystemTzRules(), User::Invariant());
		pos = FindTzRuleByStreamIdL(tzZone->TzZoneStreamId(),
				iUserDefinedTzRules, ETrue);
		if (pos>=0)
			{
			tzZone->SetTzRules(iUserDefinedTzRules[pos]->TzRule());
			}
		}

	if (pos == KErrNotFound)
		{
		AddCurrentSystemTzRuleL(*tzZone);
		}
	}

/**
 Remove a tz rule from one of tz rule arraies

 It should be called when an entry is removed or an entry is updated

 @param reference to the entry to be deleted
 */
void CAgnTzRuleIndex::RemoveTzRuleL(CAgnSimpleEntry& aEntry)
	{
	CAgnTzRules* tzZone = GetTzRulesFromEntry(aEntry);
	if (tzZone == NULL || !tzZone->HasValidTzZoneStreamId())
		{
		return;
		}

	TInt pos = KErrNotFound;
	if (tzZone->SystemTzRule())
		{
		pos = FindTzRuleByStreamIdL(tzZone->TzZoneStreamId(), iSystemTzRules,
				EFalse);
		if (pos >=0)
			{
			DeleteARuleL(pos, iSystemTzRules, iSystemRuleStreamId);
			}
		}
	else
		{
		pos = FindTzRuleByStreamIdL(tzZone->TzZoneStreamId(),
				iUserDefinedTzRules, EFalse);
		if (pos >=0)
			{
			DeleteARuleL(pos, iUserDefinedTzRules, iUserDefinedRuleStreamId);
			}
		}

	__ASSERT_DEBUG(pos != KErrNotFound, Panic(EAgmErrTzRuleIsNotFound));

	User::LeaveIfError(pos);
	}

/**
 Add a tz rule to the array if it is not existing in the array otherwise
 increase the refference count for that rule.

 It is called when:

 An entry is added with a user defined tz rule
 An old format file (9.4 prior) is converted to a new format

 @param reference to the entry to be added.
 */
void CAgnTzRuleIndex::AddTzRuleL(CAgnEntry& aEntry)
	{
	CAgnTzRules* tzZone = GetTzRulesFromEntry(aEntry);
	if (tzZone == NULL)
		{
		return;
		}

	if (tzZone->TzRules() == NULL || tzZone->UseCurrentSystemTzRules())
		{
		AddCurrentSystemTzRuleL(*tzZone);
		return;
		}

	TBool isSysRule = EFalse;
	TInt pos = FindRule(*tzZone->TzRules(), isSysRule);
	if (pos >= 0)
		{
		if (isSysRule)
			{
			iSystemTzRules[pos]->IncrementRef();
			}
		else
			{
			iUserDefinedTzRules[pos]->IncrementRef();
			}

		}
	else
		{
		CTzRules* tzRule = tzZone->TzRules()->CloneL();
		CleanupStack::PushL(tzRule);
		TStreamId streamId = PersistTzRuleL(*tzRule);//It will not be commited until model commit the entry into the file
		CAgnTzRuleIndexItem* item = CAgnTzRuleIndexItem::NewL(streamId, tzRule);
		CleanupStack::Pop(tzRule);

		CleanupStack::PushL(item);
		iUserDefinedTzRules.AppendL(item);
		pos = iUserDefinedTzRules.Count()-1;
		CleanupStack::Pop(item);
		}

	if (isSysRule)
		{
		UpdateItemsInStoreL(iSystemTzRules, iSystemRuleStreamId);
		tzZone->SetTzRules(iSystemTzRules[pos]->TzRule());
		tzZone->SetSystemTzRule(ETrue);
		tzZone->SetTzZoneStreamId(iSystemTzRules[pos]->StreamId());
		}
	else
		{
		UpdateItemsInStoreL(iUserDefinedTzRules, iUserDefinedRuleStreamId);
		tzZone->SetTzRules(iUserDefinedTzRules[pos]->TzRule());
		tzZone->SetSystemTzRule(EFalse);
		tzZone->SetTzZoneStreamId(iUserDefinedTzRules[pos]->StreamId());
		}
	}

/**
 Called when entry's tz rule changed

 @param aOldEntry reference to the original entry to be updated.
 @param aNewEntry reference to the entry containing updated content.
 */
void CAgnTzRuleIndex::UpdateTzRuleL(CAgnEntry& /*aOldEntry*/, CAgnEntry& aNewEntry)
	{
	CAgnTzRules* newTzZone = GetTzRulesFromEntry(aNewEntry);
	if (newTzZone != NULL && newTzZone->HasValidTzZoneStreamId())
		{
		TInt pos = KErrNotFound;
		if (newTzZone->SystemTzRule())
			{
			pos = FindTzRuleByStreamIdL(newTzZone->TzZoneStreamId(), iSystemTzRules, EFalse);
			iSystemTzRules[pos]->IncrementRef();
			newTzZone->SetTzRules(iSystemTzRules[pos]->TzRule());
			UpdateItemsInStoreL(iSystemTzRules, iSystemRuleStreamId);
			}
		else
			{
			pos = FindTzRuleByStreamIdL(newTzZone->TzZoneStreamId(), iUserDefinedTzRules, EFalse);
			iUserDefinedTzRules[pos]->IncrementRef();
			newTzZone->SetTzRules(iUserDefinedTzRules[pos]->TzRule());
			UpdateItemsInStoreL(iUserDefinedTzRules, iUserDefinedRuleStreamId);
			}

		__ASSERT_DEBUG(pos !=KErrNotFound, Panic(EAgmErrTzRuleIsNotFound));
		}
	else
		{
		AddTzRuleL(aNewEntry);
		}

	}

void CAgnTzRuleIndex::UpdateItemsInStoreL(
		RPointerArray<CAgnTzRuleIndexItem>& aArray, TStreamId aStreamId)
	{
	RStoreWriteStream writeStream;
	writeStream.ReplaceLC(*iStore, aStreamId);
	ExternalizeRulesL(writeStream, aArray);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	}

TInt CAgnTzRuleIndex::FindTzRuleByStreamIdL(TStreamId aStreamId, RPointerArray<CAgnTzRuleIndexItem>& aArray, TBool aTryPosLastTime)
	{
	TInt count = aArray.Count();
	if (aTryPosLastTime && iPosFetchedLastTime >= 0 && count
			> iPosFetchedLastTime)
		{
		if (aStreamId == aArray[iPosFetchedLastTime]->StreamId())
			{
			return iPosFetchedLastTime;
			}
		}

	TInt pos = KErrNotFound;
	TInt ii = 0;
	while (pos == KErrNotFound && ii < count)
		{
		if (aStreamId == aArray[ii]->StreamId())
			{
			pos = ii;
			}
		++ii;
		}

	if (aTryPosLastTime)
		{
		iPosFetchedLastTime = pos;
		}

	return pos;
	}

TInt CAgnTzRuleIndex::FindRule(RPointerArray<CAgnTzRuleIndexItem>& aRuleArray, const CTzRules& aRule)
	{
	TInt pos = KErrNotFound;
	TInt count = aRuleArray.Count();
	TInt ii = 0;
	while (pos < 0 && ii<count)
		{
		if (aRule.IsEqualTo(*(aRuleArray[ii]->TzRule())))
			{
			pos = ii;
			}
		++ii;
		}

	return pos;
	}

TInt CAgnTzRuleIndex::FindRule(const CTzRules& aTzRule, TBool& aIsSystemRule)
	{
	TInt pos = KErrNotFound;
	pos = FindRule(iSystemTzRules, aTzRule);
	if (pos>=0)
		{
		aIsSystemRule = ETrue;
		}
	else
		{
		pos = FindRule(iUserDefinedTzRules, aTzRule);
		if (pos>=0)
			{
			aIsSystemRule = EFalse;
			}
		}
	return pos;
	}

/**
 Add the current system tz rule to the array if it is not there otherwise
 increase the refference count for the rule.


 It is called when:

 An entry is added with the current tz rule
 @param reference to a CAgnTzRules object which is added to the index
 */
void CAgnTzRuleIndex::AddCurrentSystemTzRuleL(CAgnTzRules& aTzZone)
	{
	if (iPosCurSystemTzRule >= 0)
		{
		iSystemTzRules[iPosCurSystemTzRule]->IncrementRef();
		}
	else
		{
		RTz& tzServer = iTimeZoneAccessor->TzServer();
		CTzId* tzId = tzServer.GetTimeZoneIdL();
		CleanupStack::PushL(tzId);
		CTzRules* rule = tzServer.GetTimeZoneRulesL(*tzId, KMinYear, KMaxYear, ETzUtcTimeReference);
		CleanupStack::PushL(rule);
		TStreamId streamId = PersistTzRuleL(*rule);//It will not be commited until model commit the entry into the file
		CAgnSystemTzRuleIndexItem* item = CAgnSystemTzRuleIndexItem::NewL(streamId, tzId->TimeZoneNameID(), rule);
		CleanupStack::Pop(rule);
		CleanupStack::PushL(item);
		iSystemTzRules.AppendL(item);
		iPosCurSystemTzRule = iSystemTzRules.Count()-1;
		CleanupStack::Pop(item);

		CleanupStack::PopAndDestroy(tzId);
		}

	UpdateItemsInStoreL(iSystemTzRules, iSystemRuleStreamId);
	aTzZone.SetTzZoneStreamId(iSystemTzRules[iPosCurSystemTzRule]->StreamId());
	aTzZone.SetTzRules(iSystemTzRules[iPosCurSystemTzRule]->TzRule());
	aTzZone.SetSystemTzRule(ETrue);
	aTzZone.SetUseCurrentSystemTzRules(ETrue);
	}

void CAgnTzRuleIndex::DeleteARuleL(TInt aPos, RPointerArray<CAgnTzRuleIndexItem>& aArray, TStreamId aStreamId)
	{
	TInt ref = aArray[aPos]->DecrementRef();
	if (ref ==0)
		{
		iStore->DeleteL(aArray[aPos]->StreamId());
		delete aArray[aPos];
		aArray.Remove(aPos);

		if (aPos == iPosCurSystemTzRule)
			{
			iPosCurSystemTzRule = KErrNotFound;
			}
		else if (aPos < iPosCurSystemTzRule)
			{
			--iPosCurSystemTzRule;
			}
		}

	UpdateItemsInStoreL(aArray, aStreamId);
	}

TStreamId CAgnTzRuleIndex::PersistTzRuleL(const CTzRules& aTzRule)
	{
	TStreamId streamId = iStore->ExtendL();
	RStoreWriteStream writeStream;
	writeStream.ReplaceLC(*iStore, streamId);
	aTzRule.ExternalizeL(writeStream);
	writeStream.CommitL();//Don't commit to store until the CAgnEntryModel does it
	CleanupStack::PopAndDestroy(&writeStream);
	return streamId;
	}

void CAgnTzRuleIndex::HandleCurrentTimeZoneChangeL(const CTzId& aSystemTzId)
	{
	if (iPosCurSystemTzRule != KErrNotFound && 0 == static_cast<CAgnSystemTzRuleIndexItem*>(iSystemTzRules[iPosCurSystemTzRule])->TzId().Compare(aSystemTzId.TimeZoneNameID()))
		{
		//the rule doesn't change.
		return;
		}

	CTzRules* rule = iTimeZoneAccessor->GetCurrentSystemTzRulesL();
	CleanupStack::PushL(rule);
	iPosCurSystemTzRule = FindRule(iSystemTzRules, *rule);
	CleanupStack::PopAndDestroy(rule);
	}

void CAgnTzRuleIndex::HandleTzRulesChangeL(const TTime& aTime)
	{
	if(!iAgnFile->IsLocked())
	    {
        if (iTzDbChangeTimeStamp < aTime)
            {
            iTzDbChangeTimeStamp = aTime;
            if (RefreshTzRulesL())
                {
                iTzRulesLastModifiedTime = aTime;
                }
            WriteTimeStampL();
            }
	    }
	else // set the flag so that it can be done later when agendar server is unlocked.
	    {
	    iAgnFile->SetRefreshTzRules(ETrue);
	    }
	}

/* Refresh cached system tz rules and broadcast the change if any of rules has been updated
 */
TBool CAgnTzRuleIndex::RefreshTzRulesL()
	{
	TBool rulesChanged = EFalse;
	const TInt KCount = iSystemTzRules.Count();
	for (TInt ii = 0; ii<KCount; ++ii)
		{
		CTzId* id = CTzId::NewL(static_cast<CAgnSystemTzRuleIndexItem*>(iSystemTzRules[ii])->TzId());
		CTzRules* rules= NULL;
		TRAPD(err, rules = iTimeZoneAccessor->TzServer().GetTimeZoneRulesL(*id, KMinYear, KMaxYear , ETzUtcTimeReference));
		delete id;
		if(err == KErrNone)
		    {
            CleanupStack::PushL(rules);
            if (rules && !iSystemTzRules[ii]->TzRule()->IsEqualTo(*rules))
                {//If a tz rule cached is different from the one in tz server for the same TZ ID, it needs to be updated with the new one.
                CAgnTzRuleIndexItem* item = iSystemTzRules[ii];
                item->TzRule()->CopyL(*rules);
                RStoreWriteStream writeStream;
                writeStream.ReplaceLC(*iStore, item->StreamId());
                item->TzRule()->ExternalizeL(writeStream);
                writeStream.CommitL();
                CleanupStack::PopAndDestroy(&writeStream);
                iStore->CommitL();
                rulesChanged = ETrue;
                }
            CleanupStack::PopAndDestroy(rules);
            }
		}
	if (rulesChanged)
		{
		iAgnFile->TzRulesHaveChangedL();
		}
	return rulesChanged;
	}

void CAgnTzRuleIndex::RollBackL()
	{
	ReadItemsFromStoreL(KUidAgnSystemTzIndex, ETrue);
	ReadItemsFromStoreL(KUidAgnImportedTzIndex, ETrue);
	}

void CAgnTzRuleIndex::ReloadStore(CStreamDictionary& aDictionary, CFileStore& aStore)
	{
	iDictionary = &aDictionary;
	iStore = &aStore;
	}

CTzRules* CAgnTzRuleIndex::FindTzRuleByStreamIdL(TStreamId aStreamId, TBool aIsSystemRule)
	{
	CTzRules* tzRules= NULL;
	if (aIsSystemRule)
		{
		TInt pos = FindTzRuleByStreamIdL(aStreamId, iSystemTzRules, EFalse);
		if (pos != KErrNotFound)
			{
			tzRules = iSystemTzRules[pos]->TzRule()->CloneL();
			}
		}
	else
		{
		TInt pos = FindTzRuleByStreamIdL(aStreamId, iUserDefinedTzRules, EFalse);
		if (pos != KErrNotFound)
			{
			tzRules = iUserDefinedTzRules[pos]->TzRule()->CloneL();
			}
		}

	if (tzRules == NULL)
		{
		tzRules = iTimeZoneAccessor->GetCurrentSystemTzRulesL();
		}

	return tzRules;
	}

CAgnTzRules* CAgnTzRuleIndex::GetTzRulesFromEntry(CAgnSimpleEntry& aEntry)
	{
	CAgnTzRules* tzZone= NULL;
	if (aEntry.TimeMode() != MAgnCalendarTimeMode::EFloating)
		{
		CAgnRptDef* rptDef = aEntry.RptDef();
		if (rptDef != NULL)
			{
			tzZone = rptDef->AgnTzRules();
			}
		}

	return tzZone;
	}

void CAgnTzRuleIndex::RollbackItemL(RPointerArray<CAgnTzRuleIndexItem>& aRuleArray,	CAgnTzRuleIndexItem* aItem)
	{
	CleanupStack::PushL(aItem);
	TInt pos = FindTzRuleByStreamIdL(aItem->StreamId(), aRuleArray, EFalse);
	if (pos == KErrNotFound)
		{
		aRuleArray.AppendL(aItem);
		CleanupStack::Pop(aItem);
		}
	else
		{
		CAgnTzRuleIndexItem* item = aRuleArray[pos];
		item->SetReferenceCount(aItem->ReferenceCount());
		CleanupStack::PopAndDestroy(aItem);
		}
	}

/** Get the last modified time for tz rules
 * It is called when the client getting the last modified date for an entry since the
 * last modified time of an entry is not updated when tz database is updated.
*/
TTime CAgnTzRuleIndex::TzRulesLastModifiedDateL()
	{
	return iTzRulesLastModifiedTime;
	}
