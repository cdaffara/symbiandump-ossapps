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

#include "agsinstanceiterator.h"
#include "agsinstancesorter.h"
#include "agmfilter.h"
#include "agmsortcriteria.h"
#include "agsentrymodel.h"
#include "agssortinstance.h"
#include "agmdate.h"
#include "agmpanic.h"
#include "agmsimpleentry.h"
#include "agsfilemanager.h"

const TInt KMaxCachedNumber = 1000; //The maxium number of instance IDs cahed in the server side iterator
const TInt KInitialInstanceCount = -1;
const TInt KInstanceArrayGranularity = 16;

// CAgsInstanceIterator //

CAgsInstanceIterator::CAgsInstanceIterator(TInt aIteratorId,  CAgnServFileMgr&	aFileManager)
	:iIteratorId(aIteratorId),
	iInstanceCount(KInitialInstanceCount),
	iCachedOffset(0),
	iFileManager(aFileManager)
	{
	}

CAgsInstanceIterator::~CAgsInstanceIterator()
	{
	iCachedInstances.Close();
	delete iFindParams;
	delete iSorter;
	iModels.Close();//Doesn't own the elements of the array.
	}

CAgsInstanceIterator* CAgsInstanceIterator::NewLC(TInt aIteratorId, TFindInstanceParams* aFindParams, CAgnSortCriteria* aSortCriteria, 
								const RArray<CCalEntry::TType>& aDefaultTypeOrder, RArray<TInt64>& aFileIds, CAgnServFileMgr&	aFileManager)
	{
	CAgsInstanceIterator* self = new (ELeave) CAgsInstanceIterator(aIteratorId, aFileManager);
	CleanupStack::PushL(self);
	self->ConstructL(aSortCriteria, aDefaultTypeOrder, aFindParams, aFileIds);
	return self;
	}

void CAgsInstanceIterator::ConstructL(CAgnSortCriteria* aSortCriteria, const RArray<CCalEntry::TType>& aDefaultTypeOrder, TFindInstanceParams* aFindParams, RArray<TInt64>& aFileIds)
	{
	iFindParams = NULL;
	iSorter = NULL;
	const TInt count = aFileIds.Count();
	for(TInt ii=0;ii<count;++ii)
		{
		CAgnServFile* file = iFileManager.GetFileL(aFileIds[ii]);
		iModels.AppendL(file->Model());
		}
	iSorter = CAgsInstanceSorter::NewL(aSortCriteria, aDefaultTypeOrder);
	iFindParams = aFindParams;
	}

void CAgsInstanceIterator::AddInstanceL(TAgnSortInstance& aSortInstance, RArray<TAgnInstance>& aInstances)
	{
	TAgnInstance instance;
	instance.iId = aSortInstance.InstanceIdL();
	instance.iCollectionId = aSortInstance.SimpleEntry().CollectionId();
	aInstances.AppendL(instance);
	}

/**
 * Get the instances started after specified index
@param aInstances reference to instance array that will store matched instances
@param aIndex The start index of instance to look for
*/
void CAgsInstanceIterator::NextInstancesL(RArray<TAgnInstance>& aInstances, TInt aIndex)
	{
	if(IsCached(aIndex, EFalse))
		{
		//The required instances are in cache, so suck out the instances
		const TInt KNumber(iFindParams->iNumInstances);
		while(0 < iCachedInstances.Count() && aInstances.Count() < KNumber)
			{
			if(iCachedOffset >= aIndex)
				{
				aInstances.AppendL(iCachedInstances[0]);
				}
			iCachedInstances.Remove(0);
			++iCachedOffset;
			}
		}
	else
		{
		//Either the cache is empty or out of range to aIndex so reloading all instance is needed
		
		//reset cache
		iCachedInstances.Reset();
		iCachedOffset = 0; 
		
		//Fetching and sorting instances
		CArrayFixSeg<TAgnSortInstance>* instances = FindSortedInstanceLC();
		
		const TInt KInstanceCount(instances->Count());
		if(iInstanceCount != KInstanceCount)
			{
			iInstanceCount = KInstanceCount;
			}
		
		if(aIndex < KInstanceCount)
			{
			//get rid of the instances that are before aIndex
			instances->Delete(0, aIndex);
			
			//appending the instances
			const TInt KMaxCount = (instances->Count() > iFindParams->iNumInstances) ? iFindParams->iNumInstances : instances->Count();
			for(TInt index = 0; index < KMaxCount; index++)
				{
				AddInstanceL((*instances)[index], aInstances);
				}
			
			if(KMaxCount == iFindParams->iNumInstances)
				{
				//cache the rest instances which are fetched and indexed after aIndex 
				instances->Delete(0, KMaxCount);
				const TInt KCachedCount = (instances->Count() > KMaxCachedNumber) ? KMaxCachedNumber : instances->Count();  
				
				for(TInt index = 0; index < KCachedCount; index++)
					{
					AddInstanceL((*instances)[index], iCachedInstances);
					}
				
				iCachedOffset = aIndex + KMaxCount;
				}
			}
		
		CleanupStack::PopAndDestroy(instances);
		}
	}

