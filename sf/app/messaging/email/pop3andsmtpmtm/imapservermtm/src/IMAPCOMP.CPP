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
// IMAP4 compound operations
// This class wraps up operations which require more than one IMAP command
// (the imapsess.cpp class deals with single IMAP commands generally),
// allowing increased neatness in the higher classes.
// 
//

#include <e32base.h>
#include <e32cons.h>
#include <mentact.h>
#include "impspan.h"
#include "imapsess.h"
#include <imapset.h>
#include "imapcomp.h"

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
const TInt ECompoundPriority=1;


// Operations that can be chained
enum
	{
	EFinished=0,
	ESelectSourceMailboxRW,
	ESelectSourceMailboxRO,
	ESelectDestinationMailboxRW,
	ESelectDestinationMailboxRO,
	EFetchMessage,
	ECopyMessage,
	EAppendMessage,
	EDeleteMessage,
	EDeleteLocalMessage,
	EDeleteAllMessages,
	ENewSyncFolder,
	ESyncFolder,
	EInboxDuplicateMove,
	EInboxDuplicateCopy,
	ECloseFolderWithoutExpunge,
	EDeleteFolder,
	EStartIdle,
	EStopIdle,
	ECreate,
	ERename,
	ESelectInbox
	};

//DS - Split EInboxDuplicate into EInboxDuplicateCopy and EInboxDuplicateMove to allow
//use of either CMsvServerEntry::CopyL or MoveL depending on what operation the user
//is attempting.

CImImap4Compound::CImImap4Compound():CMsgActive(ECompoundPriority)
	{
	__DECLARE_NAME(_S("CImImap4Compound"));
	}

CImImap4Compound::~CImImap4Compound()
	{
	// We don't delete iSession as we don't own it, we were just passed it to use.
	delete iSourceSel;
	}

CImImap4Compound* CImImap4Compound::NewL(CImImap4Session* aSession)
	{
	CImImap4Compound* self=new (ELeave) CImImap4Compound();
	CleanupStack::PushL(self);

	// Non-trivial constructor
	self->ConstructL(aSession);
	CleanupStack::Pop();
	return self;
	}

