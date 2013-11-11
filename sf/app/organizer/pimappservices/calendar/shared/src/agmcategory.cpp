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

#include "agmcategory.h"

#include <s32stor.h>

//
// CAgnCategory
//

CAgnCategory::CAgnCategory()
	{
	}

EXPORT_C CAgnCategory* CAgnCategory::NewL(const TDesC& aExtendedName)
/** Allocates and constructs a category object of user-defined type, with the 
specified category name.

To create a category of a standard type, use the other overload of NewL().

@internalAll
@param aExtendedName The extended category name. */
	{
	if (aExtendedName.Length() == 0)
		{
		User::Leave(KErrArgument);
		}
	return CreateCategoryL(CCalCategory::ECalExtended, aExtendedName);
	}

EXPORT_C CAgnCategory* CAgnCategory::NewL(RReadStream& aStream)
	{
	CAgnCategory* self = new(ELeave)CAgnCategory();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self); 
	return self;
	}

EXPORT_C CAgnCategory* CAgnCategory::NewL(CCalCategory::TCalCategoryType aCategory)
/** Allocates and constructs a category object of one of the standard types.

To create a category of user-defined type, use the other overload of NewL().

@internalAll
@param aCategory The category type. */
	{
	if (aCategory >= CCalCategory::ECalExtended)
		{
		User::Leave(KErrArgument);
		}
	return CreateCategoryL(aCategory, KNullDesC);
	}

CAgnCategory* CAgnCategory::CreateCategoryL(CCalCategory::TCalCategoryType aCategory,const TDesC& aExtendedName)
	{
	CAgnCategory* self = new(ELeave)CAgnCategory();
	CleanupStack::PushL(self);
	self->ConstructL(aCategory, aExtendedName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAgnCategory* CAgnCategory::NewL(const CAgnCategory& aCategory)
	{
	CAgnCategory* self = new(ELeave)CAgnCategory();
	CleanupStack::PushL(self);
	self->CopyL(aCategory);
	CleanupStack::Pop(self);
	return self;
	}

void CAgnCategory::ConstructL(CCalCategory::TCalCategoryType aCategory, const TDesC& aExtendedName) 
	{
	iCategoryType=aCategory;
	iExtendedCategoryName= aExtendedName.AllocL();
	}

CAgnCategory::~CAgnCategory()
	{
	delete iExtendedCategoryName;
	}


EXPORT_C const TDesC& CAgnCategory::ExtendedCategoryName() const
/** Gets the extended category name.

@internalAll
@return The extended category name if there is one, otherwise an empty descriptor. */
	{
	__ASSERT_DEBUG(iExtendedCategoryName,User::Invariant());	
	return *iExtendedCategoryName;
	}

EXPORT_C void CAgnCategory::InternalizeL(RReadStream& aStream)
	{	
	iCategoryType = STATIC_CAST(CCalCategory::TCalCategoryType, aStream.ReadUint8L());

	// Delete any extended category name that already exists
	delete iExtendedCategoryName;
	iExtendedCategoryName = NULL;

	TInt size = aStream.ReadUint32L();
	iExtendedCategoryName = HBufC::NewL(size);
	TPtr bufPtr = iExtendedCategoryName->Des();
	aStream.ReadL(bufPtr,size);
	}

EXPORT_C void CAgnCategory::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(iCategoryType);

	TInt size = 0;		
	if (iExtendedCategoryName)
		{
		size = iExtendedCategoryName->Length();
		aStream.WriteUint32L(size);
		aStream.WriteL(iExtendedCategoryName->Des(), size);
		}
	else
		{
		aStream.WriteUint32L(size);		
		}
	}

void CAgnCategory::CopyL(const CAgnCategory& aSource)
/** Makes a copy of a category.

@internalAll
@param aSource Pointer to the category to copy. */
	{
	iCategoryType = aSource.Category();

	// Delete any extended category name that already exists
	delete iExtendedCategoryName;
	iExtendedCategoryName = NULL;
	iExtendedCategoryName = aSource.ExtendedCategoryName().AllocL();
	}
