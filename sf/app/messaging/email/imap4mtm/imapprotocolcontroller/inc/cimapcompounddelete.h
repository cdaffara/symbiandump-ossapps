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

#ifndef __CIMAPCOMPOUNDDELETE_H__
#define __CIMAPCOMPOUNDDELETE_H__

#include "cimapcompoundbase.h"
#include "tmessageflaginfo.h"
#include <imapset.h>

// forward declarations
class CImapFolder;

/**
Compound operation class, performs necessary sequence of commands to delete
a message from a remote mailbox. This may involve SELECTing the appropriate
mailbox prior to issuing the STORE command to mark the message as \deleted.
Note that this sequence of events calls EXPUNGE - the message is deleted
immediately and, once completed, cannot be undeleted.

Stop For Migrate Policy:
	If messages are from multiple folders, the current folder shall be
	completed and then the migration shall be allowed to occur. Remaining
	messages shall be be deleted after the migration has completed.


@internalTechnology
@prototype
*/
class CImapCompoundDelete : public CImapCompoundBase
	{
public:
	~CImapCompoundDelete();
	
	static CImapCompoundDelete* NewL(CImapSyncManager& aSyncManager,
						 			 CMsvServerEntry& aServerEntry,
						 			 CImapSettings& aImapSettings,
						 			 const CMsvEntrySelection& aSourceSel);
	
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	

private:
	CImapCompoundDelete(CImapSyncManager& aSyncManager,
						CMsvServerEntry& aServerEntry,
						CImapSettings& aImapSettings);
	void ConstructL(const CMsvEntrySelection& aSourceSel);
	
	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();

	void DeleteLocalMessagesL();
	
	
private:
	TBool        iParentFolderId;
	CImapFolder* iParentFolder;		// no ownership
	
	CMsvEntrySelection* iSourceSel;
	RArray<TUint> iMessageUids;
	RArrayMessageFlagInfo iFlagInfoResponseArray;
	
	// count of messages marked for delete
	TInt iSelectionToDelete;

	// progress
	TInt iProgressMsgsToDo;
	TInt iProgressMsgsDone;
	};

#endif	// __CIMAPCOMPOUNDDELETE_H__

