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
//

#include "impspan.h"

#include "impsmtm.h"
#include <msventry.h>
#include <imapset.h>
#include <miutset.h>
#include <offop.h>
#include <msvreg.h>
#include <imapcmds.h>
#include <logwrap.h>
#include <imcvutil.h>
#include <cemailaccounts.h>

#include "imapsess.h"
#include "fldsync.h"
#include "imapsync.h"
#include "imapcomp.h"
#include "imapoffl.h"
#include "impsutil.h"

#ifdef _DEBUG
#define LOG_COMMANDS(a) iPrimarySession->iSession->LogText a
#define DBG(a) a
#define PRINTING
#else
#define LOG_COMMANDS(a)
#define DBG(a)
#undef PRINTING
#endif

// Type of MTM request: used only in this file
enum
	{
	EMtmCopyToLocal=1,
	EMtmMoveToLocal,
	EMtmCopyFromLocal,
	EMtmMoveFromLocal,
	EMtmCopyWithinService,
	EMtmMoveWithinService,
	EMtmPopulate
	};

// Code for the active wrapper
CActiveWrapper* CActiveWrapper::NewL(TInt aID)
	{
	CActiveWrapper* self=new(ELeave)CActiveWrapper(aID);
	CleanupStack::PushL(self);
	self->ConstructL();
	CActiveScheduler::Add( self );
	CleanupStack::Pop();
	return self;
	}

CActiveWrapper::CActiveWrapper(TInt aID) : CActive(0), iID(aID)
	{
	}

void CActiveWrapper::ConstructL()
	{
	// The object is a session
	iSession=CImImap4Session::NewL(iID, *this);

	// Get a compound operation
	iCompound=CImImap4Compound::NewL(iSession);

	// If we're the primary session, we get a fullsync object too
	if (iID==1)
		iFullSync=CImImap4Synchronise::NewL(iSession);
	}

void CActiveWrapper::StartL(MActiveWrapperObserver* aManager)
	{
	DBG((iSession->LogText(_L8("CActiveWrapper::StartL(ID=%d)"),iID)));

	// The request has already been issued with our iStatus, so here all we
	// do is note down the completion information and do a SetActive()
	iParentPtr=aManager;
	// Activate the observer as well, in case the request is cancelled.
	iParentPtr->Activate();
	SetActive();
	}

// Pointer to ActiveWrapper's iStatus, so direct commands to session (see above)
// use the correct iStatus.
TRequestStatus* CActiveWrapper::SessionStatus()
	{
	// Return the pointer
	return(&iStatus);
	}

// Set entry pointers for classes the wrapper owns
void CActiveWrapper::SetEntry(CMsvServerEntry* aEntry)
	{
	iSession->SetEntry(aEntry);
	iCompound->SetEntry(aEntry);
	if (iFullSync)
		iFullSync->SetEntry(aEntry);
	}

void CActiveWrapper::DoCancel()
	{
	DBG((iSession->LogText(_L8("CActiveWrapper::DoCancel(ID=%d)"),iID)));

	// Cancel anything we might have outstanding
	if (iFullSync)
		iFullSync->Cancel();
	iCompound->Cancel();
	if( !iSession->IsCancelling())
		{
		// Can only cancel the session if we're not waiting for a cancelled fetch
		// to complete.
		iSession->Cancel();	
		}

	// Tell parent we've completed: DON'T complete it ourselves.
	iParentPtr->RequestCompleted(iID,KErrCancel);
	}

#ifdef PRINTING
void CActiveWrapper::DoComplete(TInt aStatus)
#else
void CActiveWrapper::DoComplete(TInt /*aStatus*/)
#endif
	{
	DBG((iSession->LogText(_L8("CActiveWrapper::DoComplete(%d, id=%d)"),aStatus,iID)));
	}

void CActiveWrapper::RunL()
	{
	// IMPORTANT: The contents of this CActiveWrapper::RunL() MUST NOT ever User::Leave()
	DBG((iSession->LogText(_L8("ActiveWrapper RunL(ID=%d, result=%d)"),iID,iStatus.Int())));

	TInt error=iStatus.Int();
	Cancel(); // Obvious to do it here
	iParentPtr->RequestCompleted(iID,error);
	}

CActiveWrapper::~CActiveWrapper()
	{
	Cancel();
	delete iSession;
	delete iCompound;
	delete iFullSync;
	}

void CActiveWrapper::NonCompletedFailure()
	{
	DBG((iSession->LogText(_L8("ActiveWrapper::NonCompletedFailure (%d)"), iID)));

	// A failure has occured on the session, but there is no outstanding asynchronous
	// request on it. This can happen if for instance we get a disconnect while doing
	// a cancel and idle operation. Pass the failure on to the parent.

	iParentPtr->NonCompletedFailureOnSession(iID);
	}

// Return of current imap session
CImImap4Session* CActiveWrapper::GetImap4Session()
	{
	return iSession;	
	}

// -----------------------------------------------------------------------

// The actual MTM
EXPORT_C CImap4ServerMtm* CImap4ServerMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll,CMsvServerEntry* aEntry)
	{
	CImap4ServerMtm* self=new CImap4ServerMtm(aRegisteredMtmDll,aEntry);
	if (self==NULL)
		{
		aRegisteredMtmDll.ReleaseLibrary();
		User::Leave(KErrNoMemory);
		}
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CImap4ServerMtm::ConstructL()
	{
	// Make an Imap4 session: make it under the activewrapper, so we can have
	// more than one at a time

	// The primary session is the one that does the full sync
	iPrimarySession=CActiveWrapper::NewL(1);

	// The secondary session is used if we get fetch requests while the
	// primary session is busy
	iSecondarySession=CActiveWrapper::NewL(2);

	// create an offline op controller
	iOffLineControl=CImap4OffLineControl::NewL(iServerEntry, iPrimarySession->iSession);

	// create the utils object
	iUtils = CImap4Utils::NewL(iServerEntry);
	
	// tell the FullSync session about them
	iPrimarySession->iFullSync->SetOffLineControl(iOffLineControl);
	iPrimarySession->iFullSync->SetUtils(iUtils);

	// Get an entry selection
	iSelection=new (ELeave) CMsvEntrySelection;

	// We're disconnected at first
	iLastSessionState=TImap4GenericProgress::EDisconnected;

	// We need to see invisible entries
	TMsvSelectionOrdering invisible;
	invisible=iServerEntry->Sort();
	invisible.SetShowInvisibleEntries(ETrue);
	iServerEntry->SetSort(invisible);

	// Tell sessions and foldersync about it
	iPrimarySession->SetEntry(iServerEntry);
	iSecondarySession->SetEntry(iServerEntry);
	}

// MTM destructor: clean up - this involves removing the sessions.
CImap4ServerMtm::~CImap4ServerMtm()
	{
	// We may still be active
	Cancel();

	// Ensure service is marked as offline (if the serviceid was initialised, anyway)
	if (iServiceId && iServiceSettings)
		{
		// Make stuff invisible as necessary
		TRAP_IGNORE(MarkOnOrOfflineL(EFalse));

		// Get rid of settings
		delete iServiceSettings;
		}

	// Clean up
	delete iSelection;
	delete iOneSelection;
	delete iPrimarySession;
	delete iSecondarySession;
	delete iOffLineControl;
	delete iUtils;
	}

// Do setentry, leave if there is an error
void CImap4ServerMtm::SetEntryL(const TMsvId aId)
	{
	User::LeaveIfError(iServerEntry->SetEntry(aId));
	}

// Change entry, leave if error
void CImap4ServerMtm::ChangeEntryL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iServerEntry->ChangeEntry(aEntry));
	}

// Get children, leave if error
void CImap4ServerMtm::GetChildrenL(CMsvEntrySelection& aSelection)
	{
	User::LeaveIfError(iServerEntry->GetChildren(aSelection));
	}

// remove an id, leave if error, moves to the parent first
void CImap4ServerMtm::DeleteEntryL(TMsvId aId)
	{
	SetEntryL(aId);
	SetEntryL(iServerEntry->Entry().Parent());
	User::LeaveIfError(iServerEntry->DeleteEntry(aId));
	}

// Hierarchically make all folders visible/invisible: we go down until we can't
// find any more folders, so this won't affect messages with subfolders.
//
// We don't use the bulk functions as we don't want to affect messages in the
// folder, and we have to scan the entries to check for recursion anyway.
void CImap4ServerMtm::ChangeVisibilityL(TMsvId aParent, TBool aInvisible)
	{
	ChangeVisibilityL(aParent, aInvisible, ETrue, KUidMsvFolderEntry);
	}

