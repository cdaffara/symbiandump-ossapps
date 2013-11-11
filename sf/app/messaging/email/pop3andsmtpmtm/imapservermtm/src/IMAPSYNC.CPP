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
// IMAP4 connect & synchronise operation control
// This class deals with stepping through a synchronise operation - this
// involves:
// 1. Select inbox
// 2. Perform any outstanding operations on inbox
// 3. Synchronise inbox
// 4. Synchronise folder list
// 5. Build list of folders to synchronise, sorted by 'last sync' date
// (oldest first), and outstanding operations (folders with outstanding
// operations are first, whatever their date).
// 6. Mirror subscription flags as necessary
// 7. For each folder in the 'to synchronise' list...
// i)   Select folder
// ii)  Perform any outstanding operations
// iii) Synchronise folder
// 8. Reselect inbox
// 
//

#include <e32base.h>
#include <e32cons.h>
#include <mentact.h>
#include "impspan.h"
#include "imapsess.h"
#include "imapsync.h"
#include "fldsync.h"
#include <imapset.h>
#include <offop.h>
#include "imapcomp.h"
#include "imapoffl.h"
#include "impsutil.h"

#ifdef _DEBUG
#define DBG(a) iSession->LogText a
#define PRINTING
#else
#define DBG(a)
#undef PRINTING
#endif

// Inbox name for newly created one
_LIT(KInbox, "Inbox");
#ifdef PRINTING
LOCAL_C const TText8* SynchroniseStateString(CImImap4Synchronise::TSynchroniseState aState)
	{
	switch (aState)
		{
	case CImImap4Synchronise::ESyncStateIdle:
		return _S8("Idle");
	case CImImap4Synchronise::EInboxSelect:
		return _S8("InboxSelect");
	case CImImap4Synchronise::EInboxPendingOps:
		return _S8("InboxPendingOps");
	case CImImap4Synchronise::EInboxSync:
		return _S8("InboxSync");
	case CImImap4Synchronise::ESynchroniseFolderTree:
		return _S8("SynchroniseFolderTree");
	case CImImap4Synchronise::ECheckRemoteSubscription:
		return _S8("CheckRemoteSubscription");
	case CImImap4Synchronise::EProcessRemoteSubscription:
		return _S8("ProcessRemoteSubscription");
	case CImImap4Synchronise::EUpdateRemoteSubscription:
		return _S8("UpdateRemoteSubscription");
	case CImImap4Synchronise::EFolderSelect:
		return _S8("FolderSelect");
	case CImImap4Synchronise::EFolderPendingOps:
		return _S8("FolderPendingOps");
	case CImImap4Synchronise::EFolderSynchronise:
		return _S8("FolderSynchronise");
	case CImImap4Synchronise::EInboxEarlyDeletes:
		return _S8("InboxEarlyDeletes");
	case CImImap4Synchronise::EFolderEarlyDeletes:
		return _S8("FolderEarlyDeletes");
	case CImImap4Synchronise::EFolderEarlyExpunge:
		return _S8("FolderEarlyExpunge");
	case CImImap4Synchronise::EInboxLateExpunge:
		return _S8("InboxLateExpunge");
	case CImImap4Synchronise::EFolderLateDeletes:
		return _S8("FolderLateDeletes");
	case CImImap4Synchronise::EFolderLateExpunge:
		return _S8("FolderLateExpunge");
#if 0
	case CImImap4Synchronise::EInboxLateNewSync:
		return _S8("InboxLateNewSync");
	case CImImap4Synchronise::EFolderLateNewSync:
		return _S8("FolderLateNewSync");
#endif
	case CImImap4Synchronise::EEndSelectInbox:
		return _S8("EndSelectInbox");
	case CImImap4Synchronise::EInboxLogMessage:
		return _S8("InboxLogMessage");
	case CImImap4Synchronise::EFolderLogMessage:
  		return _S8("FolderLogMessage");
	case CImImap4Synchronise::EStartIdle:
		return _S8("StartIdle");
	case CImImap4Synchronise::ESynchronise:
		return _S8("Synchronise");	
	case CImImap4Synchronise::ESynchroniseTree:
		return _S8("SynchroniseTree");		
	case CImImap4Synchronise::ESynchroniseDeletes:
		return _S8("SynchroniseDeletes");		
		}
	return _S8("Unknown");
	}
#endif

CImImap4Synchronise::CImImap4Synchronise()
	: CMsgActive(1), iState(ESyncStateIdle)
	{
	__DECLARE_NAME(_S("CImImap4Synchronise"));
	}

CImImap4Synchronise::~CImImap4Synchronise()
	{
	// Global bits
	delete iFolderList;
	delete iFolderListDest;
	delete iFolderSync;
	delete iSubscribeList;
	delete iUnsubscribeList;
	delete iOutstandingOps;
	delete iCompound;
	delete iOutstandingMoveTypeDeletes;
	delete iOutstandingLocalDeletes;

	// We don't delete iSession as we don't own it, we were just
	// passed it to use.  similarly iOffLineControl
	}

CImImap4Synchronise* CImImap4Synchronise::NewLC(CImImap4Session *aSession)
	{
	CImImap4Synchronise* self=new (ELeave) CImImap4Synchronise();
	CleanupStack::PushL(self);

	// Non-trivial constructor
	self->ConstructL(aSession);
	return self;
	}

CImImap4Synchronise* CImImap4Synchronise::NewL(CImImap4Session *aSession)
	{
	CImImap4Synchronise* self=NewLC(aSession);
	CleanupStack::Pop();
	return self;
	}

// The non-trivial constructor
void CImImap4Synchronise::ConstructL(CImImap4Session *aSession)
	{
	// Save session
	iSession=aSession;

	// We're an active object...
	CActiveScheduler::Add(this);

	// One-off bits
	iFolderList=new (ELeave) CArrayFixFlat<TImImap4SyncList>(16);
	iFolderListDest=new (ELeave) CArrayFixFlat<TImImap4SyncList>(16);
	iFolderSync=CImImap4FolderSync::NewL(iSession);
	iSubscribeList=new (ELeave) CArrayFixFlat<TMsvId>(4);
	iUnsubscribeList=new (ELeave) CArrayFixFlat<TMsvId>(4);
	iOutstandingMoveTypeDeletes=new (ELeave) CArrayFixFlat<TMsvId>(4);
	iOutstandingLocalDeletes=new (ELeave) CArrayFixFlat<TMsvId>(4);

	// Compound operation
	iCompound=CImImap4Compound::NewL(iSession);

	// Make a thread-local timer
	iCheckMailbox.CreateLocal();

	iProgress.iType=EImap4SyncProgressType;

	iIdleBeforeCommand = EFalse;
	}

