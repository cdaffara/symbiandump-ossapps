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
// imapsyncmanager.cpp
// 
//


#include "cimapsyncmanager.h"
#include "cimapopsyncfoldertree.h"
#include "cimapopsyncsubs.h"
#include "cimapfolder.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"

#ifdef __IMAP_LOGGING
#include <utf.h>
#endif

// Inbox mailbox name
_LIT(KIMAP_INBOX, "INBOX");

// Inbox mailbox name used for display purposes
_LIT(KImapInboxDisplayName, "Inbox");

/**
Private default class constructor
*/
CImapSyncManager::CImapSyncManager(CMsvServerEntry& aEntry, CImapSettings& aImapSettings)
:	CMsgActive(EPriorityStandard), 
	iServerEntry(aEntry), 
	iImapSettings(aImapSettings),
	iInboxClearNewFlags(ETrue),
	iNonInboxClearNewFlags(ETrue)
	{
	}

/**
Class destructor
*/
CImapSyncManager::~CImapSyncManager()
	{
	delete iFolderTreeSync;
	delete iSyncSubs;
	delete iSubscribeList;
	delete iUnsubscribeList;
	delete iInbox;
	iFolderList.ResetAndDestroy();
	}

/**
Static factory constructor

@param aServiceId
The TMsvId of the Imap service that is being accessed.
@param aOffLine The CImapOfflineControl that provides access methods to pending offline operations
@return The constructed CImapSuncManager object pushed onto the cleanup stack
*/
EXPORT_C CImapSyncManager* CImapSyncManager::NewLC(CMsvServerEntry& aEntry, CImapSettings& aImapSettings)
	{
	CImapSyncManager* self=new (ELeave) CImapSyncManager( aEntry, aImapSettings);
	CleanupStack::PushL(self);

	// Non-trivial constructor
	self->ConstructL();
	return self;
	}

/**
Static factory constructor with cleanup

@param aServiceId
The TMsvId of the Imap service that is being accessed.
@param aOffLine The CImapOfflineControl that provides access methods to pending offline operations
@return The constructed CImapSuncManager object
*/
EXPORT_C CImapSyncManager* CImapSyncManager::NewL(CMsvServerEntry& aEntry, CImapSettings& aImapSettings)
	{
	CImapSyncManager* self=NewLC( aEntry, aImapSettings);
	CleanupStack::Pop();
	return self;
	}

/**
The non-trival constructor. Initialise the data members and create the subcribed local folder list
*/
void CImapSyncManager::ConstructL()
	{

	// Get the flags from iImapSettings
	iSynchroniseStrategy = iImapSettings.Synchronise();
	iSubscribeStrategy = iImapSettings.Subscribe();
	iServiceId = iImapSettings.ServiceId();
	// Find the inbox
	GetInboxL();
	// Create the list for the rest of the folders
	iSubscribeList=new (ELeave) RArray<TUint>(4);
	iUnsubscribeList=new (ELeave) RArray<TUint>(4);
	CreateLocalFolderListL();
	// We're an active object...
	CActiveScheduler::Add(this);
	}

	
/** 
Set the current entry context on the Message server

@param aId
The entry id to set in the Message server
@leave KErrNotFound if the entry does not exist or KErrLocked if the entry is locked.
*/
void CImapSyncManager::SetEntryL(const TMsvId aId)
	{
	User::LeaveIfError(iServerEntry.SetEntry(aId));
	}

/**
Sets the context's index entry to the specified values on the Message server

@param  aEntry
The new details for the entry.
@leave KErrAccessDenied - the entry is read only (deleted entry, standard folder, or locked); 
KErrNotSupported - aEntry is invalid or the ID specified in aEntry is not the same as the context ID
or no context has been set for the object; 
KErrNoMemory - a memory allocation failed. 
*/
void CImapSyncManager::ChangeEntryL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iServerEntry.ChangeEntry(aEntry));
	}

/**
Change entry in bulk mode (i.e no index file commit. no notify)

@param  aEntry
The new details for the entry.
@leave KErrAccessDenied - the entry is read only (deleted 
entry, standard folder, or locked); 
KErrNotSupported - aEntry is invalid or the 
ID specified in aEntry is not the same as the context ID or no context has been 
set for the object; 
KErrNoMemory - a memory allocation failed.
*/
void CImapSyncManager::ChangeEntryBulkL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iServerEntry.ChangeEntryBulk(aEntry));
	}

/**
Return a list of all child entries of the currently selected entry node.

@param  aSelection
The ids for the found child entries will be put into this parameter
*/
void CImapSyncManager::GetChildrenL(CMsvEntrySelection& aSelection)
	{
	User::LeaveIfError(iServerEntry.GetChildren(aSelection));
	}

