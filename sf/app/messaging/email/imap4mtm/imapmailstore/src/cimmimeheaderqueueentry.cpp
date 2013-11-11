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

#include <miuthdr.h>
#include "cimmimeheaderqueueentry.h"

/**
Constructor.
*/
CImMimeHeaderQueueEntry::CImMimeHeaderQueueEntry(CImMimeHeader* aHeader,TMsvId aId,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,MMailStoreObserver& aObserver)
	: CQueueEntryBase(aId,EIMMimeHeader,aParent,aServerEntry,aObserver),iHeader(aHeader)
	{
	}
	
/**
Factory constructors.
Ownership aHeader is taken.
*/	
CImMimeHeaderQueueEntry* CImMimeHeaderQueueEntry::NewL(CImMimeHeader* aHeader,TMsvId aId,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,MMailStoreObserver& aObserver)
	{
	CleanupStack::PushL(aHeader);
	CImMimeHeaderQueueEntry* self=new(ELeave) CImMimeHeaderQueueEntry(aHeader,aId,aParent,aServerEntry,aObserver);
	CleanupStack::Pop(aHeader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CImMimeHeaderQueueEntry::ConstructL()
	{
	BaseConstructL();
	CompleteSelf();		
	}

/**
Destructor.
*/	
CImMimeHeaderQueueEntry::~CImMimeHeaderQueueEntry()
	{
	delete iHeader;
	}

/**
Uses CMsvEntry to synchronousy store the requests data object.
@param	
@return
*/

void CImMimeHeaderQueueEntry::RunL()
	{
	iServerEntry.SetEntry(iId);
	CMsvStore* store = iServerEntry.EditStoreL();
	CleanupStack::PushL(store);
	iHeader->StoreL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy(store); 
	//call back to the observer as the operation is complete
	StoreComplete(KErrNone);
	//request can be removed from the queue
	iParent.RemoveFromQueueAndDelete(this);	
	}