// The non-trivial constructor
void CImImap4Compound::ConstructL(CImImap4Session* aSession)
	{
	// Save session & entry
	iSession=aSession;
	
	iPopulateCommand=EFalse;
	iIdleBeforeFirstPopulate = EFalse;
	// We're an active object...
	CActiveScheduler::Add(this);

	// Initialise static sequences. There must be a better way
	// than this without causing complaints :-(
	SeqCopyToLocal[0]=EStopIdle;
	SeqCopyToLocal[1]=ESelectSourceMailboxRO;
	SeqCopyToLocal[2]=EFetchMessage;
	SeqCopyToLocal[3]=EInboxDuplicateCopy;
	SeqCopyToLocal[4]=ESelectInbox;
	SeqCopyToLocal[5]=EStartIdle;
	SeqCopyToLocal[6]=EFinished;

	SeqMoveToLocal[0]=EStopIdle;
	SeqMoveToLocal[1]=ESelectSourceMailboxRW;
	SeqMoveToLocal[2]=EFetchMessage;
	SeqMoveToLocal[3]=EInboxDuplicateMove;
	SeqMoveToLocal[4]=EDeleteMessage;
	SeqMoveToLocal[5]=ESelectInbox;
	SeqMoveToLocal[6]=EStartIdle;
	SeqMoveToLocal[7]=EFinished;

	SeqCopyWithinService[0]=EStopIdle;
	SeqCopyWithinService[1]=ESelectSourceMailboxRO;
	SeqCopyWithinService[2]=ECopyMessage;
	SeqCopyWithinService[3]=ESelectDestinationMailboxRO;
	SeqCopyWithinService[4]=ENewSyncFolder;
	SeqCopyWithinService[5]=ESelectInbox;
	SeqCopyWithinService[6]=EStartIdle;
	SeqCopyWithinService[7]=EFinished;

	SeqMoveWithinService[0]=EStopIdle;
	SeqMoveWithinService[1]=ESelectSourceMailboxRW;
	SeqMoveWithinService[2]=ECopyMessage;
	SeqMoveWithinService[3]=EDeleteMessage;
	SeqMoveWithinService[4]=ESelectDestinationMailboxRO;
	SeqMoveWithinService[5]=ENewSyncFolder;
	SeqMoveWithinService[6]=ESelectInbox;
	SeqMoveWithinService[7]=EStartIdle;
	SeqMoveWithinService[8]=EFinished;

	SeqCopyFromLocal[0]=EAppendMessage;
	SeqCopyFromLocal[1]=ESelectDestinationMailboxRO;
	SeqCopyFromLocal[2]=ENewSyncFolder;
	SeqCopyFromLocal[3]=EFinished;

	SeqMoveFromLocal[0]=EStopIdle;
	SeqMoveFromLocal[1]=EAppendMessage;
	SeqMoveFromLocal[2]=EDeleteLocalMessage;
	SeqMoveFromLocal[3]=ESelectDestinationMailboxRO;
	SeqMoveFromLocal[4]=ENewSyncFolder;
	SeqMoveFromLocal[5]=ESelectInbox;
	SeqMoveFromLocal[6]=EStartIdle;
	SeqMoveFromLocal[7]=EFinished;

	SeqDelete[0]=EStopIdle;
	SeqDelete[1]=ESelectSourceMailboxRW;
	SeqDelete[2]=EDeleteMessage; // Explicit expunge
	SeqDelete[3]=ESelectInbox;
	SeqDelete[4]=EStartIdle;
	SeqDelete[5]=EFinished;

	SeqDeleteFolder[0]=EStopIdle;
	SeqDeleteFolder[1]=ESelectSourceMailboxRW;
	SeqDeleteFolder[2]=EDeleteAllMessages;
	SeqDeleteFolder[3]=ECloseFolderWithoutExpunge;
	SeqDeleteFolder[4]=EDeleteFolder;
	SeqDeleteFolder[5]=ESelectInbox;
	SeqDeleteFolder[6]=EStartIdle;
	SeqDeleteFolder[7]=EFinished;

	SeqNewSync[0]=EStopIdle;
	SeqNewSync[1]=ESelectSourceMailboxRW;
	SeqNewSync[2]=ENewSyncFolder;
	SeqNewSync[3]=ESelectInbox;
	SeqNewSync[4]=EStartIdle;
	SeqNewSync[5]=EFinished;

	SeqFullSync[0]=EStopIdle;
	SeqFullSync[1]=ESelectSourceMailboxRW;
	SeqFullSync[2]=ESyncFolder;
	SeqFullSync[3]=ESelectInbox;
	SeqFullSync[4]=EStartIdle;
	SeqFullSync[5]=EFinished;

	SeqSelect[0]=EStopIdle;
	SeqSelect[1]=ESelectSourceMailboxRW;
	SeqSelect[2]=EFinished;

	SeqSynchronise[0]=EStopIdle;
	SeqSynchronise[1]=ESyncFolder;
	SeqSynchronise[2]=ESelectInbox;
	SeqSynchronise[3]=EStartIdle;
	SeqSynchronise[4]=EFinished;

	SeqCreate[0]=EStopIdle;
	SeqCreate[1]=ECreate;
	SeqCreate[2]=ESelectInbox;
	SeqCreate[3]=EStartIdle;
	SeqCreate[4]=EFinished;

	SeqRename[0]=EStopIdle;
	SeqRename[1]=ERename;
	SeqRename[2]=ESelectInbox;
	SeqRename[3]=EStartIdle;
	SeqRename[4]=EFinished;

	// operations when done as part of a synchronisation process
	SeqSyncCopyToLocal[0]=EFetchMessage;
	SeqSyncCopyToLocal[1]=EInboxDuplicateCopy;
	SeqSyncCopyToLocal[2]=EFinished;
	}

// Set a new session
void CImImap4Compound::SetSession(CImImap4Session* aSession)
	{
	iSession=aSession;
	}