void CImap4ServerMtm::ChangeVisibilityL(TMsvId aParent, TBool aInvisible, TBool aRecurse, TUid aType)
	{
	DBG((iPrimarySession->iSession->LogText(_L8("ChangeVisibilityL(%x, %d)"),aParent,aInvisible)));

	// Get children at this level
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);

	CMsvEntrySelection* folders=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(folders);

	SetEntryL(aParent);
	GetChildrenL(*selection);

	if (selection->Count())
		{
		DBG((iPrimarySession->iSession->LogText(_L8("  Found %d children"),selection->Count())));

		for(TInt child=0;child<selection->Count();child++)
			{
			// Move to this child
			SetEntryL((*selection)[child]);
			TMsvEntry message=iServerEntry->Entry();

			DBG((iPrimarySession->iSession->LogText(_L8("  type %x visible %d"),message.iType,message.Visible())));
			
			// Is this the type we want to change?
			if (message.iType==aType)
				{
				// Add to selection to do bulk change on, if necessary
				if ((message.Visible() && aInvisible) ||
					(!message.Visible() && !aInvisible))
					{
					DBG((iPrimarySession->iSession->LogText(_L8("  Adding %x to change list"),message.Id())));

					folders->AppendL(message.Id());
					}
				}
			
			// Recurse downwards
			if (aRecurse && message.iType==KUidMsvFolderEntry)
				ChangeVisibilityL(message.Id(),aInvisible,aRecurse,aType);
			}

		// Change its visibility off all children if necessary
		if (folders->Count())
			{
			// Do the change to the invisible flag (actual constant for the
			// flag we want is private :( )
			SetEntryL(aParent);
			User::LeaveIfError(iServerEntry->ChangeAttributes(*folders,
								aInvisible?0:KMsvVisibilityAttribute,
								aInvisible?KMsvVisibilityAttribute:0));
			}
		}

	// Release the service entry
	SetEntryL(KMsvNullIndexEntryId);

	// Get rid of selection
	CleanupStack::PopAndDestroy(2);
	}
void CImap4ServerMtm::ClearNewFlagL(TMsvId aParent)
	{
	// Get children at this level
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	CMsvEntrySelection* msgs=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msgs);
	SetEntryL(aParent);
	GetChildrenL(*selection);
	// for each child
	TInt count=selection->Count();
	for(TInt child=0;child<count;child++)
		{
		SetEntryL((*selection)[child]);
		TMsvEntry entry=iServerEntry->Entry();
		if (entry.New()&& entry.iType==KUidMsvMessageEntry)
			msgs->AppendL(entry.Id());
		// if this is a folder then recurse
		if (entry.iType==KUidMsvFolderEntry)
			ClearNewFlagL((*selection)[child]);
		}
	if (msgs->Count())// change attribute only when it has finished looking at all the children
		{
		SetEntryL(aParent);
		User::LeaveIfError(iServerEntry->ChangeAttributes(*msgs,0,KMsvNewAttribute));
		iPrimarySession->iSession->LogText(_L8("number of new msgs = (%d)"),msgs->Count());
		}
	CleanupStack::PopAndDestroy(2); // selection,msgs
	}



// Mark service as on or offline
void CImap4ServerMtm::MarkOnOrOfflineL(const TBool aOnline)
	{
	// Mark service entry as on/offline	
	SetEntryL(iServiceId);
	TMsvEntry entry=iServerEntry->Entry();
	entry.SetConnected(aOnline);
	ChangeEntryL(entry);

	// Release the service entry
	SetEntryL(KMsvNullIndexEntryId);

	// Going offline?
	if (!aOnline && iServiceSettings->DisconnectedUserMode())
		{
		// We're an expert user going offline: don't touch anything
		return;
		}

	// Mark all immediate children of the service as invisible
	if (!aOnline)
		ChangeVisibilityL(iServiceId,!aOnline);
	}

// Do the misc bits we need to when we've gone offline
void CImap4ServerMtm::GoneOffline()
	{
	DBG((iPrimarySession->iSession->LogText(_L8("GoneOffline called"))));

	// Cancel sessions: parking might be dangerous otherwise
	iPrimarySession->Cancel();
	iSecondarySession->Cancel();

	// Park entries (in case a move entry has been left somewhere dangerous by a
	// leave)
	iPrimarySession->iSession->Park();
	iSecondarySession->iSession->Park();

	// Only do this is we have a valid service - we may not, due to being called
	// both by the state machine and the progress entry
	if (iServiceId)
		{
		// Offline, make folders invisible if we're not a disconnected mode user
		TRAP_IGNORE(MarkOnOrOfflineL(EFalse));
	
		// Clear serviceid, it's not valid anymore
		iServiceId=0;
		}

	// Settings can be dumped now
	delete iServiceSettings;
	iServiceSettings=NULL;

	// We can now be deleted
	iCanBeDeletedNow=ETrue;
	}

// Check selection: all entries should be of appropriate
// type. Messages in this context means complete messages, not RFC888
// parts of another message. parts means attachment or text
// bodies. Make a local copy of valid entries

TInt CImap4ServerMtm::CheckSelectionL(const CMsvEntrySelection& aSelection,
									  CMsvEntrySelection* aLocalCopy,
									  const TBool aMessages,
									  const TBool aParts,
									  const TBool aFolders,
									  const TBool aIsInService)
	{
	// Reset copy selection
	aLocalCopy->Reset();

	// Check all entries are messages
	for(TInt a=0;a<aSelection.Count();a++)
		{
		// Does entry exist?
		TBool addIt = EFalse;

		if (iServerEntry->SetEntry(aSelection[a])==KErrNone)
			{
			TUid type = iServerEntry->Entry().iType;
			if ((aMessages && type==KUidMsvMessageEntry) ||
				(aParts && (type==KUidMsvEmailTextEntry || type==KUidMsvAttachmentEntry || type==KUidMsvMessageEntry)) ||
				(aFolders && type==KUidMsvFolderEntry))
				{
				TBool inEnclosingMessage=EFalse;

				// Do we need to check if it's in the local service or
				// if it is a complete message
				if (aIsInService || (!aParts && type==KUidMsvMessageEntry))
					{
					// Work up the tree until we get to the service or the root
					do
						{
						SetEntryL(iServerEntry->Entry().Parent());
						if (iServerEntry->Entry().iType==KUidMsvMessageEntry)
							inEnclosingMessage=ETrue;
						}
					while(iServerEntry->Entry().iType!=KUidMsvServiceEntry &&
						  iServerEntry->Entry().Id()!=KMsvRootIndexEntryId);		
	
					// Are we at the service that this MTM referrs to?
					// SJM: if offline iServiceId==0 so allow all
					if (!aIsInService || iServiceId==0 || iServerEntry->Entry().Id()==iServiceId)
						{
						// it's OK if it is not a message type (in
						// which case it has already been checked and
						// passed) or it is not within an enclosing message
						if (type!=KUidMsvMessageEntry || !inEnclosingMessage || aParts)
							addIt = ETrue;
						}
					}
				else
					{
					// Add to local copy
					addIt = ETrue;
					}
				}
			}

		if (addIt)
			aLocalCopy->AppendL(aSelection[a]);
#ifdef _DEBUG
		// UI shouldn't really be giving us bogus items so panic
		else
			gPanic(EInvalidMsvTypeToCommand);
#endif
		}

	// Anything to do?
	if (!aLocalCopy->Count())
		{
		// Nothing valid to work with
		User::RequestComplete(iRequest,KErrNotSupported);
		return(KErrNotSupported);
		}
	
	// All OK, the selection isn't empty
	return(KErrNone);
	}

