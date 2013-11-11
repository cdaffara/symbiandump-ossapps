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

#include "agsindexbase.h"
#include "agmsimpleentry.h"
#include "agmdate.h"
#include "agmpanic.h"
#include "agssortinstance.h"
#include "agmallocator.h"

const TInt KArrayGranularity = 32;

// -------------------------------- CAgnIndex ------------------------------

void CAgnIndex::ExternalizeL(RWriteStream& aWriteStream) const
	{
	TUint32 count = iArray->Count();
	aWriteStream.WriteUint32L(count);
	for (TUint32 i = 0; i < count; ++i)
		{
		aWriteStream.WriteUint32L(iArray->At(i)->Type());
		iArray->At(i)->ExternalizeL(aWriteStream);
		}
	}

void CAgnIndex::InternalizeL(RReadStream& aReadStream, CAgnSimpleEntryAllocator* aAllocator)
	{
	TUint32 count = aReadStream.ReadUint32L();
	
	for(TUint32 i = 0; i < count; ++i)
		{
		CCalEntry::TType type = static_cast<CCalEntry::TType>(aReadStream.ReadUint32L());
		
		CAgnSimpleEntry* simpleEntry = aAllocator->CreateSimpleEntryL(type);
		// the simple entry has been created using the allocator so it
		// cannot be pushed on the cleanup stack because deleting it on
		// a Leave would cause a memory leak. Therefore we have to manually
		// make this this Leave-safe
		TInt err = KErrNone;
		TRAP(err, simpleEntry->InternalizeL(aReadStream));
		if (err != KErrNone)
			{
			aAllocator->Delete(simpleEntry);
			User::Leave(err);
			}
		TRAP(err, iArray->AppendL(simpleEntry));
		if (err != KErrNone)
			{
			aAllocator->Delete(simpleEntry);
			User::Leave(err);
			}	
		}
	
	}



void CAgnIndex::ConstructL()
//
// Create array to insert index entries into
//
	{

	iArray = new(ELeave) CArrayFixSeg<CAgnSimpleEntry*>(KArrayGranularity);
	}

CAgnIndex::~CAgnIndex()
//
// Destructor
//
	{
	delete iArray;
	}

TInt CAgnIndex::Count() const
//
// return number entries in index
//
	{

	return (iArray->Count());
	}

TBool CAgnIndex::IsValid(const CAgnSimpleEntry*) const
// 
// Virtual function : return True if then entry meets the criteria
// for being included in this index, o.w. return False
//
	{

	return (ETrue);
	}

void CAgnIndex::Compress()
//
// Compress index
//
	{
	iArray->Compress();
	}

TBool CAgnIndex::AppendL(CAgnSimpleEntry* const aEntry)
//
// Append entry to end of index - used on loading
//
	{
	if (!IsValid(aEntry))
		{
		return EFalse;
		}		
	Array()->AppendL(aEntry);
	return ETrue;
	}

CArrayFixSeg<CAgnSimpleEntry*>* CAgnIndex::Array() const
//
// returns the pointer array
//
	{ 
		
	return (iArray); 
	}

void CAgnIndex::Delete(TInt aPos)
//
// remove an entry from the array
//
	{
	__ASSERT_ALWAYS(aPos>=0 && aPos < iArray->Count(),Panic(EAgmErrOutsideArrayRange));
	iArray->Delete(aPos);
	}

CAgnSimpleEntry* CAgnIndex::operator[](TInt aPos) const
//
// return entry at aPos
//
	{

	__ASSERT_ALWAYS(aPos>=0 && aPos < iArray->Count(),Panic(EAgmErrOutsideArrayRange));
	return (*iArray)[aPos];
	}

CAgnSimpleEntry* CAgnIndex::Get(TInt aPos) const
//
// return entry at aPos
//
	{

	__ASSERT_ALWAYS(aPos>=0 && aPos < iArray->Count(),Panic(EAgmErrOutsideArrayRange));
	return (*iArray)[aPos];
	}


void CAgnIndex::Reset()
//
// Clear all elements of index
//
	{
	Array()->Reset();
	}

CAgnIndex::CAgnIndex(TAgnKeyBase<CAgnSimpleEntry,TTime>& aKey,MAgnCalendarTimeMode::TTimeMode aTimeMode)
	: iKey(aKey), iTimeMode(aTimeMode)
	{
	}

 TBool CAgnIndex::AddL(CAgnSimpleEntry* const aEntry)
//
// Add index item that refers to supplied entry
//
	{	
	Array()->InsertIsqAllowDuplicatesL(aEntry, iKey);
	
	return ETrue;
	}

 void CAgnIndex::GetKeyValueL(TInt aPos,TTime& aKeyValue) const
