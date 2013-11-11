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

#include "calsortedinstanceiterator.h"
#include "calinstanceview.h"
#include "calclient.h"
#include "calinstance.h"
#include "calsessionimpl.h"
#include "agmentry.h"
#include "agmrptdef.h"
#include "calcommonimpl.h"
#include "CalinterimApiPanic.h"

const TInt KCalendarClientMaxFetchedInstances(256); //The maxium number of the instances fetched from server each time
													//It's also the half size of client cache

const TInt KCalendarIteratorInitialNumber(-1); //Initial offset to an empty iterator

#include "calsessionimpl.h"

/** Sorted instance iterator NewL().

@internalComponent
*/
CCalSortedInstanceIterator* CCalSortedInstanceIterator::NewL(const CCalFindInstanceSettings& aSettings, const CCalInstanceViewImpl& aInstanceViewImpl)
	{
	CCalSortedInstanceIterator* self = new (ELeave) CCalSortedInstanceIterator(aInstanceViewImpl);
	CleanupStack::PushL(self);
	self->ConstructL(aSettings);
	CleanupStack::Pop(self);
	return self;
	}

/** Sorted instance iterator NewL().

@internalComponent
*/
CCalSortedInstanceIterator* CCalSortedInstanceIterator::NewL(const CCalFindInstanceSettings& aSettings, const CCalInstanceViewImpl& aInstanceViewImpl, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TUint8 aShortFileId)
	{
	CCalSortedInstanceIterator* self = new (ELeave) CCalSortedInstanceIterator(aInstanceViewImpl);
	CleanupStack::PushL(self);
	self->ConstructL(aSettings, aLocalUid, aInstanceTime, aShortFileId);
	CleanupStack::Pop(self);
	return self;
	}

/** Sorted instance iterator Destructor.

@internalComponent
*/
CCalSortedInstanceIterator::~CCalSortedInstanceIterator()
	{
	if(iIteratorId >= 0)
		{
		iInstanceViewImpl.GetServ().DestroyInstanceIterator(iIteratorId);
		}
	}

/** Sorted instance iterator Constructor.

@internalComponent
*/
CCalSortedInstanceIterator::CCalSortedInstanceIterator(const CCalInstanceViewImpl& aInstanceViewImpl)
	: CCalInstanceIteratorAll(aInstanceViewImpl), iIteratorId(KCalendarIteratorInitialNumber), iStartOfIndex(0) 
	{
	}

/** Sorted instance iterator ContructorL.

Initialize server side instance iterator

@internalComponent
*/
void CCalSortedInstanceIterator::ConstructL(const CCalFindInstanceSettings& aSettings, TCalLocalUid aLocalUid, const TCalTime& aInstanceTime, TUint8 aShortFileId)
	{
	CCalInstanceIteratorAll::ConstructL(aSettings, aLocalUid, aInstanceTime, aShortFileId);
	RArray<TInt64> fileIds;
	iInstanceViewImpl.GetFileIdLC(fileIds);
	iIteratorId = iInstanceViewImpl.GetServ().CreateInstanceIteratorL(iFindInstanceParams, aSettings.SortCriteria(), fileIds);
	CleanupStack::PopAndDestroy(&fileIds);
	
	__ASSERT_DEBUG(iInstanceIdCache->Count() == 1, CalUtils::Panic(EInstanceIteratorIndexCorrupt));
	__ASSERT_DEBUG(iCurrentIndex == 0, CalUtils::Panic(EInstanceIteratorIndexCorrupt));
	
	iStartOfIndex = iInstanceViewImpl.GetServ().InstanceIteratorLocateIndexL(iIteratorId, iInstanceIdCache->At(iCurrentIndex));
	}

/** Sorted instance iterator ContructorL.

Initialize server side instance iterator

@internalComponent
*/
void CCalSortedInstanceIterator::ConstructL(const CCalFindInstanceSettings& aSettings)
	{
	CCalInstanceIteratorAll::ConstructL(aSettings);
	RArray<TInt64> fileIds;
	iInstanceViewImpl.GetFileIdLC(fileIds);
	iIteratorId = iInstanceViewImpl.GetServ().CreateInstanceIteratorL(iFindInstanceParams, aSettings.SortCriteria(),fileIds);
	CleanupStack::PopAndDestroy(&fileIds);
	}