/** 
Returns a pointer to the CImapFolder object associated with the passed entry ID.
CImapSyncManager maintains an array of CImapFolder objects that are subscribed
according to sync and subscribe strategy settings - if the folder exists in this
array then the pointer to this instance is returned. Otherwise a CImapFolder
instance is created for the specified folder and appended to the list.

As the folder list is rebuilt on folder tree synscronisation, code using this
API cannot expect a returned pointer to remain valid following an account or
folder tree synchronisation operation.

This method preserves the context of the shared CMsvServerEntry

@param aMsvId The TMsvId of the folder as stored in the message server

@param aAddToSubscribedList holds bool value,  ETure adds to subscribed list, EFalse does not add.

@return NULL if the folder does not exist, pointer to the CImapFolder object otherwise.
*/
EXPORT_C CImapFolder* CImapSyncManager::GetFolderL(TMsvId aMsvId, TBool aAddToSubscribedList)
	{
	TMsvId preservedId = iServerEntry.Entry().Id();
	
	// Retrieve the folder from the maintained list if it is present.
	// Will be present if it is subscribed according to settings.
	CImapFolder* folder = GetFolderFromList(aMsvId);
	
	if (folder==NULL)
		{
		// Requested folder is not in the list of subscribed mailboxes.
		// It may have been subscribed locally since the last folder sync 
		// operation, or the client application may be attempting to access
		// a non-subscribed folder.
		// Create an instance for the folder and add it to the folder list.
		SetEntryL(aMsvId);
		TMsvEmailEntry entry = iServerEntry.Entry();
		folder = CImapFolder::NewLC(*this, iServerEntry, entry, iImapSettings, KNullDesC());
		if(aAddToSubscribedList)
			{
			iFolderList.AppendL(folder);
			}
		CleanupStack::Pop(folder);

		// recover original serverentry context
		SetEntryL(preservedId);
		}
	return folder;
	}

/**
Returns a pointer to the CImapFolder object associated with the passed entry ID,
if it appears in the array of CImapFolders.

@param aMsvId The TMsvId of the folder as stored in the message server
@return NULL if the folder is not in the list, pointer to the CImapFolder object otherwise.
*/
CImapFolder* CImapSyncManager::GetFolderFromList(TMsvId aMsvId)
	{
	// Check the inbox first
	if(iInbox)
		{
		if(iInbox->MailboxId() == aMsvId)
			{
			return iInbox;
			}
		}
		
	TInt count = iFolderList.Count();
	
	for(TInt i = 0; i < count; i++)
		{
		if(iFolderList[i]->MailboxId() == aMsvId)
			{
			return iFolderList[i];
			}
		}

	// the folder does not exist
	return NULL;
	}

/** 
This function populates the CImapfolder object passed in by the caller using the TMsvId given to get the relevent folder from
the message server.

@param aMsvId.
The TMsvId of the folder as stored in the message server to be use to find the imap folder 
@return Returns NULL if the folder is not subscribed or does not exist, pointer to the CImapFolder object otherwise.
*/
EXPORT_C CImapFolder* CImapSyncManager::GetTempFolderL(TMsvId aMsvId)
	{
	CImapFolder* tempfolder = NULL;
	SetEntryL(aMsvId);
	TMsvEmailEntry entry = iServerEntry.Entry();
	tempfolder = CImapFolder::NewLC(*this, iServerEntry, entry, iImapSettings, KNullDesC());
	CleanupStack::Pop(tempfolder);
		
	return tempfolder;
	}

/** 
This function populates the CImapfolder object that represent the current service Inbox

@return Returns the pointer to the CImapFolder representing the Inbox.
*/
EXPORT_C CImapFolder* CImapSyncManager::Inbox()
	{
	return iInbox;
	}
	
RArray<TUint>* CImapSyncManager::GetUnSubscribedFolderToDoList()
	{
	return iUnsubscribeList;
	}
	
RArray<TUint>* CImapSyncManager::GetSubscribedFolderToDoList()
	{
	return iSubscribeList;
	}


/** 
Search through the messaging server to find the entry that represented the inbox and if not already
in the list of CImapFolder objects, create one.
*/
void CImapSyncManager::GetInboxL()
	{

	// Remove the old inbox
	delete iInbox;
	iInbox = NULL;
		
	// First of all, synchronise the inbox
	CMsvEntrySelection *findinbox=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(findinbox);
	SetEntryL(iServiceId);
	GetChildrenL(*findinbox);
	
	// Find inbox	
	TMsvId inboxid=0;
	for(TInt a=0;a<findinbox->Count();a++)
		{
		SetEntryL((*findinbox)[a]);
		if(iServerEntry.Entry().iDetails.CompareF(KIMAP_INBOX)==0)
			{
			inboxid=(*findinbox)[a];
			TMsvEmailEntry e = iServerEntry.Entry();
			if((iInbox = GetFolderFromList(inboxid)) == NULL)
				{
				CImapFolder* tempfolder = CImapFolder::NewLC(*this, iServerEntry, e, iImapSettings, KIMAP_INBOX());
				CleanupStack::Pop(tempfolder);
				iInbox = tempfolder;
				}

			// Mailbox flag not set? It always should be. This will 'repair' it.
			if(!e.Mailbox())
				{
				// Set it
				e.SetMailbox(ETrue);
				ChangeEntryL(e);
				}
			
			// Found the inbox, so no need to search any further
			break; // from for loop
			}
		}

	// Clean up
	CleanupStack::PopAndDestroy(findinbox);
	findinbox = NULL;

	// Not found/no entries?
	if(inboxid == 0)
		{
		// Create an inbox entry below the service. This is a local-only
		// creation as the server *WILL* have an inbox.
		SetEntryL(iServiceId);
		TMsvEmailEntry entry;

		entry.iType = KUidMsvFolderEntry;
		entry.iMtm = KUidMsgTypeIMAP4;
		entry.iServiceId = iServiceId;
		entry.SetMtmData1(0);
		entry.SetMtmData2(0);
		entry.SetMtmData3(0);
		entry.iSize = 0;
		entry.SetUID(0);
		entry.SetValidUID(EFalse);
		entry.SetMailbox(ETrue);
		entry.SetLocalSubscription(ETrue);
		entry.SetComplete(ETrue);
		entry.iDetails.Set(KImapInboxDisplayName);
		User::LeaveIfError(iServerEntry.CreateEntry(entry));

		// Set inbox
		// not there so add it
		CImapFolder* tempfolder = CImapFolder::NewLC(*this, iServerEntry, entry, iImapSettings, KIMAP_INBOX());
		CleanupStack::Pop(tempfolder);
		iInbox = tempfolder;
		}
	}

