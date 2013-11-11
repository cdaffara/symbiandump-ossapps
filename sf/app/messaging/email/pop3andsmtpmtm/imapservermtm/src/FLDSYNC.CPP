// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// IMAP4 folder list synchronise
// This class deals with keeping the local mirror folder tree in sync with the
// remote server's directory structure.
// 980914 - Modifications to local tree traverse so that it will not
// look for children below message level: previously, it was
// picking up attachment folders in messages!
// 990304 - Orphaned folder check no longer orphans the inbox: we
// basically never touch it. The problem with orphaning it
// comes about with servers which have a namespace (ie
// folder path) of INBOX, which means we never "see" the
// actual inbox as we are inside it. We still know it
// exists, though.
// 990502 - Was calling CMsgActive::Cancel() as opposed to DoCancel().
// 
//

#include <e32base.h>
#include <e32cons.h>
#include <mentact.h>
#include "fldsync.h"
#include "impspan.h"
#include "imapsess.h"

#ifdef _DEBUG
#define LOG_COMMANDS(a) a
#define DBG(a) a
#define PRINTING
#else
#define LOG_COMMANDS(a)
#define DBG(a)
#undef PRINTING
#endif

// Priority of MsgActive object
const TInt EFolderSyncPriority=1;


// Debugging of folder tree traversal
#undef DEBUG_FOLDERSYNC

CImImap4FolderSync::CImImap4FolderSync():CMsgActive(EFolderSyncPriority)
	{
	__DECLARE_NAME(_S("CImImap4FolderSync"));
	}

CImImap4FolderSync::~CImImap4FolderSync()
	{
	// Global bits
	delete iFolderContents;

	if (iFolderList)
		iFolderList->ResetAndDestroy();
	delete iFolderList;

	// Get rid of bits at each level
	for(TInt a=0;a<KFolderDepth;a++)
		delete iFolderIds[a];

	// Delete local folder list
	delete iLocalFolders;
	}

CImImap4FolderSync* CImImap4FolderSync::NewLC(CImImap4Session *aSession)
	{
	CImImap4FolderSync* self=new (ELeave) CImImap4FolderSync();
	CleanupStack::PushL(self);

	// Non-trivial constructor
	self->ConstructL(aSession);
	return self;
	}

CImImap4FolderSync* CImImap4FolderSync::NewL(CImImap4Session *aSession)
	{
	CImImap4FolderSync* self=NewLC(aSession);
	CleanupStack::Pop();
	return self;
	}

// The non-trivial constructor
void CImImap4FolderSync::ConstructL(CImImap4Session *aSession)
	{
	// Save session
	iSession=aSession;

	// We're an active object...
	CActiveScheduler::Add(this);

	// One-off bits
	iFolderList=new (ELeave) CArrayPtrFlat<CImImap4DirStruct>(8);
	iFolderContents=new (ELeave) CMsvEntrySelection;

	// ...and per-level bits
	for(TInt a=0;a<KFolderDepth;a++)
		iFolderIds[a]=new (ELeave) CArrayFixFlat<TMsvId>(8);

	// Local folder list
	iLocalFolders=new (ELeave) CArrayFixFlat<TMsvId>(16);
	}

// Do setentry, leave if there is an error
void CImImap4FolderSync::SetEntryL(const TMsvId aId)
	{
	User::LeaveIfError(iEntry->SetEntry(aId));
	}

// Change entry, leave if error
void CImImap4FolderSync::ChangeEntryL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iEntry->ChangeEntry(aEntry));
	}

// Change entry in bulk mode (i.e. no index file commit), leave if error
void CImImap4FolderSync::ChangeEntryBulkL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iEntry->ChangeEntryBulk(aEntry));
	}
// Get children, leave if error
void CImImap4FolderSync::GetChildrenL(CMsvEntrySelection& aSelection)
	{
	User::LeaveIfError(iEntry->GetChildren(aSelection));
	}