void CImImap4Synchronise::SetOffLineControl(CImap4OffLineControl* aOffLineControl)
	{
	iOffLineControl = aOffLineControl;
	}

void CImImap4Synchronise::SetUtils(CImap4Utils* aUtils)
	{
	iUtils = aUtils;
	}

// Set the entry to use to talk to the server
void CImImap4Synchronise::SetEntry(CMsvServerEntry* aEntry)
	{
	// Save it
	iEntry=aEntry;
	
	// Tell compound about it
	iCompound->SetEntry(iEntry);
	}

// Do setentry, leave if there is an error
void CImImap4Synchronise::SetEntryL(const TMsvId aId)
	{
	User::LeaveIfError(iEntry->SetEntry(aId));
	}

// Change entry, leave if error
void CImImap4Synchronise::ChangeEntryL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iEntry->ChangeEntry(aEntry));
	}

// Change entry in bulk mode (i.e no index file commit. no notify),
// leave if error
void CImImap4Synchronise::ChangeEntryBulkL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iEntry->ChangeEntryBulk(aEntry));
	}
// Get children, leave if error
void CImImap4Synchronise::GetChildrenL(CMsvEntrySelection& aSelection)
	{
	User::LeaveIfError(iEntry->GetChildren(aSelection));
	}

// Given an id of a folder or the service then restore the offline
// operation array out of it. Returns the number of operations in the
// array
TBool CImImap4Synchronise::RefreshOutstandingOpsL(TMsvId aId)
	{
	if (iOutstandingOps)
		{
		delete iOutstandingOps;
		iOutstandingOps=NULL;
		}
	
	iOutstandingOps = iOffLineControl->OffLineOpArrayL(aId);
	iOutstandingOpsFolder = aId;

	// reset the count
	iProgress.iMsgsToDo = iOutstandingOps->CountOperations();
	iProgress.iMsgsDone = 0;
	
	iMovedId = KMsvNullIndexEntryId;
	iShadowId = KMsvNullIndexEntryId;

	return iProgress.iMsgsToDo;
	}

// Called when async child completes with an error
#ifdef PRINTING
void CImImap4Synchronise::DoComplete(TInt& aStatus)
#else
void CImImap4Synchronise::DoComplete(TInt& /*aStatus*/)
#endif
	{
	DBG((_L8("CImImap4Synchronise::DoComplete(state=%s, istatus=%d)"),
		 SynchroniseStateString(iState),aStatus));

	// No alteration of the error code
	}

// This routine sets up iShadowId which will be deleted when the
// operation completes successfully
void CImImap4Synchronise::DoOpL(const CImOffLineOperation& aOp)
	{
	iShadowId = iMovedId = KMsvNullIndexEntryId;
	
	// clean the disconnected op flags and ensure its visible and get
	// an entry copy
	SetEntryL(aOp.MessageId());
	TMsvEmailEntry entry = iEntry->Entry();
	entry.SetVisible(ETrue);
	entry.SetDisconnectedOperation(ENoDisconnectedOperations);
	ChangeEntryBulkL(entry);

	// check and see if there is a shadow and whether it has been
	// removed or marked for deletion
	TBool shadowOK = ETrue;
	if ( aOp.OpType() != CImOffLineOperation::EOffLineOpMtmSpecific &&
		 aOp.OpType() != CImOffLineOperation::EOffLineOpDelete )
		{
		iShadowId = iOffLineControl->FindShadowIdL(aOp);

		shadowOK = iShadowId != KMsvNullIndexEntryId &&
			iEntry->SetEntry(iShadowId) == KErrNone &&
			((TMsvEmailEntry)iEntry->Entry()).DisconnectedOperation() != EDisconnectedDeleteOperation;
		}

	iUtils->ClearLogMessage();
	
	// Deal with operation
	switch(aOp.OpType())
		{
	case CImOffLineOperation::EOffLineOpCopyToLocal:
		// do the copy, if not a message
		if (entry.iType != KUidMsvMessageEntry ||
			// or the shadow exists
			shadowOK )
			{
			iUtils->SetUpLogMessageL(aOp.MessageId());
			SetActive();
			iCompound->SyncCopyToLocalL(iStatus,aOp.MessageId(),aOp.TargetMessageId());
			}
		break;
		
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
		if (shadowOK)
			{
			SetActive();
			iSession->Append(iStatus,aOp.MessageId(),aOp.TargetMessageId());
			}
		break;

	case CImOffLineOperation::EOffLineOpCopyWithinService:
		if (shadowOK)
			{
			SetActive();
			iSession->Copy(iStatus,aOp.MessageId(),aOp.TargetMessageId(),EFalse);
			}
		break;
		
	case CImOffLineOperation::EOffLineOpMoveToLocal:
		if (shadowOK)
			{
			iUtils->SetUpLogMessageL(aOp.MessageId());
			SetActive();
			iCompound->SyncCopyToLocalL(iStatus,aOp.MessageId(),aOp.TargetMessageId());
			}
		// even if the shadow has been removed we still want to delete
		// the original
		iMovedId=aOp.MessageId();
		break;

	case CImOffLineOperation::EOffLineOpMoveFromLocal:
		if (shadowOK)
			{
			SetActive();
			iSession->Append(iStatus,aOp.MessageId(),aOp.TargetMessageId());
			}
		// even if the shadow has been removed we still want to delete
		// the original
		iMovedId=aOp.MessageId();
		break;

	case CImOffLineOperation::EOffLineOpMoveWithinService:
		if (shadowOK)
			{
			SetActive();
			iSession->Copy(iStatus,aOp.MessageId(),aOp.TargetMessageId(),EFalse);
			}
		// even if the shadow has been removed we still want to delete
		// the original, unless the folder itself has been removed
		if (iEntry->SetEntry(aOp.TargetMessageId()) == KErrNone)
			iMovedId=aOp.MessageId();
		break;

	case CImOffLineOperation::EOffLineOpMtmSpecific:
		switch (aOp.MtmFunctionId())
			{
		case EFnOffLineOpPopulate:
			{
			TImap4GetMailOptions options;
			TPckgC<TImap4GetMailOptions> package(options);
			package.Set(aOp.MtmParameters());
			
			// Copy TImImap4GetMailOptions information into TImImap4GetPartialMailInfo
			TImImap4GetPartialMailInfo imap4GetPartialMailInfo;	
			imap4GetPartialMailInfo.iGetMailBodyParts = package();
			// Set to default
			imap4GetPartialMailInfo.iMaxEmailSize = KMaxTInt;
			// Set the remaining members to default so that the server can check 
			// if these are defaults, then this package is for TImImap4GetMailInfo
			imap4GetPartialMailInfo.iTotalSizeLimit	= KMaxTInt;
			imap4GetPartialMailInfo.iBodyTextSizeLimit = KMaxTInt;
			imap4GetPartialMailInfo.iAttachmentSizeLimit = KMaxTInt;
			imap4GetPartialMailInfo.iPartialMailOptions = ENoSizeLimits;
			
			TPckgBuf<TImImap4GetPartialMailInfo> partialPackage(imap4GetPartialMailInfo);
	
			iUtils->SetUpLogMessageL(aOp.MessageId());
			SetActive();
			iSession->FetchBody(iStatus,aOp.MessageId(),partialPackage());
			break;
			}
		default:
			break;
			}
		break;

	case CImOffLineOperation::EOffLineOpDelete:
	default:
		break;
		}
	}

