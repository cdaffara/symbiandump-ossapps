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

#ifndef __CIMAPCOMPOUNDRENAME_H__
#define __CIMAPCOMPOUNDRENAME_H__

#include "cimapcompoundbase.h"

/**
Compound operation object for renaming a specified mailbox.
This class calls the sequence of IMAP Session calls necessary to
rename a mailbox and also updates local mirror folders as necessary.
Note that special behaviour is implemented when renaming the INBOX,
as per RFC 3501.

@internalTechnology
@prototype
*/
class CImapCompoundRename : public CImapCompoundBase
	{
public:
	~CImapCompoundRename( );
	
	static CImapCompoundRename* NewL( CImapSyncManager& aSyncManager,
						 			  CMsvServerEntry& aServerEntry,
						 			  CImapSettings& aImapSettings,
						 			  const TMsvId aTarget,
						 			  const TDesC& aNewName  );
	
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress( TImap4CompoundProgress& aCompoundProgress );
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	
private:
	CImapCompoundRename( CImapSyncManager& aSyncManager,
						 CMsvServerEntry& aServerEntry,
						 CImapSettings& aImapSettings,
						 const TMsvId aTarget );
	void ConstructL(const TDesC& aNewName);
	
	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();

	void IssueRenameL();
	void RenameInboxL();
	
private:
	TMsvId    iFolderId;
	TMsvId    iNewFolderId;
	HBufC16*  iNewLeafName;
	CMsvEntrySelection* iMsgselection;
	};

#endif	// __CIMAPCOMPOUNDRENAME_H__