// Set entry for access to server's database
void CImImap4Compound::SetEntry(CMsvServerEntry* aEntry)
	{
	iEntry=aEntry;
	}

// Do setentry, leave if there is an error
void CImImap4Compound::SetEntryL(const TMsvId aId)
	{
#ifdef PRINTING
	TInt error=iEntry->SetEntry(aId);
	if (error)
		iSession->LogText(_L8("SetEntryL(%x) returned %d"),aId,error);
	User::LeaveIfError(error);
#else
	User::LeaveIfError(iEntry->SetEntry(aId));
#endif
	}


// Called when async child completes
void CImImap4Compound::DoRunL()
	{
	DBG((iSession->LogText(_L8("COMPOUND dorunl, step %d, status=%d"),iStep,iStatus.Int())));

	// Are we still connected? Worth checking...
	if (!iSession->Connected())
		{
		Complete(KErrDisconnected);
		return;
		}

	// Any error from last operation?
	if (iStep>0 && iStatus.Int()!=KErrNone)
		{
		switch(iSequence[iStep-1])
			{
		// If we get KErrIMAPNO from a selection state, we need to report this as KErrNotFound
		case ESelectSourceMailboxRW:
		case ESelectSourceMailboxRO:
		case ESelectDestinationMailboxRW:
		case ESelectDestinationMailboxRO:
		case ESelectInbox:
			if (iStatus.Int()==KErrIMAPNO)
				{
				Complete(KErrNotFound);
				break;
				}

			// Fall through

		default:
			// Complete with this error
			Complete(iStatus.Int());
			break;
			}

		// Reset step (shouldn't really be necessary, but it looks neat!)
		iStep=0;
		return;
		}

	while (!IsActive() && !DoRunLoopL())
		{
		// do nothing in the body of this
		}
	}

