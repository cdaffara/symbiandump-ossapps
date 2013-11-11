// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agscategoryindex.h"

#include "agscategorylist.h"
#include "agmentry.h"
#include "agsentrymodel.h"
#include "agmcategory.h"
#include "agsfilemanager.h"
#include "agssess.h"

//
//	CAgnCategoryIndexItem
//
//

const TInt KCategoryMinimumStepSize = 10; 

CAgnCategoryIndexItem::CAgnCategoryIndexItem()
	{
	}


CAgnCategoryIndexItem::~CAgnCategoryIndexItem()
	{
	iCategories.Close();
	}


CAgnCategoryIndexItem* CAgnCategoryIndexItem::NewL()
	{
	return new (ELeave) CAgnCategoryIndexItem();
	}


CAgnCategoryIndexItem* CAgnCategoryIndexItem::NewL(CAgnCategoryIndexItem& aCategoryIndexItem)
	{
	CAgnCategoryIndexItem* self = NewL();

	CleanupStack::PushL(self);
	self->ConstructL(aCategoryIndexItem);
	CleanupStack::Pop(self);

	return self;
	}


void CAgnCategoryIndexItem::ConstructL(CAgnCategoryIndexItem& aCategoryIndexItem)
	{
	iEntryId = aCategoryIndexItem.iEntryId;

	const TInt KNumberOfCategories = aCategoryIndexItem.iCategories.Count();
	for ( TInt i = 0; i < KNumberOfCategories; ++i )
		{
		iCategories.AppendL(aCategoryIndexItem.iCategories[i]);
		}
	}


void CAgnCategoryIndexItem::AddCategoryL(TInt aCategory)
// adds a category to the list of categories for this entry
//	@param: aCategory is an index into the category list for the agenda file
	{
	iCategories.AppendL(aCategory);
	}


TBool CAgnCategoryIndexItem::Contains(TInt aCategoryIndex, TInt& aEntryCategoryIndex) const
//	@param aCategoryIndex is the index to match a category in iCategoryList against
//	@return	ETrue if this index item contains a category of value aCategoryIndex
	{
	const TInt KNumberOfCategories = iCategories.Count();
	for ( TInt i = 0; i < KNumberOfCategories; ++i )
		{
		if ( iCategories[i] == aCategoryIndex )
			{
			aEntryCategoryIndex = i;

			return ETrue;
			}
		}

	return EFalse;
	}


void CAgnCategoryIndexItem::UpdateEntryId(const TAgnEntryId& aNewEntryId)
	{
	iEntryId = aNewEntryId;
	}


void CAgnCategoryIndex::UpdateIndexesL(CAgnCategoryIndexItem& aItem, CAgnEntry& aEntry)
	{
	aItem.UpdateEntryId(aEntry.EntryId());
	}


TBool CAgnCategoryIndexItem::UpdateForCategoryDeleted(TInt aIndexDeleted, TInt& aEntryIndex)
// Update this item when a category is deleted from the global category list (CAgnCategoryList).
//
// If the item contains the category represented by aIndexDeleted, remove it from iCategories
// if the item has a category with index greater than aIndexDeleted, decrement it.
//
//	@param:	TInt aIndexDeleted is the value of the category index that has been
//			removed from the global category list for the agenda file
//  @param: TInt& aEntryIndex the index for the item's category list
// return true if a category has been removed, which implies the agenda entry represented by the item needs to be updated. 

	{
	TBool ret = EFalse;

	for ( TInt i = iCategories.Count() - 1; i >= 0; --i )
		{
		if ( iCategories[i] == aIndexDeleted )
			{
			// Remove this category from entry's category list
			iCategories.Remove(i);

			aEntryIndex = i;

			ret = ETrue;
			}
		else if( iCategories[i] > aIndexDeleted )
			{
			--iCategories[i];
			}
		}

	return ret;
	}

void CAgnCategoryIndexItem::InternalizeL(RReadStream& aReadStream)
	{
	TAgnEntryId id;
	id.InternalizeL(aReadStream);
	SetEntryId(id);
	TInt32 catCount = aReadStream.ReadInt32L();
	for (TInt32 counter = 0; counter < catCount; counter++)
		{
		TInt cat = aReadStream.ReadInt16L();
		AddCategoryL(cat);
		}
	}


