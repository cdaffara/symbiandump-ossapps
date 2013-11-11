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

#ifndef __CQUEUEENTRYBASE_H__
#define __CQUEUEENTRYBASE_H__

#include "cimapmailstore.h"

/**
Abstract base class derived from CActive that is implemented by storage requests wrapper classed.
@internalComponent
@prototype
*/
class CQueueEntryBase:	public CActive
	{
public:
	//the different types of requests
	enum TQueueEntryType
	{
	EIMHeader=0,
	EIMMimeHeader,
	EBody8,
	EBody16,
	EAttachment,
	EPlainTextBody
	};
public:
	virtual ~CQueueEntryBase();
	TMsvId GetId() const;
	TQueueEntryType GetEntryType() const;
	MMailStoreObserver& GetObserver();
	void StoreComplete(TInt aErrorCode);
	virtual void CancelRequest();
protected:
	CQueueEntryBase(TMsvId aId,TQueueEntryType aQueueEntryType,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,MMailStoreObserver& aObserver);
	void BaseConstructL();
	void CompleteSelf();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
protected:
	TMsvId iId;
	MMailStoreObserver& iObserver;	
	TQueueEntryType iQueueEntryType;
	CImapMailStore& iParent;
	CMsvServerEntry& iServerEntry;
	};

#endif //__CQUEUEENTRYBASE_H__