/**
 * Get the instances indexed before aIndex
@param aInstances reference to instance array that will store matched instances
@param aIndex The end index of instances to look for
*/
void CAgsInstanceIterator::PreviousInstancesL(RArray<TAgnInstance>& aInstances, TInt aIndex)
	{
	if(IsCached(aIndex, ETrue))
		{
		//The required instances are in cache, so suck out the instances
		const TInt KNumber(iFindParams->iNumInstances);
		TInt startIndex = (aIndex > KNumber) ? (aIndex - KNumber) : 0;   
		startIndex -= iCachedOffset;   
		
		while(iCachedInstances.Count() > startIndex)
			{
			if(aInstances.Count() < KNumber)
				{
				aInstances.AppendL(iCachedInstances[startIndex]);
				}
			iCachedInstances.Remove(startIndex);
			}
		}
	else
		{
		//Either the cache is empty or out of range to aIndex so reloading all instance is needed
		
		//reset cache
		iCachedInstances.Reset();
		iCachedOffset = 0;
		
		//reloading instances
		CArrayFixSeg<TAgnSortInstance>* instances = FindSortedInstanceLC();

		const TInt KInstanceCount(instances->Count());
		if(iInstanceCount != KInstanceCount)
			{
			iInstanceCount = KInstanceCount;
			}
		
		//Adding the instances indexed before aIndex to aInstances
		TInt startIndex = (aIndex > iFindParams->iNumInstances) ? aIndex - iFindParams->iNumInstances : 0;   	
		TInt endIndex = (aIndex < instances->Count()) ? aIndex : instances->Count() - 1;  
		
		__ASSERT_DEBUG(endIndex >= startIndex, Panic(EAgmErrOutsideArrayRange));
		
		for(TInt index = startIndex; index <= endIndex; index++)
			{
			AddInstanceL((*instances)[index], aInstances);
			}
		
		//Cache in the rest instances 
		iCachedOffset = (startIndex > KMaxCachedNumber) ? startIndex - KMaxCachedNumber : 0;
		
		for(TInt index = iCachedOffset; index < startIndex; index++)
			{
			AddInstanceL((*instances)[index], iCachedInstances);
			}
		
		CleanupStack::PopAndDestroy(instances);
		}
	}

/**
 * Get the instances started after specified index
@return The number of overall instances which shall be contained in the iterator  
*/
TInt CAgsInstanceIterator::CountL()
	{
	if(iInstanceCount == KInitialInstanceCount)
		{
		CArrayFixSeg<TAgnSortInstance>* instances = NULL;
		if(iCachedInstances.Count() == 0 && iCachedOffset == 0)
			{
			instances = FindSortedInstanceLC();
			//cache is empty so just cached in the first part of instances
			const TInt KCachedCount = (iInstanceCount > KMaxCachedNumber) ? KMaxCachedNumber : iInstanceCount;  
			
			for(TInt index = 0; index < KCachedCount; index++)
				{
				AddInstanceL((*instances)[index], iCachedInstances);
				}
			}
		else
			{
			const TInt count = iModels.Count();
			instances = new (ELeave) CArrayFixSeg<TAgnSortInstance>(KInstanceArrayGranularity);
			CleanupStack::PushL(instances);
			for(TInt ii =0; ii<count; ++ii )
				{
				iModels[ii]->FindInstancesL(*instances, *iFindParams);
				}
			}
		iInstanceCount = instances->Count();
		CleanupStack::PopAndDestroy(instances);
		}
	
	return iInstanceCount; 
	}

