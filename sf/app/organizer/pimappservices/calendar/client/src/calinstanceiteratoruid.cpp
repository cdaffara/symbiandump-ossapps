// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calinstanceiteratoruid.h"
#include "calsessionimpl.h"
#include "calinstance.h"
#include "calinstanceimpl.h"
#include "calentryimpl.h"
#include "calclient.h"
#include "agmrptdef.h"
#include "agmentry.h"
#include "agmsimpleentry.h"
#include "calcommonimpl.h"
#include "agmdate.h"

/** Uid instance iterator NewL

@internalComponent
*/
CCalInstanceIteratorUid* CCalInstanceIteratorUid::NewL(const CCalInstanceViewImpl& iInstanceViewImpl, const TDesC8& aGuid, const TCalTime& aStartInstance, TUint8 aShortFileId)
	{
	CCalInstanceIteratorUid* self = new(ELeave) CCalInstanceIteratorUid(iInstanceViewImpl);
	CleanupStack::PushL(self);
	self->ConstructL(aGuid, aStartInstance, aShortFileId);
	CleanupStack::Pop(self);
	return self;
	}

/** Uid instance iterator constructor

@internalComponent
*/
CCalInstanceIteratorUid::CCalInstanceIteratorUid(const CCalInstanceViewImpl& iInstanceViewImpl)
:CCalInstanceIterator(iInstanceViewImpl), iCurrentIndex(-1)
	{
	}

/** Uid instance iterator ConstructL

Fetches all the related entries to the UID and sets the currently indexed time

@internalComponent
*/
void CCalInstanceIteratorUid::ConstructL(const TDesC8& aUid, const TCalTime& aInstanceTime, TCalCollectionId aCollectionId)
	{
	// record the time to use for undated todos
	TTime now;
	now.HomeTime();
	iUndatedTodoTime.SetTimeLocalL(now);
	
	// Fetch all the entries that relate to the instance
	RPointerArray<CAgnSimpleEntry> simpleEntries;
	TCleanSimpleEntryArray cleanSimpleEntryArray(simpleEntries, iInstanceViewImpl.GetServ());
	CleanupStack::PushL(TCleanupItem(CCalInstanceViewImpl::DestroySimpleEntryArray, &cleanSimpleEntryArray));
	RArray<TInt> fileIds;
	iInstanceViewImpl.GetShortFileIdLC(fileIds);//It is in order 
	iInstanceViewImpl.GetServ().FetchSimpleEntriesByGuidL(aUid, simpleEntries, fileIds);
	CleanupStack::PopAndDestroy(&fileIds);
	
	const TInt KEntryCount(simpleEntries.Count());
	
	// There must be entries associated with this UID
	__ASSERT_ALWAYS(KEntryCount != 0, User::Leave(KErrNotFound));
	
	TBool instanceExists(EFalse);
	
	for (TInt i(0) ; i < KEntryCount ; ++i)
		{
		CCalLiteEntry* liteEntry = CCalLiteEntry::NewL(*simpleEntries[0], iInstanceViewImpl.GetServ());
		liteEntry->IncrementRefCount();
		simpleEntries.Remove(0);
		TInt appendError = iCalLiteEntries.Append(liteEntry);	
		if (appendError != KErrNone)
			{
			liteEntry->DecrementRefCount();
			User::Leave(appendError);
			}
		
		if (iInstanceViewImpl.IsValidInstanceL(liteEntry->LiteEntry(), aInstanceTime))
			{//Add the index of the entry which has the same time as aInstanceTime into iEntryWithSameTime
			if(liteEntry->LiteEntry().CollectionId() == aCollectionId)
				{
				instanceExists = ETrue;
				iCurrentIndexTime = aInstanceTime;
				iCurrentIndex = iEntryWithSameTime.Count();
				}
			iEntryWithSameTime.AppendL(i);
			}
		}
	
	__ASSERT_ALWAYS(instanceExists, User::Leave(KErrNotFound));
	CleanupStack::PopAndDestroy(); // simpleEntries
	}

