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

#ifndef __CIMAPCOMPOUNDSYNCFOLDER_H__
#define __CIMAPCOMPOUNDSYNCFOLDER_H__

#include "cimapcompoundbase.h"

// Forward declarations
class CImapFolder;
class CImapSession;
class CImapMailStore;

/**
Compound operation object for synchronising a folder.
What is synchronised is controlled at contruction, identifying the folder
that is to be synchronised and whether only new or all messages should
be synchronised.

@internalTechnology
@prototype
*/
class CImapCompoundSyncFolder : public CImapCompoundBase
	{
public:
	~CImapCompoundSyncFolder();
	
	static CImapCompoundSyncFolder* NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aNewOnly, TMsvId aFolderId = KMsvNullIndexEntryId);

	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void StopForMigrate();

private:
	CImapCompoundSyncFolder(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore, TBool aNewOnly, TMsvId aFolderId);
	void ConstructL();

	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();
	void DoComplete(TInt& aErr);

private:
	// reference to the imap mail store.
	CImapMailStore& iImapMailStore;
	
	// Synchronise newly received mail only
	TBool iNewOnly;
		
	// ID of folder to synchronise.
	TMsvId iFolderId;

	// Compound operation, used for second phase synchronisation.
	CImapCompoundBase* iImapCompound;

	// Folder to synchronise. No ownership.
	CImapFolder* iSyncFolder;
	};

#endif	// __CIMAPCOMPOUNDSYNCFOLDER_H__