/**
Synchronise outstanding delete operations on all subscribe folders on the IMAP service

@param aStatus
The TRequestStatus to complete
@param aSession
The session to be use for the sync operation
*/
EXPORT_C void CImapSyncManager::SynchroniseDeletesL(TRequestStatus& /*aStatus*/, CImapSession& /*aSession*/)
	{
	}

/**
Function to synchronise the inbox only.

@param aStatus
The TRequestStatus to complete
@param aSession
The session to be use for the sync operation
*/
EXPORT_C void CImapSyncManager::SynchroniseInboxL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	//initialise progress 
	iFoldersToDo=0;
	iFoldersDone=0;
	iFoldersNotFound=0;
	iErrorCode=KErrNone;
	iSyncDoSingleStep = ETrue;
	
	iSession = &aSession;
	
	// Note the invisibility
	iNewFoldersAreInvisible = iImapSettings.UpdatingSeenFlags();
	iPerformDeletes = iImapSettings.DeleteEmailsWhenDisconnecting();

	// Get the synchronise settings
	iSynchroniseStrategy = iImapSettings.Synchronise();
	iSubscribeStrategy = iImapSettings.Subscribe();

	GetInboxL();

	// Reset stats: 1 folder remaining (inbox)
	iFoldersToDo = 1;
	iSyncProgressState = TImap4SyncProgress::ESyncInbox;
	
	// Synchronise with the inbox
	iInbox->SelectL(iStatus, aSession, EFalse);
	iSyncState = EInboxSelect;
	iNextSyncAction = EInboxSync;

	Queue(aStatus);
	SetActive();
	}
	
/**
Synchronise the subscription status for the folders on the remote server only.

@param aStatus
The TRequestStatus to complete
@param aSession
The session to be use for the sync operation
*/
EXPORT_C void CImapSyncManager::SynchroniseFolderSubscriptionsL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	// Only do this step
	iSyncDoSingleStep = ETrue;

	// Save the session
	iSession = &aSession;

	// Run the sync folder subscriptions status step
	// set iSyncState when folder subscription is kicked off in DoRunL()
	iNextSyncAction = ECheckRemoteSubscription;

	Queue(aStatus);
	CompleteSelf();
	SetActive();
	}
	
/**
At the first call the value of aFolderId should be set to 0. This parameter will be updated with
each call. Therefore after each calll to this function the value of aFolderCounter should be check,
if the value is -1 then the last folder has already been processed.
The Inbox is not part of this function call.

@param aFolderCounter
Folder counter to keep track of the current folder to be process. 
*/
EXPORT_C void CImapSyncManager::NextSubscribedFoldersL(TMsvId& aFolderId)
	{
	// Only do this step
	iSyncDoSingleStep = ETrue;

	if(aFolderId > 0)
		{
		// Get the next folder TMsvId and sets the variable
		++iSyncFolderCounter;
		if((iSyncFolderCounter) < iFolderList.Count())
			{
			CImapFolder *folder = iFolderList[iSyncFolderCounter];
			aFolderId = folder->MailboxId();
			}
		else
			{			
			aFolderId = -1;
			}
		}
	else if(aFolderId == 0)
		{
		iSyncFolderCounter = 0;

		if(iFolderList.Count() > 0)
			{
			CImapFolder *folder = iFolderList[0];
			aFolderId = folder->MailboxId();
			}
		else
			{
			aFolderId = -1;
			}
		}
	else
		{
		aFolderId = -1;
		__ASSERT_DEBUG(EFalse,TImapServerPanic::ImapPanic(TImapServerPanic::EImapSyncManagerInvalidFolderID));
		}
	}

/**
Perform the next synchronise step for all subscribed folders on the IMAP service.
This function should only be call after one to NextSubscribedFoldersL to setup the folder to be synchronise.
The Inbox will not be synchronise with this function call.

@param aStatus
The TRequestStatus to complete
@param aSession
The session to be use for the sync operation
@param aFolderId
Folder id to keep track of the current folder to be process. 
*/
EXPORT_C void CImapSyncManager::SynchroniseSubscribedFoldersL(TRequestStatus& aStatus, CImapSession& aSession, TMsvId& aFolderId)
	{
	// Only do this step
	iSyncDoSingleStep = ETrue;

	// Save the session
	iSession = &aSession;

	// Run the sync folder subscriptions status step
	if(aFolderId > 0)
		{
		iNextSyncAction = EFolderSelect;
		Queue(aStatus);
		CompleteSelf();
		SetActive();
		}
	}

/**
Synchronises the local view of the directory structure on the remote IMAP server

@param aStatus
The TRequestStatus to complete
@param aSession
The session to be use for the sync operation
*/
EXPORT_C void CImapSyncManager::SynchroniseFolderTreeL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	// Only do this step
	iSyncDoSingleStep = ETrue;

	// Save the session
	iSession = &aSession;

	// Note where this list came from
	iServiceId = iImapSettings.ServiceId();

	// Create the synchronise tree object and starts the sync
	if(iFolderTreeSync)
		{
		delete iFolderTreeSync;
		iFolderTreeSync = NULL;
		}

	iSyncState = ESynchroniseTree;
	iNextSyncAction = EEndSynchroniseTree;
	//update the progress object
	iSyncProgressState= TImap4SyncProgress::ESyncFolderTree;

	iFolderTreeSync = CImapOpSyncFolderTree::NewL(iServerEntry, *iSession, *this, iImapSettings);
	iFolderTreeSync->SynchroniseTreeL(iStatus);

	Queue(aStatus);
	SetActive();
	}

