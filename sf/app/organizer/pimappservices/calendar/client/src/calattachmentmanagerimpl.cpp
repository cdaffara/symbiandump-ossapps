// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calattachmentmanagerimpl.h"
#include <calsession.h>
#include "calsessionimpl.h"
#include "calcommonimpl.h"
#include "calclient.h"

// CCalAttachmentManagerImpl // 

CCalAttachmentManagerImpl* CCalAttachmentManagerImpl::NewL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack)
	{
	CCalAttachmentManagerImpl* self = new (ELeave) CCalAttachmentManagerImpl(aSession);
	CleanupStack::PushL(self);
	self->ConstructL(aProgressCallBack);
	CleanupStack::Pop(self);
	return self;
	}

CCalAttachmentManagerImpl::CCalAttachmentManagerImpl(CCalSession& aSession) : 
	iCalSessionImpl(aSession.Impl())
	{
	iCalSessionImpl.IncrementReferenceCount();
	}

void CCalAttachmentManagerImpl::ConstructL(MCalProgressCallBack& aProgressCallBack)
	{
	iCalSessionImpl.AsyncTaskManager().StartBuildIndexL(aProgressCallBack);
	}
	
CCalAttachmentManagerImpl::~CCalAttachmentManagerImpl()
	{
	if(iCalSessionImpl.AsyncTaskManager().BuildingIndex())
		{
		iCalSessionImpl.AsyncTaskManager().Cancel();
		}
	iCalSessionImpl.DecrementReferenceCount();
	}

CCalAttachmentIteratorImpl* CCalAttachmentManagerImpl::FetchAttachmentsL(CCalAttachmentManager::TSortOrder aSortOrder) const
	{
	CCalAttachmentIteratorImpl* iteratorImpl = new (ELeave) CCalAttachmentIteratorImpl(iCalSessionImpl);
	CleanupStack::PushL(iteratorImpl);
	iCalSessionImpl.Server().FetchAttachmentsL(*iteratorImpl, aSortOrder, iCalSessionImpl.FileId());
	CleanupStack::Pop(iteratorImpl);
	return iteratorImpl;
	}

void CCalAttachmentManagerImpl::EntriesReferencingFileAttachmentL(RArray<TCalLocalUid>& aUids, const CAgnAttachment& aAttachment)
	{
	iCalSessionImpl.Server().EntriesReferencingFileAttachmentL(aUids, aAttachment, iCalSessionImpl.FileId());
	}

// CCalAttachmentIteratorImpl // 

CCalAttachmentIteratorImpl::CCalAttachmentIteratorImpl(CCalSessionImpl& aSessionImpl)
	: iCalSessionImpl(aSessionImpl)
	{
	}

CCalAttachmentIteratorImpl::~CCalAttachmentIteratorImpl()
	{
	iArray.Reset();
	}

void CCalAttachmentIteratorImpl::AppendL(TCalAttachmentUid aId)
	{
	iArray.AppendL(aId);
	iTotalAttachments = iArray.Count();
	}

TBool CCalAttachmentIteratorImpl::HasMore() const
	{
	return (iArray.Count() > 0);
	}

CCalAttachment* CCalAttachmentIteratorImpl::NextL()
	{
	CCalAttachment* calAttachment = NULL;
	if (HasMore())
		{
		CAgnAttachment* agnAttachment = iCalSessionImpl.Server().FetchAttachmentByIdL(iArray[0], iCalSessionImpl.FileId());
		if (agnAttachment)
			{
			CleanupStack::PushL(agnAttachment);
			agnAttachment->SetFlag(CAgnAttachment::EOwnedByCalAttachment);
			calAttachment = CCalAttachment::NewL(*agnAttachment, iCalSessionImpl);
			CleanupStack::Pop(agnAttachment);
			iArray.Remove(0);
			}
		else
			{
			// attachment may have been deleted since initial search
			User::Leave(KErrNotFound); 
			}
		}
	return calAttachment;
	}

TInt CCalAttachmentIteratorImpl::Count() const
	{
	return iTotalAttachments;
	}