//
// Set aKeyValue to the key (w.r.t iKey) value of the entry at position aPos
//
	{

	aKeyValue = iKey.GetKeyValueL(Get(aPos));
	}

 void CAgnIndex::GetKeyValueL(CAgnSimpleEntry* const aEntry, TTime& aKeyValue) const
//
// Set aKeyValue to the key (w.r.t iKey) value of aEntry
//
	{

	aKeyValue = iKey.GetKeyValueL(aEntry);
	}


 TBool CAgnIndex::FindL(const TTime& aKeyValue, TInt& aPos,TBool aFirst) const
//
// Find the position of the first or last item that is equal to aKeyValue
// If aFirst == ETrue then find first
// If aFirst == EFalse then find last
//
	{

	iKey.SetValueL(aKeyValue);
	CAgnSimpleEntry* nullTPtr = NULL;

	if (Array()->FindIsq(nullTPtr, iKey, aPos) != 0)
		{
		return (EFalse);
		}
		
	// duplicates always allowed so iterate back to find the first 
	if (aFirst)		// position on first entry with same key
		{	
		for(;aPos != 0 && !iKey.Compare((*this)[aPos-1]); aPos--)
			;
		}
	else			// position on last entry with same key
		{
		TInt count = Count();
		for(;aPos != count-1 && !iKey.Compare((*this)[aPos+1]); aPos++)
			;
		}
	return (ETrue);
	}
 	
 void CAgnIndex::SetUndatedTodoTime(TTime& aUndatedTodoTime)
 	{
 	iKey.SetUndatedTodoTime(aUndatedTodoTime);
 	Array()->Sort(iKey);
 	}

 TInt CAgnIndex::FindWithEntry(CAgnSimpleEntry* const aEntry)
//
// Find the index entry that refers to aEntry
//
	{
	
	TInt pos;
	if(Array()->FindIsq(aEntry, iKey, pos) != 0)
		{
		return (KErrNotFound);
		}
		
	// duplicates always allowed so iterate back to find the first 
	for(;pos != 0 && !iKey.Compare(aEntry,(*this)[pos-1]); pos--)
		;
		
	// Iterate through to find exact entry
	for(;pos < Count() && aEntry != (*this)[pos];pos++)
		;
	return (pos < Count()? pos : KErrNotFound);
	}

 TBool CAgnIndex::FirstL(TTime& aKeyValue, TInt& aPos) const
//
// Position to first entry that is >= aKeyValue
//
	{
	aPos = 0;
	return FirstValidL(aKeyValue,aPos);
	}

TBool CAgnIndex::FirstValidL(TTime& aKeyValue, TInt& aPos) const
//
// set aPos to the position of the first entry whose key value is >= aKeyValue
//
	{

	for(;aPos < Count() && Get(aPos)->ValidToTimeLocalL() < aKeyValue;aPos++)
		;
	if(aPos < Count())
		{
		if (Get(aPos)->StartTime().LocalL() > aKeyValue)
			{
			aKeyValue = Get(aPos)->StartTime().LocalL();
			}			
		return (ETrue);
		}
	return (EFalse);
	}
	

 TBool CAgnIndex::FirstLessOrEqualL(TTime& aKeyValue, TInt& aPos) const
 //
 // Position to first entry that is <= aKeyValue
 //
	{
	if ( ! FindL(aKeyValue,aPos,EFalse))	// position on last key with same key value
		{
		aPos--;
		}		
	if (aPos >= 0)
		{
		const CAgnSimpleEntry* entry = Get(aPos);
		aKeyValue = iKey.GetKeyValueL(entry);
		return (ETrue);
		}
	return (EFalse);
	}

 TBool CAgnIndex::NextL(TTime& aKeyValue, TInt& aPos) const
//
// Get position of next entry and return its key value
//
	{
	aPos++;
	return FirstValidL(aKeyValue,aPos);
	}

 TBool CAgnIndex::PreviousL(TTime& aKeyValue, TInt& aPos) const
//
// Get position of previous entry and return its key value
//
	{

	aPos--;
	if (aPos > -1)
		{
		aKeyValue = iKey.GetKeyValueL(Get(aPos));
		return (ETrue);
		}
	return (EFalse);
	}


 TBool CAgnIndex::DeleteEntry(CAgnSimpleEntry* const aEntry) 
//
// Delete the index entry that refers to aEntry
//
	{

	TInt pos = FindWithEntry(aEntry);
	if (pos != KErrNotFound)
		{
		CAgnIndex::Delete(pos);		
		return ETrue;
		}

	return EFalse;
	}

MAgnCalendarTimeMode::TTimeMode CAgnIndex::TimeMode() const
	{
	return iTimeMode;
	}

// -------------------------------- EOF ----------------------------------------