/**
Callback function for the operation class CImapOpSyncSubs.
Cycle through the list of local subscribed folders and creates one from the CImapListFolderInfo if
it does not exist in the list.

@param aFolderInfo
The CImapListFolderInfo to be processed
*/
#ifdef __IMAP_LOGGING
void CImapSyncManager::AddSubscribedFolderFromInfoL(CImapListFolderInfo* aFolderInfo, TInt aLogId)
#else //__IMAP_LOGGING
void CImapSyncManager::AddSubscribedFolderFromInfoL(CImapListFolderInfo* aFolderInfo, TInt /*aLogId*/)
#endif //__IMAP_LOGGING
	{
#ifdef __IMAP_LOGGING
	HBufC8* tempBuf = HBufC8::NewL(aFolderInfo->FolderName().Length());
	TPtr8 tempBufPtr = tempBuf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf7(tempBufPtr, aFolderInfo->FolderName(), EFalse);
	__LOG_FORMAT((aLogId, "AddSubscribedFolderFromInfoL - %S", &tempBufPtr));
	delete tempBuf;
#endif

	// Don't add the INBOX
	if(aFolderInfo->FolderName().CompareF(KIMAP_INBOX) == 0)
		{
		return;
		}

	// Search the folder list and don't add it if it's already there
	TInt foldercount = iFolderList.Count();
	for (int i = 0; i < foldercount; i++)
		{
		if(iFolderList[i]->FullFolderPathL().Compare(aFolderInfo->FolderName()) == 0)
			{
			// Found the folder object just return
			return;
			}
		}

	TMsvId folderid = FindAndSetFolderL(aFolderInfo->FolderName(), aFolderInfo->iHierarchySeperator);
	
	if(folderid != -1)
		{
		// Set subscribed flag
		TMsvEmailEntry entry = iServerEntry.Entry();
		if(!entry.Subscribed())
			{
			// It needs changing, do it
			entry.SetVisible(ETrue);
			if (iSubscribeStrategy == EUpdateLocal || iSubscribeStrategy == EUpdateBoth)
				{
				// set local subscribe flag if updating local flags
				entry.SetLocalSubscription(ETrue);
				}
			entry.SetSubscribed(ETrue);
			ChangeEntryL(entry);
			}

		// Create and add the CImapfolder object
		CImapFolder* tempfolder = CImapFolder::NewLC(*this, iServerEntry, entry, iImapSettings, KNullDesC());
		iFolderList.AppendL(tempfolder);
		CleanupStack::Pop(tempfolder);
		__LOG_TEXT(aLogId, "Folder Added");
		}
	}

/**
Create a CImapFolder list of local subscribed folder from scratch
*/
void CImapSyncManager::CreateLocalFolderListL()
	{
	// Clear list of folders to sync, and subscribe/unsubscribe lists
	iFolderList.ResetAndDestroy();

	AddLocalFoldersL(iImapSettings.ServiceId());
	
	// Complete any bulk entry changes that occured during AddLocalFoldersL()
	iServerEntry.CompleteBulk();
	}

/**
For a given folder id cycle through the current list of folders and create a new CImapFolder item 
if it's not already in the list. Do not add the Inbox or the service.

@param aFolder
The TMsvId of the folder to be added.
*/
void CImapSyncManager::AddIfNotThereL(TMsvId aFolder)
	{
	// first see if we already have this folder and return if we do
	for (int i=0; i < iFolderList.Count(); i++)
		{
		if( iFolderList[i]->MailboxId() == aFolder)
			{
			return;
			}
		}

	// don't add the inbox or service
	if(aFolder == iInbox->MailboxId() || aFolder == iServiceId)
		{
		return;
		}
	
	// visit folder
	SetEntryL(aFolder);
	TMsvEmailEntry e=iServerEntry.Entry();
	// not there so add it
	CImapFolder* tempfolder = CImapFolder::NewLC(*this, iServerEntry, e, iImapSettings, KNullDesC());
	iFolderList.AppendL(tempfolder);
	CleanupStack::Pop(tempfolder);
	}

