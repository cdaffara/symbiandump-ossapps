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

#include "calinstanceiteratorall.h"
#include "calentryimpl.h"
#include "agmentry.h"
#include "calinstanceview.h"
#include "agmentry.h"
#include "calsessionimpl.h"
#include "calclient.h"
#include "calinstance.h"
#include "agmrptdef.h"
#include "calcommonimpl.h"
#include "CalinterimApiPanic.h"

const TInt KNumInstances(256);

/** All instance iterator NewL().

@internalComponent
*/
CCalInstanceIteratorAll* CCalInstanceIteratorAll::NewL(const CCalFindInstanceSettings& aSettings, const CCalInstanceViewImpl& aInstanceViewImpl)
	{ 
	CCalInstanceIteratorAll* self = new (ELeave) CCalInstanceIteratorAll(aInstanceViewImpl);
	CleanupStack::PushL(self);
	self->ConstructL(aSettings);
	CleanupStack::Pop(self);
	return self;
	}
	
/** All instance iterator NewL() when starting instance is defined.

@internalComponent
*/
CCalInstanceIteratorAll* CCalInstanceIteratorAll::NewL(const CCalFindInstanceSettings& aSettings, const CCalInstanceViewImpl& aInstanceViewImpl, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TUint8 aShortFileId)
	{ 
	CCalInstanceIteratorAll* self = new (ELeave) CCalInstanceIteratorAll(aInstanceViewImpl);
	CleanupStack::PushL(self);
	self->ConstructL(aSettings, aLocalUid, aInstanceTime, aShortFileId);
	CleanupStack::Pop(self);
	return self;
	}

/** All instance iterator constructor

@internalComponent
*/
CCalInstanceIteratorAll::CCalInstanceIteratorAll(const CCalInstanceViewImpl& aInstanceViewImpl)
	:CCalInstanceIterator(aInstanceViewImpl), iCurrentIndex(-1)
	{
	}

/** All instance iterator ConstructL() for when a starting instance is provided.

Set the find instance settings that are provided.

@internalComponent
*/
void CCalInstanceIteratorAll::ConstructL(const CCalFindInstanceSettings& aSettings, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TCalCollectionId aCollectionId)	
	{
	iInstanceIdCache = new(ELeave) CArrayFixSeg<TAgnInstance> (KNumInstances / 4);
	
	SetFindInstanceSettingsL(aSettings);
	
	CAgnSimpleEntry* simpleEntry = iInstanceViewImpl.GetServ().GetSimpleEntryL(aCollectionId, aLocalUid);
	TCleanSimpleEntry cleanSimpleEntry(*simpleEntry, iInstanceViewImpl.GetServ().SimpleEntryAllocator());
	TCleanupItem cleanupItem(TCleanSimpleEntry::DestroySimpleEntry, &cleanSimpleEntry);
	CleanupStack::PushL(cleanupItem);
	
	// The entry should exist
	__ASSERT_ALWAYS(simpleEntry, User::Leave(KErrNotFound));
	
	// Check that the instance is a valid instance of the entry
	__ASSERT_ALWAYS(iInstanceViewImpl.IsValidInstanceL(*simpleEntry, aInstanceTime), User::Leave(KErrNotFound));
	
	// Create and put the starting instance as the first in the array
	
	TAgnInstance instance;
	instance.iCollectionId = aCollectionId;
	if (simpleEntry->EndTime().IsSet())
		{
		// for an undated todo we do not set the instance time
		instance.iId.SetDate(CalUtils::TCalTimeToTAgnCalendarTimeL(aInstanceTime));
		}	
	
	instance.iId.SetEntryId(simpleEntry->EntryId());
	iInstanceIdCache->AppendL(instance);
	iCurrentIndex = 0;
	
	CleanupStack::PopAndDestroy(); // simpleEntry
	}

/** All instance iterator ConstructL()

@internalComponent
*/
void CCalInstanceIteratorAll::ConstructL(const CCalFindInstanceSettings& aSettings)	
	{
	iInstanceIdCache = new(ELeave) CArrayFixSeg<TAgnInstance> (KNumInstances / 4);
	
	SetFindInstanceSettingsL(aSettings);
	}

/** All instance iterator destructor

Frees the cached instance list.

@internalComponent
*/
CCalInstanceIteratorAll::~CCalInstanceIteratorAll()
	{
	delete iInstanceIdCache;
	}