void CAgnCategoryIndexItem::ExternalizeL(RWriteStream& aWriteStream) const
	{
	EntryId().ExternalizeL(aWriteStream);
	TInt32 catCount = iCategories.Count();
	aWriteStream.WriteInt32L(catCount);
	for (TInt32 counter = 0; counter < catCount; counter++)
		{
		aWriteStream.WriteInt16L(iCategories[counter]);
		}
	}

//
//	CAgnCategoryIndex
//
//
void CAgnCategoryIndex::InternalizeCategoryItemsL(TBool aIsBufferIndex, RReadStream& aReadStream)
	{
	RPointerArray<CAgnCategoryIndexItem>* indexToUse = NULL;
	if (aIsBufferIndex)
		{
		indexToUse = iIndexBuffer;
		}
	else
		{
		indexToUse = iIndex;
		}
		
	TInt32 indexCount = aReadStream.ReadInt32L();
	for (TInt32 counter=0; counter < indexCount; counter++)
		{
		CAgnCategoryIndexItem* item = CAgnCategoryIndexItem::NewL();
		CleanupStack::PushL(item);
		item->InternalizeL(aReadStream);
		indexToUse->AppendL(item);
		CleanupStack::Pop(item);  //indexToUse now owns item
		}
	
	}


void CAgnCategoryIndex::InternalizeL(RReadStream& aReadStream)
	{
	TInt32 counter = 0;
	
	// read in the items for iIndex
	InternalizeCategoryItemsL(EFalse, aReadStream);
		
	TInt32 entryCount = aReadStream.ReadInt32L();
	for (counter = 0; counter < entryCount; counter++)
		{
		TAgnEntryId entryId;
		entryId.InternalizeL(aReadStream);
		iEntryList->AppendL(entryId);
		}
		
	TInt32 rollbackCount = aReadStream.ReadInt32L();
	for (counter = 0; counter < rollbackCount; counter++)
		{
		TInt entryIdx = aReadStream.ReadInt16L();
		iEntryIndexToRollBack.AppendL(entryIdx);
		}
		
	iCurrentIndex = aReadStream.ReadInt16L();
	iStepSize = aReadStream.ReadInt16L();
	iFirstStep = aReadStream.ReadUint8L();
	iCategoryIndex = aReadStream.ReadInt16L();
	
	// read in the items for iIndexBuffer
	InternalizeCategoryItemsL(ETrue, aReadStream);
	}
	
void CAgnCategoryIndex::ExternalizeCategoryItemsL(TBool aIsBufferIndex, RWriteStream& aWriteStream) const
	{
	RPointerArray<CAgnCategoryIndexItem>* indexToUse = NULL;
	if (aIsBufferIndex)
		{
		indexToUse = iIndexBuffer;
		}
	else
		{
		indexToUse = iIndex;
		}
		
	TInt32 indexCount = indexToUse->Count();
	aWriteStream.WriteInt32L(indexCount);
	for (TInt32 counter = 0; counter < indexCount; counter++)
		{
		CAgnCategoryIndexItem* item = (*indexToUse)[counter];
		item->ExternalizeL(aWriteStream);
		}
	}

	
void CAgnCategoryIndex::ExternalizeL(RWriteStream& aWriteStream) const
	{
	TInt32 counter = 0;
	
	if (iIndex)
		{
		// externalize the items in the index
		ExternalizeCategoryItemsL(EFalse, aWriteStream);
		}
	else
		{
		aWriteStream.WriteInt32L(0);
		}
		
		
	TInt32 entryCount = iEntryList->Count();
	aWriteStream.WriteInt32L(entryCount);
	for (counter = 0; counter < entryCount; counter++)
		{
		iEntryList->At(counter).ExternalizeL(aWriteStream);
		}
		
	TInt32 rollbackCount = iEntryIndexToRollBack.Count();
	aWriteStream.WriteInt32L(rollbackCount);
	for (counter = 0; counter < rollbackCount; counter++)
		{
		aWriteStream.WriteInt16L(iEntryIndexToRollBack[counter]);
		}
		
	aWriteStream.WriteInt16L(iCurrentIndex);
	aWriteStream.WriteInt16L(iStepSize);
	aWriteStream.WriteUint8L(iFirstStep);
	aWriteStream.WriteInt16L(iCategoryIndex);

	if (iIndexBuffer)
		{
		//externalize the items in the index
		ExternalizeCategoryItemsL(ETrue, aWriteStream);
		}
	else
		{
		aWriteStream.WriteInt32L(0);
		}
	}

