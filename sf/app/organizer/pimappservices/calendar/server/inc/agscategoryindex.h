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

#ifndef __AGSCATEGORYINDEX_H__
#define __AGSCATEGORYINDEX_H__

#include "agmids.h"

//
//	CAgnCategoryIndexItem
//
//	Represents an entry with categories
//	iCategoryList contains an array of indices into the category list,
//	held by the server, for the agenda file
//

class CAgnCategoryIndexItem : public CBase
	{
public:

	static CAgnCategoryIndexItem* NewL();
	static CAgnCategoryIndexItem* NewL(CAgnCategoryIndexItem& aCategoryIndexItem);
	~CAgnCategoryIndexItem();

	inline const TAgnEntryId& EntryId() const;
	inline void SetEntryId(const TAgnEntryId& aId);

	void AddCategoryL(TInt aCategory);
	TBool Contains(TInt aCategoryIndex, TInt& aEntryCategoryIndex) const;

	// called when a category is deleted from the category list
	// decrements all elements in iCategoryList by 1 if they are greater than aIndexDeleted
	TBool UpdateForCategoryDeleted(TInt aIndexDeleted, TInt& aEntryIndex);
	void UpdateEntryId(const TAgnEntryId& aNewEntryId);
		
	void InternalizeL(RReadStream& aReadStream);
	void ExternalizeL(RWriteStream& aWriteStream) const;

private:

	CAgnCategoryIndexItem();
	void ConstructL(CAgnCategoryIndexItem& aCategoryIndexItem);


private:

	TAgnEntryId				iEntryId;
	RArray<TInt>			iCategories;
	};

// inline functions
const TAgnEntryId& CAgnCategoryIndexItem::EntryId() const		{ return iEntryId; 	  }
void CAgnCategoryIndexItem::SetEntryId(const TAgnEntryId& aId)	{ iEntryId = aId;	  }


//
//	CAgnCategoryIndex
//
//	Contains an index item for each entry containing categories in 
//	the agenda file
//

class CAgnCategoryList;
class CAgnEntry;

class CAgnCategoryIndex : public CBase
	{
public:

	static CAgnCategoryIndex* NewL();
	~CAgnCategoryIndex();

	inline void SetCategoryList(CAgnCategoryList* aCategoryList);
	inline CAgnCategoryList* CategoryList() const;

	void DeleteEntryL(const TAgnEntryId& aEntryId);
	void UpdateEntryL(const TAgnEntryId& aOldId, CAgnEntry& aEntry);
	
	TInt CategoryIndex() const;
	void Reset();

	TBool FirstStep() const;
	void PrepareStepTaskL(TInt aTask, TInt aIndex);	
	TInt DoStepCategoryL(TInt aStep);

	void RollBackDeleteCategoryL();
	
	void GetArrayFilterCategoryL(CArrayFixSeg<TAgnEntryId>& aEntries) const;

public:
	void InternalizeL(RReadStream& aReadStream);
	void ExternalizeL(RWriteStream& aWriteStream) const;

private:

	CAgnCategoryIndex();
	void ConstructL();

	void CreateIndexBackupL();
	TInt Index(const TAgnEntryId& aId);
	void RevertForCategoryDeletedL();


	void FinishStepTaskL();

	TInt DoStepDeleteL();
	TInt DoStepFilterL();
	void InternalizeCategoryItemsL(TBool aIsBufferIndex, RReadStream& aReadStream);
	void ExternalizeCategoryItemsL(TBool aIsBufferIndex, RWriteStream& aWriteStream) const;
	void UpdateIndexesL(CAgnCategoryIndexItem& item, CAgnEntry& entry);


private:

	RPointerArray<CAgnCategoryIndexItem>*	iIndex;

	// this is just a handle to an object; this class does not own the category list
	CAgnCategoryList*						iCategoryList;
	CArrayFixSeg<TAgnEntryId>*				iEntryList;
	RArray<TInt>							iEntryIndexToRollBack;
	TInt									iCurrentIndex;
	TInt									iStepSize;
	TBool									iFirstStep;
	TInt									iCategoryIndex;

	RPointerArray<CAgnCategoryIndexItem>*	iIndexBuffer;
	};

// inline functions
void CAgnCategoryIndex::SetCategoryList(CAgnCategoryList* aCategoryList)	{ iCategoryList = aCategoryList; }
CAgnCategoryList* CAgnCategoryIndex::CategoryList() const					{ return iCategoryList; }

#endif
