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
// CImImap4FolderSync header
// 
//

#if !defined(__FLDSYNC_H__)
#define __FLDSYNC_H__

#include <mentact.h>
#include <msventry.h>

// Forward declarations
class CImImap4Session;
class CImImap4DirStruct;
class TImap4SyncProgress;

// Maximum depth of folder tree to sync to
const TInt KFolderDepth=8;

// States
/**
@internalComponent
@released
*/
enum TFolderSyncState
	{
	ETreeSync=1,
	};

class CImImap4FolderSync : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	~CImImap4FolderSync();
	static CImImap4FolderSync* NewLC(CImImap4Session *aSession);
	static CImImap4FolderSync* NewL(CImImap4Session *aSession);
	void ConstructL(CImImap4Session *aSession);

	// Mirror folder tree of remote server
	void SynchroniseTreeL(TRequestStatus& aStatus, const TMsvId aRoot, const TBool aNewFoldersAreInvisible);

	// Set serverentry to use
	void SetEntry(CMsvServerEntry* aEntry);

	// increment relevant entries in the progress structure
	void IncProgress(TImap4SyncProgress& aProgress);
	void ResetStats();
	
private:
	CImImap4FolderSync();
	void DoRunL();
	void DoCancel();
	TBool ProcessDirListL();
	void BuildLocalL(const TMsvId aFolder, const TBool aDoThisOne);
	void OrphanedFolderCheckL();

	// Set entry, leave if error (not related to setentry() which sets iEntry)
	void SetEntryL(const TMsvId aId);

	// Change entry, leave if error
	void ChangeEntryL(const TMsvEntry& aEntry);
	
	// Change entry in bulk mode (i.e. no index file commit), leave if error
	void ChangeEntryBulkL(const TMsvEntry& aEntry);

	// Get children, leave if error
	void GetChildrenL(CMsvEntrySelection& aSelection);

private:
	// IMAP4 session to use
	CImImap4Session*		iSession;

	// Tree synchronisation state
	TFolderSyncState		iState;
	TMsvId					iServiceId;
	TMsvId					iFolderId;
	TInt					iFolderLevel;
	CArrayPtr<CImImap4DirStruct>* iFolderList;
	CMsvEntrySelection*		iFolderContents;
	CArrayFix<TMsvId>*		iFolderIds[KFolderDepth];
	TInt					iFolderPosition[KFolderDepth];
	CArrayFix<TMsvId>*		iLocalFolders;
	TBool					iNewFoldersAreInvisible;

	// Access to message database
	CMsvServerEntry*		iEntry;

	// Current request status
	TRequestStatus*			iRequestStatus;

	TInt					iOrphanedFolders;
	TInt					iNewFolders;
	};

#endif