// aId has been unsubscribed. If it has no visible child folders then
// make it invisible and check its parent with the same test
void CImap4ServerMtm::PropagateInvisibleFlagL(TMsvId aId)
	{
	DBG((iPrimarySession->iSession->LogText(_L8("PropagateInvisibleFlagL: 0x%x"), aId)));

	// finish if we've reached the top
	if (aId == KMsvRootIndexEntryId)
		return;
	
	SetEntryL(aId);

	// finish if we've reached a service
	if (iServerEntry->Entry().iType == KUidMsvServiceEntry)
		return;

	// return if we've found a subscribed folder since we can't make
	// it invisible
	if (((TMsvEmailEntry)iServerEntry->Entry()).LocalSubscription())
		return;
	
	// check the children of this unsubscribed folder
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);

	GetChildrenL(*selection);

	TBool visible=EFalse;
	for (TInt i=0; i < selection->Count(); i++)
		{
		SetEntryL((*selection)[i]);

		// look for a visible folder
		TMsvEmailEntry entry = (TMsvEmailEntry)iServerEntry->Entry();
		if (entry.iType == KUidMsvFolderEntry && entry.Visible())
			{
			visible=ETrue;
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(); // selection

	// if no child folders were visible then make this folder
	// invisible and continue up
	if (!visible)
		{
		SetEntryL(aId);
		
		// make this invisible
		TMsvEntry entry = iServerEntry->Entry();
		entry.SetVisible(EFalse);
		ChangeEntryL(entry);

		// go up
		PropagateInvisibleFlagL(entry.Parent());
		}
	}

// Simply change local subscription flag on a folder immediately: This is used
// as opposed to ChangeL() as this operation can occur offline.

// SJM: If SubscribeStrategy is UpdateRemote or UpdateBoth then doing
// this should trigger a RemoteSubscription?

TInt CImap4ServerMtm::SetLocalSubscription(const TMsvId aFolder, TBool aSubscribed)
	{
	TInt err;

	// Move to the entry
	if ((err=iServerEntry->SetEntry(aFolder))!=KErrNone)
		return(err);

	// Check it's a folder
	if (iServerEntry->Entry().iType!=KUidMsvFolderEntry)
		return(KErrNotSupported);

	DBG((iPrimarySession->iSession->LogText(_L8("SetLocalSubscription: 0x%x %d"), aFolder, aSubscribed)));
		
	// Twiddle flag
	TMsvEmailEntry entry=iServerEntry->Entry();
	entry.SetLocalSubscription(aSubscribed);
	return(iServerEntry->ChangeEntry(entry));
	}


// Perform a general move/copy function: this tidies up a lot of the Copy/Move
// MTM specifics into one place
void CImap4ServerMtm::MtmCommandL(const TInt aType,const CMsvEntrySelection& aSelection,
								  TMsvId aDestination, TRequestStatus& aStatus)
	{
	iRequest=&aStatus;
	// reset error code
	iProgressErrorCode=KErrNone;
	
	// Is source remote? Ensure we check it's from our service
	TBool checksource=ETrue;
	if (aType==EMtmCopyFromLocal || aType==EMtmMoveFromLocal)
		checksource=EFalse;

	// we can only handle parts of messages on Populate
	TBool handleParts=EFalse;
	if (aType==EMtmPopulate||aType==EMtmCopyToLocal)
		handleParts=ETrue;
	
	// Check selection contains messages, or maybe parts
	if (CheckSelectionL(aSelection,iSelection,ETrue,handleParts,EFalse,checksource))
		return;

	// Save destination
	iDestination=aDestination;

	// Are we online?
	if (!iPrimarySession->iSession->Connected())
		{
		iProgressMsgsToDo=iSelection->Count();
		iProgressMsgsDone=0;

		// set state for dorunl
		switch(aType)
			{
		case EMtmCopyToLocal:			
			iState=iSavedState=EMtmStateOffLineCopyToLocal;
			iRequestedOperation=TImap4GenericProgress::EOffLineCopyToLocal;
			break;

		case EMtmCopyFromLocal:
			iState=iSavedState=EMtmStateOffLineCopyFromLocal;
			iRequestedOperation=TImap4GenericProgress::EOffLineCopyFromLocal;
			break;

		case EMtmCopyWithinService:
			iState=iSavedState=EMtmStateOffLineCopyWithinService;
			iRequestedOperation=TImap4GenericProgress::EOffLineCopyWithinService;
			break;

		case EMtmMoveToLocal:
			iState=iSavedState=EMtmStateOffLineMoveToLocal;
			iRequestedOperation=TImap4GenericProgress::EOffLineMoveToLocal;
			break;

		case EMtmMoveFromLocal:
			iState=iSavedState=EMtmStateOffLineMoveFromLocal;
			iRequestedOperation=TImap4GenericProgress::EOffLineMoveFromLocal;
			break;

		case EMtmMoveWithinService:
			iState=iSavedState=EMtmStateOffLineMoveWithinService;
			iRequestedOperation=TImap4GenericProgress::EOffLineMoveWithinService;
			break;

		case EMtmPopulate:
			{
			iState=iSavedState=EMtmStateOffLinePopulate;
			iRequestedOperation=TImap4GenericProgress::EOffLinePopulate;
			break;
			}
		default:
			break;
			}
		TRequestStatus* pS=(&iStatus);
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	else
		{
		TRequestStatus* status;

		// Default to primary session
		iCurrentSession=iPrimarySession;
		status=iCurrentSession->SessionStatus();

		// Initialise messages to do, etc
		iProgressMsgsToDo=iSelection->Count();
		iProgressMsgsDone=0;

		switch(aType)
			{
		case EMtmPopulate:
		case EMtmCopyToLocal:
			// We're online: is the primary session busy?
 			if( iBackgroundSyncInProgress || iCurrentSession->iSession->IsCancelling() ||
 							 		iPrimarySession->iSession->Busy())
				{
				DBG((iPrimarySession->iSession->LogText(_L8("Primary session busy, using secondary"))));

				// We'll have to use the secondary session
				iCurrentSession=iSecondarySession;

				// Is the secondary session connected?
				if (!iCurrentSession->iSession->Connected())
					{
					DBG((iPrimarySession->iSession->LogText(_L8("Connecting secondary"))));

					// We need to connect the secondary session before
					// continuing with the fetch. Do it. The source message
					// selection has been checked and stored in iSelection,
					// so it should be safe until the connect goes through
					// for later issue.
					iState=EMtmStateSecondaryConnect;
					iSavedState=aType == EMtmPopulate ? EMtmStatePopulate : EMtmStateCopyToLocal;

					// Issue the connect
					iCurrentSession->StartL(this);
					status=iCurrentSession->SessionStatus();
					// Providing the reference of primary session 
					iCurrentSession->iSession->SetPrimarySession(iPrimarySession);
					iCurrentSession->iSession->ConnectL(*status,iServiceId);
					break;
					}
				else if( iCurrentSession->iSession->IsCancelling() )
					{
					// Opps! Cannot do the populate currently - server busy.
					DBG((iPrimarySession->iSession->LogText(_L8("Secondary already cancelling"))));
					
					User::RequestComplete(iRequest,KErrServerBusy);
					return;
					}

				DBG((iPrimarySession->iSession->LogText(_L8("Secondary already connected, reusing"))));
				}

			// Calculate the total size of the messages to be downloaded in this selection.
			iTotalSize = iCurrentSession->iSession->CalculateDownloadSizeL(*iSelection);

			// Select relevant folder
			iCurrentSession->StartL(this);
			status=iCurrentSession->SessionStatus();
			iUtils->SetUpLogMessageL((*iSelection)[0]);
			if (aType == EMtmPopulate)
				{
				iState = EMtmStatePopulate;
				// compound need to know the number of msgs to suspend imap idle calls if > 1
				if(iProgressMsgsToDo > 0)
					{
					iCurrentSession->iCompound->SetMessageCount(iProgressMsgsToDo);
					}
				DBG((iPrimarySession->iSession->LogText(_L8("CImapMTM::MTMCommandL(): Calling iCurrentSession->iCompound->Populate()"))));
 				iCurrentSession->iCompound->PopulateL(*status,(*iSelection)[0],iPartialMailInfo);
				}
			else
				{
				iState = EMtmStateCopyToLocal;
				iCurrentSession->iCompound->CopyToLocalL(*status,(*iSelection)[0],iDestination);
				}
			break;

		case EMtmMoveToLocal:
			// We're online, are we busy? (it's a write op, and so we must use the
			// primary session)
			if( iBackgroundSyncInProgress || iCurrentSession->iSession->IsCancelling() )
				{
				// Can't do it, we're busy
				User::RequestComplete(iRequest,KErrServerBusy);
				return;
				}

			// Calculate the total size of the messages to be downloaded in this selection.
			iTotalSize = iCurrentSession->iSession->CalculateDownloadSizeL(*iSelection);

			// Select relevant folder
			iCurrentSession->StartL(this);
			iState=EMtmStateMoveToLocal;
			iUtils->SetUpLogMessageL((*iSelection)[0]);
			iCurrentSession->iCompound->MoveToLocalL(*status,(*iSelection)[0],iDestination);
			break;

		case EMtmCopyFromLocal:
		case EMtmMoveFromLocal:

			// As this is a write operation, is must be done on the primary session:
			// is it free?
			if( iBackgroundSyncInProgress || iCurrentSession->iSession->IsCancelling() )
				{
				// Can't do it
				User::RequestComplete(iRequest,KErrServerBusy);
				return;
				}

			// Calculate the total size of the messages to be downloaded in this selection.
			iTotalSize = iCurrentSession->iSession->CalculateDownloadSizeL(*iSelection);

			iState=(aType==EMtmCopyFromLocal)?EMtmStateCopyFromLocal:EMtmStateMoveFromLocal;


			// Start the first operation
			iCurrentSession->StartL(this);
			if (aType==EMtmCopyFromLocal)
				iCurrentSession->iCompound->CopyFromLocalL(*status,(*iSelection)[0],iDestination);
			else
				iCurrentSession->iCompound->MoveFromLocalL(*status,(*iSelection)[0],iDestination);
			break;

		case EMtmCopyWithinService:
		case EMtmMoveWithinService:
			// As this is a write operation, is must be done on the primary session:
			// is it free?
			if( iBackgroundSyncInProgress || iCurrentSession->iSession->IsCancelling() )
				{
				// Can't do it
				User::RequestComplete(iRequest,KErrServerBusy);
				return;
				}

			// Set the state, as we may be dealing with multiple entries
			iState=(aType==EMtmCopyWithinService)?EMtmStateCopyWithinService:
												  EMtmStateMoveWithinService;

			// Do the copy with the compound
			iCurrentSession->StartL(this);
			if (aType==EMtmCopyWithinService)
				iCurrentSession->iCompound->CopyWithinServiceL(*status,*iSelection,iDestination);
			else
				iCurrentSession->iCompound->MoveWithinServiceL(*status,*iSelection,iDestination);
			break;
			}
		}
	}


// CopyToLocal fetches message parts into the mirror: aDestination is currently
// not used. This is the only command which can cause the second session to
// connect, as it is for fetching *only* (not 'modifiable' commands which may
// cause the current synchronisation in the primary session to hiccup). This
// also rules out MoveToLocal, as it involves a delete on the remote server.
void CImap4ServerMtm::CopyToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;

	LOG_COMMANDS((_L8("MTMCOMMAND CopyToLocal(%d items to %x)"),aSelection.Count(),aDestination));

	iRequestedOperation=TImap4GenericProgress::ECopyToLocal;
	iGetMailOptions=EGetImap4EmailBodyTextAndAttachments;
	MtmCommandL(EMtmCopyToLocal,aSelection,aDestination,aStatus);
	}

// Move To Local moves a message from the remote server to a local folder:
// this is performed as a fetch, then a delete on the remote server.
void CImap4ServerMtm::MoveToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;

	LOG_COMMANDS((_L8("MTMCOMMAND MoveToLocal(%d items to %x)"),aSelection.Count(),aDestination));

	// We can't do a 'movetolocal' with the destination being the same as the
	// source, as then the mirror will be out of sync. We must be moving to somewhere
	// outside this service. Check this.
	if (!iOffLineControl->IdIsLocalL(aDestination))
		{
		TRequestStatus* status=&aStatus;
		User::RequestComplete(status,KErrNotSupported);
		}
	else
		{
		iRequestedOperation=TImap4GenericProgress::EMoveToLocal;
		iGetMailOptions=EGetImap4EmailBodyTextAndAttachments;
		MtmCommandL(EMtmMoveToLocal,aSelection,aDestination,aStatus);
		}
	}