CAgnCategoryIndex::CAgnCategoryIndex()
	: iFirstStep(ETrue), iCategoryIndex(KErrNotFound)
	{
	}


CAgnCategoryIndex::~CAgnCategoryIndex()
	{
	iEntryIndexToRollBack.Close();

	delete iEntryList;

	if( iIndex )
		{
		iIndex->ResetAndDestroy();
		delete iIndex;
		}

	if ( iIndexBuffer )
		{
		iIndexBuffer->ResetAndDestroy();
		delete iIndexBuffer;
		}
	}


CAgnCategoryIndex* CAgnCategoryIndex::NewL()
	{
	CAgnCategoryIndex* self = new (ELeave) CAgnCategoryIndex();

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}


void CAgnCategoryIndex::ConstructL()
	{
	const TInt KGranEntries = 20;
	iEntryList = new (ELeave) CArrayFixSeg<TAgnEntryId>(KGranEntries);
	iIndex = new (ELeave) RPointerArray<CAgnCategoryIndexItem>();
	}


void CAgnCategoryIndex::CreateIndexBackupL()
	{
// Copy the existing index into a backup object - iIndexBuffer
	if ( iIndexBuffer )
		{
		iIndexBuffer->ResetAndDestroy();
		delete iIndexBuffer;
		iIndexBuffer = NULL;
		}

	iIndexBuffer = new (ELeave) RPointerArray<CAgnCategoryIndexItem>();

	const TInt KCount = iIndex->Count();
	for ( TInt i = 0; i < KCount; ++i )
		{
		CAgnCategoryIndexItem* indexBufferItem = CAgnCategoryIndexItem::NewL(*(*iIndex)[i]);
		CleanupStack::PushL(indexBufferItem);

		User::LeaveIfError(iIndexBuffer->Append(indexBufferItem));

		CleanupStack::Pop(indexBufferItem);
		}
	}

void CAgnCategoryIndex::RevertForCategoryDeletedL()
	{
//Adds the category back, which has been deleted during asynchronous deletion, for each entry in the entry list (iEntryList).
	const TInt KNumEntries = iEntryList->Count();

	for ( TInt i = 0; i < KNumEntries; ++i )
		{
		CAgnEntry* entry = CategoryList()->AgnServFile().Model()->FetchEntryL(iEntryList->At(i));
		if (entry)
			{
			CleanupStack::PushL(entry);

			CAgnCategory* cat = CAgnCategory::NewL(iCategoryList->CategoryOldName());
			CleanupStack::PushL(cat);

			entry->AddCategoryL(cat);

			CleanupStack::Pop(cat);

			CategoryList()->AgnServFile().Model()->UpdateEntryL(*entry, NULL, EFalse);

			if( entry->EntryId() != iEntryList->At(i) )
				{
				TInt index = Index(iEntryList->At(i));

				CAgnCategoryIndexItem& item = *((*iIndex)[index]);

				UpdateIndexesL(item, *entry);
				}

			CleanupStack::PopAndDestroy(entry);
			}
		}
	}

void CAgnCategoryIndex::DeleteEntryL(const TAgnEntryId& aEntryId)
// deletes the entry with aEntryId from the index
//	@param: aEntryId is for the entry to be deleted
	{
	TInt index = Index(aEntryId);
	
	if ( index != KErrNotFound )
		{
		CAgnCategoryIndexItem* item = (*iIndex)[index];

		iIndex->Remove(index);

		delete item;
		}
	}