/** UID instance iterator destructor

@internalComponent
*/
CCalInstanceIteratorUid::~CCalInstanceIteratorUid()
	{
	// should use CleanArray of LiteEntries 
	const TInt KEntryCount(iCalLiteEntries.Count());
	for (TInt i(0) ; i < KEntryCount ; ++i)
		{
		iCalLiteEntries[i]->DecrementRefCount(); 
		}
	
	iCalLiteEntries.Close();
	iEntryWithSameTime.Close();
	}

TCalTime CCalInstanceIteratorUid::NonRptEntryInstanceTimeL(const CAgnSimpleEntry& aEntry)
	{	
	// The entry does not have a repeat rule so it is just a single instance
	TCalTime entryInstanceTime;
	if (aEntry.Type() == CCalEntry::ETodo)
		{
		TTime completionDate = aEntry.CompletedDateUtc();
		if (completionDate == Time::NullTTime())
			{
			if (!aEntry.EntryTime().IsSet())
				{
				entryInstanceTime = iUndatedTodoTime;
				}
			else
				{
				entryInstanceTime = CalUtils::TAgnCalendarTimeToTCalTimeL(aEntry.EntryTime());
				}
			}
		else
			{
			if (aEntry.TimeMode() == MAgnCalendarTimeMode::EFloating)
				{
				entryInstanceTime.SetTimeLocalFloatingL(AgnDateTime::ConvertToLocalTimeL(completionDate));
				}
			else
				{
				entryInstanceTime.SetTimeUtcL(completionDate);
				}
			}
		}
	else
		{
		entryInstanceTime = CalUtils::TAgnCalendarTimeToTCalTimeL(aEntry.EntryTime());
		}
	
	return entryInstanceTime;
	}

/** Returns the next instance

Loops through all the entries in the cached entry list and finds
the earliest instance that appears after the current index instance time.

@return The next instance

@internalComponent
*/
CCalInstance* CCalInstanceIteratorUid::NextL()
	{
	CCalInstance* retInstance = NULL;
	TInt numSameTimeEntry = iEntryWithSameTime.Count();
	if(numSameTimeEntry >0 && iCurrentIndex < numSameTimeEntry - 1)
		{
		//If there are entries in iEntryWithSameTime, their instance time is same as the iCurrentIndexTime, we should return this instance. 
		return iInstanceViewImpl.CreateInstanceL(*iCalLiteEntries[iEntryWithSameTime[++iCurrentIndex]], iCurrentIndexTime);
		}
	
	TCalTime bestSoFar;
	bestSoFar.SetTimeUtcL(TCalTime::MaxTime());
	TInt thisAgnSimpleEntryIndex(KErrNotFound);
	
	const TInt KEntryCount(iCalLiteEntries.Count());
	TInt i(0);
	for (; i < KEntryCount ; ++i)
		{
		TCalTime entryNextTime;
		entryNextTime.SetTimeUtcL(TCalTime::MaxTime());
		
		CAgnRptDef* thisEntryRptDef = iCalLiteEntries[i]->LiteEntry().RptDef();
		if (thisEntryRptDef)
			{
			TTime entryNextTimeUtc;
			TBool instanceFound(EFalse);
			TTime fromTimeUtc(iCurrentIndexTime.TimeUtcL() + TTimeIntervalMicroSeconds(1));
			
			do
				{
				// keep looping until we find an unexcepted instance or there are no more instances
				instanceFound = thisEntryRptDef->NudgeNextInstanceUtcL(fromTimeUtc, entryNextTimeUtc);
				fromTimeUtc = entryNextTimeUtc + TTimeIntervalMicroSeconds(1);
				}
			while (instanceFound && !thisEntryRptDef->IsAnUnexceptedInstanceL(AgnDateTime::ConvertToLocalTimeL(entryNextTimeUtc) ) );
			
			if (instanceFound)
				{
				entryNextTime.SetTimeUtcL(entryNextTimeUtc);
				}
			}
		else
			{
			TCalTime entryInstanceTime = NonRptEntryInstanceTimeL(iCalLiteEntries[i]->LiteEntry());
						
			if (entryInstanceTime.TimeUtcL() > iCurrentIndexTime.TimeUtcL())
				{
				// The entry's instance time is after the current index time
				// so it is a valid possible next instance
				entryNextTime = entryInstanceTime;
				}
			}
		
		if (entryNextTime.TimeUtcL() < bestSoFar.TimeUtcL()) 
			{
			// This time is better than best time so far so update the best so far
			bestSoFar = entryNextTime;
			thisAgnSimpleEntryIndex = i;
			iEntryWithSameTime.Reset();
			iCurrentIndex = -1;
			}
		else if (entryNextTime.TimeUtcL() == bestSoFar.TimeUtcL() && entryNextTime.TimeUtcL()!= TCalTime::MaxTime())
			{
			//Store the index of the entry which has the instance time as the "bestSoFar" time
			iEntryWithSameTime.AppendL(i);
			}
		}
	
	
	if (bestSoFar.TimeUtcL() != TCalTime::MaxTime())
		{
		retInstance = iInstanceViewImpl.CreateInstanceL(*iCalLiteEntries[thisAgnSimpleEntryIndex], bestSoFar);
		iCurrentIndexTime = bestSoFar;
		}
	
	return retInstance;
	}