// CopyFromLocal appends entire messages in the mirror to the server.
void CImap4ServerMtm::CopyFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;

	LOG_COMMANDS((_L8("MTMCOMMAND CopyFromLocal(%d items to %x)"),aSelection.Count(),aDestination));

	iRequestedOperation=TImap4GenericProgress::ECopyFromLocal;
	MtmCommandL(EMtmCopyFromLocal,aSelection,aDestination,aStatus);
	}

// Does a CopyFromLocal (ie, IMAP APPEND of the message), then deletes the local message
// if it was sucessful.
void CImap4ServerMtm::MoveFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;

	LOG_COMMANDS((_L8("MTMCOMMAND MoveFromLocal(%d items to %x)"),aSelection.Count(),aDestination));

	iRequestedOperation=TImap4GenericProgress::EMoveFromLocal;
	MtmCommandL(EMtmMoveFromLocal,aSelection,aDestination,aStatus);
	}

// CopyWithinService copies entire messages to other folders on the server, using
// the IMAP COPY command.
void CImap4ServerMtm::CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;

	LOG_COMMANDS((_L8("MTMCOMMAND CopyWithinService(%d items to %x)"),aSelection.Count(),aDestination));

	iRequestedOperation=TImap4GenericProgress::ECopyWithinService;
	MtmCommandL(EMtmCopyWithinService,aSelection,aDestination,aStatus);
	}

// MoveWithinService copies entire messages to other folders on the server, using
// the IMAP COPY command, then deletes the original if the copy was sucessful,
// so performing a move.
void CImap4ServerMtm::MoveWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;

	LOG_COMMANDS((_L8("MTMCOMMAND MoveWithinService(%d items to %x)"),aSelection.Count(),aDestination));

	iRequestedOperation=TImap4GenericProgress::EMoveWithinService;
	MtmCommandL(EMtmMoveWithinService,aSelection,aDestination,aStatus);
	}

void CImap4ServerMtm::UndeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	LOG_COMMANDS((_L8("MTMCOMMAND UndeleteAll(%d items)"),aSelection.Count()));
	if(IsActive())
		{
		TRequestStatus* request = &aStatus;
		User::RequestComplete(request,KErrImapServerBusy);
		return;
		}
		
	iRequest=&aStatus;
	iProgressErrorCode=KErrNone;

	// Check selection contains only messages in the service
	if (CheckSelectionL(aSelection,iSelection,ETrue,EFalse,EFalse,ETrue))
		return;

	iRequestedOperation=TImap4GenericProgress::EOffLineUndelete;

	iProgressMsgsToDo=iSelection->Count();
	iProgressMsgsDone=0;
	iState=iSavedState=EMtmStateOffLineUndelete;

	TRequestStatus* pS=(&iStatus);
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}


void CImap4ServerMtm::DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;

	LOG_COMMANDS((_L8("MTMCOMMAND DeleteAll(%d items)"),aSelection.Count()));

	iRequest=&aStatus;
	// reset error code
	iProgressErrorCode=KErrNone;

	if (PruneMessages(aSelection))
		// Was this call to DeleteAllL an instruction to delete local parts of a message ?
		// If so then we don't need to continue.
		return;
   
	// Are we online?
	if (!iPrimarySession->iSession->Connected())
		{
		// SJM: Surely we need something like...
		// Check selection contains only messages - not folders
		if (CheckSelectionL(aSelection,iSelection,ETrue,EFalse,EFalse,EFalse))
			return;

		iRequestedOperation=TImap4GenericProgress::EOffLineDelete;

		iProgressMsgsToDo=iSelection->Count();
		iProgressMsgsDone=0;
		iState=iSavedState=EMtmStateOffLineDelete;

		TRequestStatus* pS=(&iStatus);
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	else
		{
		iRequestedOperation=TImap4GenericProgress::EDelete;

		// We need to do this in the primary session: is it busy?
		if (iBackgroundSyncInProgress || iPrimarySession->iSession->IsCancelling() ||
		    iPrimarySession->iSession->Busy())
			{
			// It's busy, we can't do it
			User::RequestComplete(iRequest,KErrServerBusy);
			return;
			}

		// Message or folder to delete?
		DBG((iPrimarySession->iSession->LogText(_L8("First is %x"),aSelection[0])));

		SetEntryL(aSelection[0]);
		if (iServerEntry->Entry().iType==KUidMsvMessageEntry)
			{
			DBG((iCurrentSession->iSession->LogText(_L8("Delete message"))));

			// Check selection contains only messages
			if (CheckSelectionL(aSelection,iSelection,ETrue,EFalse,EFalse,ETrue))
				return;

			iProgressMsgsToDo=iSelection->Count();
			iProgressMsgsDone=0;
			// Select parent folder of message
			iCurrentSession=iPrimarySession;
			iState=iSavedState=EMtmStateDelete;
			TRequestStatus* status=iCurrentSession->SessionStatus();
			iCurrentSession->iCompound->DeleteL(*status,*iSelection);
			iCurrentSession->StartL(this);
			}
		else if (iServerEntry->Entry().iType==KUidMsvFolderEntry)
			{
			DBG((iCurrentSession->iSession->LogText(_L8("Delete folder"))));

			// Check selection contains only folders
			if (CheckSelectionL(aSelection,iSelection,EFalse,EFalse,ETrue,ETrue))
				return;

			// Delete folder
			iCurrentSession=iPrimarySession;
			iState=iSavedState=EMtmStateDeleteFolder;
			TRequestStatus* status=iCurrentSession->SessionStatus();
			iCurrentSession->iCompound->DeleteFolderL(*status,(*iSelection)[0]);
			iCurrentSession->StartL(this);
			}
		else
			gPanic(EDeleteOfUnknownType);
		}
	}

// Create: make a folder or mailbox. Can only happen online
void CImap4ServerMtm::CreateL(TMsvEntry aNewEntry, TRequestStatus& aStatus)
	{
	LOG_COMMANDS((_L8("MTMCOMMAND Create(parent %x)"),aNewEntry.Parent()));

	aStatus=KRequestPending;
	iRequest=&aStatus;
	// reset error code
	iProgressErrorCode=KErrNone;

	// Creating a folder?
	if (aNewEntry.iType!=KUidMsvFolderEntry)
		{
		// No - illegal op
		User::RequestComplete(iRequest,KErrNotSupported);
		return;
		}

	// Are we online and not busy?
	if( iBackgroundSyncInProgress || iPrimarySession->iSession->IsCancelling() )
		{
		User::RequestComplete(iRequest,KErrServerBusy);
		return;
		}

	// Folder or mailbox?
	TBool isfolder=ETrue;
	if (((TMsvEmailEntry)aNewEntry).Mailbox())
		isfolder=EFalse;

	// Use primary session: issue create folder command
	iState=EMtmStateCreateFolder;
	iCurrentSession=iPrimarySession;
	TRequestStatus* status=iCurrentSession->SessionStatus();
	iCurrentSession->iCompound->CreateL(*status,aNewEntry.Parent(),aNewEntry.iDetails,isfolder);
	iCurrentSession->StartL(this);
	}

void CImap4ServerMtm::ChangeL(TMsvEntry aNewEntry, TRequestStatus& aStatus)
	{
	LOG_COMMANDS((_L8("MTMCOMMAND Change(%x)"),aNewEntry.Id()));

	User::LeaveIfError(iServerEntry->SetEntry( aNewEntry.Id() ));
	User::LeaveIfError(iServerEntry->ChangeEntry( aNewEntry ));

	iRequest=&aStatus;
	// reset error code
	iProgressErrorCode=KErrNone;
	
	User::RequestComplete(iRequest, KErrNone);
	}

// SJM: Old ChangeL entry. used to be able to update flags as well as
// rename folder. Change to only rename folder as its not obvious how
// you'd pass the flags across
TBool CImap4ServerMtm::RenameFolderL(TMsvId aId, const TImap4RenameFolder& aRename)
	{
	LOG_COMMANDS((_L8("MTMCOMMAND RenameFolder(%x) to %S"),aId,&aRename.iNewName));

	// Are we online and not busy?
	if( iBackgroundSyncInProgress || iPrimarySession->iSession->IsCancelling() )
		{
		User::RequestComplete(iRequest,KErrServerBusy);
		return EFalse;
		}

	// Issue a rename command on this object
	iState=EMtmStateRenameFolder;
	iCurrentSession=iPrimarySession;
	TRequestStatus* status=iCurrentSession->SessionStatus();
	iCurrentSession->iCompound->RenameL(*status, aId, aRename.iNewName);
	return ETrue;
	}

