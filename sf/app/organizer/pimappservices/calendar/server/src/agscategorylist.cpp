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

#include "agscategorylist.h"

#include "agmentry.h"
#include "agmcategory.h"
#include "agsfilemanager.h"
#include "agssess.h"

//
// CAgnCategoryList
//
//

CAgnCategoryList::CAgnCategoryList(CAgnServFile& aAgnServFile)
:iAgnServFile(aAgnServFile)
	{
	}

CAgnCategoryList::~CAgnCategoryList()
	{
	delete iCatOldName;
	if(iCategories)
		{
		iCategories->ResetAndDestroy();
		iCategories->Close();
		delete iCategories;
		}
	if (iCategoriesBuffer)
		{
		iCategoriesBuffer->ResetAndDestroy();
		iCategoriesBuffer->Close();
		delete iCategoriesBuffer;
		}
	}

CAgnCategoryList* CAgnCategoryList::NewL(CAgnServFile& aAgnServFile)
	{
	CAgnCategoryList* self = new (ELeave) CAgnCategoryList(aAgnServFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAgnCategoryList::ConstructL()
	{
	iCategories = new(ELeave) RPointerArray<CAgnCategory>();
	
	// create the default vCal categories
	for(TInt a = 0; a < CCalCategory::ECalExtended; ++a)
		{
		CAgnCategory* category = CAgnCategory::NewL(static_cast<CCalCategory::TCalCategoryType>(a));
		CleanupStack::PushL(category);	
		User::LeaveIfError(iCategories->Append(category));
		CleanupStack::Pop(category);				
		}
	}

void CAgnCategoryList::AddCategoryL(const TDesC& aCategoryName)
// Adds a category to the list; the category is appended to the category list
// aCategoryName must be valid and not in the category list otherwise it leaves.
	{
	// check that category is new
	TInt index = 0;
	if(ListContains(aCategoryName, index, EFalse))
		{
		// Leave with error if client tries to add category which already exists
		User::Leave(KErrAlreadyExists);
		}

	if (aCategoryName.Length() == 0)
		User::Leave(KErrArgument);
	
	CreateCategoryBackupL();

	CAgnCategory* category = CAgnCategory::NewL(aCategoryName);
	CleanupStack::PushL(category);				
	User::LeaveIfError(iCategories->Append(category));
	CleanupStack::Pop(category);
	UpdateCategoryListInFileL();
	
	DestroyCategoryBackup();
	}

void CAgnCategoryList::AddCategoryL(CAgnCategory& aCategory)
// Adds a category to the list
// aCategory must be the extended type and has a name otherwise it leaves
	{
	if(aCategory.Category() == CCalCategory::ECalExtended)
		AddCategoryL(aCategory.ExtendedCategoryName());
	else
		User::Leave(KErrAlreadyExists);
	}

void CAgnCategoryList::DeleteCategoryL(TInt aIndex)
// Deletes a category from the list if only it is in the category list and it is an extended category
// otherwise leaves 
	{
	if(aIndex >= iCategories->Count() || aIndex < 0 )
		User::Leave(KErrNotFound);

	if(aIndex < CCalCategory::ECalExtended)
		User::Leave(KErrInUse);

	if(iCatOldName)
		{
		delete iCatOldName;
		iCatOldName = NULL;
		}
	iCatOldName = (*iCategories)[aIndex]->ExtendedCategoryName().AllocL();
	delete (*iCategories)[aIndex];
	iCategories->Remove(aIndex);
	}

const CAgnCategory& CAgnCategoryList::Category(TInt aIndex) const
	{
	return *(*iCategories)[aIndex];
	}

void CAgnCategoryList::AddEntryL(CAgnEntry& aEntry)
// adds all categories from aEntry to the category list
// @param: aEntry is the entry which must have its categories acknowledged
	{
	if(!aEntry.CategoryCount())
		return;

	const TInt catCount = aEntry.CategoryCount();
	for(TInt a = 0; a < catCount; ++a)
		{
		CAgnCategory& cat = aEntry.FetchCategory(a); // doesn't return ownership

		TInt catIndex = FindCategoryIndex(cat);
		if(catIndex == KErrNotFound)
			{
			AddCategoryL(cat);
			}
		}
	}

TInt CAgnCategoryList::FindCategoryIndex(CAgnCategory& aCategory)
// returns the category index in the global category list (iCategories)  
// KErrNotFound will be returned if the category is not in the list
//	@param:	aCategory is the category to find an index in the category list for
	{
	if(aCategory.Category() != CCalCategory::ECalExtended)
		{
		// Predefined categories are always in the list, at the index defined by the CCalCategory enum
		return aCategory.Category();
		}

	const TInt KNumberCategories = iCategories->Count();
	for(TInt a = CCalCategory::ECalExtended; a < KNumberCategories; ++a)
		{
		const CAgnCategory& category = *(*iCategories)[a];
		if(category.ExtendedCategoryName().CompareF(aCategory.ExtendedCategoryName()) == 0)
			return a;
		}

	return KErrNotFound;
	}

TBool CAgnCategoryList::ListContains(const TDesC& aName, TInt& aIndex, TBool aCompareCase)
// returns ETrue if aName is contained in a category within the list,
// slots into aIndex the index of the category
//	@param:	aName is the name of the category to look for in the category list
//	@param: aIndex is where the index of the category with aName in the category list is placed
//  @param:	aCompareCase is how to compare the aName with the ones in the category list. Do exact comparison if aCompareCase=ETrue otherwise do a folding comparison
	{
	const TInt numberCategories = iCategories->Count();
	for(TInt a = 0; a < numberCategories; ++a)
		{
		TBool foundCat=EFalse;
		if(aCompareCase)
			foundCat=((*iCategories)[a]->ExtendedCategoryName()== aName);
		else
			foundCat=((*iCategories)[a]->ExtendedCategoryName().CompareF(aName) == 0);
		if(foundCat)
			{
			aIndex = a;
			return ETrue;
			}
		}

	return EFalse;
	}

void CAgnCategoryList::ExternalizeL(RWriteStream& aStream) const
// externalize the category list to aStream
// we only externalize user-defined categories (i.e. those with category index 
// greater or equal to ECalExtended)
	{
	const TInt KNumberCategories = iCategories->Count();
	aStream.WriteInt16L(KNumberCategories - CCalCategory::ECalExtended);
	for(TInt a = CCalCategory::ECalExtended; a < KNumberCategories; ++a)
		{
		(*iCategories)[a]->ExternalizeL(aStream);
		}
	}

void CAgnCategoryList::InternalizeL(RReadStream& aStream)
// Internalize the category list from aStream
// Note this does not import the default categories, only the extended ones.
	{
	const TInt numberCategories = aStream.ReadInt16L();
	for(TInt a = 0; a < numberCategories; ++a)
		{
 		CAgnCategory* category = CAgnCategory::NewL(aStream);
		CleanupStack::PushL(category);			
		User::LeaveIfError(iCategories->Append(category));
		CleanupStack::Pop(category);					
		}
	}

void CAgnCategoryList::CreateCategoryBackupL()
	{
	if(iCategoriesBuffer)
		{
		iCategoriesBuffer->ResetAndDestroy();
		iCategoriesBuffer->Close();
		delete iCategoriesBuffer;
		iCategoriesBuffer=NULL;
		}
	iCategoriesBuffer = new(ELeave) RPointerArray<CAgnCategory>();
	TInt numCategories=iCategories->Count();
	for (TInt i=0; i<numCategories; ++i)
		{
		CAgnCategory* catBufferItem = CAgnCategory::NewL(*(*iCategories)[i]);
		CleanupStack::PushL(catBufferItem);
		User::LeaveIfError(iCategoriesBuffer->Append(catBufferItem));
		CleanupStack::Pop(catBufferItem);
		}
	}

void CAgnCategoryList::UpdateCategoryListInFileL()
	{ // saves the changed category list to the agenda file
	TRAPD(err, iAgnServFile.ExternalizeCategoryListL());
	if (err != KErrNone)
		{
		RollBackL();
		User::Leave(err);
		}
	}

void CAgnCategoryList::RollBackL()
	{ 
// Destroy the content of the existing category list (iCategories) and make the iCategories point to 
// the back up category list (iCategoriesBuffer) which is a copy before an asynchronous task started.
	if(iCategories)
		{
		iCategories->ResetAndDestroy();
		iCategories->Close();
		delete iCategories;
		iCategories = NULL;
		}

	if (iCategoriesBuffer)
		{
		iCategories = iCategoriesBuffer;
		iCategoriesBuffer = NULL;
		}
	}

void CAgnCategoryList::DestroyCategoryBackup()
	{
	if(iCategoriesBuffer)
		{
		iCategoriesBuffer->ResetAndDestroy();
		iCategoriesBuffer->Close();
		delete iCategoriesBuffer;
		iCategoriesBuffer=NULL;
		}
	}


CAgnServFile& CAgnCategoryList::AgnServFile()
	{
	return iAgnServFile;
	}