/**
Add local (subscribed) folders to iFolderList - this is the list of
folders that are to be synchronised according to flags and settings.
Recursive.

Also updates subscription flags on the TMsvEntry for the folder
and adds the folder to iSubscribeList or iUnsubscribeList if remote
update of subscription flag is required. These arrays are used
during subscription synchronisation later in the sync procedure.


@param aFolder
TMsvId of the folder in the local message store.
*/
void CImapSyncManager::AddLocalFoldersL(const TMsvId aFolder)
	{
	// Select the entry
	SetEntryL(aFolder);
	TMsvEmailEntry entry = iServerEntry.Entry();

	// Is it actually a folder or service? If not, ignore it. 
	if(entry.iType != KUidMsvServiceEntry &&
		entry.iType != KUidMsvFolderEntry)
		{
		return;
		}

	// What we do now depends on the strategy

	// addthisone indicates this folder should be added to the list of folders
	// that are to be synchronised according to subscription flags and settings.
	TBool addthisone = EFalse;
	
	switch(iSynchroniseStrategy)
		{
	case EUseLocal:
		{
		// Is it locally subscribed?
		if(entry.LocalSubscription())
			{
			addthisone = ETrue;
			}
		}
		break;

	case EUseRemote:
		{
		// Is it remotely subscribed?
		if(entry.Subscribed())
			{
			addthisone = ETrue;
			}
		}
		break;

	case EUseCombination:
		{
		// Either will do
		if(entry.LocalSubscription() || entry.Subscribed())
			{
			addthisone = ETrue;
			}
		}
		break;
	default:
		{
		__ASSERT_DEBUG(EFalse,TImapServerPanic::ImapPanic(TImapServerPanic::EAddLocalFolderInvalidSynchStrategy));
		break;
		}
		}

	
	// Any childrenlist?
	CMsvEntrySelection *childrenlist = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(childrenlist);
	GetChildrenL(*childrenlist);
	TInt noofchildren = childrenlist->Count();

	if(aFolder != iInbox->MailboxId() && aFolder != iServiceId)
		{
		if(addthisone)
			{
			// Do updating
			switch(iSubscribeStrategy)
				{
			case EUpdateNeither:
				break;

			case EUpdateLocal:
				{
				if(!entry.LocalSubscription())
					{
					// Update local subscription flag
					entry.SetLocalSubscription(ETrue);
					SetEntryL(aFolder);
					ChangeEntryBulkL(entry); // bulk op completed by the caller of this method
					}
				}
				break;
			case EUpdateRemote:
				{
				if(!entry.Subscribed())
					{
					iSubscribeList->InsertInOrder(entry.Id());
					}
				}
				break;
			case EUpdateBoth:
				{
				if(entry.LocalSubscription() || entry.Subscribed())
					{
					if (!entry.LocalSubscription())
						{
						// Update local subscription flag
						entry.SetLocalSubscription(ETrue);
						SetEntryL(aFolder);
						ChangeEntryBulkL(entry); // bulk op completed by the caller of this method
						}
					if (!entry.Subscribed())
						{
						// set the remote subscribed flag...
						iSubscribeList->InsertInOrder(entry.Id());
						}
					}
				}
				break;
			default:
				{
				__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EAddLocalFolderInvalidSubscribeStrategy));
				break;	
				}
				}
			}
		else // if(!addthisone)
			{
			// Do updating
			switch(iSubscribeStrategy)
				{
			case EUpdateNeither:
				break;

			case EUpdateLocal:
				{
				if(entry.LocalSubscription())
					{
					// Update local subscription flag
					entry.SetLocalSubscription(EFalse);
					SetEntryL(aFolder);
					ChangeEntryBulkL(entry); // bulk op completed by the caller of this method
					}
				}
				break;
			case EUpdateRemote:
				{
				if(entry.Subscribed())
					{
					iUnsubscribeList->InsertInOrder(entry.Id());
					}
				else if (entry.LocalSubscription())
					{
					iSubscribeList->InsertInOrder(entry.Id());
					addthisone = ETrue;
					}
				}
				break;
			case EUpdateBoth:
				{
				if(entry.LocalSubscription() || entry.Subscribed())
					{
					if (!entry.LocalSubscription())
						{
						// Update local subscription flag
						entry.SetLocalSubscription(ETrue);
						SetEntryL(aFolder);
						ChangeEntryBulkL(entry); // bulk op completed by the caller of this method
						}
					if (!entry.Subscribed())
						{
						iSubscribeList->InsertInOrder(entry.Id());
						addthisone = ETrue;
						}
					}
				}
				break;	
				} // end of switch 
			} // end of if (addthisone)

		// add subscribed afterwards
		if(addthisone)
			{
			AddIfNotThereL(aFolder);
			}
		else
			{
			// This folder is not subscribed, but has childrenlist. 
			// If any childrenlist are messages, then delete.
			if(noofchildren)
				{
				// This folder is not subscribed to, so check if it has any messages.
				// Do each in turn
				TInt child = childrenlist->Count();
				while (child--)
					{
					SetEntryL((*childrenlist)[child]);
					TMsvEmailEntry deletingentry = iServerEntry.Entry();

					// Is it a message?
					if(deletingentry.iType == KUidMsvMessageEntry)
						{
						SetEntryL(deletingentry.Id());
						SetEntryL(iServerEntry.Entry().Parent());
						iServerEntry.DeleteEntry(deletingentry.Id());
						}
					}

				// If we've some childrenlist then get the childrenlist list again
				SetEntryL(aFolder);
				GetChildrenL(*childrenlist);
				noofchildren=childrenlist->Count();
				} // end of if(noofchildren) 
			}
		} // end of if(aFolder != iInbox->MailboxId() && aFolder != iServiceId)
	

	// Any childrenlist?
	if(noofchildren)
		{
		// Do each in turn
		for(TInt child=0;child<childrenlist->Count();child++)
			{
			AddLocalFoldersL((*childrenlist)[child]);
			}
		
		// Complete any bulk entry changes that occured during AddLocalFoldersL()
		iServerEntry.CompleteBulk();
		}

	CleanupStack::PopAndDestroy(childrenlist);
	}