// Set the entry to use to talk to the server
void CImImap4FolderSync::SetEntry(CMsvServerEntry* aEntry)
	{
	// Save it
	iEntry=aEntry;
	}

// Build list of local folders
void CImImap4FolderSync::BuildLocalL(const TMsvId aFolder, const TBool aDoThisOne)
	{
	// Select it
	SetEntryL(aFolder);
	TMsvEmailEntry entry=iEntry->Entry();

	// If we're a folder, set the flag
	if (entry.iType==KUidMsvFolderEntry && aDoThisOne)
		{
		// Add to list
		iLocalFolders->AppendL(iEntry->Entry().Id());
		}

	// If current entry is a message, don't look for children
	if (entry.iType==KUidMsvMessageEntry)
		return;

	// Any children?
	CMsvEntrySelection *children=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);
	GetChildrenL(*children);
	if (children->Count())
		{
		// Do each in turn
		for(TInt child=0;child<children->Count();child++)
			BuildLocalL((*children)[child],ETrue);
		}
	CleanupStack::PopAndDestroy();
	}

// Called when async child completes
void CImImap4FolderSync::DoRunL()
	{
	if (ProcessDirListL())
		{
		// Done whole list, check to see what has vanished on the
		// server with regard to what's in the mirror: basically,
		// see what's still flagged in the tree.
		OrphanedFolderCheckL();

		// All done!
		Complete(KErrNone);
		}
	else
		{
		// Otherwise, another command has been issued
		SetActive();
		}
	}

// Cancel this operation
void CImImap4FolderSync::DoCancel()
	{
	DBG((iSession->LogText(_L8("CImImap4FolderSync::DoCancel()"))));

	// Cancel any outstanding session operation
	iSession->Cancel();

	// ...and parent
	CMsgActive::DoCancel();
	}

// Check for orphaned folders
void CImImap4FolderSync::OrphanedFolderCheckL()
	{
	DBG((iSession->LogText(_L8("CImImap4FolderSync::OrphanedFolderCheckL()"))));

	// Set stats
	iOrphanedFolders=iLocalFolders->Count();

	// Selection for storing children
	CMsvEntrySelection *children=new CMsvEntrySelection;
	CleanupStack::PushL(children);

	// Any folders left in iLocalFolders are orphans
	for(TInt a=0;a<iLocalFolders->Count();a++)
		{
		// Go to this folder
		if (iEntry->SetEntry((*iLocalFolders)[a])!=KErrNone)
			{
			// Probably been removed already (nested?)
			continue;
			}
		TMsvEmailEntry entry=iEntry->Entry();

		// Is it the INBOX? If so, ignore it
		if (entry.Parent()==iServiceId &&
			entry.iDetails.CompareF(KIMAP_INBOX)==0)
			continue;

		// Check all the entries in the folder
		TInt subfolders=0;
		children->Reset();
		GetChildrenL(*children);
		if (children->Count())
			{
			// Check each message
			for(TInt a=0;a<children->Count();a++)
				{
				SetEntryL((*children)[a]);
				TMsvEmailEntry child=iEntry->Entry();
				if (child.iType==KUidMsvMessageEntry)
					{
					// Orphan this message: it may delete it, if there
					// are no downloaded bodyparts

					DBG((iSession->LogText(_L8("Orphaning message %x"),(*children)[a])));

					iSession->OrphanMessageL((*children)[a]);
					}
				else if (child.iType==KUidMsvFolderEntry)
					{
					// There's a subfolder
					subfolders++;
					}
				}
			}

		// Check number of children again: if all the orphaned children
		// have been deleted (ie, they had no downloaded body parts)
		// then we can delete the folder locally, otherwise we just have
		// to mark it as orphaned.
		SetEntryL((*iLocalFolders)[a]);
		GetChildrenL(*children);
		if ((children->Count() || subfolders>0) && !entry.Orphan())
			{

			DBG((iSession->LogText(_L8("Orphaning folder %x"),(*iLocalFolders)[a])));

			// Orphan folder
			TMsvEmailEntry message=iEntry->Entry();
			message.SetOrphan(ETrue);
			ChangeEntryL(message);
			}
		else
			{

			DBG((iSession->LogText(_L8("Deleting folder %x"),(*iLocalFolders)[a])));

			// Delete folder
			SetEntryL(iEntry->Entry().Parent());
			iEntry->DeleteEntry((*iLocalFolders)[a]);
			}
		}

	// Get rid of children list
	CleanupStack::PopAndDestroy();

	// Remove list
	iLocalFolders->Reset();

	DBG((iSession->LogText(_L8("CImImap4FolderSync::OrphanedFolderCheckL() done"))));
	}

