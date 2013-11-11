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
// CImImap4Synchronise header
// 
//

#if !defined(__IMAPSYNC_H__)
#define __IMAPSYNC_H__

#include <mentact.h>
#include <msventry.h>
#include <imapset.h>

// Forward declarations
class CImImap4Session;
class CImImap4FolderSync;
class CImap4OffLineControl;
class CImap4Utils;

// List of folders to be synchronised
class TImImap4SyncList
/**
@internalComponent
@released
*/
	{
public:
	TMsvId				iFolder;
	TTime				iLastSync;
	TBool				iNotSelectable;
	};

class CImImap4Synchronise : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
// States
enum TSynchroniseState
	{
	ESyncStateIdle=1,
	EInboxSelect,
	EInboxPendingOps,
	EInboxSync,
	ESynchroniseFolderTree,		// this must be in the correct place
	ECheckRemoteSubscription,
	EProcessRemoteSubscription,
	EUpdateRemoteSubscription,
	EFolderSelect,
	EFolderPendingOps,
	EFolderSynchronise,
	EInboxEarlyDeletes,
	EFolderEarlyDeletes,
	EFolderEarlyExpunge,
	EEndSelectInbox,
	EInboxLateExpunge,
	EFolderLateDeletes,
	EFolderLateExpunge,
	EInboxLogMessage,
	EFolderLogMessage,
	EStartIdle,
	ESynchronise,
	ESynchroniseTree,
	ESynchroniseDeletes
	};

public:
	~CImImap4Synchronise();
	static CImImap4Synchronise* NewLC(CImImap4Session *aSession);
	static CImImap4Synchronise* NewL(CImImap4Session *aSession);
	void ConstructL(CImImap4Session *aSession);

	// Mirror folder tree of remote server
	void SynchroniseL(TRequestStatus& aStatus, TMsvId aRoot, TBool aNewInvisible, TBool aPerformDeletes, TBool aConnectAndSync = EFalse);
	void SynchroniseDeletesL(TRequestStatus& aStatus, TMsvId aServiceId);
	void SynchroniseTreeL(TRequestStatus& aStatus, TMsvId aServiceId, TBool aNewFoldersAreInvisible);

	// Set serverentry to use
	void SetEntry(CMsvServerEntry* aEntry);

	// set OffLineControl to use
	void SetOffLineControl(CImap4OffLineControl *aOffLine);
	void SetUtils(CImap4Utils *aUtils);
	
	// Progress
	TImap4SyncProgress Progress();

private:
	CImImap4Synchronise();
	void DoRunL();
	void DoCancel();
	void DoComplete(TInt& aStatus);

	// once round the state machine, returns whether to go round again
	TBool DoRunLoopL();

	// do the appropriate offline operation
	void DoOpL(const CImOffLineOperation& aOp);

	// finish up the op just done (send log message, append delete
	// etc.)
	void DonePendingOpL();

	// process the next one in the outstanding ops list. Returns ETrue
	// if finished
	TBool NextPendingOpL();
	
	// process pending deletes of the type given in the given
	// folder. Returns ETrue if any items were deleted
	TBool MatchDeleteOp(const CImOffLineOperation& aOp , TBool aMoveDeletesOnly );
	TBool ProcessPendingDeleteOpsL( TMsvId aFolder, TBool aMoveDeletesOnly );
	TBool ProcessPendingDeleteOpsListL( TBool aMoveDeletesOnly );
	
	// Add folders to 'to do' list according to prevailing strategy
	void AddLocalFoldersL(const TMsvId aFolder);

	// helper function for AddLocalFolders
	void AddIfNotThereL(TMsvId aFolder, CArrayFix<TImImap4SyncList>* aFolderList);

	// Sort to-do list
	void SortFolderListL();

	// Set entry, leave if error (not related to setentry() which sets iEntry)
	void SetEntryL(const TMsvId aId);

	// Change entry, leave if error
	void ChangeEntryL(const TMsvEntry& aEntry);
	
	// Change entry in bulk mode (i.e. no index file commit), leave if error
	void ChangeEntryBulkL(const TMsvEntry& aEntry);

	// Get children, leave if error
	void GetChildrenL(CMsvEntrySelection& aSelection);

	// make aId and its parents visible
	void MakeVisibleL(TMsvId aId);
	
	// get the offlineOpArray from the id and set up iOutstandingOps and Folder
	TBool RefreshOutstandingOpsL(TMsvId aId);

	void ResetStats();
	void GetInboxL();

private:
	// IMAP4 session to use
	CImImap4Session*		iSession;

	// Folder tree synchronisation
	CImImap4FolderSync*		iFolderSync;

	// Compound operation used for multi-step IMAP operations
	CImImap4Compound*		iCompound;

	// Synchronisation state
	TSynchroniseState		iState;
	TMsvId					iServiceId;
	TMsvId					iFolderId;
	TMsvId					iInbox;
	TBool					iNewFoldersAreInvisible;
	TBool					iPerformDeletes;

	// Folders to synchronise
	TFolderSyncType			iSynchroniseStrategy;
	TFolderSubscribeType	iSubscribeStrategy;
	CArrayFix<TImImap4SyncList>*	iFolderList;
	// temporary list of folders to be added to iFolderList at end
	CArrayFix<TImImap4SyncList>*	iFolderListDest;

	// List of folders to subscribe or unsubscribe from
	CArrayFix<TMsvId>*		iSubscribeList;
	CArrayFix<TMsvId>*		iUnsubscribeList;

	// List of outstanding operations on inbox/other folder
	CImOffLineOperationArray* iOutstandingOps;
	// current folder p=being processed
	TMsvId				    iOutstandingOpsFolder;

	// list of ids that need to be deleted because they were the
	// source of a move
	CArrayFix<TMsvId>*		iOutstandingMoveTypeDeletes;
	CArrayFix<TMsvId>*		iOutstandingLocalDeletes;

	// source id of current operation if it was a move and so should
	// be deleted afterwards
	TMsvId					iMovedId;

	// shadow id of current operation - to be deleted when op finishes
	TMsvId					iShadowId;
	
	// Access to message database
	CMsvServerEntry*		iEntry;
	
	// Timer for new mail check
	RTimer					iCheckMailbox;

	// Current request status
	TRequestStatus*			iRequestStatus;

	// pointer to offline control object held by MTM
	CImap4OffLineControl*	iOffLineControl;

	TImap4SyncProgress		iProgress;

	CImap4Utils*			iUtils;

	TBool					iIdleBeforeCommand;
	
	 // Flag to indicate Pending Operation/s are done on folder, need to sync Inbox
	TBool                   iPendingOpOnFolder;
	
	};

#endif