/** Returns the previous instance

Loops through all the entries in the cached entry list and finds
the latest instance that appears before the current index instance time.

@return The previous instance

@internalComponent
*/
CCalInstance* CCalInstanceIteratorUid::PreviousL()
	{
	CCalInstance* retInstance = NULL;
	TInt numOfsameTimeInstance = iEntryWithSameTime.Count();
	if(numOfsameTimeInstance > 0 && iCurrentIndex >= 1)
		{
		// we should return the last instance in the array to make it consistent with NextL which returns the first entry's instance.
		return iInstanceViewImpl.CreateInstanceL(*iCalLiteEntries[iEntryWithSameTime[--iCurrentIndex]], iCurrentIndexTime);
		}

	TCalTime bestSoFar;
	bestSoFar.SetTimeUtcL(TCalTime::MinTime());
	TInt thisAgnSimpleEntryIndex(KErrNotFound);
	
	const TInt KEntryCount(iCalLiteEntries.Count());
	TInt i(0);
	for (; i < KEntryCount ; ++i)
		{
		TCalTime entryPreviousTime;
		entryPreviousTime.SetTimeUtcL(TCalTime::MinTime());
		
		CAgnRptDef* thisEntryRptDef = iCalLiteEntries[i]->LiteEntry().RptDef();
		
		if (thisEntryRptDef)
			{
			TTime entryPreviousTimeUtc(iCurrentIndexTime.TimeUtcL());
			TBool instancefound(EFalse);
			TTime fromTimeUtc;
			
			do
				{
				fromTimeUtc = entryPreviousTimeUtc - TTimeIntervalMicroSeconds(1);
				// keep calling until an unexcepted instance is found or there are no more instances
				instancefound = thisEntryRptDef->NudgePreviousInstanceUtcL(fromTimeUtc, entryPreviousTimeUtc);
				}
			while (instancefound && !thisEntryRptDef->IsAnUnexceptedInstanceL(AgnDateTime::ConvertToLocalTimeL(entryPreviousTimeUtc) ) );
			
			if (instancefound)
				{
				entryPreviousTime.SetTimeUtcL(entryPreviousTimeUtc);
				}
			}
		else
			{
			// The entry does not have a repeat rule so it is just a single instance
			TCalTime entryInstanceTime = NonRptEntryInstanceTimeL(iCalLiteEntries[i]->LiteEntry());
			
			if (entryInstanceTime.TimeUtcL() < iCurrentIndexTime.TimeUtcL())
				{
				// The entry's instance time is before the current index time
				// so it is a valid possible next instance
				entryPreviousTime = entryInstanceTime;
				}
			}
		
		if (entryPreviousTime.TimeUtcL() > bestSoFar.TimeUtcL()) 
			{
			// This time is better than best time so far so update the best so far
			bestSoFar = entryPreviousTime;
			thisAgnSimpleEntryIndex = i;
			iEntryWithSameTime.Reset();
			iCurrentIndex = -1;
			}
		else if (entryPreviousTime.TimeUtcL() == bestSoFar.TimeUtcL() && entryPreviousTime.TimeUtcL()!= TCalTime::MinTime())
			{
			iEntryWithSameTime.AppendL(thisAgnSimpleEntryIndex);
			thisAgnSimpleEntryIndex = i;
			}
		}
	
	if (bestSoFar.TimeUtcL() != TCalTime::MinTime())
		{
		retInstance = iInstanceViewImpl.CreateInstanceL(*iCalLiteEntries[thisAgnSimpleEntryIndex], bestSoFar);
		iCurrentIndexTime = bestSoFar;
		numOfsameTimeInstance = iEntryWithSameTime.Count();
		if(numOfsameTimeInstance>0)
			{
			iCurrentIndex = numOfsameTimeInstance;
			}
		}
		
	return retInstance;
	}