/**
Deleting the child messages of the given folder from the message server.

@param aDeleteFolderId
The TMsvId of the foler whose content is to be deleted.
*/
EXPORT_C void CImapSyncManager::DeleteFolderContentsL(TMsvId aDeleteFolderId)
	{
	// Select the entry
	SetEntryL(aDeleteFolderId);
	TMsvEmailEntry entry = iServerEntry.Entry();

	// Is it actually a folder or service? If not, ignore it. 
	if(entry.iType != KUidMsvServiceEntry &&
		entry.iType != KUidMsvFolderEntry)
		{
		return;
		}

	// Any childrenlist?
	CMsvEntrySelection *childrenlist = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(childrenlist);
	GetChildrenL(*childrenlist);
	TInt noofchildren = childrenlist->Count();

	// Any childrenlist?
	if(noofchildren)
		{
		// Do each in turn
		for(TInt child = 0; child < childrenlist->Count(); ++child)
			{
			TMsvEntry* entryPtr;
			TMsvId id = (*childrenlist)[child];
			User::LeaveIfError(iServerEntry.GetEntryFromId(id, entryPtr));

			// Is it a message? And is it real (not shadow)
			if(entryPtr->iType == KUidMsvMessageEntry &&
				entryPtr->iRelatedId == KMsvNullIndexEntryId )
				{
				// yes it is... remove it
				TInt err (iServerEntry.DeleteEntry(id));
				if(err == KErrInUse)
					{
					// Dont leave if err = KErrInUse
					}
				else
					{
					User::LeaveIfError(err);
					}
				}
			}
		}

	CleanupStack::PopAndDestroy(childrenlist);
	}

/**
Remove the given folder from the folder index and delete the contents.

@param aDeleteFolderId
The TMsvId of the foler to be deleted.
*/
EXPORT_C void CImapSyncManager::RemoveFolderL(TMsvId aMsvId)
	{
	// Search the folder list and remove the folder
	TInt foldercount = iFolderList.Count();
	for (int i = 0; i < foldercount; i++)
		{
		if( iFolderList[i]->MailboxId() == aMsvId)
			{
			// Found the folder object remove then return
			delete iFolderList[i];
			iFolderList.Remove(i);
			break;
			}
		}

	DeleteFolderContentsL(aMsvId);

	SetEntryL(aMsvId);
	TMsvEmailEntry entry = iServerEntry.Entry();
	SetEntryL(entry.Parent());
	iServerEntry.DeleteEntry(aMsvId);
	}
	
/**
Called to notify the sync manager of a change in a folder's name.
The TMsvEmailEntry for the folder is updated with the new name.
The CImapFolder object representing the folder and that of any children
folders affected by the namechange is also updated to show the new path.
This must be called after a remote folder has been renamed.

@param aFolderId id of the folder that has been renamed
@param aNewName  the new folder name
*/
EXPORT_C void CImapSyncManager::RenameLocalL(TMsvId aFolderId, const TDesC& aNewName)
	{

	// First rename the folder on the local message server
	SetEntryL(aFolderId);
		
	TMsvEmailEntry entry = iServerEntry.Entry();
	entry.iDetails.Set(aNewName);
	ChangeEntryL(entry);
	
	// Search the folder list and rename the folder if we have it subscribed
	// then search the rest of the folder list and resets the full path for any subfolders.
	TInt foldercount = iFolderList.Count();
	CImapFolder* folder = NULL;
	
	for (int i = 0; i < foldercount; i++)
		{
		if( iFolderList[i]->MailboxId() == aFolderId)
			{
			folder = iFolderList[i];
			// Found the folder object
			break;
			}
		}
	
	if(folder)
		{
		// Save a copy of the old folder name
		RBuf folderparentnamebuffer;
		folderparentnamebuffer.CleanupClosePushL();
		folderparentnamebuffer.CreateL(folder->FullFolderPathL());

		// reset the path for the rename folder
		// - this will be rebuilt when next requested
		folder->SetFullFolderPathL(KNullDesC());
		
		TPtrC folderparentpath;
		// Try and find the parent folder in the folder list
		for (int i = 0; i < foldercount; i++)
			{
			folderparentpath.Set((iFolderList[i]->FullFolderPathL().Left(folderparentnamebuffer.Length())));
			if( folderparentpath.Compare(folderparentnamebuffer) == 0)
				{
				// Found the parent name, reset the path
				iFolderList[i]->SetFullFolderPathL(KNullDesC());
				}
			}
			
		CleanupStack::PopAndDestroy(&folderparentnamebuffer);
		}
	}


/**
Find the folder in the message server using the given path the sets the server context.

@param aFullPath
The full pathname of the folder relative to the service.
@param aHierarchySeperator
The character that is use as the path separator for the full pathname.
@return The TMsvId of the folder if it was found in the message server.
*/
TMsvId CImapSyncManager::FindAndSetFolderL(const TPtrC& aFullPath, TChar& aHierarchySeperator)
	{
	TPtrC path(aFullPath);
	TMsvId foundfolderid = -1;
	
	if (iImapSettings.FolderPath().Length() != 0)
		{
		//FolderPath will be 8 bit Descriptor, so we need to convert into 16 bit
		HBufC* tempPath = HBufC::NewL(iImapSettings.FolderPath().Length());
		tempPath->Des().Copy(iImapSettings.FolderPath());
		
		if (path.Match(tempPath->Des()) == 0)
			{
			TInt separator = path.Locate(aHierarchySeperator);
			if (separator == iImapSettings.FolderPath().Length())
				{
				++separator;
				if (separator < path.Length())
					{
					path.Set(path.Mid(separator));
					}
				else
					{
					// Path contains only the settings FolderPath and separator. This is an error.
					delete tempPath;
					return KErrNotFound;
					}
				}
			}
		delete tempPath;
		}
 			
	// Go down through path
	CMsvEntrySelection*	selection = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(selection);
	
	TMsvId currentpos = iServiceId;
			
	while(currentpos)
		{
		// Truncate this bit of path: look for a hierarchy separator
		TInt separator = path.Locate(aHierarchySeperator);
		TPtrC element(path);

		// Anything?
		if(separator != KErrNotFound)
			{
			// Truncate this element there
			element.Set(element.Left(separator));
			}

		// Try to find this element at the search level
		SetEntryL(currentpos);
		GetChildrenL(*selection);

		// Nothing? Give up.
		if(!selection->Count())
			{
			break;				// out of while
			}
			
		// Check all the children
		TInt a;

		for(a = 0; a < selection->Count(); ++a)
			{
			// This one?
			SetEntryL((*selection)[a]);

			// Check if the path element matches the current entry details field.
			// We need to do a specific check if the entry is the inbox, because
			// inbox is case insensitive.
			if ((iServerEntry.Entry().iType == KUidMsvFolderEntry && iServerEntry.Entry().iDetails.Compare(element) == 0 )||
			    (element.CompareF(KIMAP_INBOX) == 0 && EntryIsInbox(iServerEntry.Entry())))
				{
				// Found it! Are we at the end, ie no more elements?
				if(separator == KErrNotFound)
					{
					// Get the entry TMsvId
					TMsvEmailEntry entry = iServerEntry.Entry();
					foundfolderid = entry.Id();
					
					// All done: this will exit the loop
					currentpos = 0;
					break;
					}
				else
					{
					// Update path
					currentpos = (*selection)[a];
					path.Set(path.Mid(separator+1, path.Length()-separator-1));

					// Exit loop
					break;
					}
				}
			}

		if(a == selection->Count())
			{
			// Didn't find it. Humm
			// Didn't find entry : tree out of date?
			break;				// out of while
			}
		}
	CleanupStack::PopAndDestroy(selection);
	selection = NULL;
	
	return foundfolderid;
	}

