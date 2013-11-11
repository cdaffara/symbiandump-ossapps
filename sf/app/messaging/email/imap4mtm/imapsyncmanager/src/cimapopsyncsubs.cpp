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
// imapopsyncsubs.cpp
// 
//

#include "cimapsyncmanager.h"
#include "cimapopsyncsubs.h"
#include "cimapsettings.h"
#include "cimapfolder.h"

CImapOpSyncSubs* CImapOpSyncSubs::NewL(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan,CImapSettings& aImapSettings)
	{
	CImapOpSyncSubs* self=NewLC(aEntry, aSession, aSyncMan, aImapSettings);
	CleanupStack::Pop();

	return self;
	}

CImapOpSyncSubs* CImapOpSyncSubs::NewLC(CMsvServerEntry& aEntry, CImapSession& aSession,CImapSyncManager& aSyncMan,CImapSettings& aImapSettings)
	{
	CImapOpSyncSubs* self=new (ELeave) CImapOpSyncSubs(aEntry, aSession, aSyncMan, aImapSettings);
	CleanupStack::PushL(self);

	// Non-trivial constructor
	self->ConstructL();

	return self;
	}

CImapOpSyncSubs::~CImapOpSyncSubs()
	{
	iImapListFolderInfo.ResetAndDestroy();
	}

void CImapOpSyncSubs::ConstructL()
	{
	// We're an active object...
	CActiveScheduler::Add(this);

	}

void CImapOpSyncSubs::SynchroniseSubscriptionsL(TRequestStatus& aStatus)
	{
	// Do we need to know what folders are subscribed remotely?
	if (iImapSettings.Synchronise() == EUseLocal && 
		(iImapSettings.Subscribe() == EUpdateNeither || iImapSettings.Subscribe() == EUpdateLocal))
		{
		// Just update any remote subscriptions
		iNextSyncAction = CImapSyncManager::EUpdateRemoteSubscription;
		Queue(aStatus);
		CompleteSelf();
		SetActive();
		}
	else
		{
		// Update our list of remotely subscribed folders
		// First, we need to go through the entire service, resetting all
		// the 'remote subscribed' flags.
		ResetSubscriptionFlagsL(iServiceId);

		// Build a buffer to quote the folder path (it may be necessary)
		HBufC16* path=HBufC16::NewLC(iImapSettings.FolderPath().Length()+2);
		path->Des().Copy(iImapSettings.FolderPath());
		if (iImapSettings.FolderPath().Length())
			{
			path->Des().Append(iImapSettings.PathSeparator());
			}

		path->Des().Append(KStarChar);
		
		// Quote it
		CImapFolder::DoQuoteL(path);

		// List this tree
		iNextSyncAction = CImapSyncManager::EProcessRemoteSubscription;
		iServerEntry.SetEntry(iServiceId);
		TMsvEmailEntry entry = iServerEntry.Entry();
		
		iSession.LsubL(iStatus, KNullDesC, path->Des(), iImapListFolderInfo);

		Queue(aStatus);
		SetActive();
		CleanupStack::PopAndDestroy(path);
		}
	}

CImapOpSyncSubs::CImapOpSyncSubs(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan,CImapSettings& aImapSettings)
: CImapSyncOperation(aEntry, aSession, aSyncMan, aImapSettings, EPriorityStandard)
	{
	}

	
/** 
Set the current entry context on the Message server

@param aId
The entry id to set in the Message server
@leave KErrNotFound if the entry does not exist or KErrLocked if the entry is locked.
*/
void CImapOpSyncSubs::SetEntryL(const TMsvId aId)
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
void CImapOpSyncSubs::ChangeEntryL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iServerEntry.ChangeEntry(aEntry));
	}

/**
Return a list of all child entries of the currently selected entry node.

@param  aSelection
The ids for the found child entries will be put into this parameter
*/
void CImapOpSyncSubs::GetChildrenL(CMsvEntrySelection& aSelection)
	{
	User::LeaveIfError(iServerEntry.GetChildren(aSelection));
	}

