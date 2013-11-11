// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CACHEMAN.CPP
//

#include "CACHEMAN.H"
#include "MIUTHDR.H"
#include <msvapi.h>
#include <msvuids.h>
#include <msvids.h>
#include <mmsvattachmentmanager.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cimprunemessage.h>
#include "cimfinder.h"
#include "timrfc822datefield.h"
#include "msvconsts.h"
#endif

// The CImEntryStack implementation

CImFinder::CImEntryStack* CImFinder::CImEntryStack::NewL()
	{
	CImFinder::CImEntryStack* self;
	self = new (ELeave) CImFinder::CImEntryStack();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CImFinder::CImEntryStack::~CImEntryStack()
	{
	delete iFolders;
	}

void CImFinder::CImEntryStack::PushL(TMsvId aId)
	{
	iFolders->AppendL(aId);
	}

void CImFinder::CImEntryStack::Reset()
	{
	iFolders->Reset();
	}

TMsvId CImFinder::CImEntryStack::PopL()
	{
	TInt32 count = iFolders->Count();
	if (count == 0)
		{
		User::Leave(KErrOverflow);
		}
	
	TMsvId folderId = (*iFolders)[count - 1];
	iFolders->Delete(count-1);
	return folderId;
	}

void CImFinder::CImEntryStack::ConstructL()
	{
	iFolders = new (ELeave) CMsvEntrySelection;
	}

TBool CImFinder::CImEntryStack::Empty() const
	{
	return (iFolders->Count() == 0);
	}


// Implementation of abstract CImFinder class

EXPORT_C void CImFinder::FindFirstL(TMsvId aRootEntry, TRequestStatus &aStatus)
	{
	iEntryStack->Reset();
	iEntryStack->PushL(aRootEntry);
	ASSERT(iState != EImmfFindingEntry);
	FindNextL(aStatus);
	}

void CImFinder::FindFirstL(const CMsvEntrySelection& aInitialSelection, TRequestStatus &aStatus)
	{
	iEntryStack->Reset();

	TInt index = aInitialSelection.Count();
	while (index--)
		// Push each given entry on the stack so that we will visit it later.
		{
		iEntryStack->PushL(aInitialSelection[index]);
		}

	ASSERT(iState != EImmfFindingEntry);
	FindNextL(aStatus);
	}

EXPORT_C CImFinder::~CImFinder()
	{
	delete iEntryStack;
	}

void CImFinder::ConstructL()
	{
	iEntryStack = CImEntryStack::NewL();
	CActiveScheduler::Add(this);
	}

void CImFinder::DoRunL()
	{	
	if (iState == EImmfFindingEntry)
		{
		FindNextL(iStatus);
		}
	}

void CImFinder::DoCancel()
    {   
    CMsgActive::DoCancel();   
    }


CImFinder::CImFinder(CMsvEntry& aEntry) : CMsgActive(EPriorityStandard), 	iCurrentEntry(aEntry)
	{
	}

EXPORT_C void CImFinder::FindNextL(TRequestStatus &aStatus)
	{
	TImmfState tempState = iState;
	
	if (!(iEntryStack->Empty()))
		// If there are entries on the stack then check them.
		{
		iCurrentEntry.SetEntryL(iEntryStack->PopL());
		TUid entryType = iCurrentEntry.Entry().iType;
		
		AddChildEntriesL();

		iStatus=KRequestPending;
		SetActive();
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);

		if (IsRequiredEntryType(entryType))
			// If the entry is of a required type then return it.
			{
			iState = EImmfEntryFound;
			}
		else
			// Otherwise keep looking for one.
			{
			iState = EImmfFindingEntry;
			}
		}
	else
		// If there is nothing on the stack then there are no more entries.
		{
		iStatus=KRequestPending;
		SetActive();
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNotFound);
		iState = EImmfNothingFound;
		}

	if (tempState != EImmfFindingEntry)
		{
		Queue(aStatus);
		}
	}



// Implementation of CImMessageFinder class