// id can be either the service or anything below it. If it is not the
// service then we move up the tree until we find the service.
void CImap4ServerMtm::LoadSettingsL(TMsvId aId)
	{
	// Old settings shouldn't be hanging about, but...
	if (iServiceSettings)
		{
		delete iServiceSettings;
		iServiceSettings=NULL;
		}

	// Get somewhere to store service settings
	iServiceSettings=new (ELeave) CImImap4Settings;

	// find the service
	SetEntryL(aId);
	while (iServerEntry->Entry().iType != KUidMsvServiceEntry)
		SetEntryL(iServerEntry->Entry().Parent());

	// Get settings for this service
	CEmailAccounts* account = CEmailAccounts::NewLC();
	TImapAccount id;
	id.iImapAccountId = iServerEntry->Entry().MtmData2();  // iMtmData2 of the service entry contains TImapAccountId
	id.iImapAccountName = iServerEntry->Entry().iDetails;
	id.iImapService = iServerEntry->Entry().iServiceId;
	id.iSmtpService = iServerEntry->Entry().iRelatedId;

	account->LoadImapSettingsL(id, *iServiceSettings);
   	CleanupStack::PopAndDestroy(account);    
	}

void CImap4ServerMtm::StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus)
	{
	LOG_COMMANDS((_L8("MTMCOMMAND StartCommand(%x)"),aCommand));
	
	iTotalSize = 0;
	aStatus=KRequestPending;
	iRequest=&aStatus;
	// reset error code
	iProgressErrorCode=KErrNone;
	
	if(iPrimarySession->iSession->IsActive())
		LOG_COMMANDS((_L8("CImap4ServerMTM::StartCommand(): iPrimarySession->iSession is ACTIVE")));
	else
		LOG_COMMANDS((_L8("CImap4ServerMTM::StartCommand(): iPrimarySession->iSession is NOT ACTIVE")));

        
	// Who are we sending this request to? Get their session
	iCurrentSession=iPrimarySession;
	TRequestStatus* status=iCurrentSession->SessionStatus();
	
	// Certain commands require that we are online: check that we're online (the command may have
	// been queued when we were online, but we've lost the connection now)
	if (aCommand==KIMAP4MTMSynchronise || aCommand==KIMAP4MTMFullSync ||
		aCommand==KIMAP4MTMInboxNewSync || aCommand==KIMAP4MTMFolderFullSync ||
		aCommand==KIMAP4MTMRenameFolder || aCommand==KIMAP4MTMDisconnect ||
		aCommand==KIMAP4MTMSyncTree || aCommand==KIMAP4MTMSelect)
		{
		if (!iPrimarySession->iSession->Connected())
			{
			// Can't hack this!
			User::RequestComplete(iRequest,KErrDisconnected);
			return;
			}
		}
	
	// Save command, as when it completes we would like to know what happened...
	iState=EMtmStateMiscCommand;
	TBool notComplete=EFalse;
	switch(iLastCommand=aCommand)
		{
		case KIMAP4MTMIsConnected:
			// Are we connected?
			User::RequestComplete(iRequest,
								  iPrimarySession->iSession->Connected()?KErrNone:KErrDisconnected);
			break;

		case KIMAP4MTMBusy:
			// Are we busy?
			if (iBackgroundSyncInProgress ||
				iSecondarySession->iSession->Busy())
				User::RequestComplete(iRequest,KErrServerBusy);
			else
				User::RequestComplete(iRequest,KErrNone);
			break;

		case KIMAP4MTMConnect:
		case KIMAP4MTMConnectAndSynchronise:
			if (iPrimarySession->iSession->Busy() ||
			    iPrimarySession->iSession->Connected())
				{
				User::RequestComplete(iRequest,KErrServerBusy);
				}
			else
				{
				iServiceId=aSelection[0];

				LoadSettingsL(iServiceId);

				// We want to hang around for this one
				iCanBeDeletedNow=EFalse;

				// The selection passed through will be any messages selected for download.
				// Inform the session about these as they may potentially be deleted during sync.
				iCurrentSession->iSession->SetSynchronisationSelectionL(aSelection);

				// Queue a connect
				iRequestedOperation=TImap4GenericProgress::EConnect;
				iCurrentSession->iSession->ConnectL(*status,iServiceId);

				notComplete = ETrue;
				}
			break;

		case KIMAP4MTMStartBatch:
			// We're about to receive multiple commands from the client: ensure we
			// stay loaded by faking the CommandExpected() result to be ETrue
			iBatchInProgress=ETrue;
			User::RequestComplete(iRequest,KErrNone);
			break;

		case KIMAP4MTMEndBatch:
			// Stop faking the CommandExpected(), the client has finished with their
			// batch of commands.
			iBatchInProgress=EFalse;
			User::RequestComplete(iRequest,KErrNone);
			break;

		case KIMAP4MTMCancelBackgroundSynchronise:
			// Anything to cancel?
			if (iBackgroundSyncInProgress)
				{
				// Cancel it
				iPrimarySession->Cancel();
				iBackgroundSyncInProgress=EFalse;
				User::RequestComplete(iRequest,KErrCancel);
				}
			else
				User::RequestComplete(iRequest,KErrNone);
			break;

		case KIMAP4MTMSelect:
			// Queue a selection command
			iRequestedOperation=TImap4GenericProgress::ESelect;
			iCurrentSession->iCompound->SelectL(*status,aSelection[0]);
			notComplete = ETrue;
			break;

		case KIMAP4MTMSynchronise:
			// Queue a synchronise operation
			iRequestedOperation=TImap4GenericProgress::ESync;
			if (iClearNewFlagOnNextSync)
				{
				ClearNewFlagL(iServiceId);
				iClearNewFlagOnNextSync = EFalse;
				}
			
			iCurrentSession->iCompound->SynchroniseL(*status);
			notComplete = ETrue;
			break;

		case KIMAP4MTMSyncTree:
			// Queue a Synchronise Tree operation
			if (iBackgroundSyncInProgress)
				{
				User::RequestComplete(iRequest,KErrServerBusy);
				return;
				}
			
			iRequestedOperation=TImap4GenericProgress::ESync;
			iCurrentSession->iFullSync->SynchroniseTreeL(*status,iServiceId,ETrue);
			notComplete = ETrue;
			break;
			
		case KIMAP4MTMLocalSubscribe:
			// Change local subscription flag on a folder
			User::RequestComplete(iRequest,SetLocalSubscription(aSelection[0],ETrue));
			break;

		case KIMAP4MTMLocalUnsubscribe:
			{
			// Change local subscription flag on a folder
			TMsvId folder = aSelection[0];
			TInt err=SetLocalSubscription(folder,EFalse);
			if(err==KErrNone)
				{
				// if we don't have any service settings then load
				// them
				TBool loadedSettings = EFalse;
				if (iServiceSettings==NULL)
					{
					LoadSettingsL(folder);
					loadedSettings = ETrue;
					}

				// if synchronisation setting is not remote only then
				// update the invisibility flags
				if (iServiceSettings->Synchronise() != EUseRemote)
					{
					PropagateInvisibleFlagL(folder);
					ChangeVisibilityL(folder,ETrue,EFalse,KUidMsvMessageEntry);
					}

				// if we loaded settings especially then free them
				// again
				if (loadedSettings)
					{
					delete iServiceSettings;
					iServiceSettings=NULL;
					}
				}
			User::RequestComplete(iRequest,err);
			break;
			}

		case KIMAP4MTMFullSync:
			// Do a full synchronise if we're not doing one already
			if (iBackgroundSyncInProgress)
				{
				// We're busy, go away
				User::RequestComplete(iRequest,KErrServerBusy);
				}
			else
				{
				iRequestedOperation=TImap4GenericProgress::ESync;
				if (iClearNewFlagOnNextSync)
					{
					ClearNewFlagL(iServiceId);
					iClearNewFlagOnNextSync = EFalse;
					}

				// SJM: Note new folders should always be invisible as
				// they are not subscribed
				iCurrentSession->iFullSync->SynchroniseL(*status,iServiceId, ETrue,
														 !iServiceSettings->DeleteEmailsWhenDisconnecting());
				iState=EMtmStateForegroundSync;
				notComplete = ETrue;
				}
			break;

		case KIMAP4MTMPopulate:
			// this function is just a copy to mirror.
			{
			iRequestedOperation=TImap4GenericProgress::EPopulate;
			if (aParameter.Length() > 0)
				{
				TImImap4GetPartialMailInfo imap4GetPartialMailInfo;	
				TPckgC<TImImap4GetPartialMailInfo> paramPartialPack(imap4GetPartialMailInfo);
				paramPartialPack.Set(aParameter);
				iPartialMailInfo = paramPartialPack();
				}

			MtmCommandL(EMtmPopulate,aSelection,KMsvNullIndexEntryId,aStatus);
			break;
			}
			
		case KIMAP4MTMInboxNewSync:
			if (iBackgroundSyncInProgress)
				{
				// We're busy, go away
				User::RequestComplete(iRequest,KErrServerBusy);
				}
			else
				{
				// First of all, find the inbox
				CMsvEntrySelection *findinbox=new (ELeave) CMsvEntrySelection;
				CleanupStack::PushL(findinbox);
				SetEntryL(iServiceId);
				GetChildrenL(*findinbox);
				TMsvId inbox=0;
				for(TInt a=0;a<findinbox->Count();a++)
					{
					SetEntryL((*findinbox)[a]);
					if (iServerEntry->Entry().iDetails.CompareF(KIMAP_INBOX)==0)
						{
						inbox=(*findinbox)[a];
						break;
						}
					}

				// Clean up
				CleanupStack::PopAndDestroy();
			
				// Found it?
				if (inbox)
					{
					// Start a new-only sync of the inbox
					iRequestedOperation=TImap4GenericProgress::ESync;
					iCurrentSession->iCompound->NewOnlySyncL(*status,inbox);
					notComplete = ETrue;
					}
				else
					{
					// Couldn't find it!
					User::RequestComplete(iRequest,KErrNotFound);
					}
				}
			break;

		case KIMAP4MTMFolderFullSync:
			// Start a full sync of the folder
			iRequestedOperation=TImap4GenericProgress::ESync;
			iCurrentSession->iCompound->FullSyncL(*status,aSelection[0]);
			notComplete = ETrue;
			break;

		case KIMAP4MTMWaitForBackground:
			// Wait for background operation to complete: is one running?
			if (!iBackgroundSyncInProgress)
				{
				// No, just return
				User::RequestComplete(iRequest,KErrNone);
				}
			else
				{
				// Otherwise, wait for completion
				iState=EMtmStateWaitingForBackgroundToFinish;
				// Activate instead of SetActive()
				Activate();
				}
			break;

		case KIMAP4MTMDisconnect:
			// if we want to delete emails on disconnect and we've
			// finished the background sync			
			if ( iServiceSettings->DeleteEmailsWhenDisconnecting() &&
				 !(iBackgroundSyncInProgress || iPrimarySession->iSession->IsCancelling()) )
				{
				// Disconnecting
				iRequestedOperation=TImap4GenericProgress::EDelete;
			
				iPrimarySession->iFullSync->SynchroniseDeletesL(*status, iServiceId);
				iPrimarySession->StartL(this);
				}
			else
				{
				// Disconnecting
				iRequestedOperation=TImap4GenericProgress::EDisconnect;
			
				// Cancel both sessions
				iPrimarySession->Cancel();

				// No sync in progress now...
				if (iBackgroundSyncInProgress)
					iBackgroundSyncInProgress=EFalse;

				// Disconnect both sessions
				status=iPrimarySession->SessionStatus();
				iPrimarySession->iSession->DisconnectL(*status);
				iPrimarySession->StartL(this);
				}

			// kill off the secondary session whatever
			iSecondarySession->Cancel();
			status=iSecondarySession->SessionStatus();
			iSecondarySession->iSession->DisconnectL(*status);
			iSecondarySession->StartL(this);
			break;

		case KIMAP4MTMRenameFolder:
			{
			TImap4RenameFolder cmd;
			TPckgC<TImap4RenameFolder> package(cmd);
			package.Set(aParameter);
			if (RenameFolderL(aSelection[0], package()))
				notComplete = ETrue;
			break;
			}

		case KIMAP4MTMUndeleteAll:
			UndeleteAllL(aSelection, aStatus);
			break;
			
		case KIMAP4MTMCancelOffLineOperations:
			iOffLineControl->CancelOffLineOperationsL(aSelection);
			User::RequestComplete(iRequest,KErrNone);
			break;
			
		default:
			User::RequestComplete(iRequest,KErrNotSupported);
			break;
		}

	if (notComplete)
		{
		// Stuff to do!
		iCurrentSession->StartL(this);
		}
	}

