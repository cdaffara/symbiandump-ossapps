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
//

#include "cimapopsyncfoldertree.h"

#include "cimapsettings.h"
#include "cimapsession.h"
#include "cimapfolder.h"
#include "cimaplogger.h"

_LIT(KImapInbox, "INBOX");

/**
Factory constructor

@param aEntry Server entry
@param aSession IMAP session
@param aSyncMan Sync manager
@param aImapSettings IMAP settings

@return Instance of class created on heap
*/
CImapOpSyncFolderTree* CImapOpSyncFolderTree::NewL(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan, CImapSettings& aImapSettings)
	{
	CImapOpSyncFolderTree* self=NewLC(aEntry, aSession, aSyncMan, aImapSettings);
	CleanupStack::Pop();

	return self;
	}

/**
Factory constructor

@param aEntry Server entry
@param aSession IMAP session
@param aSyncMan Sync manager
@param aImapSettings IMAP settings

@return Instance of class created on heap
*/
CImapOpSyncFolderTree* CImapOpSyncFolderTree::NewLC(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan, CImapSettings& aImapSettings)
	{
	CImapOpSyncFolderTree* self=new (ELeave) CImapOpSyncFolderTree(aEntry, aSession, aSyncMan, aImapSettings);
	CleanupStack::PushL(self);

	// Non-trivial constructor
	self->ConstructL();

	return self;
	}

/**
Constructor
*/
CImapOpSyncFolderTree::CImapOpSyncFolderTree(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan, CImapSettings& aImapSettings)
: CImapSyncOperation(aEntry, aSession, aSyncMan, aImapSettings, EPriorityLow)
	{
	}

/**
Second phase constructor
*/
void CImapOpSyncFolderTree::ConstructL()
	{
	// Create some paths that are used in folder name pattern matching

	// Create a path constisting of the inbox followed by the hierarchy
	// separator
	iInboxPathWithSeparator = HBufC::NewL(KImapInbox().Length() + iImapSettings.PathSeparator().Length());
	TPtr inboxPathPtr = iInboxPathWithSeparator->Des();
	inboxPathPtr.Copy(KImapInbox());
	inboxPathPtr.Append(iImapSettings.PathSeparator());

	// If the settings folder path is defined, create one path consisting of
	// the folder path followed by the hierarchy separator, and one path
	// consisting of the folder path only.
	if (iImapSettings.FolderPath().Length() > 0)
		{
		iFolderPathWithSeparator = HBufC::NewL(iImapSettings.FolderPath().Length() + iImapSettings.PathSeparator().Length());
		TPtr folderPathPtr = iFolderPathWithSeparator->Des();
		folderPathPtr.Copy(iImapSettings.FolderPath());

		iFolderPath.Set(folderPathPtr);

		folderPathPtr.Append(iImapSettings.PathSeparator());
		}

	// We're an active object...
	CActiveScheduler::Add(this);
	}

/**
Destructor
*/
CImapOpSyncFolderTree::~CImapOpSyncFolderTree()
	{
	Cancel();

	iImapListFolderInfo.ResetAndDestroy();
	iFolderList.ResetAndDestroy();

	delete iSelection;
	delete iSearchFolder;
	delete iInboxPathWithSeparator;
	delete iFolderPathWithSeparator;
	}

/**
Start tree synchronisation

@param aStatus Caller's request status
*/
void CImapOpSyncFolderTree::SynchroniseTreeL(TRequestStatus& aStatus)
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::SynchroniseTreeL");
	StartBuildingLocalFolderListL();
	Queue(aStatus);
	}

/**
Called when async child completes with >=KErrNone
*/
void CImapOpSyncFolderTree::DoRunL()
	{
	// Finish if any server errors.
	if (iStatus.Int()!=KErrNone)
		{
		Complete(iStatus.Int());
		return ;
		}

	switch (iState)
		{
		case EStateBuildingLocalFolderList:
			{
			ProcessNextFolderInLocalFolderListL();
			break;
			}

		case EStateFetchingFolderList:
			{
			StartProcessingReceivedFolderListL();
			break;
			}

		case EStateProcessingReceivedFolders:
			{
			ProcessNextFolderInReceivedFolderListL();
			break;
			}

		case EStateIdle:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedStateSyncFolderTreeDoRunL));
			break;
			}
		}
	}