EXPORT_C CImMessageFinder* CImMessageFinder::NewL(CMsvEntry& aEntry)
	{
	CImMessageFinder* self = CImMessageFinder::NewLC(aEntry);
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CImMessageFinder* CImMessageFinder::NewLC(CMsvEntry& aEntry)
	{
	CImMessageFinder* self = new (ELeave) CImMessageFinder(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CImMessageFinder::AddChildEntriesL()
	{
	if ((iCurrentEntry.Entry().iType == KUidMsvFolderEntry)
		|| (iCurrentEntry.Entry().iType == KUidMsvServiceEntry))
		{
		// Push the folders on to the entry stack
		TMsvSelectionOrdering ordering;
		ordering.SetShowInvisibleEntries(EFalse);
		iCurrentEntry.SetSortTypeL(ordering);
		CMsvEntrySelection* entrySelection = iCurrentEntry.ChildrenWithTypeL(KUidMsvFolderEntry);
		CleanupStack::PushL(entrySelection);
		TInt counter;
		TInt count = entrySelection->Count();
		for (counter = 0; counter < count; counter++)
			{
			iEntryStack->PushL((*entrySelection)[counter]);
			}
		CleanupStack::PopAndDestroy(); // entrySelection

		// Push the messages on to the entry stack
		entrySelection = iCurrentEntry.ChildrenWithTypeL(KUidMsvMessageEntry);
		CleanupStack::PushL(entrySelection);
		count = entrySelection->Count();
		for (counter = 0; counter < count; counter++)
			{
			iEntryStack->PushL((*entrySelection)[counter]);
			}
		CleanupStack::PopAndDestroy(); // entrySelection
		}
	}

TBool CImMessageFinder::IsRequiredEntryType(TUid aEntryType) const
	{
	if (aEntryType == KUidMsvMessageEntry)
		return ETrue;
	else
		return EFalse;
	}

CImMessageFinder::CImMessageFinder(CMsvEntry& aEntry) : CImFinder(aEntry)
	{
	}



// Implementation of CImMessageFinder class

EXPORT_C CImEntryFinder* CImEntryFinder::NewL(CMsvEntry& aEntry)
	{
	CImEntryFinder* self = CImEntryFinder::NewLC(aEntry);
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CImEntryFinder* CImEntryFinder::NewLC(CMsvEntry& aEntry)
	{
	CImEntryFinder* self = new (ELeave) CImEntryFinder(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CImEntryFinder::AddChildEntriesL()
	{
	if (iCurrentEntry.Entry().Owner())
		{
		// Push the child entries on to the entry stack
		TMsvSelectionOrdering ordering;
		ordering.SetShowInvisibleEntries(ETrue);
		iCurrentEntry.SetSortTypeL(ordering);
		CMsvEntrySelection* entrySelection = iCurrentEntry.ChildrenL();
		CleanupStack::PushL(entrySelection);
		TInt counter;
		TInt count = entrySelection->Count();
		for (counter = 0; counter < count; counter++)
			{
			iEntryStack->PushL((*entrySelection)[counter]);
			}
		CleanupStack::PopAndDestroy(); // entrySelection		
		}
	}

TBool CImEntryFinder::IsRequiredEntryType(TUid /*aEntryType*/) const
// Always true because all entry types are needed.
	{
	return ETrue;
	}

CImEntryFinder::CImEntryFinder(CMsvEntry& aEntry) : CImFinder(aEntry)
	{
	}



// Implementation of CImMessage Finder

EXPORT_C CImMessageCounter* CImMessageCounter::NewL(CMsvEntry& aEntry)
	{
	CImMessageCounter* self = CImMessageCounter::NewLC(aEntry);
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CImMessageCounter* CImMessageCounter::NewLC(CMsvEntry& aEntry)
	{
	CImMessageCounter* self = new (ELeave) CImMessageCounter(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C TInt CImMessageCounter::Count()
	{
	return iCount;
	}

void CImMessageCounter::AddChildEntriesL()
	{
	if ((iCurrentEntry.Entry().iType == KUidMsvFolderEntry)
		|| (iCurrentEntry.Entry().iType == KUidMsvServiceEntry))
		{
		// Push the folders on to the entry stack
		TMsvSelectionOrdering ordering;
		ordering.SetShowInvisibleEntries(EFalse);
		iCurrentEntry.SetSortTypeL(ordering);
		CMsvEntrySelection* entrySelection = iCurrentEntry.ChildrenWithTypeL(KUidMsvFolderEntry);
		CleanupStack::PushL(entrySelection);
		TInt counter;
		TInt count = entrySelection->Count();
		for (counter = 0; counter < count; counter++)
			{
			iEntryStack->PushL((*entrySelection)[counter]);
			}
		CleanupStack::PopAndDestroy(); // entrySelection

		// Count the messages in the folder
		entrySelection = iCurrentEntry.ChildrenWithTypeL(KUidMsvMessageEntry);
		CleanupStack::PushL(entrySelection);
		iCount += entrySelection->Count();
		CleanupStack::PopAndDestroy(); // entrySelection
		}
	else if (iCurrentEntry.Entry().iType == KUidMsvMessageEntry)
		{ // count itself if it's a message entry
		iCount++;
		}
	}

TBool CImMessageCounter::IsRequiredEntryType(TUid) const
	{
	// The message counter doesn't require any entry types to be returned.
	return EFalse;
	}

CImMessageCounter::CImMessageCounter(CMsvEntry& aEntry) : CImFinder(aEntry)
	{

	}


// Implementation of CImPruneMessage

EXPORT_C CImPruneMessage* CImPruneMessage::NewL(CMsvEntry& aEntry, RFs& aFs)
/** Allocates and constructs a new CImPruneMessage object.

@param aEntry CMsvEntry for use by the object in performing the delete operations. 
It does not need to be set to any particular message context. The caller retains 
ownership.
@param aFs Open file server session handle
@return New CImPruneMessage object */
	{
	CImPruneMessage* self = CImPruneMessage::NewLC(aEntry, aFs);
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CImPruneMessage* CImPruneMessage::NewLC(CMsvEntry& aEntry, RFs& aFs)
/** Allocates and constructs a new CImPruneMessage object, leaving the object on 
the cleanup stack.

@param aEntry CMsvEntry for use by the object in performing the delete operations. 
It does not need to be set to any particular message context. The caller retains 
ownership.
@param aFs Open file server session handle
@return New CImPruneMessage object */
	{
	CImPruneMessage* self = new (ELeave) CImPruneMessage(aEntry, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C void CImPruneMessage::StartL(TMsvId aMessageEntry, TRequestStatus &aStatus)
/** Starts the asynchronous message clean operation.

@param aMessageEntry The ID of the entry from which to delete the body text 
and attachment data
@param aStatus Asynchronous status word that completes with KErrNone when the 
body text and all of the attachments have been deleted. */
	{
	iCurrentEntry.SetEntryL(aMessageEntry);
	TMsvEmailEntry entry(iCurrentEntry.Entry());

	if (entry.Complete() || entry.BodyTextComplete())
		{
		entry.SetComplete(EFalse);
		entry.SetBodyTextComplete(EFalse);
		iChangeOperation = iCurrentEntry.ChangeL(entry, iStatus);
		SetActive();
		}
	else
		{
		iStatus=KRequestPending;
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}

	iState = EImPruneUpdatingFirstEntry;
	Queue(aStatus);
	iRootEntryId = aMessageEntry;
	}

CImPruneMessage::~CImPruneMessage()
/** Destructor. */
	{
	Cancel();
	delete iEntryFinder;
	delete iFileManager;
	delete iStore;
	delete iChangeOperation;	
	}

void CImPruneMessage::ConstructL()
	{
	CActiveScheduler::Add(this);
	iEntryFinder = CImEntryFinder::NewL(iCurrentEntry);
	iFileManager = CFileMan::NewL(iFs);
	}

void CImPruneMessage::DoRunL()
	{
	TInt error = KErrNone;
	switch (iState)
		{
		case EImPruneUpdatingFirstEntry:
			delete iChangeOperation;
			iChangeOperation = 0;
			PruneEntryL();
			iState = EImPruneFindFirstEntry;			
			SetActive();
			break;
		case EImPruneFindFirstEntry:
			ResetStoreL();
			iEntryFinder->FindFirstL(iRootEntryId, iStatus);
			iState = EImPruneFindingEntry;
			SetActive();
			break;
		case EImPruneUpdatingEntry:
			delete iChangeOperation;
			iChangeOperation = 0;
			PruneEntryL();
			iState = EImPruneFindNextEntry;
			SetActive();
			break;
		case EImPruneFindNextEntry:
			ResetStoreL();
			iEntryFinder->FindNextL(iStatus);
			iState = EImPruneFindingEntry;
			SetActive();
			break;
		case EImPruneFindingEntry:
			delete iStore;
			iStore = NULL;
			if (iStatus == KErrNone)
				// An entry has been found, so prune it.
				{
				// Ignore any error as it probably just means that the entry is locked.
				// We can afford to skip locked entries.
				if (iCurrentEntry.Entry().Complete())
					{
					TMsvEntry entry = iCurrentEntry.Entry();
					entry.SetComplete(EFalse);
					TRAPD(ignore, iChangeOperation = iCurrentEntry.ChangeL(entry, iStatus));
					}

				if (!iChangeOperation)
					{
					TRequestStatus* status = &iStatus;
					iStatus=KRequestPending;
					User::RequestComplete(status, KErrNone);	
					}
				iState = EImPruneUpdatingEntry;
				SetActive();
				}
			break;
		}
	}

void CImPruneMessage::DoComplete(TInt& status)
	{
	if ((iState == EImPruneFindingEntry) && (status == KErrNotFound))
	// The not found status is expected, it just means that the entry finder has already found all the entries.
		{
		status = KErrNone;
		}
	}

void CImPruneMessage::DoCancel()
	{
	iEntryFinder->Cancel();
	if (iChangeOperation)
		iChangeOperation->Cancel();
	CMsgActive::DoCancel();
	}

CImPruneMessage::CImPruneMessage(CMsvEntry& aEntry, RFs& aFs) 
	: CMsgActive(EPriorityStandard), 
	iCurrentEntry(aEntry), 
	iFs(aFs)
	{
	__DECLARE_NAME(_S("CImPruneMessage"));
	}

void CImPruneMessage::PruneEntryL()
	{
	// delete the rich text stream
	iStore = iCurrentEntry.EditStoreL();
	if (iStore->IsPresentL(KMsvEntryRichTextBody))
		{	
		iStore->DeleteBodyTextL();
		iStore->CommitL();
		}
		
	// If the entry has an attachment we need to remove it.
	// In order to preserve the message structure, we won't delete the attachment
	// completely, but instead we will set the file size to 0 and mark the attachment
	// as not complete.
	
	TBool completeSelf = ETrue;
		
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	
	// as this is email attachment entry , it has one attachment per entry
	if(attachmentMgr.AttachmentCount() > 0)
		{
		CMsvAttachment* attachmentInfo = attachmentMgr.GetAttachmentInfoL(0);
		CleanupStack::PushL(attachmentInfo);

		// We only want to remove file attachments
		if (attachmentInfo->Type() == CMsvAttachment::EMsvFile)
			{
			RFile file = attachmentMgr.GetAttachmentFileForWriteL(0);
			TInt error = file.SetSize(0);
			file.Close();

			if (error == KErrNone)
				{
				attachmentInfo->SetComplete(EFalse);
				attachmentMgr.ModifyAttachmentInfoL(attachmentInfo, iStatus);
				// ModifyAttachmentInfoL takes ownership of the attachment info, so just pop
				// it off the cleanup stack and set the pointer to NULL to show we don't
				// own it
				CleanupStack::Pop(attachmentInfo);
				attachmentInfo = NULL;

				// We have called an async routine, so set the flag to show that we don't
				// need to complete ourself
				completeSelf = EFalse;
				}
			}

		if (attachmentInfo != NULL)
			{
			CleanupStack::PopAndDestroy(attachmentInfo);
			}	
		}
	
	if (completeSelf)
		{
		TRequestStatus* status = &iStatus;
		iStatus=KRequestPending;
		User::RequestComplete(status, KErrNone);
		}
	}

void CImPruneMessage::ResetStoreL()
	{
	if (iStore != NULL)
	   {
	   iStore->CommitL();
	   delete iStore;
	   iStore = NULL;	
	   }
	}

// Implementation of CImCacheManager

EXPORT_C void CImCacheManager::StartL(TMsvId aRootEntry, TRequestStatus &aStatus)
/** Performs a recursive cache cleanup starting from a specified root entry.

Cached data for entries below the specified root are cleaned-up if they meet 
the criteria specified by the implementation of Filter().

@param aRootEntry Entry from which to start cache cleanup
@param aStatus Asynchronous status word signalled when the operation is complete */
	{
	delete iMessagesToPrune;
	iMessagesToPrune = 0;
	iState = EImcmCountingMessages;
	aStatus = KRequestPending;
	iReport = &aStatus;
	iMessageCounter->FindFirstL(aRootEntry, iStatus);
	iRootEntry = aRootEntry;
	SetActive();
	}

EXPORT_C void CImCacheManager::StartL(const CMsvEntrySelection& aSelection, TRequestStatus &aStatus)
/** Performs a recursive cache cleanup for a specified selection of root entries.

Cached data for entries below the specified roots are cleaned-up if they meet 
the criteria specified by the implementation of Filter().

@param aSelection List of entries from which to start cache cleanup
@param aStatus Asynchronous status word signalled when the operation is complete */
	{
	delete iMessagesToPrune;
	iMessagesToPrune = 0;
	iState = EImcmCountingMessages;
	aStatus = KRequestPending;
	iReport = &aStatus;

	// Save the selection for later as we need it when we actually prune the messages
	iMessagesToPrune = new (ELeave) CMsvEntrySelection();
	TInt index = aSelection.Count();
	while (index--)
		{
		iMessagesToPrune->AppendL(aSelection[index]);
		}

	// Set off the operation to count all the messages
	iMessageCounter->FindFirstL(aSelection, iStatus);

	SetActive();
	}


EXPORT_C void CImCacheManager::ConstructL()
/** Second phase constructor.

All classes derived from CImCacheManager must call this function before StartL() 
is called. */
	{
	TMsvSelectionOrdering ordering;
	iCurrentEntry = CMsvEntry::NewL(iSession, KMsvRootIndexEntryId, ordering);
	iMessageFinder = CImMessageFinder::NewL(*iCurrentEntry);
	iMessageCounter = CImMessageCounter::NewL(*iCurrentEntry);
	iProgress.iTotalMessages = 1;
	iProgress.iMessagesProcessed = 0;
	CActiveScheduler::Add(this);
	}

EXPORT_C const TDesC8& CImCacheManager::ProgressL()
/** Gets information on the progress of the operation.

@return Packaged TImCacheManagerProgress object holding operation progress 
details */
	{
	if (iProgress.iTotalMessages == 0)
		{
		iProgress.iTotalMessages = 1;
		}

	if (iProgress.iMessagesProcessed > iProgress.iTotalMessages)
		{
		iProgress.iMessagesProcessed = iProgress.iTotalMessages;
		}

	iProgressBuf = iProgress;
	return iProgressBuf;
	}

/** Implements the active object completion protocol for a cache cleanup operation.   

@see CActive::RunL() */
EXPORT_C void CImCacheManager::RunL()
	{
	TInt error = iStatus.Int();
	
	if ((iState == EImcmCountingMessages)
		&& (error == KErrNotFound))
		// A KErrNotFound is a normal condition from the message counter.
		{
		error = KErrNone;
		}

	if (error == KErrNone)
		{
		TRAP(error, DoRunL());
		}

	if(error != KErrNone)
		{
		if (error == KErrNotFound)
			// A KErrNotFound is a normal condition from the message finder.
			{
			error = KErrNone;
			}

		User::RequestComplete(iReport, error);
		}
	}

/** Implements the active object cancellation protocol for a cache cleanup operation.

As the cache manager is an active object, a cache cleanup  
operation started by Start() can be cancelled using CActive::Cancel().

Cancellation halts any further deletion of message data, but does 
not restore any data that has already been deleted by the cleanup operation.    

@see CActive::Cancel() */
EXPORT_C void CImCacheManager::DoCancel()
	{
	iMessageFinder->Cancel();
	iMessageCounter->Cancel();

	if (iDeleteOperation)
		iDeleteOperation->Cancel();

	User::RequestComplete(iReport,KErrCancel);
	}

void CImCacheManager::DoRunL()
	{
	switch (iState)
		{
		case EImcmCountingMessages:
			iProgress.iTotalMessages = iMessageCounter->Count();
			
			if (iMessagesToPrune)
				// If we are pruning a selection the prune from the selection
				iMessageFinder->FindFirstL(*iMessagesToPrune, iStatus);
			else
				// otherwise start at the root entry
				iMessageFinder->FindFirstL(iRootEntry, iStatus);

			iState = EImcmLookingForMessage;
			SetActive();
			break;

		case EImcmLookingForMessage:
			if (iStatus == KErrNone)
				{
				if (Filter())
					{
					PruneMessageL();
					}
				else
					{
					iMessageFinder->FindNextL(iStatus);
					iState = EImcmLookingForMessage;
					SetActive();
					}
				}
			iProgress.iMessagesProcessed++;
			break;

		case EImcmPruningMessages:
		case EImcmSkippingPrune:
			// The current message has been pruned, find another one.
			if (iDeleteOperation)
				{
				delete iDeleteOperation;
				iDeleteOperation = 0;
				}
			iMessageFinder->FindNextL(iStatus);
			iState = EImcmLookingForMessage;
			SetActive();
			break;

		default:
			break;
		}

	}

void CImCacheManager::PruneMessageL()
	{
	if (iCurrentEntry->Entry().Owner())
		{
		iState = EImcmPruningMessages;

		// Remove any associated store 
		if (iCurrentEntry->HasStoreL())
			{
			CMsvStore* store=iCurrentEntry->EditStoreL();
			CleanupStack::PushL(store);
			store->DeleteL();
			CleanupStack::PopAndDestroy(store);
			}

		// Get a list of the child entries
		CMsvEntrySelection* children = iCurrentEntry->ChildrenL();
		CleanupStack::PushL(children);

		// Delete the child entries
		// The Complete and BodyTextComplete flags are cleared by the
		// server MTM when the delete is processed.
		iDeleteOperation = iCurrentEntry->DeleteL(*children, iStatus);
		SetActive();
		CleanupStack::PopAndDestroy(children);
		}
	else
		{
		// If this entry doesn't own any children,
		// or we can't do the ChangeL then skip this entry and move on.
		iState = EImcmSkippingPrune;
		iStatus=KRequestPending;
		SetActive();		
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

EXPORT_C CImCacheManager::CImCacheManager(CMsvSession& aSession, TRequestStatus& aObserverRequestStatus) 
	: CMsvOperation(aSession, EPriorityStandard, aObserverRequestStatus), 
	iSession(aSession)
/** Constructor.

@param aSession Message server session
@param aObserverRequestStatus Unused */
	{
	__DECLARE_NAME(_S("CImCacheManager"));
	}

EXPORT_C CImCacheManager::~CImCacheManager()
/** Destructor.

This cancels any operation in progress. */
	{
	Cancel();
	delete iCurrentEntry;
	delete iMessageFinder;
	delete iMessageCounter;
	delete iMessagesToPrune;
	delete iDeleteOperation;
	}