CArrayFixSeg<TAgnSortInstance>* CAgsInstanceIterator::FindSortedInstanceLC()
	{
	const TInt count = iModels.Count();
	CArrayFixSeg<TAgnSortInstance>* instances = new (ELeave) CArrayFixSeg<TAgnSortInstance>(KInstanceArrayGranularity);
	CleanupStack::PushL(instances);

	for(TInt ii =0; ii<count; ++ii )
		{
		if(iModels[ii]->AgnServFile().IsLocked())
			{
			User::Leave(KErrLocked);
			}
		iModels[ii]->FindInstancesL(*instances, *iFindParams);
		}
	iSorter->SortL(*instances);
	return instances;
	}


TInt CAgsInstanceIterator::LocateIndexL(TAgnInstance& aInstanceId)
	{
	__ASSERT_DEBUG(!aInstanceId.iId.IsNullId(), Panic(EAgmErrInstanceIteratorLocatedIndexFault));
	
	//We are assuming the function is called just after the iterator is created
	__ASSERT_DEBUG(iInstanceCount == KInitialInstanceCount && 
			       iCachedInstances.Count() == 0 && 
			       iCachedOffset == 0, Panic(EAgmErrInstanceIteratorLocatedIndexFault));
	
	//Find all instances in the iterator and sort them
	CArrayFixSeg<TAgnSortInstance>* instances = FindSortedInstanceLC();
	iInstanceCount = instances->Count();
	
	//Find the index of the given instance id.
	CAgnEntryModel* model = iFileManager.GetFileL(aInstanceId.iCollectionId)->Model();

	const CAgnSimpleEntry* searchFromEntry = model->GetSimpleEntryFromIndexes(aInstanceId.iId);
	const_cast<CAgnSimpleEntry*>(searchFromEntry)->SetCollectionId(aInstanceId.iCollectionId);
	
	TAgnSortInstance searchFromInstance(*searchFromEntry);
	
	searchFromInstance.SetL(aInstanceId.iId.Date().LocalL(), iFindParams->iUndatedTodoTimeLocal);
	
	//the search from instance must match the filter settings
	__ASSERT_ALWAYS(searchFromInstance.CheckStartAndEndDateOverlap(*iFindParams), User::Leave(KErrNotFound));

	TInt foundIndex = iSorter->FindL(*instances, searchFromInstance);
	
	//cache the instances on server side
	TInt endIndex;
	if(iInstanceCount > KMaxCachedNumber)
		{
		//The instances are more than the size of cache so just store those after the given instance
		iCachedOffset = foundIndex;
		endIndex = (iInstanceCount - foundIndex) > KMaxCachedNumber ? (foundIndex + KMaxCachedNumber) : iInstanceCount; 
		}
	else
		{
		//cache all instances
		iCachedOffset = 0;
		endIndex = iInstanceCount; 
		}
	
	for(TInt index = iCachedOffset; index < endIndex; index++)
		{
		AddInstanceL((*instances)[index], iCachedInstances);
		}

	CleanupStack::PopAndDestroy(instances);
	
	return foundIndex;
	}


/**
 * Check if the instance at aIndex is cached and the cached instances are
 * enough for fetch(more than iFindParams->iNumInstances)
*/
TBool CAgsInstanceIterator::IsCached(TInt aIndex, TBool aPrevious) const
	{
	const TInt KNumber(iFindParams->iNumInstances);
	TInt startIndex(aIndex);
	if(aPrevious)
		{
		startIndex = (aIndex > KNumber) ? (aIndex - KNumber) : 0;   
		}
	
	return ((iCachedOffset <= startIndex) && (iCachedInstances.Count() >= KNumber));
	}	

// CAgsInstanceIteratorMgr //