/**
Returns updated progress information on outstanding synchronisation operations.

@param aProgress
The reference to the progress object to be fill in with the local progress information.
*/
EXPORT_C void CImapSyncManager::Progress(TImap4SyncProgress& aProgress)
	{
	//copy values from member progress ob into aProgress
	//this includes the progress information set by CImapFolder
	
	if(iInbox)
		{
		iInbox->Progress(aProgress);
		}
	aProgress.iFoldersToDo=iFoldersToDo;
	aProgress.iFoldersDone=iFoldersDone;
	aProgress.iFoldersNotFound=iFoldersNotFound;
	aProgress.iState=iSyncProgressState;
	aProgress.iErrorCode=iErrorCode;	
	}

/**
Called when async child completes with >=KErrNone
*/
void CImapSyncManager::DoRunL()
	{
	//update the progress object
	iErrorCode=iStatus.Int();
	
	// Handle any server errors
	if(iStatus.Int()!=KErrNone)
		{
		if(TInt err=ProcessSessionError() != KErrNone)
			{
			Complete(err);
			return;
			}
		}
		
	switch(iNextSyncAction)
		{
	case EInboxSync:
		{
		// Start the sync of the current folder (ie, the inbox)
		iNextSyncAction = EEndSingleSyncStep;
		iSyncState = EInboxSync;
		iSyncProgressState=TImap4SyncProgress::ESyncInbox;
        iInbox->SynchroniseL(iStatus, *iSession, EFalse, 0);
		SetActive();
		}
		break;

	case ECheckRemoteSubscription:
		{
		// Create the sync subscriptions object to update the local subscriptions
		if(iSyncDoSingleStep)
			{
			iNextSyncAction = EEndSingleSyncStep;
			}
		else
			{
			iNextSyncAction = ESynchronise;
			}
		
		// Update folder tree
		if(iSyncSubs)
			{
			delete iSyncSubs;
			iSyncSubs = NULL;
			}
		
		iSyncState = ECheckRemoteSubscription;
		iSyncSubs = CImapOpSyncSubs::NewL(iServerEntry, *iSession, *this, iImapSettings);
		//update the progress object		
		iSyncProgressState=TImap4SyncProgress::EUpdateRemoteSubscription;
		iSyncSubs->SynchroniseSubscriptionsL(iStatus);
		SetActive();
		}
		break;
	case EFolderSelect:
		{	
		//update the progess object
		++iFoldersDone;
		iSyncProgressState=TImap4SyncProgress::ESyncOther;
		
		// This case is for the picking the next folder for the full sync.
		if(iSyncFolderCounter >= iFolderList.Count())
			{
			// Ran out of folders. Finishing sync
			Complete(iStatus.Int());
			}
		else
			{
			CImapFolder *folder = iFolderList[iSyncFolderCounter];
			iNextSyncAction = EFolderSynchronise;
			iSyncState = EFolderSelect;
			folder->SelectL(iStatus, *iSession);
			SetActive();
			}
		}
		break;
	case EFolderSynchronise:
		{
		// This case is for synching the selected folder during a full sync.
		// Back to select next folder
		CImapFolder *folder = iFolderList[iSyncFolderCounter];

		if(iSyncDoSingleStep)
			{
			iNextSyncAction = EEndSingleSyncStep;
			}
		else
			{
			iNextSyncAction = EFolderSelect;
			++iSyncFolderCounter;
			}
		
		iSyncState = EFolderSynchronise;
		iSyncProgressState=TImap4SyncProgress::ESyncOther;
		folder->SynchroniseL(iStatus, *iSession, EFalse, 0);
		SetActive();
		}
		break;
	case ESynchronise:
		{
		// This case is for the full synching start with selecting the inbox.
		// Starts the full sync process by reseting the folder counter
		iSyncFolderCounter = 0;

		iSyncState = ESynchronise;
		iNextSyncAction = EFolderSelect;
		CompleteSelf();
		SetActive();
		}
		break;

	case EEndSynchroniseTree:
		{
		// We have finished synchronising the folder tree, so now build the
		// list of locally subscribed folders
		CreateLocalFolderListL();
		// Fall through to next state
		}

	case EEndInboxSync:
		// Finishing the Synching of the inbox
	case EEndSingleSyncStep:
		{
		iSyncProgressState = TImap4SyncProgress::EIdle;
		Complete(iStatus.Int());
		}
		break;
	
	default:
		break;
		}
	}