/**
Cancels any outstanding asynchronous service requests.
*/
void CImapOpSyncFolderTree::DoCancel()
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::DoCancel");

	switch (iState)
		{
		case EStateBuildingLocalFolderList:
		case EStateProcessingReceivedFolders:
			{
			// Self completing states
			break;
			}

		case EStateFetchingFolderList:
			{
			iSession.Cancel();
			break;
			}

		case EStateIdle:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EUnexpectedStateSyncFolderTreeDoCancel));
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrCancel);
			break;
			}
		}

	CMsgActive::DoCancel();
	}

/**
Called when completing a client request

@param aStatus Completion code
*/
#ifdef __IMAP_LOGGING
void CImapOpSyncFolderTree::DoComplete(TInt& aStatus)
#else //__IMAP_LOGGING
void CImapOpSyncFolderTree::DoComplete(TInt& /*aStatus*/)
#endif //__IMAP_LOGGING
	{
	__LOG_FORMAT((iSession.LogId(), "CImapOpSyncFolderTree::DoComplete - %d", aStatus));

	iImapListFolderInfo.ResetAndDestroy();
	iFolderList.ResetAndDestroy();

	delete iSelection;
	iSelection = NULL;

	delete iSearchFolder;
	iSearchFolder = NULL;

	delete iInboxPathWithSeparator;
	iInboxPathWithSeparator = NULL;

	delete iFolderPathWithSeparator;
	iFolderPathWithSeparator = NULL;
	
	iState = EStateIdle;
	}

/**
Starts building a list of folders for this service that are in the
message store
*/
void CImapOpSyncFolderTree::StartBuildingLocalFolderListL()
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::StartBuildingLocalFolderListL");

	iState = EStateBuildingLocalFolderList;

	iFolderList.ResetAndDestroy();
	iCurrentFolder = 0;

	User::LeaveIfError(iServerEntry.SetEntry(iImapSettings.ServiceId()));

	iSelection = new (ELeave) CMsvEntrySelection;

	// Get all the folders directly under the service entry
	User::LeaveIfError(iServerEntry.GetChildrenWithType(KUidMsvFolderEntry, *iSelection));

	// If the inbox is in the selection, we want to remove it from the selction and
	// specifically add it to the folder list. This ensures that we use the uppercase
	// spelling of inbox and helps in the folder matching that is done against the list
	// of folders we receive from the server.
	for (TInt selLoop = 0; selLoop < iSelection->Count(); ++selLoop)
		{
		User::LeaveIfError(iServerEntry.SetEntry((*iSelection)[selLoop]));

		TMsvEmailEntry entry = iServerEntry.Entry();
		if (entry.iDetails.CompareF(KImapInbox) == 0)
			{
			iSelection->Delete(selLoop);

			CImapFolder* tempFolder = CImapFolder::NewLC(iSyncMan, iServerEntry, entry, iImapSettings, KImapInbox());
			tempFolder->SetFolderMatched(ETrue);
			iFolderList.AppendL(tempFolder);
			CleanupStack::Pop(tempFolder);
			break;
			}
		}

	// If the settings has a defined folder path, then append this at the start
	// of each new folder path. Where the settings has no folder path defined,
	// iFolderPath will just be an empty pointer.
	iCurrentPath.Set(iFolderPath);

	// We can now self complete to keep the state machine running. This will
	// cause the current selection of folders to be processed
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