// Called when async child completes
TBool CImImap4Compound::DoRunLoopL()
	{
        DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): step to execute: %d"), iSequence[iStep])));

	switch(iSequence[iStep++])
		{
	case EFinished:
		// All done
		DBG((iSession->LogText(_L8("COMPOUND finished"))));

		Complete(KErrNone);
		iStep=0;
		return ETrue;

	case ESelectSourceMailboxRW:
		iSession->Select(iStatus,iSourceFolder,ETrue);
		break;

	case ESelectSourceMailboxRO:
		iSession->Select(iStatus,iSourceFolder,EFalse);
		break;

	case ESelectDestinationMailboxRW:
		iSession->Select(iStatus,iDestinationFolder,ETrue);
		break;

	case ESelectDestinationMailboxRO:
		iSession->Select(iStatus,iDestinationFolder,EFalse);
		break;

	case ESelectInbox:
		{
		// Selecting inbox before starting IDLE command
		if (iSession->ImapIdleSupported()==EFalse || iIdleBeforeCommand==EFalse || iIdleBeforeFirstPopulate == EFalse || iMsgCount > 0)
			{
			return EFalse;
			}

		TMsvId inbox = iSession->GetInbox();
                DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): selecting inbox"))));
		iSession->Select(iStatus,inbox,ETrue);
		break;
		}

	case ECreate:
		iSession->Create(iStatus,iSource,iLeafName,iFolder);
		break;

	case ERename:
		iSession->Rename(iStatus,iSource,iLeafName);
		break;

	case EFetchMessage:
		{
		// Don't fetch the message if it is marked for delete
		SetEntryL(iSource);

		TMsvEmailEntry entry = static_cast<TMsvEmailEntry> (iEntry->Entry()); 
		
		if (((TMsvEmailEntry)iEntry->Entry()).DisconnectedOperation() == EDisconnectedDeleteOperation)
			return EFalse;

		// Don't attempt to fetch the message if it has either been fully or partially downloaded, unless this is a populate command.
        if ( entry.Complete() && entry.PartialDownloaded() && !iPopulateCommand )
   			return EFalse;

		// We fetch to the mirror in all cases
                DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): fetching message"))));
		iSession->FetchBody(iStatus,iSource,iGetPartialMailInfo);
		
		break;
		}

	case ECopyMessage:
		if (iSequence==SeqCopyWithinService || iSequence==SeqMoveWithinService)
			{
			if (iSelectionStillToCopy-->0) 
				{
				 // Copy all messages before moving onto next step.
				iSession->Copy(iStatus,(*iSourceSel)[iSelectionStillToCopy],iDestinationFolder,ETrue);
				if (iSelectionStillToCopy>0)
					iStep--; // still copying.
				}
			}
		else
			iSession->Copy(iStatus,iSource,iDestinationFolder,ETrue);
		break;

	case EAppendMessage:
		iSession->Append(iStatus,iSource,iDestinationFolder);
		break;

	case EDeleteMessage:
		// Deletes a message remotely
		if (iSequence==SeqMoveWithinService && iSession->CommandFailure()==KErrIMAPNO)
			{
			DBG((iSession->LogText(_L8("Copy failed, Cancelling Delete"))));
			// Copy failed, so do not continue with delete.
			return EFalse;
			}
		
		if (iSelectionStillToDelete>1)
			iSession->Delete(iStatus,*iSourceSel);
		else
			iSession->Delete(iStatus,iSource);
		iSelectionStillToDelete=0;
		break;

	case EDeleteLocalMessage:
		// Deletes a message locally: only for messages not within the service
		iSession->DeleteMessageL(iSource);

		// As this is an instant operation, loop
		return EFalse;

	case EDeleteAllMessages:
		// Deletes all messages/folder (must have no subfolders)
		iSession->DeleteAllMessagesL(iStatus);
		break;

	case EDeleteFolder:
		// Delete the folder
		iSession->Delete(iStatus,iSourceFolder);
		break;

	case ENewSyncFolder:
		// New only sync.

		// if the current folder hasn't actually changed then don't
		// bother with the Sync
		if (!iSession->FolderChanged())
			return EFalse;
		
		// This used to do just that, new only messages, but it causes a lot of problems 
		// with sync limits, as for that to work deletes need to be taken into account also.
		// This will fix that.
                DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): ESyncNewFolder: synchronising"))));
		iSession->Synchronise(iStatus,EFalse);
		break;

	case ESyncFolder:
		// Full sync
                DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): ESyncFolder: synchronising"))));
		iSession->Synchronise(iStatus,EFalse);
		break;

	case ECloseFolderWithoutExpunge:
		// Close current folder
		iSession->Close(iStatus,EFalse);
		break;

	case EInboxDuplicateCopy:
	case EInboxDuplicateMove:
		{
		// SJM: if we asked to fetch to the mirror then we can skip
		// this stage
		if (iDestinationFolder == KMsvNullIndexEntryId)
			{
			return EFalse;
			}
		
		// Move the message to the destination (the local inbox) then duplicate it
		// back to the original folder for the mirror.
		iEntry->SetEntry(iSource);
		TMsvId sourcefolder=iEntry->Entry().Parent();

		// Do the move & copy back structure: we update iSource here as the message
		// in the mirror is actually a recreated one with a new TMsvId. We may want
		// to delete this (if we're doing a MoveToLocal, for example) and so we need
		// the correct TMsvId
		
#if 1
		// always copy so that on a move the delete happens on the original correctly
		iSession->CopyMessage(iStatus,sourcefolder,iSource,iDestinationFolder,
							  &iSource,EFalse);
#else
		//DS - Added the iSequence[iStep]==EInboxDuplicateMove to represent whether or not
		//to remove the original (i.e. copy or move).
		iSession->CopyMessage(iStatus,sourcefolder,iSource,iDestinationFolder,
							  &iSource,iSequence[iStep-1]==EInboxDuplicateMove);
#endif
		break;
		}

	case EStartIdle:
		 // if there are more messages(iMsgCount) to populate don't go to IDLE state.
		if (iSession->ImapIdleSupported()==EFalse || iIdleBeforeCommand==EFalse || iIdleBeforeFirstPopulate == EFalse || iMsgCount > 0)
			{
			return EFalse;
			}
                DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): EStartIdle: calling StartIdle()"))));
        iIdleBeforeFirstPopulate = EFalse;
		iSession->StartIdle(iStatus);
		break;

	case EStopIdle:
		if (iSession->ImapIdleSupported()==EFalse || iSession->IsIdling()==EFalse)
			{
                        DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): EStopIdle: not idling or waitng for idle"))));
			iIdleBeforeCommand = EFalse;
		
			// If last message being fetched , then should start IDLE after that.
			if (iMsgCount == 1 )
				{
				iIdleBeforeCommand = ETrue;	
				}

			return EFalse;
			}
		
 		iIdleBeforeFirstPopulate = ETrue;//Are we "IDLE" before first populate..
		iIdleBeforeCommand = ETrue;
                DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): EStopIdle: Calling StopIdle()"))));
		iSession->StopIdle(iStatus);
		break;

	default:
		gPanic(EUnknownState);
		return ETrue;
		}

	// Next step in sequence...
        DBG((iSession->LogText(_L8("CImImap4Compound::DoRunLoopL(): Setting active"))));
	SetActive();
	return EFalse;
	}