void CImImap4Synchronise::MakeVisibleL(TMsvId aId)
	{
	DBG((_L8("  This folder isn't selectable, just making it visible")));

	// Just make it visible
	SetEntryL(aId);
	TMsvEmailEntry mbcheck=iEntry->Entry();
				
	do
		{
		// Ensure visibility
		if (!mbcheck.Visible())
			{
			mbcheck.SetVisible(ETrue);
			ChangeEntryBulkL(mbcheck);
			}

		// Move up one
		SetEntryL(mbcheck.Parent());
		mbcheck=iEntry->Entry();
		}
	while(mbcheck.iType!=KUidMsvServiceEntry && mbcheck.Id()!=KMsvRootIndexEntryId);
	}

void CImImap4Synchronise::DonePendingOpL()
	{
	// if we've done one then...
	if (iProgress.iMsgsDone != 0)
		{
		// if this was a move then append a delete
		if (iMovedId != KMsvNullIndexEntryId)
			{
			SetEntryL(iMovedId);
			if (iEntry->Entry().Parent() == iOutstandingOpsFolder)
				{
				DBG((_L8("Append MoveDelete for %x"), iMovedId));
				iOutstandingMoveTypeDeletes->AppendL(iMovedId);
				}
			else
				{
				// if this id was from a MoveFrom (ie its parent is not
				// this folder) then put it in a different pending array
				DBG((_L8("Append LocalDelete for %x"), iMovedId));
				iOutstandingLocalDeletes->AppendL(iMovedId);
				}
			}
		
		// delete the shadowid if there is one, ignore errors
		if (iShadowId != KMsvNullIndexEntryId &&
			iEntry->SetEntry(iShadowId) == KErrNone &&
			iEntry->SetEntry(iEntry->Entry().Parent()) == KErrNone)
			{
			iEntry->DeleteEntry(iShadowId);
			}

		if (iUtils->SendLogMessageL(KErrNone,iStatus))
			SetActive();
		}
	}

TBool CImImap4Synchronise::NextPendingOpL()
	{
	TBool finished = EFalse;
	
	// Any operations in outstanding list?
	if (iOutstandingOps->CountOperations())
		{
		DBG((_L8("Outstanding operations on this folder (%d)"),
			 iOutstandingOps->CountOperations()));

		// Fetch operation
		CImOffLineOperation thisop;
		thisop.CopyL(iOutstandingOps->Operation(0));

		// when we get to one of the Delete operations then it is time
		// to stop
		if (thisop.OpType() == CImOffLineOperation::EOffLineOpDelete ||
			(thisop.OpType() == CImOffLineOperation::EOffLineOpMtmSpecific &&
			 thisop.MtmFunctionId() == EFnOffLineOpMoveDelete))
			{
			DBG((_L8("Reached delete op. Finished")));
			finished = ETrue;
			}
		else
			{
			// remove from list and save back
			iOutstandingOps->Delete(0);
			iOffLineControl->SetOffLineOpArrayL(iOutstandingOpsFolder, *iOutstandingOps);

			// and execute
			DoOpL(thisop);
			iProgress.iMsgsDone++;
			}
		}
	else
		{
		// No more operations to do, return to what we should be doing next
		finished = ETrue;
		}

	// when we are about to finish this folder then tidy up
	if (finished)
		{
		// add the list of pending deletes to the front of the
		// offline op array
		for (TInt i = 0; i < iOutstandingMoveTypeDeletes->Count(); i++)
			{
			TMsvId id = (*iOutstandingMoveTypeDeletes)[i];
			TBuf8<128> paramBuf(_L8(""));
			CImOffLineOperation thisop;

			// if we are doing deletes on connection then store this
			// as a delete and we will do all deletes at the end of
			// the sync.

			// if we are doing deletes on disconnection then store
			// this as a special code - it will still get done at end
			// of sync, but real deletes will be done on
			// disconnection.
			if (iPerformDeletes)
				thisop.SetDelete(id);
			else
				thisop.SetMtmSpecificCommandL(id, EFnOffLineOpMoveDelete, 0, paramBuf);

			iOutstandingOps->InsertOperationL(thisop, 0);
			}

		// if there were outstanding move type deletes then clear
		// their array and save back the main outstanding ops list
		if (iOutstandingMoveTypeDeletes->Count())
			{
			iOutstandingMoveTypeDeletes->Reset();

			iOffLineControl->SetOffLineOpArrayL(iOutstandingOpsFolder, *iOutstandingOps);
			}
		}

	return finished;
	}