/** Sets the find instance settings that have been provided

@internalComponent
*/
void CCalInstanceIteratorAll::SetFindInstanceSettingsL(const CCalFindInstanceSettings& aSettings)
	{
	// The iterator does not support finding the next of a repeat only
	__ASSERT_ALWAYS( !(aSettings.Filter() & CalCommon::EIncludeRptsNextInstanceOnly) , User::Leave(KErrNotSupported));
	
	iFindInstanceParams.iFilter = TAgnFilter(aSettings.Filter(), aSettings.Behaviour());
	// The filter is not allowerd to have next instance only
	__ASSERT_ALWAYS(!iFindInstanceParams.iFilter.RptNextInstanceOnly(), User::Leave(KErrArgument));
	
	iFindInstanceParams.iUndatedTodoTimeLocal.HomeTime();
	iFindInstanceParams.iRangeStart = CalUtils::TCalTimeToTAgnCalendarTimeL(aSettings.TimeRange().StartTime());
	iFindInstanceParams.iRangeEnd = CalUtils::TCalTimeToTAgnCalendarTimeL(aSettings.TimeRange().EndTime());
	iFindInstanceParams.iNumInstances = KNumInstances;
	const TDesC& searchString = aSettings.Text();
	iFindInstanceParams.iSearchString.Append(searchString);
	iFindInstanceParams.iFilter.iHighPriority = aSettings.PriorityRange().HighestPriority();
	iFindInstanceParams.iFilter.iLowPriority = aSettings.PriorityRange().LowestPriority();
	
	iFindInstanceParams.iFilter.iUserIntFilterMask = 0;
	iFindInstanceParams.iFilter.iUserIntFilterValue = 0;
	}

TBool CCalInstanceIteratorAll::HasMoreL() const
	{
	TBool hasMore(ETrue);
	
	if (iCurrentIndex >= iInstanceIdCache->Count() - 1)
		{
		// There are no more instances in the cache
		// so find out if there are more in the store
		
		CArrayFixSeg<TAgnInstance>* instances = new(ELeave) CArrayFixSeg<TAgnInstance>(1);
		CleanupStack::PushL(instances);
		
		if (iInstanceIdCache->Count() != 0)
			{
			// we have already got some instances so fetch from there
			iFindInstanceParams.iInstance = (*iInstanceIdCache)[iCurrentIndex];
			}
		else
			{
			// we have no instances so search from the start time
			iFindInstanceParams.iInstance.iId.SetDate(iFindInstanceParams.iRangeStart);
			iFindInstanceParams.iInstance.iCollectionId = 0;
			}
	
		iFindInstanceParams.iNumInstances = 256;
		RArray<TInt64> fileIds;
		iInstanceViewImpl.GetFileIdLC(fileIds);

		iInstanceViewImpl.GetServ().NextInstancesL(fileIds, *instances, iFindInstanceParams);
		CleanupStack::PopAndDestroy(&fileIds);
		hasMore = (instances->Count() != 0);
				
		CleanupStack::PopAndDestroy(instances);
		}
	
	return hasMore;
	}

/** Returns the next instance in the iterator

If there are no more instance in the cache then the iterator will try to fetch
some more from the server. An instance is created for the next instance in the
cache, if there is one. 

@return The next instance in the iterator, or NULL if there are no more instances.

@internalComponent
*/
CCalInstance* CCalInstanceIteratorAll::NextL()
	{
	// Check that the current index is not corrupt
	__ASSERT_DEBUG(iCurrentIndex + 1 <= iInstanceIdCache->Count(), CalUtils::Panic(EInstanceIteratorIndexCorrupt));
	
	if (iCurrentIndex + 1 == iInstanceIdCache->Count())
		{
		// The index is at the end of the cache so try to find some more instances
		
		if (iInstanceIdCache->Count() != 0)
			{
			// we have already got some instances so fetch from there
			iFindInstanceParams.iInstance = (*iInstanceIdCache)[iCurrentIndex];
			}
		else
			{
			// we have no instances so search from the start time
			iFindInstanceParams.iInstance.iId.SetDate(iFindInstanceParams.iRangeStart);
			iFindInstanceParams.iInstance.iCollectionId = 0;
			}
		
		iFindInstanceParams.iNumInstances = KNumInstances;
		
		if (iInstanceIdCache->Count() > KNumInstances)
			{
			// delete some of the begining of the cache before fetching more instance ids
			iInstanceIdCache->Delete(0, iInstanceIdCache->Count() - KNumInstances);
			
			// reset the index to the current new end of the array 
			iCurrentIndex = iInstanceIdCache->Count() - 1;
			}
		RArray<TInt64> fileIds;
		iInstanceViewImpl.GetFileIdLC(fileIds);
		iInstanceViewImpl.GetServ().NextInstancesL(fileIds, *iInstanceIdCache, iFindInstanceParams);
		CleanupStack::PopAndDestroy(&fileIds);
		iInstanceIdCache->Compress();
		}
	
	CCalInstance* instance = NULL;
	
	if ( (iCurrentIndex + 1) < iInstanceIdCache->Count() )
		{
		instance = iInstanceViewImpl.CreateInstanceL((*iInstanceIdCache)[iCurrentIndex + 1]);
		++iCurrentIndex;
		}
	
	return instance;
	}

