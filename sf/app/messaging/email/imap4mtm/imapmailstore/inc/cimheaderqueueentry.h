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

#ifndef __CIMHEADERQUEUEENTRY_H__
#define __CIMHEADERQUEUEENTRY_H__

#include "cqueueentrybase.h"

/**
Used for requests to store CImHeader objects.
@internalComponent
@prototype
*/
class CImHeaderQueueEntry : public CQueueEntryBase
	{
public:
	static CImHeaderQueueEntry* NewL(CImHeader* aHeader,TMsvId aId,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,MMailStoreObserver& aObserver);
	virtual ~CImHeaderQueueEntry();
protected:
	virtual void RunL();
private:
	CImHeaderQueueEntry(CImHeader* aHeader,TMsvId aId,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,MMailStoreObserver& aObserver);
	void ConstructL();
private:
	CImHeader* iHeader;
	};

#endif //__CIMHEADERQUEUEENTRY_H__