void CAgnCategoryIndex::UpdateEntryL(const TAgnEntryId& aOldId, CAgnEntry& aEntry)
// updates the categories stored in the index item for aOldId with the ones from aEntry
// if aEntry contains no categories, then the item is removed
//	@param:	aOldId is the id of the index item to be updated
//	@param: aEntry is the new entry which can contain categories
	{
	TInt index = Index(aOldId);
	DeleteEntryL(aOldId);//remove the old one from the index if it is there
	
	const TInt KNumberCategories = aEntry.CategoryCount();
	if ( KNumberCategories > 0 )
		{
		CAgnCategoryIndexItem* newItem = CAgnCategoryIndexItem::NewL();
		CleanupStack::PushL(newItem);

		newItem->SetEntryId(aEntry.EntryId());		

		for ( TInt i = 0; i < KNumberCategories; ++i )
			{
			const TInt KCatIndex = iCategoryList->FindCategoryIndex(aEntry.FetchCategory(i));

			__ASSERT_ALWAYS(KCatIndex >= 0, User::Invariant());

			newItem->AddCategoryL(KCatIndex);
			}

		if ( index != KErrNotFound )
			{
			User::LeaveIfError(iIndex->Insert(newItem, index));
			}
		else
			{
			User::LeaveIfError(iIndex->Append(newItem));
			}

		CleanupStack::Pop(newItem);	
		}
	}


TInt CAgnCategoryIndex::Index(const TAgnEntryId& aId)
// returns the index of the entry with aId in iIndex
//	@param aId is the entry to look for
//	@return	TInt value of the index for the entry or KErrNotFound if there is no entry with aId in iIndex
	{
	const TInt numberItems = iIndex->Count();

	for ( TInt i = 0; i < numberItems; ++i )
		{
		if ( (*iIndex)[i]->EntryId() == aId )
			{
			return i;
			}
		}

	return KErrNotFound;
	}


TInt CAgnCategoryIndex::CategoryIndex() const
	{
	return iCategoryIndex;
	}


void CAgnCategoryIndex::FinishStepTaskL()
	{
//The function should be called when an synchronous task has been completed or cancelled.
	iFirstStep = ETrue;
	iCategoryIndex = KErrNotFound;
	iEntryList->Reset();
	iEntryIndexToRollBack.Reset();
	
	if ( iIndexBuffer )
		{
		iIndexBuffer->ResetAndDestroy();
		delete iIndexBuffer;
		iIndexBuffer = NULL;
		}

	iCategoryList->UpdateCategoryListInFileL();
	iCategoryList->DestroyCategoryBackup();
	}


TInt CAgnCategoryIndex::DoStepDeleteL()
	{ // The delete of the entries associated with a particular category is done in steps
	// The item which has a category delete is maintained in the iEntryList array
	// The percentage of the task completed so far is returned
	TInt percentage = KAgnPercentageComplete;
	
	const TInt KIndexCount = iIndex->Count();
	
	if ( KIndexCount > 0 )
		{
		TInt numStep;// the number (Exclusively) at which the loop will stop 
		if ( iCurrentIndex >= iStepSize )
			{
			numStep = iCurrentIndex-iStepSize;				
			}
		else
			{
			numStep = -1;//the last index at which the loop stop should be 0, so numStep must be -1				
			}

		for ( TInt i = iCurrentIndex; i > numStep;  --i )
			{
			CAgnCategoryIndexItem& item = *((*iIndex)[i]);
			TInt entryIndex = -1;
			TBool updateEntry = item.UpdateForCategoryDeleted(iCategoryIndex,entryIndex);

			if ( updateEntry )
				{
				CAgnEntry* entry = CategoryList()->AgnServFile().Model()->FetchEntryL(item.EntryId());
				if (entry)
					{
					CleanupStack::PushL(entry);

					entry->DeleteCategory(entryIndex);

					CategoryList()->AgnServFile().Model()->UpdateEntryL(*entry, NULL, EFalse);

					if ( entry->EntryId() != item.EntryId())
						{
						UpdateIndexesL(item, *entry);
						}

					CleanupStack::PopAndDestroy(entry);
					}
				iEntryList->AppendL(item.EntryId());
				iEntryIndexToRollBack.AppendL(entryIndex);
				}
			}

		iCurrentIndex = numStep;

		percentage = KAgnPercentageComplete * (KIndexCount - iCurrentIndex - 1) / KIndexCount;
		}

	if ( percentage >= KAgnPercentageComplete )
		{
		FinishStepTaskL();
		percentage = KAgnPercentageComplete;
		}
					
	return percentage;
	}