TBool CImImap4Synchronise::MatchDeleteOp(const CImOffLineOperation& aOp , TBool aMoveDeletesOnly )
	{
	return (aOp.OpType() == CImOffLineOperation::EOffLineOpMtmSpecific && aOp.MtmFunctionId() == EFnOffLineOpMoveDelete) ||
		(!aMoveDeletesOnly && aOp.OpType() == CImOffLineOperation::EOffLineOpDelete);
	}

TBool CImImap4Synchronise::ProcessPendingDeleteOpsL( TMsvId aFolder, TBool aMoveDeletesOnly )
	{
	TBool hadDeletes = EFalse;

	DBG((_L8("CImImap4Synchronise::ProcessPendingDeleteOpsL: Folder %x aMoveDeletesOnly=%d"),
		 aFolder, aMoveDeletesOnly));
		
	// get the current offline operations of this folder
	if (RefreshOutstandingOpsL(aFolder))
		{
		// Fetch operation
		CImOffLineOperation thisop;
		thisop.CopyL(iOutstandingOps->Operation(0));

		// check delete type
		if (MatchDeleteOp(thisop, aMoveDeletesOnly))
			{
			do
				{
				// if can't find the entry then just skip it so it is
				// removed from array
				if (iEntry->SetEntry(thisop.MessageId()) == KErrNone)
					{
					// set its server deleted flag
					TMsvEmailEntry entry=iEntry->Entry();
					entry.SetDeletedIMAP4Flag(ETrue);
					iEntry->ChangeEntry(entry);
					}

				// remove from the array and write back immediately
				iOutstandingOps->Delete(0);
				iOffLineControl->SetOffLineOpArrayL(iOutstandingOpsFolder, *iOutstandingOps);

				// check for finish
				if (iOutstandingOps->CountOperations() == 0)
					break;
			
				// get next op
				thisop.CopyL(iOutstandingOps->Operation(0));
				}
			while (MatchDeleteOp(thisop, aMoveDeletesOnly));
					
			hadDeletes = ETrue;
			}
		}

	return hadDeletes;
	}

TBool CImImap4Synchronise::ProcessPendingDeleteOpsListL( TBool aMoveDeletesOnly )
	{
	while (iProgress.iFoldersDone < iProgress.iFoldersToDo)
		{
		TImImap4SyncList next=(*iFolderList)[iProgress.iFoldersDone++];

		if (ProcessPendingDeleteOpsL( next.iFolder, aMoveDeletesOnly ))
			{
			iSession->SelectL(iStatus, next.iFolder, ETrue);
			SetActive();

			return ETrue;
			}
		}
	return EFalse;
	}

// Called when async child completes with >=KErrNone
void CImImap4Synchronise::DoRunL()
	{
	DBG((_L8("CImImap4Synchronise::DoRunL(istatus=%d)"), iStatus.Int()));
	
	while (!IsActive() && !DoRunLoopL())
		{
		// do nothing in the body of this
		}
	}

