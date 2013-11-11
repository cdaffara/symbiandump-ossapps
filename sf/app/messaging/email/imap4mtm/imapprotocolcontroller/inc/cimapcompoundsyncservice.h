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
//

#ifndef __CIMAPCOMPOUNDSYNCSERVICE_H__
#define __CIMAPCOMPOUNDSYNCSERVICE_H__

#include "cimapcompoundbase.h"
#include "cimapofflinecontrol.h"

// Forward declarations
class CImapFolder;
class CImapSession;
class CImapMailStore;
class CImOffLineOperation;


/**
Compound operation object for performing the necessary sequence of commands
to synchronise the local mirror of the remote mailbox tree and the contents
of mailboxes that are subscribed according to settings.

@internalTechnology
@prototype
*/
class CImapCompoundSyncService : public CImapCompoundBase
	{
public:
	~CImapCompoundSyncService();
	
	static CImapCompoundSyncService* NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, CImapOfflineControl& aImapOfflineControl, TBool aDoLateDeletes);
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void StopForMigrate();
	
	// remove messages from the download rules based fetch
	void RemoveFromSelectionL(CMsvEntrySelection& aDeleteSel);

private:
	CImapCompoundSyncService(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings,CImapMailStore& aImapMailStore, CImapOfflineControl& aImapOfflineControl ,TBool aDoLateDeletes);
	void ConstructL();

	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();

	// get the offlineOpArray from the id and set up iOutstandingOps and Folder
	TBool RefreshOutstandingOpsL(TMsvId aId);

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
	TBool MatchDeleteOp(const CImOffLineOperation& aOp, TBool aMoveDeletesOnly);
	TBool ProcessPendingDeleteOpsL(TMsvId aFolder, TBool aMoveDeletesOnly);
	TBool ProcessPendingDeleteOpsListL(TBool aMoveDeletesOnly);

	// make aId and its parents visible
	void ChangeEntryL(const TMsvEntry& aEntry);
	void DoComplete(TInt& aErr);

private:
	// Sync subscriptions folders progess
	TMsvId					iNextFolderSync;
	
	// List of outstanding operations on inbox/other folder
	CImOffLineOperationArray* iOutstandingOps;

	// current folder being processed
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

	// pointer to offline control object held by MTM
	CImapOfflineControl&	iImapOfflineControl;
	
	CImapCompoundBase* 		iImapCompound;
	TInt					iFoldersNotFound;
	TInt					iMsgsToDo;
	TInt					iMsgsDone;
	CImapMailStore& 		iImapMailStore;
	TBool					iPerformDeletes;
	TBool 					iDoLateDeletes;
	
	// download rules
	TImImap4GetPartialMailInfo iMailInfo;
	TInt32 iFolderSyncCount;
	CMsvEntrySelection* iDoNotFetchSel;
	
	// Folder that we are currently performing an operation on.
	// The class does not take ownership of the folder, so it should not
	// be deleted.
	CImapFolder* iFolder;
	};

#endif	// __CIMAPCOMPOUNDSYNCSERVICE_H__
