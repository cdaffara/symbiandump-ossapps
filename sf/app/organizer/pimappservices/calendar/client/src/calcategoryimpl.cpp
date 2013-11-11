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

#include "agmcategory.h"

CCalCategoryImpl::CCalCategoryImpl()
	{
	iOwnCategory=ETrue;
	}

CCalCategoryImpl::CCalCategoryImpl(CAgnCategory* aCategory)
	{
	iOwnCategory=ETrue;
	iCategory=aCategory;
	}

void CCalCategoryImpl::ConstructL(CCalCategory::TCalCategoryType aCategory)
	{
	iCategory=CAgnCategory::NewL(aCategory);
	}

CCalCategory::TCalCategoryType CCalCategoryImpl::CategoryType() const
	{
	return iCategory->Category();
	}

void CCalCategoryImpl::ConstructL(const TDesC& aExtendedName)
	{
	iCategory=CAgnCategory::NewL(aExtendedName);
	}

CAgnCategory* CCalCategoryImpl::AgnCategory() const
	{
	return iCategory;
	}

CCalCategoryImpl* CCalCategoryImpl::NewL(CCalCategory::TCalCategoryType aCategoryType)
	{
	CCalCategoryImpl* self = new (ELeave) CCalCategoryImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aCategoryType);
	CleanupStack::Pop(self);
	return self;
	}

CCalCategoryImpl* CCalCategoryImpl::NewL(const TDesC& aExtendedName)
	{
	CCalCategoryImpl* self = new (ELeave) CCalCategoryImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aExtendedName);
	CleanupStack::Pop(self);
	return self;
	}

CCalCategoryImpl* CCalCategoryImpl::NewL(CAgnCategory* aCategory)
	{
	// by default, this function doesn't take ownership
	CCalCategoryImpl* categoryImpl = new (ELeave) CCalCategoryImpl(aCategory);
	categoryImpl->SetOwnAgnCategory(EFalse);
	return categoryImpl;
	}

CCalCategoryImpl::~CCalCategoryImpl()
	{
	if(iOwnCategory)
		{
		delete iCategory;
		}
	}

void CCalCategoryImpl::SetOwnAgnCategory(TBool aOwnCategory)
	{
	iOwnCategory=aOwnCategory;
	}
			
const TDesC& CCalCategoryImpl::ExtendedCategoryName() const
	{
	return iCategory->ExtendedCategoryName();
	}