// Called when parent wants to cancel us
void CImImap4Compound::DoCancel()
	{
	DBG((iSession->LogText(_L8("CImImap4Compound::DoCancel()"))));

	// Cancel any session object we're using
	switch( iSequence[iStep-1] )
		{
	case EStopIdle:
	case ESelectSourceMailboxRO:
	case EFetchMessage:
	case EInboxDuplicateCopy:
	case ESelectInbox:
	case EStartIdle:
		{
		TRAP_IGNORE(iSession->CancelAndIdleL(iIdleBeforeFirstPopulate));
		} break;
		
	default:
		iSession->Cancel();
		}

	iIdleBeforeFirstPopulate = EFalse;
	iMsgCount = 0;
	// ...and parent
	CMsgActive::DoCancel();
	}

// Store an offline operation in the correct place
TMsvId CImImap4Compound::FindFolderL(const TMsvId aMessage)
	{
	DBG((iSession->LogText(_L8("FindFolderL(%x)"),aMessage)));

	// Find folder that encloses this message (has Mailbox flag set), or service,
	// whichever we find first
	SetEntryL(aMessage);
	TMsvEmailEntry entry;
	do
		{
		// Visit this entry
		SetEntryL(iEntry->Entry().Parent());
		entry=iEntry->Entry();

		DBG((iSession->LogText(_L8("  At %x, type=%x, mailbox=%d"),entry.Id(),entry.iType,entry.Mailbox())));

		// A folder & a mailbox, or a service?
		if (entry.iType==KUidMsvFolderEntry &&
			entry.Mailbox())
			{
			// This'll do!
			return(entry.Id());
			}
		}
	while(iEntry->Entry().iType!=KUidMsvServiceEntry && entry.Id()!=KMsvRootIndexEntryId);

	DBG((iSession->LogText(_L8("  Failed"))));

	return(NULL);
	}

void CImImap4Compound::GenericCopyL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination, TInt* aSequence)
	{
        DBG((iSession->LogText(_L8("CImImap4Compound::GenericCopyL()"))));
 	Queue(aStatus);

	// Save parameters
	iSource=aSource;
	iDestinationFolder=aDestination;
	iSourceFolder=FindFolderL(iSource);
	iMessageSelection=iSelectionStillToCopy=iSelectionStillToDelete=-1;

	// Find the offending source folder
	if (iSourceFolder == NULL &&
		aSequence != SeqMoveFromLocal && aSequence != SeqCopyFromLocal)
		{
		Complete(KErrNotFound);
		return;
		}

	// Select it
	iStep=0;
	iSequence=aSequence;
	DoRunL();
	}