/**
Called when completing a client request.

@param aErr
The completed error code.
*/
void CImapSyncManager::DoComplete(TInt& aErr)
	{
	// clean up the internal states
	iSyncState = ENotSyncing;
	iNextSyncAction = ENotSyncing;
	iErrorCode = aErr;
	}

/**
Handles any positive completion codes
*/
TInt CImapSyncManager::ProcessSessionError()
	{
	TInt errCode = iStatus.Int();
	if( errCode==KErrImapNo || errCode==KErrImapBad)
		{
		ProcessNegativeServerResponse(errCode);
		}
	return errCode;
	}

/** 
Specific handling for NO and BAD server responses.
The error code is updated to KErrNone if it has been successfully 
fielded by the sync manager.

@param aErrCode
Reference to the error code. 
*/
void CImapSyncManager::ProcessNegativeServerResponse(TInt& aErrCode)
	{
	// iSyncState describes current outstanding operation
	switch (iSyncState)
		{
	case EInboxSelect:
		{
		++iFoldersNotFound;
		break;
		}
	case EFolderSelect:
		{
		// SELECT folder has failed.. 
		++iFoldersNotFound;
		if(!iSyncDoSingleStep)
			{
			// Doing all folders... 
			// ignore error and proceed to the next
			iNextSyncAction = EFolderSelect;
			aErrCode = KErrNone;
			}
		break;
		}
	case EInboxSync:
	case ECheckRemoteSubscription:
	case EFolderSynchronise:
		{
		// NO/BAD responses should have been handled in the
		// folder or operation classes.
		break;
		}
	case ESynchronise:	// self completing state
	case EEndInboxSync:
	case EEndSynchroniseTree:
	case EEndSingleSyncStep:
	default:
		{
		// unexpected states.
		break;
		}
		} // end of switch (iSyncState)
	}


/**
"Silent" cancel for migration - 
Resets the error code to KErrNone following the cancel
*/
EXPORT_C void CImapSyncManager::CancelForMigrate()
	{
	Cancel();
	iErrorCode=KErrNone;
	}

/**
Cancels any outstanding asynchronous service requests.
*/
void CImapSyncManager::DoCancel()
	{
	// iSyncState describes current outstanding operation
	switch (iSyncState)
		{
	case EInboxSelect:
	case EFolderSelect:
		{
		// Note that although select operations are performed on a folder they
		// actually pass our iStatus to the session, so that is why we cancel
		// the session for them.
		iSession->Cancel();
		break;
		}
	case EInboxSync:
		{
		iInbox->Cancel();
		break;
		}
	case ESynchroniseTree:
		{
		iFolderTreeSync->Cancel();
		break;
		}
	
	case ECheckRemoteSubscription:
		{
		iSyncSubs->Cancel();
		break;
		}
	case EFolderSynchronise:
		{
		(iFolderList[iSyncFolderCounter])->Cancel();
		break;
		}

	case ESynchronise:	// self completing state
	case EEndInboxSync:
	case EEndSynchroniseTree:
	case EEndSingleSyncStep:
	case ENotSyncing:
		{
		break;
		}

	default:
		{
		__ASSERT_DEBUG(EFalse,
		               TImapServerPanic::ImapPanic(TImapServerPanic::ESyncManagerCancelUnexpectedState));
		break;
		}
		} // end of switch

	CMsgActive::DoCancel();
	}

void CImapSyncManager::CompleteSelf()
	{
	TRequestStatus* status = &iStatus;
	iStatus = KRequestPending;
	User::RequestComplete(status, KErrNone);
	}

TBool CImapSyncManager::InboxClearNewFlags()
	{
	return iInboxClearNewFlags;
	}

TBool CImapSyncManager::NonInboxClearNewFlags()
	{
	return iNonInboxClearNewFlags;
	}

EXPORT_C void CImapSyncManager::ResetInboxClearNewFlags()
	{
	iInboxClearNewFlags = EFalse;
	}

EXPORT_C void CImapSyncManager::ResetNonInboxClearNewFlags()
	{
	iNonInboxClearNewFlags = EFalse;
	}

/**
Checks if the entry passed in is the inbox.

@param aEntry Entry to check
@return ETrue if entry is inbox, EFalse otherwise
*/
TBool CImapSyncManager::EntryIsInbox(const TMsvEntry& aEntry)
	{
	if (aEntry.Parent() == iImapSettings.ServiceId() &&
	    aEntry.iDetails.CompareF(KIMAP_INBOX) == 0)
		{
		return ETrue;
		}

	return EFalse;
	}

/**
Returns pointer to the nth CImapFolder object in the subscribed folder array.
Also returns aNumSubscribedFolders updated with the total number of subscribed folders.

@param aFolderId index in the array for requested folder, the nth subscribed folder (0 offset)
@param aNumSubscribedFolders updated with total number of subscribed folders.
@return  NULL if aFolderId exceeds the number of subscribed folders, 
           pointer to subscribed CImapFolder object otherwise
*/
EXPORT_C CImapFolder* CImapSyncManager::GetSubscribedFolder(TInt32 aFolderId, TInt32& aNumSubscribedFolders)
	{
	// return updated number of folders for progress reporting
	aNumSubscribedFolders = iFolderList.Count();
	
	if (aFolderId >= aNumSubscribedFolders)
		{
		return NULL;
		}
	else
		{
		return iFolderList[aFolderId];
		}
	}



