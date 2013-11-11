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

#include "agsindexiterator.h"

#include "agmdate.h"
#include "agmpanic.h"
#include "agsindex.h"

// ----------------------------TAgnEntryDateIterator----------------------------
// Entry Iterator 
TAgnEntryDateIterator::TAgnEntryDateIterator(CAgnSimpleEntryTable& aEntryInfoTable, const TTime& aUndatedTodoTime, const TAgnFilter& aFilter):
		iNonRptFixedIter(aEntryInfoTable.EntryNonRptIndex(MAgnCalendarTimeMode::EFixedUtc), aFilter),
		iNonRptFloatingIter(aEntryInfoTable.EntryNonRptIndex(MAgnCalendarTimeMode::EFloating), aFilter),
		iRptFixedIter(aEntryInfoTable.EntryRptIndex(MAgnCalendarTimeMode::EFixedUtc), aFilter),
		iRptFloatingIter(aEntryInfoTable.EntryRptIndex(MAgnCalendarTimeMode::EFloating), aFilter),
		iTodoNonRptFixedIter(aEntryInfoTable.TodoNonRptIndex(MAgnCalendarTimeMode::EFixedUtc), aFilter),
		iTodoNonRptFloatingIter(aEntryInfoTable.TodoNonRptIndex(MAgnCalendarTimeMode::EFloating), aFilter),
		iTodoRptFixedIter(aEntryInfoTable.TodoRptIndex(MAgnCalendarTimeMode::EFixedUtc), aFilter),
		iTodoRptFloatingIter(aEntryInfoTable.TodoRptIndex(MAgnCalendarTimeMode::EFloating), aFilter)
	//
	// Populate the array of iterators with only those that are
	// required w.r.t to the filters
	//
	{
	TInt i = 0;
	if (aFilter.AreNonTodosIncluded())
		{
		iIteratorsInUse[i++] = &iNonRptFixedIter;
		iIteratorsInUse[i++] = &iNonRptFloatingIter;
		iIteratorsInUse[i++] = &iRptFixedIter;
		iIteratorsInUse[i++] = &iRptFloatingIter;
		}
	if (aFilter.AreCompletedTodosIncluded() || aFilter.AreIncompletedTodosIncluded())
		{
		iIteratorsInUse[i++] = &iTodoNonRptFixedIter;
		iIteratorsInUse[i++] = &iTodoNonRptFloatingIter;
		iIteratorsInUse[i++] = &iTodoRptFixedIter;
		iIteratorsInUse[i++] = &iTodoRptFloatingIter;
		}
	
	iCount = i;
		
	for (TInt ii =0; ii < iCount; ii++)
		{
		iIteratorsInUse[ii]->SetUndatedTodoTime(aUndatedTodoTime);
		}
	}

const CAgnSimpleEntry& TAgnEntryDateIterator::CurrentElement() const
/**
Return the current element in the iterator

@internalAll
*/
	{
	__ASSERT_ALWAYS(iCurrent >= 0 && iCurrent < iCount,Panic(EAgmErrCompoundIteratorOutOfRange));
	return iIteratorsInUse[iCurrent]->CurrentElement();
	}
	
const TTime& TAgnEntryDateIterator::CurrentKey() const
/**
return current key

@internalAll
*/
	{

	__ASSERT_ALWAYS(iCurrent>=0 && iCurrent<iCount,Panic(EAgmErrCompoundIteratorOutOfRange));
	return iIteratorsInUse[iCurrent]->CurrentKey();
	}
	

void TAgnEntryDateIterator::StartL()
/**
Position all iterator to their start

@internalAll
*/
	{

	for(TInt ii = 0; ii < iCount; ii++)
		{
		iIteratorsInUse[ii]->StartL();
		}
	SetCurrent();
	}

void TAgnEntryDateIterator::NextL()
/**
move position to the next element

@internalAll
*/
	{
	__ASSERT_ALWAYS(iCurrent>=0 && iCurrent<iCount,Panic(EAgmErrCompoundIteratorOutOfRange));

	TTime currentKey = iIteratorsInUse[iCurrent]->CurrentKey();
	iIteratorsInUse[iCurrent]->NextL();
	if (iIteratorsInUse[iCurrent]->AtEnd() || currentKey < iIteratorsInUse[iCurrent]->CurrentKey())
		SetCurrent();
	}
	
	