void CImImap4Compound::GenericCopyL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination, TInt* aSequence)
	{
	DBG((iSession->LogText(_L8("CImImap4Compound::GenericCopyL()"))));
 	Queue(aStatus);

	// Save parameters
	iSource=aSourceSel[0];
	delete iSourceSel;
	iSourceSel = NULL;
	iSourceSel=aSourceSel.CopyL();
	iDestinationFolder=aDestination;
	iMessageSelection=iSelectionStillToCopy=iSelectionStillToDelete=aSourceSel.Count();

	// Check that selection elements are contiguous. Just call Copy on contiguous selections.
	
	iSourceFolder=FindFolderL((*iSourceSel)[iSelectionStillToCopy-1]);

	// Find the offending source folder
	if (iSourceFolder == NULL &&
		aSequence != SeqMoveFromLocal && aSequence != SeqCopyFromLocal)
		{
		Complete(KErrNotFound);
		return;
		}

	// Select it
	iStep=0;
	iSequence=aSequence;
	DoRunL();
	}

// CopyToLocal
void CImImap4Compound::CopyToLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND CopyToLocal(%x to %x)"),aSource,aDestination)));

	iPopulateCommand=EFalse;
	
	UpdatePartialMailInfoToDefaults(aDestination);

	// Kick off the copy	
	GenericCopyL(aStatus,aSource,aDestination,SeqCopyToLocal);		
	}

void CImImap4Compound::PopulateL(TRequestStatus& aStatus, const TMsvId aSource, TImImap4GetPartialMailInfo aGetPartialMailInfo)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND Populate(%x,%d)"),aSource,aGetPartialMailInfo.iGetMailBodyParts)));
	
	iPopulateCommand = ETrue;
	// Set options and kick off the copy to ourselves
	iGetPartialMailInfo = aGetPartialMailInfo;
	GenericCopyL(aStatus,aSource,KMsvNullIndexEntryId,SeqCopyToLocal);
	
	if(iMsgCount > 0)
		{
		iMsgCount--;	
		}
	
	}

// MoveToLocal
void CImImap4Compound::MoveToLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND MoveToLocal(%x to %x)"),aSource,aDestination)));


	//Partial populate option is implemented only for the populate command. moving a partially
	//populated message will delete the unpopulated message from the remote server. UI takes care 
	// of notifying this and ask for confirmation whether user still wants to move the message.

	iPopulateCommand=EFalse;
	
	UpdatePartialMailInfoToDefaults(aDestination);

	// We can't do a MoveToLocal if the destination is in the service: this possibility
	// if filtered out by the things calling us, so we don't have to worry about it here.
	// We know the destination is therefore a local folder.
	GenericCopyL(aStatus,aSource,aDestination,SeqMoveToLocal);
	}

// CopyWithinService
void CImImap4Compound::CopyWithinServiceL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND CopyWithinService(%x to %x)"),aSource,aDestination)));

	GenericCopyL(aStatus,aSource,aDestination,SeqCopyWithinService);
	}

void CImImap4Compound::CopyWithinServiceL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND CopyWithinService(to %x)"),aDestination)));

	GenericCopyL(aStatus,aSourceSel,aDestination,SeqCopyWithinService);
	}

// MoveWithinService
void CImImap4Compound::MoveWithinServiceL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND MoveWithinService(%x to %x)"),aSource,aDestination)));

	GenericCopyL(aStatus,aSource,aDestination,SeqMoveWithinService);
	}

void CImImap4Compound::MoveWithinServiceL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND MoveWithinService(to %x)"),aDestination)));

	GenericCopyL(aStatus,aSourceSel,aDestination,SeqMoveWithinService);
	}

// CopyFromLocal
void CImImap4Compound::CopyFromLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND CopyFromLocal(%x to %x)"),aSource,aDestination)));

	GenericCopyL(aStatus,aSource,aDestination,SeqCopyFromLocal);
	}

// MoveFromLocal
void CImImap4Compound::MoveFromLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND MoveFromLocal(%x to %x)"),aSource,aDestination)));

	GenericCopyL(aStatus,aSource,aDestination,SeqMoveFromLocal);
	}