// Process returned directory list
TBool CImImap4FolderSync::ProcessDirListL()
	{
	// We've got a list, and the mirror folder ID that the contents belong to:
	// process it, and mirror the folder structure

	// First, find the existing children of this folder from the message server
	SetEntryL(iFolderId);
	GetChildrenL(*iFolderContents);
	TInt noofchildren=iFolderContents->Count();

	// Number of items in reply from server
	TInt noofreplies=iFolderList->Count();

#ifdef DEBUG_FOLDERSYNC
	iSession->LogText(_L("Level %d: Server has %d entries, mirror has %d"),
					  iFolderLevel,noofreplies,noofchildren);
#endif

	// Empty the 'to do' array
	iFolderIds[iFolderLevel]->Reset();

	// ...for each entry in reply
	for(TInt replyentry=0;replyentry<noofreplies;replyentry++)
		{
		// Does this already exist?
		TInt a;
		TMsvId thisentry;
		for(a=0;a<noofchildren;a++)
			{
			// See if the details field matches the server folder name.
			// For the INBOX folder the match should be case insensitive, all other folders
			// we must perform a case sensitive match
			SetEntryL(thisentry=(*iFolderContents)[a]);
			TPtrC entryName = iEntry->Entry().iDetails;
			TPtrC svrFolderName = (*iFolderList)[replyentry]->Leafname();
			TBool foldersMatch = EFalse;
			if(svrFolderName.CompareF(KIMAP_INBOX)==0)
				{
				// Server folder is the INBOX
				foldersMatch = (entryName.CompareF(svrFolderName)==0);
				}
			else
				{
				// Server folder is not the INBOX
				foldersMatch = (entryName.Compare(svrFolderName)==0);
				}
				
			if (foldersMatch)
				{
				// Update mailbox flag as necessary, but NOT if it's the inbox, which is
				// *ALWAYS* a mailbox
				TMsvEmailEntry folder=iEntry->Entry();
				if ((*iFolderList)[replyentry]->iIsMailbox!=folder.Mailbox())
					{
					if (folder.Parent()!=iServiceId ||
					    folder.iDetails.CompareF(KIMAP_INBOX)!=0)
						{
						folder.SetMailbox((*iFolderList)[replyentry]->iIsMailbox);
						ChangeEntryBulkL(folder);
						}
					else
						{
						DBG((iSession->LogText(_L8("Skipping Mailbox flag twiddle for folder %x (=%d)"),
						  folder.Id(),(*iFolderList)[replyentry]->iIsMailbox)));
						}
					}

				// Exists: remove from local folders list
				for(TInt b=0;b<iLocalFolders->Count();b++)
					{
					// Found it?
					if ((*iLocalFolders)[b]==thisentry)
						{
						// Remove from list
						iLocalFolders->Delete(b,1);
						break;
						}
					}

				// Exit loop
				break;
				}
			}

		// Found a match?
		if (a==noofchildren)
			{
			// No, create it
			TMsvEmailEntry message;
			
			// All flags unset (urrgh!)
			message.SetMtmData1(0);
			message.SetMtmData2(0);
			message.SetMtmData3(0);

			message.iType=KUidMsvFolderEntry;
			message.iMtm=iEntry->Entry().iMtm;
			message.iServiceId=iServiceId;
			message.iSize=0;
			message.iDetails.Set((*iFolderList)[replyentry]->Leafname());
			message.SetValidUID(EFalse);
			message.SetComplete(ETrue);

			// Visibility
			if (iNewFoldersAreInvisible)
				message.SetVisible(EFalse);

			// Is a mailbox? (basically, is it selectable?).
			message.SetMailbox((*iFolderList)[replyentry]->iIsMailbox);

#ifdef DEBUG_FOLDERSYNC
			iSession->LogText(_L("Creating folder '%S'"),&message.iDetails);
#endif
			SetEntryL(iFolderId);
			User::LeaveIfError(iEntry->CreateEntryBulk(message));

			// Increment stats
			iNewFolders++;
			
			// Get the ID, incase we're going into this level
			thisentry=message.Id();
			}

		// Does this have children? (and are we at the end of our nesting levels?)
		if ((*iFolderList)[replyentry]->iIsFolder && iFolderLevel<(KFolderDepth-1))
			{
#ifdef DEBUG_FOLDERSYNC
			iSession->LogText(_L("Adding folder to todolist, level %d"),iFolderLevel);
#endif
			// Add it to the list to scan at this level
			iFolderIds[iFolderLevel]->AppendL(thisentry);
			}
		}

#ifdef DEBUG_FOLDERSYNC
	iSession->LogText(_L("Level %d: %d items in todolist"),iFolderLevel,iFolderIds[iFolderLevel]->Count());
#endif

	// Anything in our 'to do' queue?
	while(!iFolderIds[iFolderLevel]->Count())
		{
#ifdef DEBUG_FOLDERSYNC
		iSession->LogText(_L("Nothing to do at level %d"),iFolderLevel);
#endif
		// Nothing at this level: are we at the top?
		if (iFolderLevel==0)
			{
#ifdef DEBUG_FOLDERSYNC
			iSession->LogText(_L("At level 0: completing"));
#endif
			// All done!
			// Commit any outstanding bulk creates
			SetEntryL(iFolderId);
			iEntry->CompleteBulk();
			return(ETrue);
			}

#ifdef DEBUG_FOLDERSYNC
		iSession->LogText(_L("Going up a level"));
#endif
		// Not at the top: back up a level
		iFolderLevel--;
		}

	// Something to process: remove it from todo list
	iFolderId=(*iFolderIds[iFolderLevel])[0];
	iFolderIds[iFolderLevel]->Delete(0,1);

	// Nest & issue command
	iFolderLevel++;
	iSession->ListL(iStatus,iFolderId,iFolderList);

#ifdef DEBUG_FOLDERSYNC
	iSession->LogText(_L("Stuff to do at level %d - scanning dir %x"),iFolderLevel,iFolderId);
#endif
	return(EFalse);
	}

// Synchronise mirror tree with remote server
void CImImap4FolderSync::SynchroniseTreeL(TRequestStatus& aStatus, const TMsvId aService, const TBool aNewFoldersAreInvisible)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMMAND CImImap4FolderSync::SychroniseTree (serviceid=%x)"),aService)));

	Queue(aStatus);

	// Note where this list came from
	iServiceId=aService;
	iFolderId=iServiceId;
	iFolderLevel=0;

	// Save invisibility state
	iNewFoldersAreInvisible=aNewFoldersAreInvisible;

	// Set flags on all folders, so we can tell which ones aren't matched by the
	// list returned from the server.
	iLocalFolders->Reset();
	BuildLocalL(iServiceId,EFalse);

	// List this tree
	iSession->ListL(iStatus,iFolderId,iFolderList);
	SetActive();
	}

void CImImap4FolderSync::IncProgress(TImap4SyncProgress& aProgress)
	{
	aProgress.iNewFolders+=iNewFolders;
	aProgress.iOrphanedFolders+=iOrphanedFolders;
	}

void CImImap4FolderSync::ResetStats()
	{
	// initialise counts
	iNewFolders=0;
	iOrphanedFolders=0;
	}	