TBool CImImap4Synchronise::DoRunLoopL()
	{
	DBG((_L8("CImImap4Synchronise::DoRunLoopL(state=%s)"),
		SynchroniseStateString(iState)));

	TBool done = EFalse;
	
	// DoRunL is only called
	switch(iState)
		{
	case EInboxSelect:
		iProgress.iState=TImap4SyncProgress::ESyncInbox;

		// if select failed then skip past INBOX ops
		if (iStatus.Int() == KErrIMAPNO)
			{
			iState = ESynchroniseFolderTree;
			iProgress.iFoldersNotFound++;
			}
		else
			{
			// Deal with operations in the array
			if (RefreshOutstandingOpsL(iInbox))
				iState=EInboxPendingOps;
			else
				iState=EInboxSync;
			}
		break;

	case EInboxPendingOps:
		iProgress.iState=TImap4SyncProgress::EProcessingPendingOps;
		
		iState = EInboxLogMessage;
		DonePendingOpL();
		break;
		
	case EInboxLogMessage:
		if (NextPendingOpL())
			iState = EInboxSync;
		else
			iState = EInboxPendingOps;
		break;

	case EInboxSync:
		iProgress.iState=TImap4SyncProgress::ESyncInbox;
		
		//Once pending operations are done on folder/s, sync the Inbox 
		if(iPendingOpOnFolder)
			{
			// reset iPendingOpOnFolder flag
			iPendingOpOnFolder=EFalse;
			if (iSession->ImapIdleSupported() && iIdleBeforeCommand)
				{
				// change the state to EStartIdle, if ImapIdle supported
				iState = EStartIdle;
				}
			else
				{
				// else  change the state to EEndSelectInbox
				iState = EEndSelectInbox;
				}
			}
		else
			{
			// After inbox sync, start folder tree sync
			iState=ESynchroniseFolderTree;
			}

		// mark inbox as having been done
		iProgress.iFoldersDone++;

		// Start the sync of the current folder (ie, the inbox)
		DBG((_L8("CImImap4Sync::DoRunLoopL(): Calling iSession->SynchroniseL()")));
                iSession->SynchroniseL(iStatus,EFalse);
		SetActive();
		break;

	case ESynchroniseFolderTree:
		iProgress.iState=TImap4SyncProgress::ESyncFolderTree;

		// After this, check the remote folder subscription if needed
		iState=ECheckRemoteSubscription;
		
		// Update folder tree
		iFolderSync->SetEntry(iEntry);
		iFolderSync->SynchroniseTreeL(iStatus,iServiceId,iNewFoldersAreInvisible);
		SetActive();
		break;

	case ECheckRemoteSubscription:
		iProgress.iState=TImap4SyncProgress::ECheckRemoteSubscription;

		// Check the remote subscription/build local folder list,
		// dependent on strategy in use
		iState=EProcessRemoteSubscription;

		// Do we need to know what folders are subscribed remotely?
		if (iSynchroniseStrategy==EUseLocal && 
			(iSubscribeStrategy==EUpdateNeither || iSubscribeStrategy==EUpdateLocal))
			{
			// No we don't: just add the folders (done in this state)
			}
		else
			{
			// Update our list of remotely subscribed folders
			iSession->LsubL(iStatus);
			SetActive();
			}
		break;

	case EProcessRemoteSubscription:
		iProgress.iState=TImap4SyncProgress::ECheckRemoteSubscription;

		// Build (from local folders) and sort the list
		SortFolderListL();

		iProgress.iFoldersDone=0;
		iProgress.iFoldersToDo=iFolderList->Count();

		// Any remote subscribing/unsubscribing to do?
		if (iSubscribeList->Count() || iUnsubscribeList->Count())
			{
			// Yes, do them
			iState=EUpdateRemoteSubscription;
			}
		else
			{
			// Start folder selection
			iState=EFolderSelect;
			}
		break;

	case EUpdateRemoteSubscription:
		iProgress.iState=TImap4SyncProgress::EUpdateRemoteSubscription;
		
		// Any subscription to do?
		if (iSubscribeList->Count())
			{
			// Take it off the head
			TMsvId folder=(*iSubscribeList)[0];
			iSubscribeList->Delete(0,1);

			// Subscribe to it
			iSession->RemoteSubscribeL(iStatus,folder,ETrue);
			SetActive();
			}
		// ...or unsubscription?
		else if (iUnsubscribeList->Count())
			{
			// Take it off the head
			TMsvId folder=(*iUnsubscribeList)[0];
			iUnsubscribeList->Delete(0,1);

			// Unsubscribe from it
			iSession->RemoteSubscribeL(iStatus,folder,EFalse);
			SetActive();
			}
		else
			{
			// All done, select the first folder
			iState=EFolderSelect;
			}
		break;

	case EFolderSelect:
		iProgress.iState=TImap4SyncProgress::ESyncOther;

		// Anything to do...
		while (iProgress.iFoldersDone < iProgress.iFoldersToDo)
			{
			// Select the next folder
			// Get next folder to-do
			TImImap4SyncList next=(*iFolderList)[iProgress.iFoldersDone++];

			DBG((_L8("Next folder to select is %x"),next.iFolder));

			// Is this a selectable folder?
			if (next.iNotSelectable)
				{
				MakeVisibleL(next.iFolder);
				}
			else
				{
				// Select it
                                DBG((_L8("CImImap4Sync::DoRunLoopL(): Calling iSession->SelectL()")));
 				iSession->SelectL(iStatus,next.iFolder,ETrue);
				SetActive();

				// get the current offline operations of this folder
				if (RefreshOutstandingOpsL(next.iFolder))
					{
					iState=EFolderPendingOps;
					// set iPendingOpOnFolder flag, according this flag will syncing the Inbox.
  	  				iPendingOpOnFolder=ETrue;					
					}
				else
					{
					iState=EFolderSynchronise;
					}

				// stop loop
				break;
				}
			}

		// Run out of folders to sync?
		if (iState == EFolderSelect)
			iState = EInboxEarlyDeletes;
		break;

	case EFolderPendingOps:
		iProgress.iState=TImap4SyncProgress::EProcessingPendingOps;

		if (iStatus.Int() == KErrIMAPNO)
			{
			iState = EFolderSelect;
			iProgress.iFoldersNotFound++;
			}
		else
			{
			iState = EFolderLogMessage;
			DonePendingOpL();
			}
		break;

	case EFolderLogMessage:
		if (NextPendingOpL())
			{
			// Where a pending operation involves a folder that is also the currently
			// selected mailbox, the IMAP session may have deselected the mailbox
			// during the operation. Just in case this has happened, we need to select it
			// again now.
			DBG((_L8("CImImap4Sync::DoRunLoopL(): Calling select after pending ops")));
			TImImap4SyncList next=(*iFolderList)[iProgress.iFoldersDone - 1];
			iSession->SelectL(iStatus,next.iFolder,ETrue);
			SetActive();

			iState = EFolderSynchronise;
			}
		else
			iState = EFolderPendingOps;
		break;

	case EFolderSynchronise:
		iProgress.iState=TImap4SyncProgress::ESyncOther;

		if (iStatus.Int() == KErrIMAPNO)
			iProgress.iFoldersNotFound++;
		else
			{
			iSession->SynchroniseL(iStatus,EFalse);
			SetActive();
			}

		// back to select next folder
		iState=EFolderSelect;
		break;

	case EInboxEarlyDeletes:
		{
		iProgress.iState=TImap4SyncProgress::EDeleting;

		// get rid of the FromLocal message sources
		for (TInt i = 0; i < iOutstandingLocalDeletes->Count(); i++)
			{
			TMsvId id = (*iOutstandingLocalDeletes)[i];
			iSession->DeleteMessageL(id);
			}

		iOutstandingLocalDeletes->Reset();

		// then do the inbox deletes
		iProgress.iFoldersDone = 0;
		if (ProcessPendingDeleteOpsL(iInbox, !iPerformDeletes))
			{
			iState = EFolderEarlyExpunge;

			iSession->SelectL(iStatus, iInbox, ETrue);
			SetActive();
			}
		else
			{
	        // DEF045009
            if (iSession->ServiceSettings()->DeleteEmailsWhenDisconnecting())
                 iState = EFolderLateDeletes;
            else
                 iState = EFolderEarlyDeletes;
			}
  		break;
		}

	case EFolderEarlyDeletes:
		iProgress.iState=TImap4SyncProgress::EDeleting;

		// if we are doing deletes on connection then all deletes will
		// be marked Delete
		if (ProcessPendingDeleteOpsListL(!iPerformDeletes))
			{
			iState = EFolderEarlyExpunge;
			}
		else
			{
			if (iSession->ImapIdleSupported() && iIdleBeforeCommand)
				{
				iState = EStartIdle;
				}
			else
				{
				iState = EEndSelectInbox;
				}

			// All done: reselect inbox r/w
			iSession->SelectL(iStatus,iInbox,ETrue);
			SetActive();
			}
		break;

	case EFolderEarlyExpunge:
		iProgress.iState=TImap4SyncProgress::EDeleting;

		if (iStatus.Int() == KErrIMAPNO)
			iProgress.iFoldersNotFound++;
		else
			{
			// expunge the folder
			iSession->Close(iStatus, ETrue);
			SetActive();
			}

		// back to select next folder
		iState=EFolderEarlyDeletes;
		break;
		
	case EEndSelectInbox:
		iProgress.iState=TImap4SyncProgress::EIdle;

		// Finish sync operation
		iState=ESyncStateIdle;
		Complete(KErrNone);
		done = ETrue;
		break;

	case EInboxLateExpunge:
		iProgress.iState=TImap4SyncProgress::EDeleting;

		if (iStatus.Int() == KErrIMAPNO)
			iProgress.iFoldersNotFound++;
		else
			{
			iSession->Close(iStatus, ETrue);
			SetActive();
			}

		iState = EFolderLateDeletes;
		iProgress.iFoldersDone = 0;
		break;

	case EFolderLateDeletes:
		iProgress.iState=TImap4SyncProgress::EDeleting;

		if (ProcessPendingDeleteOpsListL( EFalse ) )
			{
			iState = EFolderLateExpunge;
			}			
		else
			{
			iProgress.iState=TImap4SyncProgress::EIdle;
			
			if(iPendingOpOnFolder)
				{
				iSession->SelectL(iStatus,iInbox,ETrue);
				iState=EInboxSync;
				SetActive();
				}
			else if (iSession->ImapIdleSupported() && iIdleBeforeCommand)
				{
				iSession->SelectL(iStatus,iInbox,ETrue);
				iState = EStartIdle;			
				SetActive();
				}
			else
				{
				iState=ESyncStateIdle;
				Complete(KErrNone);
				done = ETrue;
				}
			}
		break;

	case EFolderLateExpunge:
		iProgress.iState=TImap4SyncProgress::EDeleting;

		if (iStatus.Int() == KErrIMAPNO)
			{
			iState = EFolderLateDeletes;
			iProgress.iFoldersNotFound++;
			}
		else
			{
			iSession->Close(iStatus, ETrue);
			SetActive();

			iState = EFolderLateDeletes;
			}
		break;

	case EStartIdle:
		iState = EEndSelectInbox;

		iSession->StartIdleL(iStatus);
		SetActive();
		break;

	case ESynchronise:
		// Synchronise with the inbox
		iSession->SelectL(iStatus,iInbox,ETrue);
		iState=EInboxSelect;
		SetActive();
		break;

	case ESynchroniseTree:
		if (iSession->ImapIdleSupported())
			{
			iState = EStartIdle;
			}
		else
			{
			iState = EEndSelectInbox;
			}

		iFolderSync->SetEntry(iEntry);
		iFolderSync->SynchroniseTreeL(iStatus, iServiceId, iNewFoldersAreInvisible);
		SetActive();
		break;

	case ESynchroniseDeletes:
		if (ProcessPendingDeleteOpsL( iInbox, EFalse ))
			{
			iSession->SelectL(iStatus,iInbox,ETrue);

			iState = EInboxLateExpunge;
			SetActive();
			}
		else if (ProcessPendingDeleteOpsListL( EFalse ) )
			{
			iState = EFolderLateExpunge;
			}
		else
			{
			iProgress.iState=TImap4SyncProgress::EIdle;

			iState=ESyncStateIdle;
			Complete(KErrNone);
			done = ETrue;
			}
		break;

	default:
		gPanic(EUnknownState);
		done = ETrue;
		break;
		}

	return done;
	}