// SyncCopyToLocal
void CImImap4Compound::SyncCopyToLocalL(TRequestStatus& aStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND SyncCopyToLocal(%x to %x)"),aSource,aDestination)));

	iPopulateCommand=EFalse;
	
	UpdatePartialMailInfoToDefaults(aDestination);

	// Kick off the copy	
	GenericCopyL(aStatus,aSource,aDestination,SeqSyncCopyToLocal);		
	}

// Delete
void CImImap4Compound::DeleteL(TRequestStatus& aStatus, const TMsvId aSource)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND Delete(%x)"),aSource)));

	Queue(aStatus);

	// Save parameters
	iSource=aSource;
	iMessageSelection=iSelectionStillToDelete=1;

	// Find the offending source folder
	if ((iSourceFolder=FindFolderL(iSource))==NULL)
		{
		Complete(KErrNotFound);
		return;
		}

	// Select it
	iStep=0;
	iSequence=SeqDelete;
	DoRunL();
	}

// Delete
void CImImap4Compound::DeleteL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel)
	{
	LOG_COMMANDS( (iSession->LogText(_L8("COMPOUND Delete selection"))));

	Queue(aStatus);

	// Save parameters
	iSource=aSourceSel[0];
	delete iSourceSel;
	iSourceSel = NULL;
	iSourceSel=aSourceSel.CopyL();
	iMessageSelection=iSelectionStillToDelete=aSourceSel.Count();

	// Find the offending source folder
	if ((iSourceFolder=FindFolderL(iSource))==NULL)
		{
		Complete(KErrNotFound);
		return;
		}

	// Select it
	iStep=0;
	iSequence=SeqDelete;
	DoRunL();
	}

// Delete all in folder, then folder itself
void CImImap4Compound::DeleteFolderL(TRequestStatus& aStatus, const TMsvId aSource)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND DeleteFolder(%x)"),aSource)));

	Queue(aStatus);

	// Save parameters
	iSourceFolder=aSource;
	iSequence=SeqDeleteFolder;

	// The folder might be a mailbox (in which case it needs selecting, messages
	// deleting, closing, then deletion of the folder) or it might be a \Noselect
	// folder, which we just delete.
	User::LeaveIfError(iEntry->SetEntry(aSource));
	TMsvEmailEntry folder=iEntry->Entry();
	if (!folder.Mailbox())
		{
		DBG((iSession->LogText(_L8("Folder is marked \\NoSelect: just delete it"))));

		// Skip the first few steps
		iStep=2;
		}
	else
		{
		DBG((iSession->LogText(_L8("Folder is a mailbox: deleting messages and folder"))));

		// Start from folder selection
		iStep=0;
		}

	DoRunL();
	}

void CImImap4Compound::CreateL(TRequestStatus& aStatus, const TMsvId aParent, const TDesC& aLeafName, const TBool aFolder)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND Create(%x)"),aParent)));

	Queue(aStatus);

	iStep=0;
	iSequence=SeqCreate;
	iSource=aParent;
	iLeafName=aLeafName;
	iFolder=aFolder;
	DoRunL();	
	}

void CImImap4Compound::RenameL(TRequestStatus& aStatus, const TMsvId aTarget, const TDesC& aNewName)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND Rename(%x)"),aTarget)));

	Queue(aStatus);

	iStep=0;
	iSequence=SeqRename;
	iSource=aTarget;
	iLeafName=aNewName;
	DoRunL();	
	}

void CImImap4Compound::SynchroniseL(TRequestStatus& aStatus)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND Synchronise"))));

	Queue(aStatus);

	iStep=0;
	iSequence=SeqSynchronise;
	DoRunL();	
	}

void CImImap4Compound::SelectL(TRequestStatus& aStatus, const TMsvId aFolder)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND Select(%x)"),aFolder)));

	Queue(aStatus);

	iStep=0;
	iSequence=SeqSelect;
	iSourceFolder=aFolder;
	DoRunL();
	}

