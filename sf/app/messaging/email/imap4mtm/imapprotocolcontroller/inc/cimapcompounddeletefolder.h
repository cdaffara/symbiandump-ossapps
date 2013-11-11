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
// CImapCompoundDeleteFolderFolder.h
// 
//

#ifndef __CIMAPCOMPOUNDDELETEFOLDER_H__
#define __CIMAPCOMPOUNDDELETEFOLDER_H__

#include "cimapcompoundbase.h"
#include "tmessageflaginfo.h"

// Forward declarations
class CImapFolder;

/**
Compound operation class performs the sequence of IMAP Session calls necessary
to delete a remote folder (mailbox). This requires the contents of the mailbox
to be deleted before issuing the DELETE command. Also calls CImapSyncManager
methods to remove the folder from the local mail store as appropriate.

Stop For Migrate Policy:
	If multiple folders selected for delete, will complete the current folder

@internalTechnology
@prototype
*/
class CImapCompoundDeleteFolder : public CImapCompoundBase
	{
public:
	~CImapCompoundDeleteFolder();
	
	static CImapCompoundDeleteFolder* NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, const CMsvEntrySelection& aSourceSel);
	
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	
private:
	CImapCompoundDeleteFolder(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings);
	void ConstructL(const CMsvEntrySelection& aSourceSel);

	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();

	void SelectSourceMailboxL();
	void DeleteAllMessagesL();
	void DeleteLocalFolderL();
	
	void IncrementProgress();

private:
	CMsvEntrySelection* iDeleteSel;

	// no ownership of iFolder - this is a pointer to an 
	// CImapFolder object owned by the CImapSyncManager
	CImapFolder* iFolder;

	// number of folders to delete
	TInt iFoldersToDelete;
	TInt iFoldersDone;
	
	// set true if failed to delete remote folder
	TBool iRemoteFolderNotDeleted;
	
	
	RArrayMessageFlagInfo iOutMessageFlagInfo;
	};

#endif	// __CIMAPCOMPOUNDDELETEFOLDER_H__
