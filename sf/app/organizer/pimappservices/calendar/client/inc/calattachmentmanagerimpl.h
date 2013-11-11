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

#ifndef __CALATTACHMENTMANAGERIMPL_H__
#define __CALATTACHMENTMANAGERIMPL_H__

#include "calattachmentmanager.h"
#include "agmattachment.h"

class CAgnAttachment;
class CCalAttachmentIteratorImpl;
class CCalSessionImpl;

/**
Implementation of the CCalAttachmentManager.
@internalComponent
 */
class CCalAttachmentManagerImpl : public CBase
	{
public:
	static CCalAttachmentManagerImpl* NewL(CCalSession& aSession, MCalProgressCallBack& aProgressCallBack); 
	~CCalAttachmentManagerImpl(); 

	CCalAttachmentIteratorImpl* FetchAttachmentsL(CCalAttachmentManager::TSortOrder aSortType) const;

	void EntriesReferencingFileAttachmentL(RArray<TCalLocalUid>& aUids, const CAgnAttachment& aAttachment);
private:
	CCalAttachmentManagerImpl(CCalSession& aSession);
	void ConstructL(MCalProgressCallBack& aProgressCallBack);

private:
	CCalSessionImpl& iCalSessionImpl;
	};

/**
Implementation of the CCalAttachmentIterator.
@internalComponent
 */
class CCalAttachmentIteratorImpl : public CBase
	{
public:
	CCalAttachmentIteratorImpl(CCalSessionImpl& aSession);
	~CCalAttachmentIteratorImpl();

	TBool HasMore() const;
	CCalAttachment* NextL();
	TInt Count() const;

	void AppendL(TCalAttachmentUid aId);

private:
	RArray<TCalAttachmentUid> iArray;
	TInt iTotalAttachments;
	CCalSessionImpl& iCalSessionImpl;
	};
	
#endif // __CALATTACHMENTMANAGERIMPL_H__
 