/** To check if there are more instance to be returned by NextL().

Calls NextL() to see if if returns another instance, and then sets the current
indexed instance time back to what it was before the call. 

@return ETrue if there are more instance to be returned by NextL(),
EFalse if there are no more instance, and EFalse if there is a problem
calculating if there are more instance to be returned by NextL().

@internalComponent
*/
TBool CCalInstanceIteratorUid::HasMore() const
	{
	TBool returnValue(EFalse);
	TRAP_IGNORE(returnValue = HasMoreL());
	return returnValue;
	}

TBool CCalInstanceIteratorUid::HasMoreL() const
	{
	TBool returnValue(EFalse);

	//Store the current state
	TCalTime storedCurrentIndex = iCurrentIndexTime;
	TInt storedArrayIndex = iCurrentIndex;
	RArray<TInt> storedArray;
	CleanupClosePushL(storedArray);
	const TInt count = iEntryWithSameTime.Count();
	for(TInt ii=0; ii<count;++ii)
		{
		storedArray.AppendL(iEntryWithSameTime[ii]);
		}

	//Find instances
	CCalInstance* nextInstance = const_cast<CCalInstanceIteratorUid*>(this)->NextL();
	returnValue = (nextInstance != NULL);
	delete nextInstance;

	// restore the state
	const_cast<CCalInstanceIteratorUid*>(this)->iCurrentIndexTime = storedCurrentIndex; 
	iCurrentIndex = storedArrayIndex;//reset index since it could be moved in NextL
	iEntryWithSameTime.Reset();
	const TInt countSortedArray = storedArray.Count();
	for(TInt ii=0; ii<countSortedArray;++ii)
		{
		iEntryWithSameTime.AppendL(storedArray[ii]);
		}

	CleanupStack::PopAndDestroy(&storedArray);

	return returnValue;
	}

/** Instance iterator count

Loops through all the entries in the cached entry list and finds each of the instance counts.
The instance count is the sum of all the entries instances.

@return The sum of the instance counts for all the entries in the cached list. 

@internalComponent
*/
TInt CCalInstanceIteratorUid::Count() const
	{
	TInt returnInstanceCount(0);
	const TInt KEntryCount(iCalLiteEntries.Count());
	for (TInt i(0) ; i < KEntryCount ; ++i)
		{
		CAgnRptDef* rptDef = iCalLiteEntries[i]->LiteEntry().RptDef();
		if (rptDef)
			{
			// This gets all the instances generated by the repeat rule
			TRAPD(error, returnInstanceCount += rptDef->InstanceCountL());
			if (error)
				{
				// an error has occoured whilst calculating the instance count
				// so quit and retrun the error code
				returnInstanceCount = error;
				break;
				}
			
			// Need to take off valid exception dates
			if (rptDef->Exceptions())
				{
				returnInstanceCount -= rptDef->Exceptions()->Count();
				}
			}
		else
			{
			// The entry does not have a repeat rule
			// so it is just a single instance
			++returnInstanceCount;	
			}
		}
	
	return returnInstanceCount;
	}