// Are we finished yet?
TBool CImap4ServerMtm::CommandExpected()
	{
	// ...basically, when we're disconnected we can be deleted
	return (!iCanBeDeletedNow || iBatchInProgress);
	}

// Report progress information back to client
const TDesC8& CImap4ServerMtm::Progress()
	{
	TImap4CompoundProgress progress;

	// get generic status from the current session, this writes a
	// complete GenericProgressState out apart from iOperation.  note
	// that if we do a populate whilst background syncing (for
	// example) then this will give details of the populate. If we are
	// only syncing then this will give details about the sync
	// operation
	CActiveWrapper *session = NULL;
	
	if(iState==EMtmStateSecondarySessionIdle)
		{
		session = iPrimarySession;
		}
	else
		{
		session = iCurrentSession ? iCurrentSession : iPrimarySession;
		}

	progress.iGenericProgress = session->iCompound->Progress();

	// read the sync status whatever state we are in to ensure that
	// the structure is not left uninitialised
	progress.iSyncProgress=iPrimarySession->iFullSync->Progress();
	  
	// Have we gone from non-disconnected to disconnected all of a sudden? If we have, this
	// means that the lower layers have detected a disconnection.
	// ignore this transition if the last thing we did is ask for a disconnection.
	if (iLastSessionState!=TImap4GenericProgress::EDisconnected &&
		progress.iGenericProgress.iState==TImap4GenericProgress::EDisconnected &&
		iRequestedOperation!=TImap4GenericProgress::EDisconnect)
		{
		// Kick ourselves so we know we're disconnected now
		LOG_COMMANDS((_L8("CImap4ServerMtm::Progress: Session %d suddenly disconnected"), iCurrentSession->iID));
		
		if(iCurrentSession->iID != 2)
			{
			GoneOffline();
			}
		}

	// Save this operation to check next time
	iLastSessionState=progress.iGenericProgress.iState;

	// Put error into progress buffer
	if( progress.iGenericProgress.iErrorCode == KErrNone )
		progress.iGenericProgress.iErrorCode=iProgressErrorCode;

	// If we're copying or moving, *we* (the mtm) keep track of the
	// messages done: this is because these operations involve many
	// more operations at MTM level (ie, syncs, etc) which also fiddle
	// with the msgsdone total, as they would during a full sync.
	switch (iRequestedOperation)
		{
	case TImap4GenericProgress::EMoveWithinService:
	case TImap4GenericProgress::ECopyWithinService:
		break;
	case TImap4GenericProgress::EMoveToLocal:
	case TImap4GenericProgress::ECopyToLocal:
	case TImap4GenericProgress::EMoveFromLocal:
	case TImap4GenericProgress::ECopyFromLocal:
	case TImap4GenericProgress::EPopulate:
	case TImap4GenericProgress::EDelete:	
	case TImap4GenericProgress::EOffLineDelete:
	case TImap4GenericProgress::EOffLineUndelete:
	case TImap4GenericProgress::EOffLineCopyToLocal:
	case TImap4GenericProgress::EOffLineMoveToLocal:
	case TImap4GenericProgress::EOffLineCopyFromLocal:
	case TImap4GenericProgress::EOffLineMoveFromLocal:
	case TImap4GenericProgress::EOffLineCopyWithinService:
	case TImap4GenericProgress::EOffLineMoveWithinService:
	case TImap4GenericProgress::EOffLinePopulate:
		progress.iGenericProgress.iMsgsToDo=iProgressMsgsToDo;
		progress.iGenericProgress.iMsgsDone=iProgressMsgsDone;
		break;
	default:
		break;
		}

#ifdef PRINTING
	// Log the error we're returning
	if (iProgressErrorCode!=KErrNone)
		{
		iPrimarySession->iSession->LogText(_L8("Progress errorcode=%d, laststate=%d currentstate=%d"),
										   iProgressErrorCode,iLastSessionState,progress.iGenericProgress.iState);
		}
#endif

	// put in the operation we've been asked to perform
	progress.iGenericProgress.iOperation=iRequestedOperation;

	// Copy the Sync iTotalSize flag into the Generic Total Size field, if the progress val is 0;
	progress.iGenericProgress.iTotalSize = iTotalSize;

	// construct the progress buffer
	iProgressBuf=TImap4ProgressBuf(progress);
	return iProgressBuf;
	}

void CImap4ServerMtm::DoCancel()
	{
	DBG((iPrimarySession->iSession->LogText(_L8("CImap4ServerMtm::DoCancel() when in state %d"),iState)));

	// What are we doing?
	switch(iState)
		{
	case EMtmStateCopyFromLocal:
	case EMtmStateMoveFromLocal:
	case EMtmStateCopyToLocal:
	case EMtmStateMoveToLocal:
	case EMtmStateCopyWithinService:
	case EMtmStateMoveWithinService:
	case EMtmStatePopulate:
		// These all use the compound objects: cancel this
		iPrimarySession->Cancel();
		iSecondarySession->Cancel();
		break;

	case EMtmStateMiscCommand:
		// What was the actual misc command?
		switch(iLastCommand)
			{
		case KIMAP4MTMConnect:
		case KIMAP4MTMConnectAndSynchronise:
			// Kill primary only
			iPrimarySession->Cancel();
			break;

		default:
			// Cancel both sessions...
			iPrimarySession->Cancel();
			iSecondarySession->Cancel();
			}
		break;

	case EMtmStateSecondaryConnect:
		// Cancel secondary session
		iSecondarySession->Cancel();
		break;

	case EMtmStateOffLineCopyToLocal:
	case EMtmStateOffLineMoveToLocal:
		iServerEntry->Cancel();
		iOffLineControl->Cancel();
		break;
	case EMtmStateSecondarySessionIdle:
		return;
	
	case EMtmStateDelete:
	case EMtmStateDeleteFolder:
	default:
		// Cancel everything in sight
		iPrimarySession->Cancel();
		iSecondarySession->Cancel();
		break;
		}

	DBG((iPrimarySession->iSession->LogText(_L8("CImap4ServerMtm::DoCancel() finished"))));

	// Park entries
	iPrimarySession->iSession->Park();
	iSecondarySession->iSession->Park();

	if (iRequest)
		User::RequestComplete(iRequest,KErrCancel);
	}