/** Returns the next instance in the iterator

If there are no more instance in the cache then the iterator will try to fetch
some more from the server. An instance is created for the next instance in the
cache, if there is one. 

@return The next instance in the iterator, or NULL if there are no more instances.

@internalComponent
*/
CCalInstance* CCalSortedInstanceIterator::NextL()
	{
	__ASSERT_DEBUG(iCurrentIndex + 1 <= iInstanceIdCache->Count(), CalUtils::Panic(EInstanceIteratorIndexCorrupt));
	
	const TInt KCacheCount(iInstanceIdCache->Count());
	
	if (iCurrentIndex + 1 == KCacheCount)
		{
		// The index is at the end of the cache so try to find some more instances
		TInt index(iStartOfIndex + KCacheCount);
		
		if (KCacheCount > KCalendarClientMaxFetchedInstances)
			{
			const TInt KDeletedCount(KCacheCount - KCalendarClientMaxFetchedInstances);
			// delete some of the begining of the cache before fetching more instance ids
			iInstanceIdCache->Delete(0, KDeletedCount);
			
			iStartOfIndex += KDeletedCount;
			
			// reset the index to the current new end of the array 
			iCurrentIndex = iInstanceIdCache->Count() - 1;
			}
		
		iInstanceViewImpl.GetServ().InstanceIteratorNextL(iIteratorId, *iInstanceIdCache, index);
		
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
some more from the server. An instance is created for the previous instance in the
cache, if there is one. 

@return The previous instance in the iterator, or NULL if there are no more instances.

@internalComponent
*/
CCalInstance* CCalSortedInstanceIterator::PreviousL()
	{
	if (iCurrentIndex == 0 && iStartOfIndex > 0)
		{
		// The index is at the beginning of the cache so try to find some more instances
		if (iInstanceIdCache->Count() > KCalendarClientMaxFetchedInstances)
			{
			// trim the cache before fetching more instance ids
			iInstanceIdCache->Delete(KCalendarClientMaxFetchedInstances, iInstanceIdCache->Count() - KCalendarClientMaxFetchedInstances);
			}

		const TInt KInstanceCountBefore(iInstanceIdCache->Count());
		iInstanceViewImpl.GetServ().InstanceIteratorPreviousL(iIteratorId, *iInstanceIdCache, iStartOfIndex);
		const TInt KInstanceCountAfter(iInstanceIdCache->Count());

		__ASSERT_DEBUG(KInstanceCountAfter >= KInstanceCountBefore, User::Invariant());
		iCurrentIndex += (KInstanceCountAfter - KInstanceCountBefore);
		iStartOfIndex -= (KInstanceCountAfter - KInstanceCountBefore);
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
if there is not it will call Count to calculate if there are more
on server side.

@return ETrue if there are more instances to be returned and EFalse if not.

@internalComponent
*/
TBool CCalSortedInstanceIterator::HasMore() const
	{
	const TInt KCacheSize(iInstanceIdCache->Count());
	
	__ASSERT_DEBUG(iCurrentIndex + 1 <= KCacheSize, CalUtils::Panic(EInstanceIteratorIndexCorrupt));
	
	if((KCacheSize - 1) == iCurrentIndex && (KCacheSize + iStartOfIndex) >= Count())
		{
		//it reaches the end of cached instance so call Count() to get the overall
		//number of instances and check if we reach the end of the whole instances.
		return EFalse;
		}
	
	return ETrue;
	}

/** Finds the total number of intances this iterator is expected to iterate through

@return The instance count

@internalComponent
*/
TInt CCalSortedInstanceIterator::Count() const
	{
	TInt instanceCount(0);
	
	TRAPD(error, instanceCount = iInstanceViewImpl.GetServ().InstanceIteratorCountL(iIteratorId));
		 
	if (error != KErrNone)
		{
		instanceCount = error;
		}
	
	return instanceCount;
	}