TBool CAgnCategoryIndex::FirstStep() const
	{
	return iFirstStep;
	}


void CAgnCategoryIndex::PrepareStepTaskL(TInt aTask, TInt aIndex)
	{
// The function should be called at first step when an synchronous task starts.
	switch ( aTask )
		{
		case CCalAsyncTaskManager::EDeleteCategory:
			{
			iCategoryList->CreateCategoryBackupL();
			iCategoryList->DeleteCategoryL(aIndex);
			CreateIndexBackupL();
			break;
			}

		case CCalAsyncTaskManager::EFilterCategory:
			// do nothing
			break;

		default:
			User::Invariant();
			break;
		}
	
	const TInt KIndexCount = iIndex->Count();
	
	iStepSize = KIndexCount / KCategoryMinimumStepSize;

	if ( iStepSize < KCategoryMinimumStepSize )
		{
		iStepSize = KCategoryMinimumStepSize;
		}
			
	iCurrentIndex = KIndexCount - 1;
	iEntryList->Reset();
	iEntryIndexToRollBack.Reset();
	iFirstStep=EFalse;
	iCategoryIndex=aIndex;
	}


TInt CAgnCategoryIndex::DoStepFilterL()
	{// The filtering of the the entries associated with a particular category is done in steps
	// The filtered list of entries is put into the iEntryList array
	// The percentage of the task completed so far is returned
	TInt percentage = KAgnPercentageComplete;

	const TInt KIndexCount = iIndex->Count();

	if ( KIndexCount > 0 )
		{
		TInt numStep;// the number (Exclusively) at which the loop will stop 

		if ( iCurrentIndex >= iStepSize )
			{
			numStep = iCurrentIndex-iStepSize;				
			}
		else
			{
			numStep = -1;//the last index at which the loop stop should be 0				
			}		

		for ( TInt i = iCurrentIndex; i > numStep; --i )
			{
			CAgnCategoryIndexItem& item = *((*iIndex)[i]);

			TInt entryCatIndex;

			if ( item.Contains(iCategoryIndex, entryCatIndex) )
				{
				iEntryList->AppendL(item.EntryId());
				}
			}

		iCurrentIndex = numStep;

		percentage = KAgnPercentageComplete * (KIndexCount - iCurrentIndex - 1) / KIndexCount;
		}
		
	if ( percentage >= KAgnPercentageComplete )
		{
		iFirstStep = ETrue;

		percentage = KAgnPercentageComplete;
		}

	return percentage;
	}


TInt CAgnCategoryIndex::DoStepCategoryL(TInt aStep)
	{
	TInt progress = KAgnPercentageComplete;

	switch ( aStep )
		{
		case CCalAsyncTaskManager::EFilterCategory:
			progress = DoStepFilterL();
			break;

		case CCalAsyncTaskManager::EDeleteCategory:
			progress = DoStepDeleteL();
			break;

		default:
			break;
		}

	return progress;
	}


void CAgnCategoryIndex::RollBackDeleteCategoryL()
	{
	// this function only called if Delete Category fails
	
	if ( iIndexBuffer )
		{// get the iIndexBuffer into iIndex.
		iIndex->ResetAndDestroy();
		delete iIndex;
		iIndex = iIndexBuffer;
		iIndexBuffer = NULL;
		}
	
	RevertForCategoryDeletedL();
	FinishStepTaskL();
	}


void CAgnCategoryIndex::GetArrayFilterCategoryL(CArrayFixSeg<TAgnEntryId>& aEntries) const
	{
	const TInt KCount = iEntryList->Count();
	for ( TInt i = 0; i < KCount; ++i )
		{
		aEntries.AppendL(iEntryList->At(i));
		}
	}

void CAgnCategoryIndex::Reset()
	{
	iEntryList->Reset();
	}

