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

#include "calcategorymanagerimpl.h"

#include "calclient.h"
#include "calcategoryimpl.h"
#include <calsession.h>
#include "calsessionimpl.h"
#include "calentryimpl.h"

CCalCategoryManagerImpl::CCalCategoryManagerImpl(CCalSession& aSession)
	:iCalSession(aSession)
	{
	iCalSession.Impl().IncrementReferenceCount();
	}

CCalCategoryManagerImpl::~CCalCategoryManagerImpl()
	{
	iCalSession.Impl().DecrementReferenceCount();
	}

TInt CCalCategoryManagerImpl::CategoryCountL() const
	{
	return iCalSession.Impl().Server().CategoryCountL(iCalSession.Impl().FileId());
	}

CCalCategory* CCalCategoryManagerImpl::CategoryL(TInt aIndex) const
	{
	CAgnCategory* agnCategory = iCalSession.Impl().Server().CategoryL(aIndex, iCalSession.Impl().FileId());
	CleanupStack::PushL(agnCategory);
	CCalCategoryImpl* categoryImpl = CCalCategoryImpl::NewL(agnCategory); // doesn't take ownership
	categoryImpl->SetOwnAgnCategory(ETrue); // take ownership
	CleanupStack::Pop(agnCategory);
	CleanupStack::PushL(categoryImpl);
	CCalCategory* ret=CCalCategory::NewL(categoryImpl);
	CleanupStack::Pop(categoryImpl);
	return ret;
	}

void CCalCategoryManagerImpl::AddCategoryL(const CCalCategory& aCategory)
	{
	if(aCategory.ExtendedCategoryName().Length()==0)
		{
		User::Leave(KErrArgument);
		}

	iCalSession.Impl().Server().AddCategoryToListL(aCategory.ExtendedCategoryName(), iCalSession.Impl().FileId());
	}

void CCalCategoryManagerImpl::FilterCategoryL(const CCalCategory& aCategory, RPointerArray<CCalEntry>& aEntries, MCalProgressCallBack* aProgressCallBack)
	{
	if (aProgressCallBack)
		{
		iCalSession.Impl().AsyncTaskManager().StartCategoryTaskL(*aProgressCallBack, CCalAsyncTaskManager::EFilterCategory,aCategory, &aEntries);
		}
	else if (! iCalSession.Impl().AsyncTaskManager().IsBusy())
		{
		iCalSession.Impl().Server().FilterCategorySynchronouslyL(*(aCategory.Impl()->AgnCategory()), iCalSession.Impl().CollectionId());
		CArrayFixSeg<TAgnEntryId>* entryId = new (ELeave) CArrayFixSeg<TAgnEntryId> (32);
		CleanupStack::PushL(entryId);
		GetFilteredEntryL(*entryId, aEntries, iCalSession.Impl());
		CleanupStack::PopAndDestroy(entryId);
		}
	else
		{
		User::Leave(KErrLocked);	
		}
	}

void CCalCategoryManagerImpl::DeleteCategoryL(const CCalCategory& aCategory, MCalProgressCallBack& aProgressCallBack)
	{
	iCalSession.Impl().AsyncTaskManager().StartCategoryTaskL(aProgressCallBack, CCalAsyncTaskManager::EDeleteCategory,aCategory);
	}

void CCalCategoryManagerImpl::GetFilteredEntryL(CArrayFixSeg<TAgnEntryId>& aEntryId, RPointerArray<CCalEntry>& aEntries, CCalSessionImpl& aCalSessionImpl)
	{// Get the light entry by its entry id and fill in it into the array past in by the client
	aCalSessionImpl.Server().FilterCategoryL(aEntryId, aCalSessionImpl.FileId());
	TInt count=aEntryId.Count();
	for (TInt ii=0;ii<count;++ii)
		{
		CAgnSimpleEntry* simpleEntry=aCalSessionImpl.Server().GetSimpleEntryL(aEntryId[ii], aCalSessionImpl.CollectionId());
		if (simpleEntry)
			{
			TCleanSimpleEntry simpleEntryCleanupItem(*simpleEntry, aCalSessionImpl.Server().SimpleEntryAllocator());
			CleanupStack::PushL(TCleanupItem(TCleanSimpleEntry::DestroySimpleEntry, &simpleEntryCleanupItem));
			CCalLiteEntry* liteEntry = CCalLiteEntry::NewL(*simpleEntry, aCalSessionImpl.Server());
			CleanupStack::Pop();
			CleanupStack::PushL(liteEntry);
			CCalEntryImpl* entryImpl = CCalEntryImpl::NewL(*liteEntry, aCalSessionImpl);
			CleanupStack::Pop(liteEntry);
			CleanupStack::PushL(entryImpl);
			CCalEntry* entry=CCalEntry::NewL(entryImpl);
			CleanupStack::Pop(entryImpl);
			CleanupStack::PushL(entry);
			aEntries.AppendL(entry);
			CleanupStack::Pop(entry);
			}		
		}
	}