void CImap4ServerMtm::DoRunL()
	{
#ifdef PRINTING
	CImImap4Session* logsession=(iId==2)?iSecondarySession->iSession:iPrimarySession->iSession;
	logsession->LogText(_L8("CImap4ServerMtm::DoRunL(id=%d, status=%d, state=%d)"),
						iId,iStatus.Int(),iState);
#endif

	// Status for kicking off next command
	TRequestStatus* status=iCurrentSession->SessionStatus();

	// What were we doing?
	switch(iState)
		{
	case EMtmStateMiscCommand:
		// A misc command has completed: do we need to do anything next?
		switch(iLastCommand)
			{
		case KIMAP4MTMConnect:
		case KIMAP4MTMConnectAndSynchronise:
			{
			// Problems?
			if (iCode!=KErrNone)
				{
				// Mark ourselves as offline, and unloadable
				GoneOffline();

				// Tell world about the error, via progress and
				// Completion code
				iProgressErrorCode=iCode;

				// If doing a connect and sync, the caller is completed after the connect
				// phase and the sync continues in the background. If we get an error during that
				// sync we need to ensure we don't try to complete the caller again by checking
				// that iRequest is not null.
				if (iRequest != NULL)
					{
					User::RequestComplete(iRequest,iCode);
					}
				return;
				}

			// Connect was successful: update connected bit in the service entry
			// Ignore errors, it's not the end of the world
			TRAP_IGNORE(MarkOnOrOfflineL(ETrue));

			// Mark that we need to clear new flag before next sync
			iClearNewFlagOnNextSync = ETrue;
			
			// Do we need to kick off sync?
			if (iLastCommand==KIMAP4MTMConnectAndSynchronise)
				{
				DBG((logsession->LogText(_L8("Kicking off background synchronise"))));

				// Start full sync
				if (iClearNewFlagOnNextSync)
					{
					ClearNewFlagL(iServiceId);
					iClearNewFlagOnNextSync = EFalse;
					}
				// SJM: New folders should always be invisible as they are not subscribed
				iCurrentSession->iFullSync->SynchroniseL(*status,iServiceId,ETrue,
					!iServiceSettings->DeleteEmailsWhenDisconnecting(), ETrue);
				iCurrentSession->StartL(this);

				// Carry on as normal, completing request back to client.
				// The synchronise will continue in the background.
				iBackgroundSyncInProgress=ETrue;
				}

			break;
			}

		case KIMAP4MTMDisconnect:
			// Disconnecting but marked as busy - that means we were
			// carrying out the pending deletes
			if (iRequestedOperation == TImap4GenericProgress::EDelete)
				{
				iRequestedOperation = TImap4GenericProgress::EDisconnect;

				// Disconnect primary sessions
				status=iPrimarySession->SessionStatus();
				iPrimarySession->iSession->DisconnectL(*status);
				iPrimarySession->StartL(this);
				return;
				}

			// Disconnecting session 1? (primary session). If so, then clear the
			// connected bit in the service entry.
			if (iId==1 && iServiceId && iServiceSettings)
				{
				// Do "we've gone offline" stuff
				GoneOffline();
				}
			break;
			}
		break;

	case EMtmStateCopyToLocal:
	case EMtmStateMoveToLocal:
	case EMtmStatePopulate:
		// if no message sent then trigger null event to get to next
		// state
		SetActive();
		if (!iUtils->SendLogMessageL(iCode,iStatus))
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}

		// send log message
		iSavedState = iState;
		iState = EMtmStateLogging;
		return;

	case EMtmStateLogging:
		// done logging, restore old state
		iState = iSavedState;
		// and deliberately fall through to next case

	case EMtmStateCopyFromLocal:
	case EMtmStateMoveFromLocal:
	case EMtmStateCopyWithinService:
	case EMtmStateMoveWithinService:
		// Note any error in the appropriate message
		MessageErrorL((*iSelection)[0],iCode);
		
		// Note any error in the appropriate message
		MessageErrorL((*iSelection)[0],iCode);
		
		// Remove completed item from selection
		if (iState==EMtmStateCopyWithinService || iState==EMtmStateMoveWithinService)
			{
			TInt count=iSelection->Count();
			iSelection->Delete(0,count);
			iProgressMsgsDone+=count;
			}
		else
			{
			iSelection->Delete(0,1);
			iProgressMsgsDone++;
			}

		// One more message done

		// Operation done. Do next one in selection
		if (iSelection->Count())
			{
			// Do the copy with the compound
			status=iCurrentSession->SessionStatus();
			switch(iState)
				{
			case EMtmStateCopyFromLocal:
				iCurrentSession->iCompound->CopyFromLocalL(*status,(*iSelection)[0],iDestination);
				break;

			case EMtmStateMoveFromLocal:
				iCurrentSession->iCompound->MoveFromLocalL(*status,(*iSelection)[0],iDestination);
				break;

			case EMtmStateCopyToLocal:
				iUtils->SetUpLogMessageL((*iSelection)[0]);
				iCurrentSession->iCompound->CopyToLocalL(*status,(*iSelection)[0],iDestination);
				break;

			case EMtmStateMoveToLocal:
				iUtils->SetUpLogMessageL((*iSelection)[0]);
				iCurrentSession->iCompound->MoveToLocalL(*status,(*iSelection)[0],iDestination);
				break;

			case EMtmStateCopyWithinService:
				// Will copy all messages in one go.
				break;

			case EMtmStateMoveWithinService:
				// Will copy all messages in one go.
				break;

			case EMtmStatePopulate:
				iUtils->SetUpLogMessageL((*iSelection)[0]);
				iCurrentSession->iCompound->PopulateL(*status,(*iSelection)[0],iPartialMailInfo);
				break;

			default: // Keep gcc quiet
				break;
				}

			iCurrentSession->StartL(this);
			return;
			}
		else if(iCurrentSession == iSecondarySession)
			{
			iSecondarySession->Cancel();
			status = iSecondarySession->SessionStatus();
			iSecondarySession->iSession->DisconnectL(*status);
			iSecondarySession->StartL(this);
			iState = EMtmStateSecondarySessionIdle;
			iSavedState = EMtmStateSecondarySessionIdle;
			}

		break;		
	
	case EMtmStateDelete:
		{
		// Problems?
		if (iCode!=KErrNone)
			{
			// Store the error on this one
			MessageErrorL((*iSelection)[0],iCode);

			// Continue through selection
			}

		// Delete completed.
 		TInt count=iSelection->Count();
 		iSelection->Delete(0,count);
 		iProgressMsgsDone+=count;

		DBG((iCurrentSession->iSession->LogText(_L8("iMsgsDone now %d"),iProgressMsgsDone)));		
 		break;
		}
	case EMtmStateDeleteFolder:
		// Delete completed: do the next one
		iSelection->Delete(0,1);

		// One more message done
		iProgressMsgsDone++;

		DBG((iCurrentSession->iSession->LogText(_L8("iMsgsDone now %d"),iProgressMsgsDone)));

		// Anything left to do?
		if (!iSelection->Count())
			break;

		iCurrentSession->iCompound->DeleteFolderL(*status,(*iSelection)[0]);
		iCurrentSession->StartL(this);
		return;

	case EMtmStateOffLineDelete:
	case EMtmStateOffLineUndelete:
	case EMtmStateOffLineCopyToLocal:
	case EMtmStateOffLineMoveToLocal:
	case EMtmStateOffLineCopyFromLocal:
	case EMtmStateOffLineMoveFromLocal:
	case EMtmStateOffLineCopyWithinService:
	case EMtmStateOffLineMoveWithinService:
	case EMtmStateOffLinePopulate:
		{		
		if(iProgressMsgsDone == iProgressMsgsToDo)
			break;
		
		if(iOneSelection)
			delete iOneSelection;
		iOneSelection=new (ELeave) CMsvEntrySelection;
		iOneSelection->AppendL((*iSelection)[iProgressMsgsDone]);		
		
		CImap4OffLineControl::TImap4OpType opType=CImap4OffLineControl::EImap4OpDelete; //have to initialise to something!
		switch(iState)
			{
		case EMtmStateOffLineDelete:
			opType=CImap4OffLineControl::EImap4OpDelete;
			break;
		case EMtmStateOffLineUndelete:
			opType=CImap4OffLineControl::EImap4OpUndelete;
			break;
		case EMtmStateOffLineCopyToLocal:
			opType=CImap4OffLineControl::EImap4OpCopyToLocal;
			break;
		case EMtmStateOffLineMoveToLocal:
			opType=CImap4OffLineControl::EImap4OpMoveToLocal;
			break;
		case EMtmStateOffLineCopyFromLocal:
			opType=CImap4OffLineControl::EImap4OpCopyFromLocal;
			break;
		case EMtmStateOffLineMoveFromLocal:
			opType=CImap4OffLineControl::EImap4OpMoveFromLocal;
			break;
		case EMtmStateOffLineCopyWithinService:
			opType=CImap4OffLineControl::EImap4OpCopyWithinService;
			break;
		case EMtmStateOffLineMoveWithinService:
			opType=CImap4OffLineControl::EImap4OpMoveWithinService;
			break;
		case EMtmStateOffLinePopulate:
			opType=CImap4OffLineControl::EImap4OpPopulate;
			break;
		default:
			break;
			}
		
		if(iState == EMtmStateOffLineDelete || iState == EMtmStateOffLineUndelete)
			{
			iOffLineControl->StoreOfflineCommandL(opType, *iOneSelection, KMsvNullIndexEntryId, iStatus);
			}
		else if(iState == EMtmStateOffLinePopulate)
			{
			TPckgBuf<TImap4GetMailOptions> package(iGetMailOptions);
			iOffLineControl->StoreOfflineCommandL(opType, *iOneSelection, iDestination, package, iStatus);
			}
		else
			{	
			iOffLineControl->StoreOfflineCommandL(opType, *iOneSelection, iDestination, iStatus);
			}

		iProgressMsgsDone++;
		SetActive();
		return;
		}
	case EMtmStateSecondaryConnect:
		// We've now connected the secondary session. Issue the
		// saved command and continue;
		switch(iState=iSavedState)
			{
		case EMtmStateCopyToLocal:
			// We've just connected, so first thing to do is to do the
			// folder selection. No need to check the selection, as
			// we know it contains at least 1 item from the originally
			// issued command.
			iUtils->SetUpLogMessageL((*iSelection)[0]);
			iCurrentSession->iCompound->CopyToLocalL(*status,(*iSelection)[0],iDestination);
			iCurrentSession->StartL(this);
			return;
		
		case EMtmStatePopulate:
			iUtils->SetUpLogMessageL((*iSelection)[0]);
			iCurrentSession->iCompound->PopulateL(*status,(*iSelection)[0],iPartialMailInfo);
			iCurrentSession->StartL(this);
			return;
		
		default:
			break;
			}
		break;

	case EMtmStateSyncCompleted:	// FALL THROUGH
	default:
		break;
		}

	// Async request completed (from wrapper): also return it in the progress
	DBG((logsession->LogText(_L8("ID %d completed with code %d"),iId,iCode)));

	iProgressErrorCode=iCode;

	// Park entry
	iServerEntry->SetEntry(KMsvNullIndexEntryId);

	// Only complete if we have an iRequest: we may not (ie, 2 sessions
	// disconnecting at once)
	if (iRequest)
		User::RequestComplete(iRequest,KErrNone);
	}

