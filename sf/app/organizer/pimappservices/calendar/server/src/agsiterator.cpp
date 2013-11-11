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

#include "agsiterator.h"
#include "agsindex.h"
#include "agsstreamidset.h"
#include "agsentrymanager.h"
#include "agmdate.h"
#include "agmentry.h"
#include "agmpanic.h"

// --------------------------TAgnIterator-------------------

TAgnIterator::TAgnIterator(CAgnIndex* aIndex, const TAgnFilter& aFilter):
         TAgnIteratorBase()
/**
@internalAll
*/
	{
	iArray = aIndex;
	iFilter = aFilter;
	}
	
TAgnIterator::TAgnIterator()
	{
	}

const CAgnIndex* TAgnIterator::Array() const
//
// return array that is being iterated over
//
	{
	return (iArray);
	}

 void TAgnIterator::SetCursor(TInt aCursor)
//
// set the current position of iterator
//
	{

	iCursor = aCursor;
	}

 TInt TAgnIterator::Cursor() const
//
// return current position of iterator
//
	{

	return (iCursor);
	}

void TAgnIterator::SetCurrentKey(const TTime& aKeyValue)
//
// set the current key value
//
	{
	iCurrentKey = aKeyValue;
	}


const CAgnSimpleEntry& TAgnIterator::CurrentElement() const
/**
return current element 

@internalAll
*/
	{

	return *(iArray->Get(iCursor));
	}

const TTime& TAgnIterator::CurrentKey() const
/**
return current key value

internalComponent
*/
	{
	return (iCurrentKey);
	}

 TBool TAgnIterator::IsValidElement()
//
// Is current element valid w.r.t. filter?
//
	{
	__ASSERT_ALWAYS(iCursor>=0 && iCursor<iArray->Count(),Panic(EAgmErrOutsideArrayRange));
	return(iFilter.IsValid(CurrentElement()));
	}

 void TAgnIterator::NextL()
/**
position to next element which is valid w.r.t. filter?

@internalAll
*/
	{
	iPreviousKey = iCurrentKey;
	if (iArray->Count() > 0)
		{
		TBool found = EFalse;
		iArray->NextL(iCurrentKey, iCursor);
		TTime target = iPreviousKey;
		MovedForwardL(target);
		while(!found && !AtEnd()) 
			{
			found = IsValidElement();
			if (!found)
				{
				iPreviousKey = iCurrentKey;
				iArray->NextL(iCurrentKey, iCursor);
				TTime target = iPreviousKey;
				MovedForwardL(target);
				}
			}
		}
	}

void TAgnIterator::PreviousL()
/**
position to previous element which is valid w.r.t. filter?

@internalAll
*/
	{
	iPreviousKey = iCurrentKey;
	if (iArray->Count() > 0)
		{
		TBool found = EFalse;
		iArray->PreviousL(iCurrentKey, iCursor);
		TTime target = iPreviousKey;
		MovedBackL(target);
		while(!found && !AtStart()) 
			{
			found = IsValidElement();
			if (!found)
				{
				iArray->PreviousL(iCurrentKey, iCursor);
				MovedBackL(target);
				}
			}
		}
	}

 void TAgnIterator::StartL() 
/**
Position to first valid element

@internalAll
*/
	{ 

	SetCursor(0);	
	if (iArray->Count() > 0)
		{
		iArray->GetKeyValueL(0,iCurrentKey);
		while(!AtEnd() && !IsValidElement())// iterate until entry matches filter
			{
			NextL();
			}
		}
	}

TBool TAgnIterator::FirstL(TTime& aTargetKey)
//
//
//
	{

	TInt pos = 0;
	iCurrentKey = aTargetKey;
	TBool found = iArray->FirstL(iCurrentKey,pos);
	SetCursor(pos);		
	found = MovedToL(aTargetKey);
	aTargetKey = iCurrentKey;
	return found;
	}

 TBool TAgnIterator::GotoL(TTime& aTargetKey) 
/**
Position to the first element whose keyvalue is >= aTargetValue

@internalComponent
*/
	{
	TBool found = EFalse;
	if (iArray->Count() > 0)
		{
		found = FirstL(aTargetKey);
		if (found) 
			{
			while(!AtEnd())
				{
				found = IsValidElement();
				if (found)
					break; // iterate until entry matches filter
				NextL();
				}
			aTargetKey = iCurrentKey;
			}
		}
	return (found);
	}

