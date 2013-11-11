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

#ifndef __CIMAPCOMPOUNDSELECT_H__
#define __CIMAPCOMPOUNDSELECT_H__

#include "cimapcompoundbase.h"
#include <msvstd.h>

// Forward declarations
class CImapFolder;

/**
@internalTechnology
@prototype
*/
class CImapCompoundSelect : public CImapCompoundBase
	{
public:
	~CImapCompoundSelect();
	
	static CImapCompoundSelect* NewL(CImapSyncManager& aSyncManager,
						 			 CMsvServerEntry& aServerEntry,
						 			 CImapSettings& aImapSettings,
						 			 TMsvId aFolderId);
	
	void StartSelectL(TRequestStatus& aStatus);
	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	
private:
	CImapCompoundSelect(CImapSyncManager& aSyncManager,
						CMsvServerEntry& aServerEntry,
						CImapSettings& aImapSettings,
						TMsvId aFolderId);
	void ConstructL();

	TBool DoRunLoopL();
	void DoCancel();

	virtual TInt ProcessNegativeServerResponse();

private:
	TMsvId 			iFolderId;
	CImapFolder* 	iImapFolder;	// not owned!!

	};

#endif	// __CIMAPCOMPOUNDSELECT_H__