void CImImap4Synchronise::GetInboxL()
	{
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
		if (iEntry->Entry().iDetails.CompareF(KIMAP_INBOX)==0)
			{
			inboxid=(*findinbox)[a];
			TMsvEmailEntry e=iEntry->Entry();

			DBG((_L8("INBOX found with id %x (mailbox=%d)"),inboxid,e.Mailbox()));

			// Mailbox flag not set? It always should be. This will 'repair' it.
			if (!e.Mailbox())
				{
				// Set it
				e.SetMailbox(ETrue);
				User::LeaveIfError(iEntry->ChangeEntry(e));
				}

			break;
			}
		}

	// Clean up
	CleanupStack::PopAndDestroy();

	// Not found/no entries?
	if ((iInbox=inboxid)==0)
		{
		// Create an inbox entry below the service. This is a local-only
		// creation as the server *WILL* have an inbox.
		SetEntryL(iServiceId);
		TMsvEmailEntry entry;

		entry.iType=KUidMsvFolderEntry;
		entry.iMtm=KUidMsgTypeIMAP4;
		entry.iServiceId=iServiceId;
		entry.SetMtmData1(0);
		entry.SetMtmData2(0);
		entry.SetMtmData3(0);
		entry.iSize=0;
		entry.SetUID(0);
		entry.SetValidUID(EFalse);
		entry.SetMailbox(ETrue);
		entry.SetLocalSubscription(ETrue);
		entry.SetComplete(ETrue);
		entry.iDetails.Set(KInbox);
		User::LeaveIfError(iEntry->CreateEntryBulk(entry));

		// Set inbox ID
		iInbox=entry.Id();

		DBG((_L8("INBOX created with id %x"),iInbox));
		}
	}

void CImImap4Synchronise::SynchroniseTreeL(TRequestStatus& aStatus, TMsvId aServiceId, TBool aNewFoldersAreInvisible)
	{
	DBG((_L8("CImImap4Synchronise::SynchroniseTreeL")));

	__ASSERT_DEBUG(aServiceId!=KMsvNullIndexEntryId, gPanic(EInvalidService));

	Queue(aStatus);

	iServiceId = aServiceId;
	iNewFoldersAreInvisible = aNewFoldersAreInvisible;

	GetInboxL();
	ResetStats();
	iSession->SetInbox(iInbox);

	iProgress.iState=TImap4SyncProgress::ESyncFolderTree;
	
	if (iSession->IsIdling())
		{
		iState=ESynchroniseTree;
		iSession->SyncStopIdleL(iStatus);
		SetActive();
		}
	else
		{
		iFolderSync->SetEntry(iEntry);
		iFolderSync->SynchroniseTreeL(iStatus, iServiceId, aNewFoldersAreInvisible);

		iState=EEndSelectInbox;
		SetActive();
		}
	}