TBool TAgnIterator::FirstLessOrEqualL(TTime& aTargetKey)

	{

	TInt pos = 0;
	iCurrentKey = aTargetKey;
	TBool found = iArray->FirstLessOrEqualL(iCurrentKey,pos);
	SetCursor(pos);		
	found = MovedBackL(aTargetKey);
	aTargetKey = iCurrentKey;
	return found;
	}

TBool TAgnIterator::GotoLessOrEqualL(TTime& aKeyValue)
/**
Position iterator at first element whose key value is less than or equal to aKeyValue

@internalAll
*/
	{
	TBool found = EFalse;
	if (iArray->Count() > 0)
		{
		found = FirstLessOrEqualL(aKeyValue);
		while(iCursor >= 0)
			{
			found = IsValidElement();
			if (found) 
				break; // iterate until entry matches filter
			PreviousL();
			}
		aKeyValue = iCurrentKey;
		return (found);
		}
	return (EFalse);
	}


TBool TAgnIterator::AtEnd()
/**
Is the current position beyond the end of the array

@internalAll
*/
	{

	return (iArray->Count() == 0 || iCursor >= iArray->Count());
	}

TBool TAgnIterator::AtStart()
/**
Is the current position before the start of the array

@internalAll
*/
	{

	return (iArray->Count() == 0 || iCursor < 0);
	}

// TAgnEntryIter //

TAgnEntryIter::TAgnEntryIter(CAgnStreamIdSet& aStreamIdSet, CAgnEntryManager& aEntryManager)
	: iStreamIdSet(aStreamIdSet), iEntryManager(aEntryManager)
/** Constructs the entry iterator.

@param aModel The agenda model over which the iterator will iterate. */
	{
	}
	

TBool TAgnEntryIter::SetToFirstL()
/** Resets the iterator to the first entry in the file.

@internalAll
@return ETrue if there is at least one entry and its ID can be extracted (using 
At()). Otherwise EFalse. */
	{
	iStreamIdSet.ResetIteratorToStart();
	if (iStreamIdSet.Count() == 0)
		{
		return (EFalse);
		}
	
	TInt currentStream=0;	
	
	// If the contents of a stream are corrupt (indicated by iEntryManager.EntryCount()
	// being 0 - assuming its not an empty file - then skip to the next stream and try that one
		FOREVER
			{
			++currentStream;
			TStreamId id;
			if (!iStreamIdSet.At(id))
				return (EFalse);
			
			//tell the Entry Manager to use the iterator buffer
			iEntryManager.iUseIteratorBuffer = ETrue;
			
			iEntryCount = iEntryManager.RestoreBufferL(id);
			
			//reset the flag in case other buffers need to be used
			iEntryManager.iUseIteratorBuffer = EFalse;
			
			if (iEntryCount == 0)
				{
				if (!iStreamIdSet.Next())
					return (EFalse);
				continue;
				}
			else
				break;
			} 

	__ASSERT_DEBUG(currentStream==1,Panic(EAgmErrCorruptFile));
	
	iEntryIter=0;
	
	return (ETrue);
	}
	

TBool TAgnEntryIter::NextL()
/** Moves the iterator to the next entry.

@internalAll
@return ETrue if there is at least one more entry. EFalse if not. */
	{
	// If we're at the last entry in this cluster, advance to the next cluster
	if (iEntryIter + 1 == iEntryCount)
		{
		FOREVER
			{
			if (!iStreamIdSet.Next())
				return (EFalse);

			TStreamId id;
			if (!iStreamIdSet.At(id))
				return (EFalse);
			
			//tell the Entry Manager to use the iterator buffer
			iEntryManager.iUseIteratorBuffer = ETrue;

			iEntryCount = iEntryManager.RestoreBufferL(id);

			//reset the flag in case other buffers need to be used
			iEntryManager.iUseIteratorBuffer = EFalse;

			if (iEntryCount == 0)
				continue;
			else
				break;
			} 

		iEntryIter=0;
		}
	else
		++iEntryIter;

	return (ETrue);
	}
	

TAgnEntryId TAgnEntryIter::At()
/** Gets the ID of the entry at the current iterator position.

@internalAll
@return The ID of the entry at the current iterator position. */
	{

//	__ASSERT_DEBUG(iEntryIter < iEntryManager.EntryCount() && iEntryIter >= 0,
//				Panic(_L("Invalid index into entry cluster")));

	//tell the Entry Manager to use the iterator buffer
	iEntryManager.iUseIteratorBuffer = ETrue;
	
	TAgnEntryId entryId = iEntryManager[iEntryIter]->EntryId();

	//reset the flag in case other buffers need to be used
	iEntryManager.iUseIteratorBuffer = EFalse;
	
	return entryId;
	}
