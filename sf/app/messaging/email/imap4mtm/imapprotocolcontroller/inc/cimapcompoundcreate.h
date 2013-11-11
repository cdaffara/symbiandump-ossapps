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

#ifndef __CIMAPCOMPOUNDCREATE_H__
#define __CIMAPCOMPOUNDCREATE_H__

#include "cimapcompoundbase.h"
#include "cimaplistfolderinfo.h"

/**
Encapsulates the creating of a remote mailbox and the local mirror folder.

Stop For Migrate Policy:
	The operation shall be allowed to complete.

@internalTechnology
@prototype
*/
class CImapCompoundCreate : public CImapCompoundBase
	{
public:
	~CImapCompoundCreate();
	
	static CImapCompoundCreate* NewL( CImapSyncManager& aSyncManager,
						 			  CMsvServerEntry& aServerEntry,
						 			  CImapSettings& aImapSettings,
						 			  const TMsvId aParent,
						 			  const TDesC& aLeafName,
						 			  const TBool aIsFolder);
	
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	
private:
	CImapCompoundCreate( CImapSyncManager& aSyncManager,
						 CMsvServerEntry& aServerEntry,
						 CImapSettings& aImapSettings,
						 const TMsvId aParent,
						 const TBool aIsFolder);
	void ConstructL(const TDesC& aLeafName);

	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();
						 
	HBufC* MakeFolderNameL(const TMsvId aParent, const TDesC& aLeafName, const TBool aIsFolder);
	void CreateLocalFolderL();

private:
	TMsvId 		iParentId;
	HBufC*		iLeafName;
	TBool		iIsFolder;

	// Stores result of calling list command 
	RArrayImapListFolderInfo iFolderList;
	
	// MsvId of newly created folder.
	TMsvId 		iNewFolderId;
	};

#endif	// __CIMAPCOMPOUNDCREATE_H__
