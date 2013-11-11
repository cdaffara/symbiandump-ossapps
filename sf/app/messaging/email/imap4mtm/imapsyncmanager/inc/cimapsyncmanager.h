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
// imapsyncmanager.h
// CImapSyncManager header
// 
//

#if !defined(__IMAPSYNCMANAGER_H__)
#define __IMAPSYNCMANAGER_H__

#include <mentact.h>
#include <msventry.h>
#include <imapset.h>
#include "cimapsettings.h"
#include "cimapofflinecontrol.h"
#include "cimapsession.h"

// Forward declarations
class CImapFolder;
class CImapOpSyncFolderTree;
class CImapOpSyncSubs;
class TImap4Progress;
class CImapSession;
class TImap4SyncProgress;

class CImapSyncManager : public CMsgActive
/**
@internalComponent
@prototype
*/
	{
public:
// States
enum TSynchroniseState
	{
	ESyncStateIdle=1,
	EInboxSelect,
	EFolderExpunge,
	EInboxPendingOps,
	EInboxSync,
	ESynchroniseFolderTree,	
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
	EEndInboxSync,
	ESyncFlags,
	EEndSyncFlags,
	ESyncProcessSeenFlagsAndComplete,
	EInboxLateExpunge,
	EFolderLateDeletes,
	EFolderLateExpunge,
	EInboxLogMessage,
	EFolderLogMessage,
	EStartIdle,
	ESynchronise,
	ESynchroniseTree,
	EEndSynchroniseTree,
	ESynchroniseDeletes,
	ENotSyncing,
	ESyncOld,
	ESyncNew,
	ESyncFetchingMessage,
	ESyncSearch,	
	ESyncListOld,	
	ESyncListNew,
	EGettingStructure,
	EEndSingleSyncStep,
	EFolderLocalExpunge,
	EFolderClose,
	EFolderReSelect
	};

enum TImapProgress
	{
/** Disconnected. */
         EDisconnected,
     /** Connecting. */
         EConnecting,
     /** Idle. */
         EIdle,
     /** Selecting the folder to use. */
         ESelecting,
     /** Fetching messages to mirror server. */
         EFetching,
     /** Appending local messages to remote server. */
         EAppending,
     /** Copying local messages to local or within service. */
         ECopying,
     /** Deleting an item, including expunging a folder. */
         EDeleting,
     /** Synchronising folder after an operation. */
         ESyncing,
     /** Disconnecting. */
         EDisconnecting,
     /** Sync already in progress. */
         EBusy,
     /** CImImap4GetMail move operation in progress. */
         EMoving,
     /** CImImap4GetMail copy new mail operation in progress. */
         ECopyNewMail,
     /** CImImap4GetMail move new mail operation in progress. */
         EMoveNewMail,
     /** CImImap4GetMail copy selected messages operation in progress. */
         ECopyMailSelection,
     /** CImImap4GetMail move selected messages operation in progress. */
         EMoveMailSelection,
     /** CImImap4GetMail copy all messages operation in progress. */
         ECopyAllMail,
     /** CImImap4GetMail move all messages operation in progress. */
         EMoveAllMail,
     /** CImImap4GetMail populate new messages operation in progress. */
         EPopulateNewMail,
     /** CImImap4GetMail populate all messages operation in progress. */
         EPopulateAllMail,
     /** CImImap4GetMail populate selected messages operation in progress. */
         EPopulateMailSelection		
	};
	
	// Class contructors and destructor	
	IMPORT_C static CImapSyncManager* NewLC(CMsvServerEntry& aEntry, CImapSettings& aImapSettings);
	IMPORT_C static CImapSyncManager* NewL(CMsvServerEntry& aEntry, CImapSettings& aImapSettings);
	~CImapSyncManager();

	// Mirror folder tree of remote server
	IMPORT_C void SynchroniseDeletesL(TRequestStatus& aStatus, CImapSession& aSession);
	
	IMPORT_C void SynchroniseInboxL(TRequestStatus& aStatus, CImapSession& aSession);
	IMPORT_C void SynchroniseFolderTreeL(TRequestStatus& aStatus, CImapSession& aSession);
	IMPORT_C void SynchroniseFolderSubscriptionsL(TRequestStatus& aStatus, CImapSession& aSession);
	IMPORT_C void SynchroniseSubscribedFoldersL(TRequestStatus& aStatus, CImapSession& aSession, TMsvId& aFolderId);

	// Folders methods
	// Default aAddToSubscribedList is set to ETrue to subscribe.
	// Only in one case EFalse is passed, ie. while creating new remote Imap folder.
	IMPORT_C CImapFolder* GetFolderL(TMsvId aMsvId, TBool aAddToSubscribedList=ETrue);
	IMPORT_C CImapFolder* GetTempFolderL(TMsvId aMsvId);
	IMPORT_C CImapFolder* Inbox();
	IMPORT_C void DeleteFolderContentsL(TMsvId aDeleteFolderId);
	IMPORT_C void RemoveFolderL(TMsvId aMsvId);
	IMPORT_C void RenameLocalL(TMsvId aFolderId, const TDesC& aNewName);
	IMPORT_C CImapFolder* GetSubscribedFolder(TInt32 aFolderId, TInt32& aNumSubscribedFolders);

	void AddSubscribedFolderFromInfoL(CImapListFolderInfo* aFolderInfo, TInt aLogId);	
	void CreateLocalFolderListL();
	TMsvId FindAndSetFolderL(const TPtrC& aFullPath, TChar& aHierarchySeperator);	
	RArray<TUint>* GetUnSubscribedFolderToDoList();
	RArray<TUint>* GetSubscribedFolderToDoList();
	
	// Getting the progress updates
	IMPORT_C void Progress(TImap4SyncProgress& aProgress);
	IMPORT_C void NextSubscribedFoldersL(TMsvId& aFolderId);

	TBool InboxClearNewFlags();
	TBool NonInboxClearNewFlags();
	IMPORT_C void ResetInboxClearNewFlags();
	IMPORT_C void ResetNonInboxClearNewFlags();
	TBool EntryIsInbox(const TMsvEntry& aEntry);
	
	IMPORT_C void CancelForMigrate();

private:
	// construction
	CImapSyncManager(CMsvServerEntry& aEntry, CImapSettings& aImapSettings);
	void ConstructL();

	// CMsgActive stuff
	void DoRunL();
	void DoCancel();
	void DoComplete(TInt& aErr);
	void CompleteSelf();
	
	// error handling	
	TInt ProcessSessionError();
	void ProcessNegativeServerResponse(TInt& errCode);

	// internal stuff
	void SetEntryL(const TMsvId aId);
	void ChangeEntryL(const TMsvEntry& aEntry);
	void ChangeEntryBulkL(const TMsvEntry& aEntry);
	void GetChildrenL(CMsvEntrySelection& aSelection);
	void AddIfNotThereL(TMsvId aFolder);
	void AddLocalFoldersL(const TMsvId aFolder);
	void GetInboxL();
	CImapFolder* GetFolderFromList(TMsvId aMsvId);

private:
	// Imap server and settings members objects owned by the class creator
	CMsvServerEntry& iServerEntry;
	CImapSettings& iImapSettings;
	CImapSession* iSession;
	
	// Sync operations member objects	
	CImapOpSyncFolderTree* iFolderTreeSync;
	CImapOpSyncSubs* iSyncSubs;
	CImapFolder* iInbox;
	TInt iSyncFolderCounter;
	TBool iSyncDoSingleStep;
		
	// Pointer to list of folders
	RPointerArray<CImapFolder> iFolderList;

	// List of folders to subscribe or unsubscribe from
	RArray<TUint>*		iSubscribeList;
	RArray<TUint>*		iUnsubscribeList;
	
	// Service status settings
	TMsvId iServiceId;
	TBool iPerformDeletes;
	TBool iNewFoldersAreInvisible;
	TBool iIdleBeforeCommand;
	TFolderSyncType iSynchroniseStrategy;
	TFolderSubscribeType iSubscribeStrategy;
	TRequestStatus* iRequestStatus;
	TSynchroniseState iSyncState;
	TSynchroniseState iNextSyncAction;
	TInt iFoldersToDo;
	TInt iFoldersDone;
	TInt iFoldersNotFound;
	TInt iErrorCode;
	TImap4SyncProgress::TImap4SyncProgressState iSyncProgressState;

	TBool iInboxClearNewFlags;
	TBool iNonInboxClearNewFlags;
	};

#endif // __IMAPSYNCMANAGER_H__