/**
Reset subscription flags for all children, and recurse into folders

@param aFolder
Folder from which to start the flags resetting
*/
void CImapOpSyncSubs::ResetSubscriptionFlagsL(const TMsvId aFolder)
	{
	// Do this one
	SetEntryL(aFolder);
	TMsvEmailEntry entry = iServerEntry.Entry();

	// A folder or service? If not, return
	if (entry.iType!=KUidMsvServiceEntry &&
		entry.iType!=KUidMsvFolderEntry)
		{
		return;
		}

	// Reset flag if needed
	if (entry.Subscribed())
		{
		// Reset flag and save
		entry.SetSubscribed(EFalse);
		ChangeEntryL(entry);
		}

	// Any children?
	CMsvEntrySelection *children = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);
	GetChildrenL(*children);
	if (children->Count())
		{
		// Do each in turn
		for(TInt child = 0; child<children->Count(); ++child)
			{
			ResetSubscriptionFlagsL((*children)[child]);
			}
		}
	CleanupStack::PopAndDestroy();
	}
	

/**
Called when async child completes with >=KErrNone
*/
void CImapOpSyncSubs::DoRunL()
	{
	// Finish if any server errors.
	if (iStatus.Int()!=KErrNone)
		{
		Complete(iStatus.Int());
		}

	switch(iNextSyncAction)
		{
	case CImapSyncManager::EProcessRemoteSubscription:
		{
		// Call returned from the LSUB command. Now process the list.
		// Update folder tree
		TInt folderscount = iImapListFolderInfo.Count();
		CImapListFolderInfo* folder;
		
		for(TInt loop = 0;loop < folderscount; ++loop)
			{
			folder = iImapListFolderInfo[loop];
			iSyncMan.AddSubscribedFolderFromInfoL(folder, iSession.LogId());
			}
		iSyncMan.CreateLocalFolderListL();

		// Now do the remote subscription
		iNextSyncAction = CImapSyncManager::EUpdateRemoteSubscription;
		CompleteSelf();
		SetActive();
		}
		break;
	case CImapSyncManager::EUpdateRemoteSubscription:
		{
		// Process the subscribed and unsubscribed list from the SyncManager.
		// This should be done after checking the online subscription list.

		RArray<TUint>* unsubscribeList = iSyncMan.GetUnSubscribedFolderToDoList();
		RArray<TUint>* subscribeList = iSyncMan.GetSubscribedFolderToDoList();

		// Any subscription to do?
		if (subscribeList->Count())
			{
			// Take it off the head
			TMsvId folderid = (*subscribeList)[0];
			subscribeList->Remove(0);

			// Subscribe to it
			CImapFolder* tempfolder = iSyncMan.GetTempFolderL(folderid);
			CleanupStack::PushL(tempfolder);
			iSession.SubscribeL(iStatus, tempfolder->FullFolderPathL());
			CleanupStack::PopAndDestroy(tempfolder);
			
			// set the subscribed flag for the folder.
			SetEntryL(folderid);
			TMsvEmailEntry entry = iServerEntry.Entry();
			entry.SetSubscribed(ETrue);
			ChangeEntryL(entry);
			
			SetActive();
			}
		// ...or unsubscription?
		else if (unsubscribeList->Count())
			{
			// Take it off the head
			TMsvId folderid = (*unsubscribeList)[0];
			unsubscribeList->Remove(0);

			// Unsubscribe from it
			CImapFolder* tempfolder = iSyncMan.GetTempFolderL(folderid);
			CleanupStack::PushL(tempfolder);
			iSession.UnsubscribeL(iStatus, tempfolder->FullFolderPathL());
			CleanupStack::PopAndDestroy(tempfolder);
			
			// clear the subscribed flag for the folder.
			SetEntryL(folderid);
			TMsvEmailEntry entry = iServerEntry.Entry();
			entry.SetSubscribed(EFalse);
			ChangeEntryL(entry);
			
			SetActive();
			}
		else
			{
			// All done.
			iNextSyncAction = CImapSyncManager::ENotSyncing;
			Complete(iStatus.Int());
			}
		}
		break;
	default:
	break;
		}
	}

/**
Cancels any outstanding asynchronous service requests.
*/
void CImapOpSyncSubs::DoCancel()
	{
	iSession.Cancel();
	CMsgActive::DoCancel();
	}

/**

*/
void CImapOpSyncSubs::DoComplete(TInt& /*aStatus*/)
	{
	}

void CImapOpSyncSubs::CompleteSelf()
	{
	TRequestStatus* status = &iStatus;
	iStatus = KRequestPending;
	User::RequestComplete(status, KErrNone);
	}
	