void TAgnEntryDateIterator::PreviousL()
/**
move position to previous element

@internalAll
*/
	{

	__ASSERT_ALWAYS(iCurrent>=0 && iCurrent<iCount,Panic(EAgmErrCompoundIteratorOutOfRange));

	TTime currentKey = iIteratorsInUse[iCurrent]->CurrentKey();
	iIteratorsInUse[iCurrent]->PreviousL();
	if (iIteratorsInUse[iCurrent]->AtStart() || currentKey > iIteratorsInUse[iCurrent]->CurrentKey())
		SetCurrentLessOrEqual();
	}

TBool TAgnEntryDateIterator::AtEnd()
/**
Are all iterators positioned to beyond the end

@internalAll
*/
	{

	TInt ii;
	for(ii = 0; ii < iCount && iIteratorsInUse[ii]->AtEnd();ii++)
		;
	return (ii == iCount);
	}

TBool TAgnEntryDateIterator::AtStart()
/**
Are all iterators positioned to before the start

@internalAll
*/
	{

	TInt ii;
	for(ii = 0; ii < iCount && iIteratorsInUse[ii]->AtStart();ii++)
		;
	return (ii == iCount);
	}

TBool TAgnEntryDateIterator::GotoL(TTime& aKeyValue)
/**
Position to the first element whose keyvalue is >= aKeyValue

@internalAll
*/
	{

	for(TInt ii = 0; ii < iCount;ii++)
		{
		TTime wkKeyValue(aKeyValue);
		iIteratorsInUse[ii]->GotoL(wkKeyValue);
		}
	return SetCurrent();
	}

TBool TAgnEntryDateIterator::GotoLessOrEqualL(TTime& aKeyValue)
/**
Position to the first element whose keyvalue is >= aKeyValue

@internalAll
*/
	{

	for(TInt ii = 0; ii < iCount;ii++)
		{
		TTime wkKeyValue(aKeyValue);
		iIteratorsInUse[ii]->GotoLessOrEqualL(wkKeyValue);
		}
	return SetCurrentLessOrEqual();
	}

TBool TAgnEntryDateIterator::SetCurrent()
//
// Set the current iterator
//
	{

	TTime best(AgnDateTime::MinDate());
	iCurrent = -1;
	for(TInt ii = 0; ii < iCount; ii++)
		{
		if (!iIteratorsInUse[ii]->AtEnd() && (iCurrent == -1 || best > iIteratorsInUse[ii]->CurrentKey()))
			{
			best = iIteratorsInUse[ii]->CurrentKey();
			iCurrent = ii;
			}
		}
	if(iCurrent == -1)
		return (EFalse);
	return (ETrue);
	}

TBool TAgnEntryDateIterator::SetCurrentLessOrEqual()

//
// Set the current iterator
//
	{

	TTime best(AgnDateTime::MinDate());
	iCurrent = -1;
	for(TInt ii = 0; ii < iCount; ii++)
		{
		if (!iIteratorsInUse[ii]->AtStart() && (iCurrent == -1 || best < iIteratorsInUse[ii]->CurrentKey()))
			{
			best = iIteratorsInUse[ii]->CurrentKey();
			iCurrent = ii;
			}
		}
	if(iCurrent == -1)
		return (EFalse);
	return (ETrue);
	}

TAgnEntryDateIterator::~TAgnEntryDateIterator()
	{
	}

// --------------------------TAgnTodoNonRptIterator-----------------------------
TAgnTodoNonRptIterator::TAgnTodoNonRptIterator(CAgnIndex* aIndex,const TAgnFilter& aFilter):
                        TAgnIterator(aIndex, aFilter)
                        {
                        }

TAgnTodoNonRptIterator::TAgnTodoNonRptIterator()
                        {
                        }

TBool TAgnTodoNonRptIterator::GotoL(TTime& aKeyValue)
	{
	const_cast<CAgnIndex*>(Array())->SetUndatedTodoTime(iUndatedTodoTime);
	return TAgnIterator::GotoL(aKeyValue);
	}

TBool TAgnTodoNonRptIterator::GotoLessOrEqualL(TTime& aKeyValue)
	{
	const_cast<CAgnIndex*>(Array())->SetUndatedTodoTime(iUndatedTodoTime);
	return TAgnIterator::GotoLessOrEqualL(aKeyValue);
	}


// -------------------------------- EOF ----------------------------------------