/**
Process the current selection of folders.
For each entry in the selection, add a new folder in the folder list. Then
take the first entry in the folder list and create a selection of all its
children folders. This process is then repeated until all the folders under
the service have been found.
*/
void CImapOpSyncFolderTree::ProcessNextFolderInLocalFolderListL()
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::ProcessNextFolderInLocalFolderListL");

	HBufC* newPath = NULL;
	TInt curPathLen = iCurrentPath.Length();

	// Loop through every entry in the selection. Each one is a folder.
	for (TInt selLoop = 0; selLoop < iSelection->Count(); ++selLoop)
		{
		User::LeaveIfError(iServerEntry.SetEntry((*iSelection)[selLoop]));

		TMsvEmailEntry entry = iServerEntry.Entry();

		// Create the full path name for this folder. This will be the path
		// name passed in (which is the path name of the parent) with the
		// folder name from the message store entry appended.
		newPath = HBufC::NewLC(curPathLen + 1 + entry.iDetails.Length());
		newPath->Des().Copy(iCurrentPath);
		if (curPathLen > 0)
			{
			newPath->Des().Append(iImapSettings.PathSeparator());
			}
		newPath->Des().Append(entry.iDetails);

#ifdef __IMAP_LOGGING
		HBufC8* tempBuf = HBufC8::NewL(newPath->Length());
		TPtr8 tempBufPtr = tempBuf->Des();
		tempBufPtr.Copy(*newPath);
		__LOG_FORMAT((iSession.LogId(), "Adding Local Folder: - %S", &tempBufPtr));
		delete tempBuf;
#endif

		// Create new IMAP folder and insert into the list.
		CImapFolder* tempFolder = CImapFolder::NewLC(iSyncMan, iServerEntry, entry, iImapSettings, *newPath);
		iFolderList.AppendL(tempFolder);
		CleanupStack::Pop(tempFolder);
		CleanupStack::PopAndDestroy(newPath);
		newPath = NULL;
		}

	if (iCurrentFolder < iFolderList.Count())
		{
		iCurrentPath.Set(iFolderList[iCurrentFolder]->FullFolderPathL());
		iSelection->Reset();

		User::LeaveIfError(iServerEntry.SetEntry(iFolderList[iCurrentFolder]->MailboxId()));
		User::LeaveIfError(iServerEntry.GetChildrenWithType(KUidMsvFolderEntry, *iSelection));

		++iCurrentFolder;

		// Self complete to go around again
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	else
		{
		// We have created the folder list. Need to sort it into alphabetical
		// order.
		TLinearOrder<CImapFolder> sorter(CImapFolder::CompareByFolderName);
		iFolderList.Sort(sorter);

		delete iSelection;
		iSelection = NULL;

		// Now get the list of folders from the server
		SendListRequestL();
		}
	}

/**
Sends the request to the server to get the list of folders
*/
void CImapOpSyncFolderTree::SendListRequestL()
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::SendListRequestL");

	iState = EStateFetchingFolderList;

	iSession.ListL(iStatus, KNullDesC, KStarChar(), iImapListFolderInfo);
	SetActive();
	}

/**
Starts processing the received list of folders on the server
*/
void CImapOpSyncFolderTree::StartProcessingReceivedFolderListL()
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::StartProcessingReceivedFolderListL");

	iState = EStateProcessingReceivedFolders;

	iCurrentFolder = 0;

	// Create a temporary folder which will be used for subsequent searches
	User::LeaveIfError(iServerEntry.SetEntry(iImapSettings.ServiceId()));
	TMsvEmailEntry entry = iServerEntry.Entry();
	iSearchFolder = CImapFolder::NewL(iSyncMan, iServerEntry, entry, iImapSettings, KNullDesC());

	// Self complete to get the process moving
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

