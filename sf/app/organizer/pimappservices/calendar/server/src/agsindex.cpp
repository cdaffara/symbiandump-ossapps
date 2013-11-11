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

#include "agsindex.h"

#include "agmentry.h"
#include "agsextractor.h"
#include "agmpanic.h"
#include "agsuidindex.h"
#include "agmdebug.h"
#include "agsentrymodel.h"
#include "agstzruleindex.h"

const TInt KAgnIndexGranularity = 64;

// --------------------------CAgnEntryNonRptIndex -------------------------------------

CAgnEntryNonRptIndex::CAgnEntryNonRptIndex(MAgnCalendarTimeMode::TTimeMode aTimeMode)
	: CAgnIndex(iKey, aTimeMode)
	{
	}


CAgnEntryNonRptIndex* CAgnEntryNonRptIndex::NewL(MAgnCalendarTimeMode::TTimeMode aTimeMode)
	{
	CAgnEntryNonRptIndex* self = new (ELeave) CAgnEntryNonRptIndex(aTimeMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TBool CAgnEntryNonRptIndex::IsValid(const CAgnSimpleEntry* aEntry) const
	{

	return ( aEntry->Type() != CCalEntry::ETodo &&
			 ! aEntry->RptDef() &&
			 aEntry->TimeMode() == TimeMode() );
	}

// --------------------------CAgnEntryRptIndex -------------------------------------

CAgnEntryRptIndex::CAgnEntryRptIndex(MAgnCalendarTimeMode::TTimeMode aTimeMode)
	: CAgnIndex(iKey,aTimeMode)
	{
	}

	
CAgnEntryRptIndex* CAgnEntryRptIndex::NewL(MAgnCalendarTimeMode::TTimeMode aTimeMode)
	{
	CAgnEntryRptIndex* self = new (ELeave) CAgnEntryRptIndex(aTimeMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


TBool CAgnEntryRptIndex::IsValid(const CAgnSimpleEntry* aEntry) const
	{

	return ( aEntry->Type() != CCalEntry::ETodo &&
			 aEntry->RptDef() &&
			 aEntry->TimeMode() == TimeMode() );
	}


// --------------------------CAgnTodoNonRptIndex -------------------------------------

CAgnTodoNonRptIndex::CAgnTodoNonRptIndex(MAgnCalendarTimeMode::TTimeMode aTimeMode)
	: CAgnIndex(iKey,aTimeMode)
	{
	}


CAgnTodoNonRptIndex* CAgnTodoNonRptIndex::NewL(MAgnCalendarTimeMode::TTimeMode aTimeMode)
	{
	CAgnTodoNonRptIndex* self = new (ELeave) CAgnTodoNonRptIndex(aTimeMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


TBool CAgnTodoNonRptIndex::IsValid(const CAgnSimpleEntry* aEntry) const
	{

	return ( aEntry->Type() == CCalEntry::ETodo &&
			! aEntry->RptDef() &&
			 aEntry->TimeMode() == TimeMode() );
	}


TBool CAgnTodoNonRptIndex::FirstL(TTime& aKeyValue, TInt& aPos) const
	{

	if ( FindL(aKeyValue,aPos) || aPos < Count() )//Find() is in CAgnIndex::Find(), which is public so OK
		{
		//const CAgnSimpleEntry* entry = Get(aPos);
		GetKeyValueL(Get(aPos),aKeyValue);

		return (ETrue);
		}
		
	return (EFalse);
	}


TBool CAgnTodoNonRptIndex::NextL(TTime& aKeyValue, TInt& aPos) const
	{

	aPos++;
	
	if ( aPos < Count() )
		{
		GetKeyValueL(Get(aPos), aKeyValue);
		
		return (ETrue);
		}

	return (EFalse);
	}


// --------------------------CAgnTodoRptIndex -------------------------------------

CAgnTodoRptIndex::CAgnTodoRptIndex(MAgnCalendarTimeMode::TTimeMode aTimeMode)
	: CAgnIndex(iKey,aTimeMode)
	{
	}


CAgnTodoRptIndex* CAgnTodoRptIndex::NewL(MAgnCalendarTimeMode::TTimeMode aTimeMode)
	{
	CAgnTodoRptIndex* self = new (ELeave) CAgnTodoRptIndex(aTimeMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


TBool CAgnTodoRptIndex::IsValid(const CAgnSimpleEntry* aEntry) const
	{

	return ( aEntry->Type() == CCalEntry::ETodo &&
			 aEntry->RptDef() &&
			 aEntry->TimeMode() == TimeMode() );
	}


// --------------------------CAgnSimpleEntryTable --------------------------------

void CAgnSimpleEntryTable::ExternalizeL(RWriteStream& aWriteStream) const
	{
	TUint32 count = 0;
	TUint32 i = 0;
	// externalize the indexes
	// externalize the simple entries
	count = iArray.Count();
	aWriteStream.WriteUint32L(count);
	for (i=0; i < count; ++i)
		{
		aWriteStream.WriteUint32L(iArray[i]->Type());
		iArray[i]->ExternalizeL(aWriteStream);
		iArray[i]->ExternalizeGuidHashL(aWriteStream);
		}
	
	// no need to externalize the Local UID index or GUID hash id index
	// because we can build these from the simple entries on import
	// for iArray
	}

void CAgnSimpleEntryTable::InternalizeL(RReadStream& aReadStream, CAgnTzRuleIndex* aTzIndex)
	{
	TUint32 count = 0;
	TUint32 i = 0;
	
	// internalize the simple entries
	count = aReadStream.ReadUint32L();
	for (i=0; i < count; ++i)
		{
		CCalEntry::TType type = static_cast<CCalEntry::TType>(aReadStream.ReadUint32L());
		
		CAgnSimpleEntry* simpleEntry = iSimpleEntryAllocator->CreateSimpleEntryL(type);
		// the simple entry has been created using the allocator so it
		// cannot be pushed on the cleanup stack because deleting it on
		// a Leave would cause a memory leak. Therefore we have to manually
		// make this this Leave-safe
		TInt err = KErrNone;
		TRAP(err, simpleEntry->InternalizeL(aReadStream));
		simpleEntry->SetCollectionId(iOwningModel->CollectionId());
		if (err != KErrNone)
			{
			iSimpleEntryAllocator->Delete(simpleEntry);
			User::Leave(err);
			}

		if(aTzIndex)
			{
			TRAP(err, aTzIndex->FetchTzRuleL(*simpleEntry));
			if (err != KErrNone)
				{
				iSimpleEntryAllocator->Delete(simpleEntry);
				User::Leave(err);
				}
			}
		
		TRAP(err, simpleEntry->InternalizeGuidHashL(aReadStream));
		if (err != KErrNone)
			{
			iSimpleEntryAllocator->Delete(simpleEntry);
			User::Leave(err);
			}
			
			
		TRAP(err, iArray.AppendL(simpleEntry));
		if (err != KErrNone)
			{
			iSimpleEntryAllocator->Delete(simpleEntry);
			User::Leave(err);
			}	
			
		const TInt KCount = iIndexes.Count();
		
		for ( TInt ii = 0; ii < KCount; ++ii )
			{
			CAgnIndex* index = iIndexes[ii];
			
			if ( index->IsValid(simpleEntry) )
				{
				index->AddL(simpleEntry);
				break;
				}
			}

		iLocalUidIndex->AddL(simpleEntry);

		// add the entry to the GuidHash index only if the hash is valid

		if( simpleEntry->GuidHash() )
			{
			iGuidHashIdIndex->AddL(simpleEntry);
			}
		}
	
	}



CAgnSimpleEntryTable::CAgnSimpleEntryTable(CAgnEntryModel& aEntryModel)
:	iIndexes(EAgnMaxIndex),			// granularity of array storing all indexes
	iArray(KAgnIndexGranularity),	// granularity of array storing all simple entries
	iOwningModel(&aEntryModel)
	{
	}


CAgnSimpleEntryTable* CAgnSimpleEntryTable::NewL(CAgnEntryModel& aEntryModel)
	{
	CAgnSimpleEntryTable* self = new (ELeave) CAgnSimpleEntryTable(aEntryModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return (self);
	}


void CAgnSimpleEntryTable::ConstructL()
//
// Allocate member data
//
	{
	// Create the extractors
	TAgnIterator* nonRptIterator = new(ELeave) TAgnIterator();
	CleanupStack::PushL(nonRptIterator);
	iNonRptExtractor = new(ELeave) CAgnInstanceExtractor(nonRptIterator, *iOwningModel);
	CleanupStack::Pop(nonRptIterator);
	
	TAgnIterator* rptIterator = new(ELeave) TAgnIterator();
	CleanupStack::PushL(rptIterator);
	iRptExtractor = new(ELeave) CAgnInstanceExtractor(rptIterator, *iOwningModel);
	CleanupStack::Pop(rptIterator);
	
	TAgnIterator* todoNonRptIterator = new(ELeave) TAgnTodoNonRptIterator();
	CleanupStack::PushL(todoNonRptIterator);
	iTodoNonRptExtractor = new(ELeave) CAgnInstanceExtractor(todoNonRptIterator, *iOwningModel);
	CleanupStack::Pop(todoNonRptIterator);
	
	TAgnIterator* todoRptExtractor = new(ELeave) TAgnIterator();
	CleanupStack::PushL(todoRptExtractor);
	iTodoRptExtractor = new(ELeave) CAgnInstanceExtractor(todoRptExtractor, *iOwningModel);
	CleanupStack::Pop(todoRptExtractor);

	iSimpleEntryAllocator = new (ELeave) CAgnSimpleEntryAllocator;
	
	iLocalUidIndex   = CAgnLocalUidIndex::NewL(*iSimpleEntryAllocator);
	iGuidHashIdIndex = CGsGuidHashIndex::NewL(*iSimpleEntryAllocator);	
	
	iFindEntry = iSimpleEntryAllocator->CreateSimpleEntryL(CCalEntry::EAppt);
	
	for ( TInt ii = 0; ii < EAgnMaxIndex; ++ii )
		{
		CAgnIndex* index = NULL;		
		
		switch (ii)
			{
		case EAgnEntryNonRptFixedIndex:
			index = CAgnEntryNonRptIndex::NewL(MAgnCalendarTimeMode::EFixedUtc);
			break;
		case EAgnEntryRptFixedIndex:
			index = CAgnEntryRptIndex::NewL(MAgnCalendarTimeMode::EFixedUtc);
			break;
		case EAgnTodoNonRptFixedIndex:
			index = CAgnTodoNonRptIndex::NewL(MAgnCalendarTimeMode::EFixedUtc);
			break;
		case EAgnTodoRptFixedIndex:
			index = CAgnTodoRptIndex::NewL(MAgnCalendarTimeMode::EFixedUtc);
			break;
		case EAgnEntryNonRptFloatingIndex:
			index = CAgnEntryNonRptIndex::NewL(MAgnCalendarTimeMode::EFloating);
			break;
		case EAgnEntryRptFloatingIndex:
			index = CAgnEntryRptIndex::NewL(MAgnCalendarTimeMode::EFloating);
			break;	
		case EAgnTodoNonRptFloatingIndex:
			index = CAgnTodoNonRptIndex::NewL(MAgnCalendarTimeMode::EFloating);
			break;					
		case EAgnTodoRptFloatingIndex:
			index = CAgnTodoRptIndex::NewL(MAgnCalendarTimeMode::EFloating);
			break;
		default:
			User::Leave(KErrOverflow);
			break;
			}

		CleanupStack::PushL(index);
		
		iIndexes.AppendL(index);
		
		CleanupStack::Pop();
		}
	}


CAgnSimpleEntryTable::~CAgnSimpleEntryTable()
//
// Release allocated memory
//
	{
	delete iNonRptExtractor;
	delete iRptExtractor;
	delete iTodoNonRptExtractor;
	delete iTodoRptExtractor;

	// destroy all entries in table
	Reset();
	
	iIndexes.ResetAndDestroy();
	
	delete iLocalUidIndex;
    delete iGuidHashIdIndex;	

	if ( iSimpleEntryAllocator )
		{
		iSimpleEntryAllocator->Delete(iFindEntry);
		
		delete iSimpleEntryAllocator;
		}	
	}


TInt CAgnSimpleEntryTable::CompareEntries(const CAgnSimpleEntry& aLeft, const CAgnSimpleEntry& aRight)
//
// Compare two entries - used when sorting entries in the array
//
	{

	if ( aLeft.EntryId() == aRight.EntryId())
		{
		return 0;
		}

	return aLeft.EntryId().Value() < aRight.EntryId().Value() ? -1 : 1;
	}

void CAgnSimpleEntryTable::Reset()
//
// Clear all entries from indices
//
	{

	Commit();

// clear all indices
	const TInt KIndexesCount = iIndexes.Count();
	
	for( TInt ii = 0; ii < KIndexesCount; ++ii )
		{
		CAgnIndex* index = iIndexes[ii];
		
		if ( index )
			{
			index->Reset();
			}	
		}

	if ( iLocalUidIndex )
		{
		iLocalUidIndex->Reset();
		}

	if ( iGuidHashIdIndex )
		{
		iGuidHashIdIndex->Reset();
		}

	// destroy all entries in table
	if ( iSimpleEntryAllocator )
		{
		const TInt KEntriesCount = iArray.Count();
		
		for( TInt ii = 0; ii < KEntriesCount; ++ii ) 
			{
			CAgnSimpleEntry* entryInfo = iArray[ii];

			iSimpleEntryAllocator->Delete(entryInfo);
			}
		}				
			
	iArray.Reset();
	}


CAgnIndex* CAgnSimpleEntryTable::Index(TInt aIndex) const
//
// return index based on index id
//
	{

	return iIndexes[aIndex];
	}


CAgnIndex* CAgnSimpleEntryTable::EntryNonRptIndex(const MAgnCalendarTimeMode::TTimeMode aTimeMode) const 
	{

	switch (aTimeMode)
		{
	case MAgnCalendarTimeMode::EFixedUtc:
		return Index(EAgnEntryNonRptFixedIndex);

	case MAgnCalendarTimeMode::EFloating:
		return Index(EAgnEntryNonRptFloatingIndex);

	default:
		return NULL;
		}
	}


CAgnIndex* CAgnSimpleEntryTable::EntryRptIndex(const MAgnCalendarTimeMode::TTimeMode aTimeMode) const 
	{

	switch (aTimeMode)
		{
	case MAgnCalendarTimeMode::EFixedUtc:
		return Index(EAgnEntryRptFixedIndex);

	case MAgnCalendarTimeMode::EFloating:
		return Index(EAgnEntryRptFloatingIndex);

	default:
		return NULL;
		}
	}


CAgnIndex* CAgnSimpleEntryTable::TodoNonRptIndex(const MAgnCalendarTimeMode::TTimeMode aTimeMode) const 
	{

	switch (aTimeMode)
		{
	case MAgnCalendarTimeMode::EFixedUtc:
		return Index(EAgnTodoNonRptFixedIndex);

	case MAgnCalendarTimeMode::EFloating:
		return Index(EAgnTodoNonRptFloatingIndex);

	default:
		return NULL;
		}
	}


CAgnIndex* CAgnSimpleEntryTable::TodoRptIndex(const MAgnCalendarTimeMode::TTimeMode aTimeMode) const 
	{

	switch (aTimeMode)
		{
	case MAgnCalendarTimeMode::EFixedUtc:
		return Index(EAgnTodoRptFixedIndex);

	case MAgnCalendarTimeMode::EFloating:
		return Index(EAgnTodoRptFloatingIndex);

	default:
		return NULL;
		}
	}


TBool CAgnSimpleEntryTable::Find(const TAgnEntryId& aId, TInt& aPos) const 
//
// Find the position of the first that is equal to aKeyValue
//
	{

	iFindEntry->SetEntryId(aId);

	TLinearOrder<CAgnSimpleEntry> order(CompareEntries);

	aPos = iArray.FindInOrder(iFindEntry, order);
	if (aPos == KErrNotFound )
		{
		return (EFalse);
		}

	return (ETrue);
	}


void CAgnSimpleEntryTable::FindByLastModifiedDateUtcL(const TTime& aLastModifiedDateUtcFilter, 
													RArray<TCalLocalUid>& aFoundIdList) const
/**
Find all entries that are newer than the provided time.
Note this does not include entries equal to the provided time, since this time will be the time of the previous sync.

@internalAll
*/								
	{
	#if defined (__CAL_ENTRY_LOGGING__) || (__CAL_VERBOSE_LOGGING__)
		TBuf<KMinTTimeStrLength> lastModTimeBuf;
		AgmDebug::TTimeStrL(aLastModifiedDateUtcFilter, lastModTimeBuf);
		AgmDebug::DebugLog("FindByLastModifiedDateUtcL: Finding all entries newer than %S",&lastModTimeBuf);
	#endif

	const TInt KSize = iArray.Count();
	TTime lastTzChangedTime = iOwningModel->TzRulesLastModifiedDateL();
	for ( TInt pos = 0; pos < KSize; ++pos )
		{
		const CAgnSimpleEntry* KEntry = iArray[pos];
		if ( KEntry->LastModifiedDateUtc() > aLastModifiedDateUtcFilter || (KEntry->RptDef() && KEntry->TimeMode()!= MAgnCalendarTimeMode::EFloating && lastTzChangedTime > aLastModifiedDateUtcFilter))
			{
			aFoundIdList.AppendL(KEntry->LocalUid());
			}
		}
	_DBGLOG_ENTRY(AgmDebug::DebugLog("FindByLastModifiedDateUtcL: Found %d entries",aFoundIdList.Count());)
	}
	

void CAgnSimpleEntryTable::FindByHashL(const TUint32& aGuidHash,
										RArray<TAgnEntryId>& aFoundIdList) const
/**
Find all entries that are newer than the provided time

@internalAll
*/								
	{

	iGuidHashIdIndex->FindByHashL(aGuidHash, aFoundIdList);		
	}


CAgnSimpleEntry* CAgnSimpleEntryTable::GetEntry(const TAgnEntryId& aEntryId) const
//
// Get entry by entry Id
//
	{

	TInt pos;

	if ( ! Find(aEntryId, pos) )
		{
		return NULL;
		}
		
	return iArray[pos];
	}


CAgnSimpleEntry* CAgnSimpleEntryTable::GetEntry(TCalLocalUid aLocalUid) const
//
// Get entry by unique Id
//
	{

	return GetEntry( iLocalUidIndex->FindByLocalUid(aLocalUid) );
	}


void CAgnSimpleEntryTable::AddEntryL(CAgnSimpleEntry* aSimpleEntry)
//
// Add entry to appropiate index(es), assumes memory has been reserved
// Takes ownership of aSimpleEntry immediately
//
	{

	TLinearOrder<CAgnSimpleEntry> order(CompareEntries);
	TInt pos = iArray.InsertInOrder(aSimpleEntry, order);
	if (pos < 0 && pos != KErrAlreadyExists)
		{
		User::Leave(pos);
		}
	
	const TInt KCount = iIndexes.Count();
	
	for ( TInt ii = 0; ii < KCount; ++ii )
		{
		CAgnIndex* index = iIndexes[ii];
		
		if ( index->IsValid(aSimpleEntry) )
			{
			index->AddL(aSimpleEntry);
			break;
			}
		}

	iLocalUidIndex->AddL(aSimpleEntry);

	// add the entry to the GuidHash index only if the hash is valid

	if( aSimpleEntry->GuidHash() )
		{
		iGuidHashIdIndex->AddL(aSimpleEntry);
		} 

	// mark the cached index as dirty to reflect the change caused 
	// by this addition of an entry
	iOwningModel->MarkIndexFileAsDirtyL();

	}


void CAgnSimpleEntryTable::AddEntryL(const CAgnEntry& aEntry)
//
// Add entry to appropiate index(es)
//
	{
	CAgnSimpleEntry* info = iSimpleEntryAllocator->CreateSimpleEntryL(aEntry.Type(), &aEntry);
	TRAPD(err, AddEntryL(info));
	if (err != KErrNone)
		{
		TInt pos = KErrNotFound;
		if ( Find(info->EntryId(), pos) )
			{
			iArray.Remove(pos);
			}
		DeleteEntryFromIndexes(info);
		User::Leave(err);
		}
	
	// mark the cached index as dirty to reflect the change caused 
	// by this addition of an entry
	iOwningModel->MarkIndexFileAsDirtyL();
	
	}


void CAgnSimpleEntryTable::DeleteEntry(const TAgnEntryId& aId)
//
// Delete a sort entry and remove from the index(es)
//
	{

	TInt pos;

	if ( Find(aId, pos) )
		{
		CAgnSimpleEntry* entry = iArray[pos];
		
		DeleteEntryFromIndexes(entry);
		
		iArray.Remove(pos);	// Delete from table	

		// mark the cached index as dirty to reflect the change caused 
		// by this deletion of an entry
		iOwningModel->MarkIndexFileAsDirtyL();
		
		}
	}


void CAgnSimpleEntryTable::DeleteEntryFromIndexes(CAgnSimpleEntry* aSimpleEntry)
//
// Remove entry from indexes
//
	{

	for( TInt ii = 0; ii < iIndexes.Count(); ++ii )
		{
		iIndexes[ii]->DeleteEntry(aSimpleEntry);
		}

 	iLocalUidIndex->Delete(aSimpleEntry);
 	
	// delete the entry from the GuidHash index only if the hash is valid

	if( aSimpleEntry->GuidHash() )
		{
		iGuidHashIdIndex->Delete(aSimpleEntry);
		}

	iSimpleEntryAllocator->Delete(aSimpleEntry);

	// mark the cached index as dirty to reflect the change caused 
	// by this deletion of an entry
	iOwningModel->MarkIndexFileAsDirtyL();
	
	}


void CAgnSimpleEntryTable::Commit()
//
// Called when committing changes 
//
	{

	// compress indexes
	for( TInt ii = 0; ii < iIndexes.Count(); ++ii )
		{
		iIndexes[ii]->Compress();
		}
	
	if (iLocalUidIndex)	
		{
		iLocalUidIndex->Compress();
		}
	if (iGuidHashIdIndex)	
		{
		iGuidHashIdIndex->Compress();
		}
		
	iArray.Compress();
	}


void CAgnSimpleEntryTable::GetFilteredIndexListL(RArray<TInt>& aIndexList,const TAgnFilter& aFilter) const
   {

   if ( aFilter.AreCompletedTodosIncluded() || aFilter.AreIncompletedTodosIncluded() )
		{
		aIndexList.AppendL(EAgnTodoRptFixedIndex);
		aIndexList.AppendL(EAgnTodoNonRptFixedIndex);
		}
 
   if ( aFilter.AreNonTodosIncluded() )
		{
		aIndexList.AppendL(EAgnEntryRptFixedIndex);
		aIndexList.AppendL(EAgnEntryNonRptFixedIndex);
		}
   }
 

// Returns the correct extractor for the indexType and TimeMode, initializing the extractor with both the index and the
// Filter
//
CAgnInstanceExtractor* CAgnSimpleEntryTable::GetExtractor(TInt aIndexType, MAgnCalendarTimeMode::TTimeMode aTimeMode, const TAgnFilter& aFilter, const TTime& aUndatedTodoTime)
	{
	CAgnInstanceExtractor* extractor = NULL;

	switch( aIndexType )
		{
		case EAgnEntryNonRptFixedIndex:
			{
		    extractor = iNonRptExtractor;
		    if( aTimeMode == MAgnCalendarTimeMode::EFixedUtc )
				{
				extractor->SetIndex(*Index(EAgnEntryNonRptFixedIndex));
				}
			else if( aTimeMode == MAgnCalendarTimeMode::EFloating )
				{
				extractor->SetIndex(*Index(EAgnEntryNonRptFloatingIndex));
				}
			}
			break;
		 
		case EAgnEntryRptFixedIndex:
		   {
		    extractor = iRptExtractor;
			if( aTimeMode == MAgnCalendarTimeMode::EFixedUtc )
				{
				extractor->SetIndex(*Index(EAgnEntryRptFixedIndex));
				}
			else if( aTimeMode == MAgnCalendarTimeMode::EFloating )
				{
				extractor->SetIndex(*Index(EAgnEntryRptFloatingIndex));
				}
			}    	
			break;

		case EAgnTodoNonRptFixedIndex:
			{
		    extractor = iTodoNonRptExtractor;
			if( aTimeMode == MAgnCalendarTimeMode::EFixedUtc )
				{
				extractor->SetIndex(*Index(EAgnTodoNonRptFixedIndex));
				}
			else if( aTimeMode == MAgnCalendarTimeMode::EFloating )
				{
				extractor->SetIndex(*Index(EAgnTodoNonRptFloatingIndex));
				}
			}
			break;
		
	    case EAgnTodoRptFixedIndex:
			{
		    extractor = iTodoRptExtractor;
			if( aTimeMode == MAgnCalendarTimeMode::EFixedUtc )
				{
				extractor->SetIndex(*Index(EAgnTodoRptFixedIndex));
				}
			else if( aTimeMode == MAgnCalendarTimeMode::EFloating )
				{
				extractor->SetIndex(*Index(EAgnTodoRptFloatingIndex));
				}
			} 
			break;
		
		default:
			Panic(EAgmErrInvalidSimpleEntryIndex);
			break;
		}

	if ( extractor )
		{
		extractor->SetFilter(aFilter);
		extractor->SetUndatedTodoTime(aUndatedTodoTime);
		}

	return extractor;
	}