void CImImap4Synchronise::ResetStats()
	{
	iProgress.iFoldersToDo = iProgress.iFoldersDone = 0;
	iProgress.iMsgsToDo = iProgress.iMsgsDone = 0;
	iProgress.iHeadersFetched = iProgress.iOrphanedFolders = 0;
	iProgress.iNewFolders = iProgress.iOrphanedMessages = 0;
	iProgress.iRemoteMessagesDeleteTagged = 0;

	iProgress.iMessagesFetchedOK = iProgress.iMessagePartsFetchedOK = 0;
	iProgress.iMessagePartsNotFound = iProgress.iFoldersNotFound = 0;
	iProgress.iErrorCode = KErrNone;

	// also reset the counts in the ImapSession
	iSession->ResetStats();
	iFolderSync->ResetStats();
	}

void CImImap4Synchronise::SynchroniseDeletesL(TRequestStatus& aStatus, TMsvId aServiceId)
	{
	DBG((_L8("CImImap4Synchronise::SynchroniseDeletesL")));

	__ASSERT_DEBUG(aServiceId!=KMsvNullIndexEntryId, gPanic(EInvalidService));

	iServiceId = aServiceId;

	ResetStats();

	// Get the list of folders which need to be synchronized
	iProgress.iFoldersToDo=iFolderList->Count();

	GetInboxL();
	iSession->SetInbox(iInbox);
	
	Queue(aStatus);

	iProgress.iState=TImap4SyncProgress::EDeleting;
	
	if (iSession->IsIdling())
		{
		iState=ESynchroniseDeletes;

		iSession->SyncStopIdleL(iStatus);
		SetActive();
		}
	else
		{
		if (ProcessPendingDeleteOpsL( iInbox, EFalse ))
			{
			iSession->SelectL(iStatus,iInbox,ETrue);

			iState = EInboxLateExpunge;
			SetActive();
			}
		else if (ProcessPendingDeleteOpsListL( EFalse ) )
			{
			iState = EFolderLateExpunge;
			}
		else
			{
			iProgress.iState=TImap4SyncProgress::EIdle;
			Complete(KErrNone);
			}
		}
	}

// Synchronise mirror tree with remote server
void CImImap4Synchronise::SynchroniseL(TRequestStatus& aStatus, TMsvId aService, TBool aNewInvisible, TBool aPerformDeletes, TBool aConnectAndSync)
	{
	DBG((_L8("CImImap4Synchronise::SynchroniseL(service=%x,newInvisible=%d,performDeletes=%d)"),
		 aService, aNewInvisible, aPerformDeletes));

	Queue(aStatus);

	// Note where this list came from
	iServiceId=aService;
	iFolderId=iServiceId;

	// Note the invisibility
	iNewFoldersAreInvisible=aNewInvisible;
	iPerformDeletes = aPerformDeletes;

	iIdleBeforeCommand = aConnectAndSync;
	
	// set iPendingOpOnFolder flag
	iPendingOpOnFolder=EFalse;
	
	// Get the synchronise settings
	iSynchroniseStrategy=iSession->ServiceSettings()->Synchronise();
	iSubscribeStrategy=iSession->ServiceSettings()->Subscribe();

	DBG((_L8("iSynchroniseStrategy=%d, iSubscribeStrategy=%d"),
		iSynchroniseStrategy,iSubscribeStrategy));

	GetInboxL();
	ResetStats();
	iSession->SetInbox(iInbox);

	// Reset stats: 1 folder to do (inbox)
	iProgress.iFoldersToDo=1;
	iProgress.iState=TImap4SyncProgress::ESyncInbox;
	
	if (iSession->IsIdling())
		{
		iIdleBeforeCommand=ETrue;
		iState=ESynchronise;

		iSession->SyncStopIdleL(iStatus);
		SetActive();
		}
	else
		{
		// Synchronise with the inbox
		iSession->SelectL(iStatus,iInbox,ETrue);
		iState=EInboxSelect;
		SetActive();
		}
	}

void CImImap4Synchronise::SortFolderListL()
	{
	// Clear list of folders to sync, and subscribe/unsubscribe lists
	iFolderList->Reset();
	iFolderListDest->Reset();
	iSubscribeList->Reset();
	iUnsubscribeList->Reset();

	// Add folders
	AddLocalFoldersL(iServiceId);

	// Sort it by last-sync date (oldest first)
	TKeyArrayFix timeKey(_FOFF(TImImap4SyncList,iLastSync),ECmpTInt64);

	// Perform the sort on each of the lists separately
	iFolderList->Sort(timeKey);
	iFolderListDest->Sort(timeKey);

	// Merge the two lists and clear the second one
	for(TInt i=0; i<iFolderListDest->Count(); i++) 
		iFolderList->AppendL((*iFolderListDest)[i]);
	iFolderListDest->Reset();
	}

void CImImap4Synchronise::AddIfNotThereL(TMsvId aFolder, CArrayFix<TImImap4SyncList>* aFolderList)
	{
	// first see if we already have this folder and return if we do
	for (TInt i=0; i < aFolderList->Count(); i++)
		{
		if ((*aFolderList)[i].iFolder == aFolder)
			return;
		}

	// don't add the inbox or service
	if (aFolder == iInbox || aFolder == iServiceId)
		return;
	
	// visit folder
	SetEntryL(aFolder);
	TMsvEmailEntry entry=iEntry->Entry();

	// not there so add it
	TImImap4SyncList add;
	add.iFolder=aFolder;
	add.iNotSelectable=EFalse;
	add.iLastSync=entry.iDate;
	// previously we adjusted the date of source folders to bring them
	// earlier in the list. No need to do this now as they
	// automatically appear before subscriptions/destinations

	// If this is a \NoSelect marked folder, then we need to note this, so
	// that at sync time we just make the folder visible, as opposed to
	// trying to select it
	if (!entry.Mailbox())
		add.iNotSelectable=ETrue;
	
	DBG((_L8("Adding folder '%S' to synchronise todo list (mailbox=%d)"),
					   &entry.iDetails, entry.Mailbox()?1:0));

	aFolderList->AppendL(add);
	}