/**
Process next folder from the received folder list.
Check if we know about the folder already or not. If we don't know about
it, then we need to add a new folder.
*/
void CImapOpSyncFolderTree::ProcessNextFolderInReceivedFolderListL()
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::ProcessNextFolderInReceivedFolderListL");

	if (iCurrentFolder < iImapListFolderInfo.Count())
		{
		TPtrC currentFolderFullPath = iImapListFolderInfo[iCurrentFolder]->FolderName();

#ifdef __IMAP_LOGGING
		HBufC8* tempBuf = HBufC8::NewL(currentFolderFullPath.Length());
		TPtr8 tempBufPtr = tempBuf->Des();
		tempBufPtr.Copy(currentFolderFullPath);
		__LOG_FORMAT((iSession.LogId(), "Processing received folder: - %S", &tempBufPtr));
		delete tempBuf;
#endif

		if (currentFolderFullPath.Length() > 0)
			{
			TBool processFolder = ETrue;

			// If the settings has a defined folder path, don't process this
			// folder unless the first part of the folder name is the settings
			// folder path or is the inbox.
			if (iFolderPathWithSeparator)
				{
				// We don't need to do a case insensitive search for the inbox
				// because the iInboxPathWithSeparator was created using uppercase,
				// and the received folder list is change by the IMAP session to
				// ensure that the inbox folder is converted to uppercase.
				// Note also that this bit of code will result in the actual inbox
				// folder being removed as it does not have a separator character
				// on the end of it. This will not cause a problem because the
				// inbox has already been specifically added to the list of local
				// folders.
				if ((iFolderPathWithSeparator->Compare(currentFolderFullPath.Left(iFolderPathWithSeparator->Length())) != 0) &&
				    (iInboxPathWithSeparator->Compare(currentFolderFullPath.Left(iInboxPathWithSeparator->Length())) != 0))
					{
					__LOG_TEXT(iSession.LogId(), "Ignoring non folder path folder");
					processFolder = EFalse;
					}
				}

			if (processFolder)
				{
				// Try to find the received folder in the local folder list
				TLinearOrder<CImapFolder> sorter(CImapFolder::CompareByFolderName);
				iSearchFolder->SetFullFolderPathL(currentFolderFullPath);
				TInt pos = iFolderList.FindInOrder(iSearchFolder, sorter);

				if (pos != KErrNotFound)
					{
					// We already know about the folder. Just set its flag to indicate
					// we have matched it.
					iFolderList[pos]->SetFolderMatched(ETrue);
					__LOG_TEXT(iSession.LogId(), "Folder already known");
					}
				else
					{
					AddReceivedFolderL(iImapListFolderInfo[iCurrentFolder], currentFolderFullPath);
					}
				}
			}

		++iCurrentFolder;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	else
		{
		// All the folders have been done. We now need to remove any folders
		// that we have in the message store, that are no longer on the server.
		RemoveMissingFolders();

		delete iSearchFolder;
		iSearchFolder = NULL;
		
		delete iInboxPathWithSeparator;
		iInboxPathWithSeparator = NULL;
		
		delete iFolderPathWithSeparator;
		iFolderPathWithSeparator = NULL;
		}
	}