/** Returns the previous instance in the iterator

If there are no more instance in the cache then the iterator will try to fetch
some more from the server. An instance is created for the next instance in the
cache, if there is one. 

@return The previous instance in the iterator, or NULL if there are no more instances.

@internalComponent
*/
CCalInstance* CCalInstanceIteratorAll::PreviousL()
	{	
	if (iCurrentIndex == 0)
		{
		// The index is at the beginning of the cache so try to find some more instances
		
		
		if (iInstanceIdCache->Count() != 0)
			{
			// we have already got some instances so fetch from there
			iFindInstanceParams.iInstance = (*iInstanceIdCache)[iCurrentIndex];
			}
		else
			{
			// we have no instances so search from the start time
			iFindInstanceParams.iInstance.iId.SetDate(iFindInstanceParams.iRangeStart);
			iFindInstanceParams.iInstance.iCollectionId = 0;
			}
		
		iFindInstanceParams.iNumInstances = KNumInstances;
		

		if (iInstanceIdCache->Count() > KNumInstances)
			{
			// delete some of the begining of the cache before fetching more instance ids
			iInstanceIdCache->Delete(KNumInstances, iInstanceIdCache->Count() - KNumInstances);
			}

		const TInt KInstanceCountBefore(iInstanceIdCache->Count());
		RArray<TInt64> fileIds;
		iInstanceViewImpl.GetFileIdLC(fileIds);
		iInstanceViewImpl.GetServ().PreviousInstancesL(fileIds, *iInstanceIdCache, iFindInstanceParams);
		CleanupStack::PopAndDestroy(&fileIds);
		const TInt KInstanceCountAfter(iInstanceIdCache->Count());

		iCurrentIndex += (KInstanceCountAfter - KInstanceCountBefore);

		iInstanceIdCache->Compress();
		}
	
	CCalInstance* instance = NULL;
	
	if ( (iCurrentIndex - 1) < iInstanceIdCache->Count() && (iCurrentIndex - 1) >= 0 )
		{
		instance = iInstanceViewImpl.CreateInstanceL((*iInstanceIdCache)[iCurrentIndex - 1]);
		--iCurrentIndex;
		}
	
	return instance;
	}

/** Finds if there are any more instances to be returned by NextL()

Checks if there are any more instances in the cached instance list
if there is not it does a next instance call to find id there are more
instances after the cache

@return ETrue if there are more instances to be returned and EFalse if not or if there was a problem.

@internalComponent
*/
TBool CCalInstanceIteratorAll::HasMore() const
	{
	TBool hasMore(ETrue);
	// HasMore can't leave so we simply return EFalse if there is an error
	TRAPD(err, hasMore = HasMoreL());
	if (err != KErrNone)
		{
		hasMore = EFalse;
		}
	return hasMore;
	}

/** Finds the total number of intances this iterator is expected to iterate through

Does a FindInstancesL() call to find the total number of instances that we will iterate through.

@return The instance count

@internalComponent
*/
TInt CCalInstanceIteratorAll::Count() const
	{
	TInt instanceCount = 0;
	TRAPD(err, instanceCount = CountL());
	return (err == KErrNone)?instanceCount:err;
	}

TInt CCalInstanceIteratorAll::CountL() const
	{
	CArrayFixSeg<TAgnInstance>* instances = new (ELeave) CArrayFixSeg<TAgnInstance>(8);
	TInt instanceCount = 0;
	if (instances)
		{
		CleanupStack::PushL(instances);
		RArray<TInt64> fileIds;
		iInstanceViewImpl.GetFileIdLC(fileIds);
	 	iInstanceViewImpl.GetServ().FindInstancesL(fileIds, *instances, iFindInstanceParams);
		CleanupStack::PopAndDestroy(&fileIds);
		instanceCount = instances->Count();
		CleanupStack::PopAndDestroy(instances);
		}
	
	return instanceCount;
	}