void CImap4ServerMtm::DoComplete(TInt aStatus)
	{
#ifdef PRINTING
	iPrimarySession->iSession->LogText(_L8("CImap4ServerMtm::DoComplete(%d)"),aStatus);
#endif

	// Park entry
	iServerEntry->SetEntry(KMsvNullIndexEntryId);
	if (iRequest)
		User::RequestComplete(iRequest,aStatus);
	}

// Save error code in a message
void CImap4ServerMtm::MessageErrorL(const TMsvId aMessageId, const TInt aError)
	{
	// Save error code: if we can't access this entry, then it's probably something to do
	// with the error we're trying to report: ignore it silently
	// SJM: this used to be != KerrNone - surely shome mistake
	if (iServerEntry->SetEntry(aMessageId)==KErrNone)
		{
		TMsvEntry entry=iServerEntry->Entry();

		// Save unnecessary writes...
		if (entry.iError!=aError)
			{
			entry.iError=aError;
			ChangeEntryL(entry);
			}
		}
	}

// Mixin - a child has completed
void CImap4ServerMtm::RequestCompleted(TInt aId, TInt aCode)
	{
#ifdef PRINTING
	CImImap4Session* logsession=(aId==2)?iSecondarySession->iSession:iPrimarySession->iSession;
	logsession->LogText(_L8("CImap4ServerMtm::RequestCompleted(id=%d, result=%d, state=%d)"),aId,aCode,iState);
#endif
	// Is this the background sync completeing?
	if (aId==1 && iBackgroundSyncInProgress)
		{
		// Not in progress any more, boyo
		iBackgroundSyncInProgress=EFalse;

		// Is anyone in the forground waiting for this to happen?
		if (iState==EMtmStateWaitingForBackgroundToFinish)
			{
			// Yes: we need to complete ourselves by falling through
			// Reset state first
			iState=EMtmStateIdle;

			// Any errors from the background sync aren't interesting to us
			aCode=KErrNone;
			}
		// Makes the secondary session request, when primary completes background sync
		else if (iState == EMtmStateSecondaryConnect || iState == EMtmStateCopyToLocal || iState == EMtmStatePopulate)
			{
			iProgressErrorCode=aCode;
			return;	
			}			
		else if( aCode == KErrNone )
			{
			// Complete with the state EMtmStateSyncCompleted.  
			// When this is caught in the DoRunL, it does nothing.
			iState = EMtmStateSyncCompleted;
			
			// NOTE - only do this if there are no errors - if there is an error 
			// then DoRunL must handle appropriately in the previous state.
			}
		}

	// Are we doing a disconnect? If so, we will get completions from both sessions, and we're only
	// interested in the primary one, really
	if (iLastCommand==KIMAP4MTMDisconnect && aId!=1)
		{
		// Silently ignore
		return;
		}

	// Is the code >0? If so, truncate it to 0 as it's IMPS-specific info
	if (aCode>KErrNone)
		aCode=KErrNone;

	// Save stuff
	iCode=aCode;
	iId=aId;

	// Set the current session to the one which has just completed so
	// that when DoRunL() is called it knows which to deal with
	if (aId==1)
		iCurrentSession=iPrimarySession;
	else
		iCurrentSession=iSecondarySession;

	// Complete *ourselves*
	TRequestStatus *a=&iStatus;
    User::RequestComplete(a,aCode);
	
	// The activation may need to be done here.
	// e.g. the primary session already completed and then the secondary completes.
	// e.g. EMtmCopyToLocal is started whilst we are online and a background 
	// sync is in progress.  One completes and then the other.
    if(!IsActive())
    	{
    	SetActive();
    	}    
	}
	
// This function allows a CActiveWrapper to set the CImap4ServerMtm active 
// when it starts a request, as opposed to just when one finishes. This means
// that Cancel()s are handled properly.
void CImap4ServerMtm::Activate()
	{
	// Object may already be active as there is a primary and secondary session.
	if(!IsActive())
		{
		iStatus = KRequestPending;
		SetActive();
		}
	}

#ifdef PRINTING
void CImap4ServerMtm::NonCompletedFailureOnSession(TInt aId)
#else
void CImap4ServerMtm::NonCompletedFailureOnSession(TInt /*aId*/)
#endif
	{
#ifdef PRINTING
	CImImap4Session* logsession=(aId==2)?iSecondarySession->iSession:iPrimarySession->iSession;
	logsession->LogText(_L8("CImap4ServerMtm::NonCompletedFailureOnSession (id=%d)"), aId);
#endif

	// A failure has occured on the session, but there is no outstanding asynchronous
	// request on it. This can happen if for instance we get a disconnect while doing
	// a cancel and idle operation. We need to go offline immediately.
	GoneOffline();
	}

CImap4ServerMtm::CImap4ServerMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry):
	CBaseServerMtm(aRegisteredMtmDll,aEntry)
	{
	__DECLARE_NAME(_S("CImap4ServerMtm"));

	// We can be deleted: at the start, we're not connected
	iCanBeDeletedNow=ETrue;

	// Add us to the scheduler
	CActiveScheduler::Add(this);
	}
TBool CImap4ServerMtm::PruneMessages(const CMsvEntrySelection& aSelection)
	{
	TInt index = aSelection.Count();
	
	// See if the parent of the first entry is a message.
	// If it is then we need to prune the entries, ie. delete them locally.
	if (index == 0)
		return EFalse;

	TInt err = iServerEntry->SetEntry(aSelection[0]);

	if (err == KErrNone)
		{
		err = iServerEntry->SetEntry(iServerEntry->Entry().Parent());
		if (KUidMsvMessageEntry != iServerEntry->Entry().iType)
			// The parent of the given entry was not a message, so we don't prune it.
			return EFalse;
		}

	while ((index--) && (err==KErrNone))
		{
		// Go to the required entry
		err = iServerEntry->SetEntry(aSelection[index]);

		if (KErrNone == err)
			{
			// Go to the parent entry to see if it is a message entry
			iServerEntry->SetEntry(iServerEntry->Entry().Parent());
			TMsvEmailEntry entry = iServerEntry->Entry();

			// assert that (KUidMsvMessageEntry == entry.iType)

			// Clear the complete flag because we are about to delete the child parts.
			entry.SetComplete(EFalse);
			entry.SetBodyTextComplete(EFalse);
			err = iServerEntry->ChangeEntry(entry);

			if (KErrNone == err)
				{
				// Delete the body of the message.
				iServerEntry->DeleteEntry(aSelection[index]);
				}
			}
		}

	User::RequestComplete(iRequest, err);

	return ETrue;
	}