// Add local (subscribed) folders to the 'to do' iFolderList. Recursive.
void CImImap4Synchronise::AddLocalFoldersL(const TMsvId aFolder)
	{
	// Select the entry
	SetEntryL(aFolder);
	TMsvEmailEntry entry=iEntry->Entry();

	// Is it actually a folder or service? If not, ignore it. 
	if (entry.iType!=KUidMsvServiceEntry &&
		entry.iType!=KUidMsvFolderEntry)
		return;

	DBG((_L8("CImImap4Synchronise::AddLocalFolders(%x), iSync=%d iSubs=%d"),
					   aFolder,iSynchroniseStrategy,iSubscribeStrategy));

	// What we do now depends on the strategy
	TBool addthisone=EFalse;
	switch(iSynchroniseStrategy)
		{
	case EUseLocal:
		// Is it locally subscribed?
		if (entry.LocalSubscription())
			addthisone=ETrue;
		break;

	case EUseRemote:
		// Is it remotely subscribed?
		if (entry.Subscribed())
			addthisone=ETrue;
		break;

	case EUseCombination:
		// Either will do
		if (entry.LocalSubscription() || entry.Subscribed())
			addthisone=ETrue;
		break;
		}

	// Any outstanding operations?
	RefreshOutstandingOpsL(aFolder);

	if (!entry.Orphan())
		{
		// check each and add the folder and destination folders. Add
		// source folders to one list, destination and just subscribed
		// ones to the second list, the two lists are merged
		// together before use
		if (iOutstandingOps->CountOperations())
			{
			AddIfNotThereL(aFolder, iFolderList);
		
			for(TInt a=0; a<iOutstandingOps->CountOperations(); a++)
				{
				TMsvId dest = iOutstandingOps->Operation(a).TargetMessageId();
				if (dest != KMsvNullIndexEntryId)
					AddIfNotThereL(dest, iFolderListDest);
				}
			}

		// add subscribed afterwards
		if (addthisone)
			AddIfNotThereL(aFolder, iFolderListDest);
		}	

	// Any children?
	CMsvEntrySelection *children=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);
	GetChildrenL(*children);
	TInt noofchildren=children->Count();

	if (!entry.Orphan() && aFolder != iInbox && aFolder != iServiceId)
		{
		if (addthisone)
			{
			// Do updating
			switch(iSubscribeStrategy)
				{
			case EUpdateNeither:
				break;

			case EUpdateLocal:
			case EUpdateBoth:
				if (!entry.LocalSubscription())
					{
					// Update local subscription flag
					entry.SetLocalSubscription(ETrue);
					SetEntryL(aFolder);
					ChangeEntryBulkL(entry);
					}

				// Doing both?
				if (iSubscribeStrategy!=EUpdateBoth)
					break;

				// Fall through...

			case EUpdateRemote:
				if (!entry.Subscribed())
					{
					// Queue subscribe command
					iSubscribeList->AppendL(aFolder);

					DBG((_L8("Adding folder '%S' to remote subscribe todo list"),&entry.iDetails));
					}
				break;
				}
			}
		else // if (!addthisone)
			{
			// Do updating
			switch(iSubscribeStrategy)
				{
			case EUpdateNeither:
				break;

			case EUpdateLocal:
			case EUpdateBoth:
				if (entry.LocalSubscription())
					{
					// Update local subscription flag
					entry.SetLocalSubscription(EFalse);
					SetEntryL(aFolder);
					ChangeEntryBulkL(entry);
					}

				// Doing both?
				if (iSubscribeStrategy!=EUpdateBoth)
					break;

				// Fall through...

			case EUpdateRemote:
				if (entry.Subscribed())
					{
					// Queue subscribe command
					iUnsubscribeList->AppendL(aFolder);

					DBG((_L8("Adding folder '%S' to remote unsubscribe todo list"),&entry.iDetails));
					}
				break;
				}

			// This folder is not subscribed, but has children. 
			// If any children are messages, then delete.
			if (noofchildren)
				{
				DBG((_L8("Checking unsubscribed folder (%S) for messages"),&entry.iDetails));

				// This folder is not subscribed to, so check if it has any messages.
				// Do each in turn
				TInt child = children->Count();
				while (child--)
					{
					SetEntryL((*children)[child]);
					TMsvEmailEntry entry=iEntry->Entry();

					// Is it a message?
					if (entry.iType==KUidMsvMessageEntry)
						{
						DBG((_L8("Deleting unsubscribed folder message(%x)"),(*children)[child]));

						// Yup its a message - delete it!
						iSession->DeleteMessageL((*children)[child]);
						}
					}

				// If we've some children then get the children list
				// again
				SetEntryL(aFolder);
				GetChildrenL(*children);
				noofchildren=children->Count();
				}
			}
		}
	
	// Any children?
	if (noofchildren)
		{
		// Do each in turn
		for(TInt child=0;child<children->Count();child++)
			AddLocalFoldersL((*children)[child]);
		}

	CleanupStack::PopAndDestroy(children);
	}

// Report progress
TImap4SyncProgress CImImap4Synchronise::Progress()
	{
	TImap4SyncProgress progress = iProgress;
	
	// Make sure we don't get stuck in sync folder mode. Fix for DEF053846.
	if (iProgress.iState == TImap4SyncProgress::ESyncFolderTree && !iFolderSync->IsActive())
		{
		iProgress.iState = TImap4SyncProgress::EIdle;
		}
	
	// get the info from the folder sync
	// currently OrphanedFolders and NewFolders
	if (iState >= ESynchroniseFolderTree)
		iFolderSync->IncProgress(progress);

	// get info from the session (note overwrites the msgs counts).
	iSession->IncSyncStats(progress);

	// when synchronising (ie getting new headers) then we use the
	// counts from the session otherwise use our own count
	if (iProgress.iState != TImap4SyncProgress::ESyncOther &&
		iProgress.iState != TImap4SyncProgress::ESyncInbox )
		{
		progress.iMsgsDone=iProgress.iMsgsDone;
		progress.iMsgsToDo=iProgress.iMsgsToDo;
		}

	// Return the modified progress
	return(progress);
	}

// Called when parent wants to cancel current operation
void CImImap4Synchronise::DoCancel()
	{
	DBG((_L8("CImImap4Synchronise::DoCancel() called. Cancelling session")));

	// Cancel any outstanding ops
	iFolderSync->Cancel();
	iCompound->Cancel();
	iSession->Cancel();

	// Not doing nuffink
	iState=ESyncStateIdle;

	// Parent
	CMsgActive::DoCancel();
	}