CAgsInstanceIteratorMgr* CAgsInstanceIteratorMgr::NewL()
	{
	CAgsInstanceIteratorMgr* self = new (ELeave) CAgsInstanceIteratorMgr();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAgsInstanceIteratorMgr::CAgsInstanceIteratorMgr()
	:iNextAvailableId(0) 
	{
	}

void CAgsInstanceIteratorMgr::ConstructL()
	{
	//buid up the default sort order for sorting entry type
	iDefaultTypeOrder.AppendL(CCalEntry::ETodo);
	iDefaultTypeOrder.AppendL(CCalEntry::EAnniv);
	iDefaultTypeOrder.AppendL(CCalEntry::EEvent);
	iDefaultTypeOrder.AppendL(CCalEntry::EAppt);
	iDefaultTypeOrder.AppendL(CCalEntry::EReminder);
	iDefaultTypeOrder.AppendL(CCalEntry::ENote);
	}

CAgsInstanceIteratorMgr::~CAgsInstanceIteratorMgr()
	{
	//release all iterators
	TInt KCount(iIteratorList.Count());
	for(TInt count = 0; count < KCount; count++)
		{
		delete iIteratorList[count]; 
		}	
	iIteratorList.Close();
	iDefaultTypeOrder.Close();
	}

/**
 * Create an iterator
@param aFindParams Finding instance settings/search criteria
@param aSortCriteria Sort criteria for the iterator
@return Iterator id assigned to the new iterator
*/
TInt CAgsInstanceIteratorMgr::CreateIteratorL(TFindInstanceParams* aFindParams, CAgnSortCriteria* aSortCriteria, RArray<TInt64>& aFileIds, CAgnServFileMgr& aFileManager)
	{
	CAgsInstanceIterator* iterator = CAgsInstanceIterator::NewLC(iNextAvailableId, aFindParams, aSortCriteria, iDefaultTypeOrder, aFileIds, aFileManager);
	iIteratorList.AppendL(iterator);
	CleanupStack::Pop(iterator);
	return iNextAvailableId++; 
	}

TInt CAgsInstanceIteratorMgr::FindIteratorById(TInt aIteratorId) const
	{
	TInt ret(KErrNotFound);
	const TInt KCount(iIteratorList.Count());
	for(TInt index = 0; index < KCount; index++)
		{
		if(iIteratorList[index]->IteratorId() == aIteratorId)
			{
			ret = index;
			break;
			}
		}
	return ret;
	}


/**
 * Remove and destroy and remove an iterator
@param Iterator id of the iterator to be destroyed
*/
void CAgsInstanceIteratorMgr::DestroyIteratorL(TInt aIteratorId)
	{
	TInt index = FindIteratorById(aIteratorId);
	User::LeaveIfError(index);
	
	CAgsInstanceIterator* iterator = iIteratorList[index];
	iIteratorList.Remove(index);
	delete iterator;
	}

void CAgsInstanceIteratorMgr::NextInstancesL(TInt aIteratorId, RArray<TAgnInstance>& aInstances, TInt aStartIndex)
	{
	TInt index = FindIteratorById(aIteratorId);
	User::LeaveIfError(index);
	
	iIteratorList[index]->NextInstancesL(aInstances, aStartIndex);
	}
	
void CAgsInstanceIteratorMgr::PreviousInstancesL(TInt aIteratorId, RArray<TAgnInstance>& aInstances, TInt aLastIndex)
	{
	TInt index = FindIteratorById(aIteratorId);
	User::LeaveIfError(index);
	
	iIteratorList[index]->PreviousInstancesL(aInstances, aLastIndex);
	}

TInt CAgsInstanceIteratorMgr::CountL(TInt aIteratorId)
	{
	TInt index = FindIteratorById(aIteratorId);
	User::LeaveIfError(index);
	
	return iIteratorList[index]->CountL();
	}

TInt CAgsInstanceIteratorMgr::LocateIndexL(TInt aIteratorId, TAgnInstance& aInstanceId)
	{
	TInt index = FindIteratorById(aIteratorId);
	User::LeaveIfError(index);
	
	return iIteratorList[index]->LocateIndexL(aInstanceId);
	}