/**
Add a new folder that is on the server but which we don't have stored locally

@param aCurrentFolderInfo Received folder info of new folder
@param aCurrentFolderFullPath Full path of new folder
*/
void CImapOpSyncFolderTree::AddReceivedFolderL(CImapListFolderInfo* aCurrentFolderInfo, const TDesC& aCurrentFolderFullPath)
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::AddReceivedFolderL");

	CImapFolder* parentFolder = NULL;
	TBool rootFolder = EFalse;
	TPtrC parentFolderFullPath = aCurrentFolderFullPath;
	TPtrC folderName = aCurrentFolderFullPath;
	TLinearOrder<CImapFolder> sorter(CImapFolder::CompareByFolderName);

	// Try to split the folder path into the folder name and the path of
	// the parent folder by searching backwards from the end for the
	// hierarchy separator character
	TInt sepPos = parentFolderFullPath.LocateReverseF(aCurrentFolderInfo->iHierarchySeperator);

	if (sepPos != KErrNotFound)
		{
		// We found the hierarchy separator. Split the path into the folder name
		// and the parent path
		parentFolderFullPath.Set(parentFolderFullPath.Left(sepPos));
		folderName.Set(folderName.Mid(sepPos + 1));

#ifdef __IMAP_LOGGING
		HBufC8* tempBuf = HBufC8::NewL(parentFolderFullPath.Length());
		TPtr8 tempBufPtr = tempBuf->Des();
		tempBufPtr.Copy(parentFolderFullPath);
		HBufC8* tempBuf2 = HBufC8::NewL(folderName.Length());
		TPtr8 tempBufPtr2 = tempBuf2->Des();
		tempBufPtr2.Copy(folderName);
		__LOG_FORMAT((iSession.LogId(), "Split path into %S and %S", &tempBufPtr, &tempBufPtr2));
		delete tempBuf;
		delete tempBuf2;
#endif

		// Check if this is directly under inbox
		if (parentFolderFullPath.CompareF(KImapInbox) == 0)
			{
			__LOG_TEXT(iSession.LogId(), "Parent is the inbox");
			parentFolder = iSyncMan.Inbox();
			}
		// Check if the parent path is the same as the settings folder path.
		// If so this is a root folder
		else if ((iFolderPath.Length() > 0) &&
		         (parentFolderFullPath.Compare(iFolderPath) == 0))
			{
			__LOG_TEXT(iSession.LogId(), "Parent is settings folder path");
			rootFolder = ETrue;
			}
		else
			{
			// Search for the parent folder in the folder list.
			// Note that the received list of folders from the server is in
			// alphabetical order, so the parent folder will have already
			// been created so we should be able to find it.
			iSearchFolder->SetFullFolderPathL(parentFolderFullPath);
			TLinearOrder<CImapFolder> sorter(CImapFolder::CompareByFolderName);
			TInt parentPos = iFolderList.FindInOrder(iSearchFolder, sorter);

			if (parentPos != KErrNotFound)
				{
				__LOG_FORMAT((iSession.LogId(), "Parent found at position %d", parentPos));
				parentFolder = iFolderList[parentPos];
				}
			}
		}
	else
		{
		// No hierarchy separator so this must be a root folder
		folderName.Set(aCurrentFolderFullPath);
		rootFolder = ETrue;
		}

	if (rootFolder || parentFolder)
		{
		if (rootFolder)
			{
			User::LeaveIfError(iServerEntry.SetEntry(iImapSettings.ServiceId()));
			}
		else
			{
			User::LeaveIfError(iServerEntry.SetEntry(parentFolder->MailboxId()));
			}

		TMsvEmailEntry entry;
		entry.iDetails.Set(folderName);
		entry.iType = KUidMsvFolderEntry;
		entry.iMtm = KUidMsgTypeIMAP4;
		entry.iServiceId = iImapSettings.ServiceId();
		entry.iSize = 0;
		entry.SetUID(0);
		entry.SetValidUID(EFalse);
		entry.SetMailbox(!(aCurrentFolderInfo->QueryFlag(CImapListFolderInfo::ENoselect)));
		entry.SetLocalSubscription(EFalse);
		entry.SetComplete(ETrue);
		entry.SetVisible(EFalse);

		User::LeaveIfError(iServerEntry.CreateEntry(entry));

		CImapFolder* tempFolder = CImapFolder::NewLC(iSyncMan, iServerEntry, entry, iImapSettings, aCurrentFolderInfo->FolderName());
		tempFolder->SetFolderMatched(ETrue);
		iFolderList.InsertInOrderL(tempFolder, sorter);
		CleanupStack::Pop(tempFolder);

		__LOG_TEXT(iSession.LogId(), "Added folder");
		}
	}

/**
Remove any local folders that are no longer on the server
*/
void CImapOpSyncFolderTree::RemoveMissingFolders()
	{
	__LOG_TEXT(iSession.LogId(), "CImapOpSyncFolderTree::RemoveMissingFolders");

	// Remove in reverse order to ensure that children folders are removed
	// before their parent
	for (TInt folderLoop = iFolderList.Count() - 1; folderLoop >= 0; --folderLoop)
		{
		if (!iFolderList[folderLoop]->FolderMatched())
			{
#ifdef __IMAP_LOGGING
			HBufC8* tempBuf = HBufC8::NewL(iFolderList[folderLoop]->FullFolderPathL().Length());
			TPtr8 tempBufPtr = tempBuf->Des();
			tempBufPtr.Copy(iFolderList[folderLoop]->FullFolderPathL());
			__LOG_FORMAT((iSession.LogId(), "Removing folder %S", &tempBufPtr));
			delete tempBuf;
#endif

			if (iServerEntry.SetEntry(iFolderList[folderLoop]->MailboxId()) == KErrNone)
				{
				if (iServerEntry.SetEntry(iServerEntry.Entry().Parent()) == KErrNone)
					{
					iServerEntry.DeleteEntry(iFolderList[folderLoop]->MailboxId());
					}
				}

			delete iFolderList[folderLoop];
			iFolderList.Remove(folderLoop);
			}
		}
	}
