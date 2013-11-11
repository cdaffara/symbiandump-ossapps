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
// CImapOpSyncFolderTree header
// 
//

#if !defined(__IMAPOPSYNCFOLDERTREE_H__)
#define __IMAPOPSYNCFOLDERTREE_H__

#include "cimapsyncoperation.h"
#include "cimaplistfolderinfo.h"

// Forward declarations
class CMsvEntrySelection;
class CImapFolder;

class CImapOpSyncFolderTree : public CImapSyncOperation
/**
Class to perform the synchronise folder tree operation. This object will call the IMAP LIST command to get the
complete list of folders on the server. For each folder listed, it will then call the syncmanager to add the folder
to the local message server.
@internalComponent
@prototype
*/
	{
public:
	static CImapOpSyncFolderTree* NewL(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan, CImapSettings& aImapSettings);
	static CImapOpSyncFolderTree* NewLC(CMsvServerEntry& aEntry, CImapSession& aSession,CImapSyncManager& aSyncMan, CImapSettings& aImapSettings);
	void SynchroniseTreeL(TRequestStatus& aStatus);
	~CImapOpSyncFolderTree();	
	
private:
	CImapOpSyncFolderTree(CMsvServerEntry& aEntry, CImapSession& aSession, CImapSyncManager& aSyncMan, CImapSettings& aImapSettings);
	
	void ConstructL();
	void DoRunL();
	void DoCancel();
	void DoComplete(TInt& aStatus);

	void StartBuildingLocalFolderListL();
	void ProcessNextFolderInLocalFolderListL();
	void SendListRequestL();
	void StartProcessingReceivedFolderListL();
	void ProcessNextFolderInReceivedFolderListL();
	void AddReceivedFolderL(CImapListFolderInfo* aCurrentFolderInfo, const TDesC& aCurrentFolderFullPath);
	void RemoveMissingFolders();

private:
	enum TState
		{
		EStateIdle,
		EStateBuildingLocalFolderList,
		EStateFetchingFolderList,
		EStateProcessingReceivedFolders
		};

	TState iState;
	RArrayImapListFolderInfo iImapListFolderInfo;
	RPointerArray<CImapFolder> iFolderList;
	TInt iCurrentFolder;	
	CMsvEntrySelection* iSelection;
	TPtrC iCurrentPath;
	CImapFolder* iSearchFolder;
	HBufC* iInboxPathWithSeparator;
	HBufC* iFolderPathWithSeparator;
	TPtrC iFolderPath;
	};

#endif // __IMAPOPSYNCFOLDERTREE_H__
