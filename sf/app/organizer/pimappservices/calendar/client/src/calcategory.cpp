// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calcategoryimpl.h"

CCalCategory::CCalCategory(CCalCategoryImpl* aImpl)
	{
	iCalCategoryImpl = aImpl;
	}

CCalCategory::CCalCategory()
	{
	}

void CCalCategory::ConstructL(CCalCategory::TCalCategoryType aCategoryType)
	{
	iCalCategoryImpl = CCalCategoryImpl::NewL(aCategoryType);
	}

void CCalCategory::ConstructL(const TDesC& aExtendedName)
	{
	iCalCategoryImpl = CCalCategoryImpl::NewL(aExtendedName);
	}

CCalCategoryImpl* CCalCategory::Impl() const
	{
	return iCalCategoryImpl;
	}

CCalCategory* CCalCategory::NewL(CCalCategoryImpl* aImpl)
	{
	return new (ELeave) CCalCategory(aImpl);;
	}

/** Allocates and constructs a category object of one of the standard types.
@publishedAll
@released
@capability None
@param aCategoryType The category type.
@return Pointer to newly created category object.
*/
EXPORT_C CCalCategory* CCalCategory::NewL(CCalCategory::TCalCategoryType aCategoryType)
	{
	CCalCategory* self = new (ELeave) CCalCategory();
	CleanupStack::PushL(self);
	self->ConstructL(aCategoryType);
	CleanupStack::Pop(self);
	return self;
	}

/** Allocates and constructs a category object of user-defined type with specified category name.
@publishedAll
@released
@capability None
@param aExtendedName The extended category name.
@return Pointer to newly created category object.
*/
EXPORT_C CCalCategory* CCalCategory::NewL(const TDesC& aExtendedName)
	{
	CCalCategory* self = new (ELeave) CCalCategory();
	CleanupStack::PushL(self);
	self->ConstructL(aExtendedName);
	CleanupStack::Pop(self);
	return self;
	}

/** The destructor frees all resources owned by the category, prior to its destruction.
@publishedAll
@released
@capability None
*/
EXPORT_C CCalCategory::~CCalCategory()

	{
	delete iCalCategoryImpl;
	}

/** Gets the category type.
@publishedAll
@released
@capability None
@return The category type.
*/
EXPORT_C CCalCategory::TCalCategoryType CCalCategory::Category() const

	{
	return iCalCategoryImpl->CategoryType();
	}

/** Gets the extended category name.
@publishedAll
@released
@capability None
@return The extended category name if there is one, otherwise an empty descriptor.
*/
EXPORT_C const TDesC& CCalCategory::ExtendedCategoryName() const
	{
	return iCalCategoryImpl->ExtendedCategoryName();
	}
