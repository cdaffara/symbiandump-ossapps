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

#ifndef __CIMMIMEHEADERQUEUEENTRY_H__
#define __CIMMIMEHEADERQUEUEENTRY_H__

#include "cqueueentrybase.h"

/**
Used for requests to store CImMimeHeader objects.
@internalComponent
@prototype
*/
class CImMimeHeaderQueueEntry : public CQueueEntryBase
	{
public:
	static CImMimeHeaderQueueEntry* NewL(CImMimeHeader* aHeader,TMsvId aId,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,MMailStoreObserver& aObserver);
	~CImMimeHeaderQueueEntry();
protected:
	void RunL();
private:
	CImMimeHeaderQueueEntry(CImMimeHeader* aHeader,TMsvId aId,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,MMailStoreObserver& aObserver);
	void ConstructL();
private:
	CImMimeHeader* iHeader;
	};
	
#endif //__CIMMIMEHEADERQUEUEENTRY_H__