void CImImap4Compound::NewOnlySyncL(TRequestStatus& aStatus, const TMsvId aFolder)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND NewOnlySync(%x)"),aFolder)));

	Queue(aStatus);

	if (iSession->ImapIdleSupported())
		{
		// Igmore sync as Imap Idle is supported
		Complete(KErrNone);
		return;
		}

	// Check folder is a mailbox
	User::LeaveIfError(iEntry->SetEntry(aFolder));
	TMsvEmailEntry mbcheck=iEntry->Entry();
	if (!mbcheck.Mailbox())
		{
		// Not a mailbox, so we can't sync it
		Complete(KErrNotSupported);
		return;
		}

	// Select it
	iStep=0;
	iSequence=SeqNewSync;
	iSourceFolder=aFolder;
	DoRunL();
	}

void CImImap4Compound::FullSyncL(TRequestStatus& aStatus, const TMsvId aFolder)
	{
	LOG_COMMANDS((iSession->LogText(_L8("COMPOUND FullSync(%x)"),aFolder)));

	Queue(aStatus);

	// Check folder is a mailbox
	User::LeaveIfError(iEntry->SetEntry(aFolder));
	TMsvEmailEntry mbcheck=iEntry->Entry();
	if (!mbcheck.Mailbox())
		{
		do
			{
			// Ensure visibility
			if (!mbcheck.Visible())
				{
				mbcheck.SetVisible(ETrue);
				User::LeaveIfError(iEntry->ChangeEntryBulk(mbcheck));
				}

			// Move up one
			User::LeaveIfError(iEntry->SetEntry(mbcheck.Parent()));
			mbcheck=iEntry->Entry();
			}
		while(mbcheck.iType!=KUidMsvServiceEntry && mbcheck.Id()!=KMsvRootIndexEntryId);

		// Not a mailbox, so we can't sync it
		Complete(KErrNone);
		return;
		}

	// Select it
	iStep=0;
	iSequence=SeqFullSync;
	iSourceFolder=aFolder;
	DoRunL();
	}

// Report progress
TImap4GenericProgress CImImap4Compound::Progress()
	{
	// First, ask session (updates byte counts & stuff)
	TImap4GenericProgress progress=iSession->Progress();
	if (iSequence==SeqDelete || iSequence==SeqCopyWithinService)
		{
		progress.iMsgsToDo=iMessageSelection;
		progress.iMsgsDone=iMessageSelection-iSelectionStillToCopy;
		}
	else if (iSequence==SeqMoveWithinService)
		{
		// DEL done in one go in imapsess, 
		// just have progress for the COPY part of the MOVE command.
		progress.iMsgsToDo=iMessageSelection;
		progress.iMsgsDone=iMessageSelection-iSelectionStillToCopy;
		}

	// don't believe there's any point in modifying the operation
	// since it'll be set correctly in the Mtm handler.
#if 0
	// Now, overlay our current operation (move or copy, at least)
	if (iSequence==SeqCopyToLocal || iSequence==SeqCopyWithinService ||
		iSequence==SeqCopyFromLocal)
		progress.iOperation=TImap4Progress::ECopying;
	else if (iSequence==SeqMoveToLocal || iSequence==SeqMoveWithinService ||
			 iSequence==SeqMoveFromLocal)
		progress.iOperation=TImap4Progress::EMoving;
#endif

	// Return the modified progress
	return(progress);
	}

void CImImap4Compound::UpdatePartialMailInfoToDefaults(TMsvId aDestination)
	{	
	iGetPartialMailInfo.iTotalSizeLimit= KMaxTInt;
	iGetPartialMailInfo.iBodyTextSizeLimit = KMaxTInt;
	iGetPartialMailInfo.iAttachmentSizeLimit = KMaxTInt;
	iGetPartialMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;		 
	iGetPartialMailInfo.iPartialMailOptions=ENoSizeLimits;
	iGetPartialMailInfo.iDestinationFolder=aDestination;
	}

void CImImap4Compound::SetMessageCount(const TInt aCount)
	{
	iMsgCount = aCount;
	}
