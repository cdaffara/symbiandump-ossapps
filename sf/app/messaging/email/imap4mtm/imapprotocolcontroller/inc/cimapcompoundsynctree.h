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

#ifndef __CIMAPCOMPOUNDSYNCTREE_H__
#define __CIMAPCOMPOUNDSYNCTREE_H__

#include "cimapcompoundbase.h"

// Forward declarations
class CImapFolder;
class CImapSession;

/**
Compound operation object for performing the necessary sequence of commands
to synchronise the local mirror of the remote mailbox tree.

@internalTechnology
@prototype
*/
class CImapCompoundSyncTree : public CImapCompoundBase
	{
public:
	~CImapCompoundSyncTree();
	
	static CImapCompoundSyncTree* NewL(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings );

	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);

private:
	CImapCompoundSyncTree(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings );
	void ConstructL();

	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();
private:
	};

#endif	// __CIMAPCOMPOUNDSYNCTREE_H__
