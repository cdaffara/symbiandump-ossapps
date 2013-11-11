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

#ifndef __CIMAPCOMPOUNDDISCONNECT_H__
#define __CIMAPCOMPOUNDDISCONNECT_H__

#include "cimapcompoundbase.h"

// forward declaration
class CImapSessionManager;
class CImapCompoundSyncService;
class CImapMailStore;
class CImapOfflineControl;

/**
Compound operation object for disconnecting all currently connected
sessions.

@internalTechnology
@prototype
*/
class CImapCompoundDisconnect : public CImapCompoundBase
	{
			
public:
	~CImapCompoundDisconnect();
	
	static CImapCompoundDisconnect* NewL( CImapSyncManager& aSyncManager,
							 			  CMsvServerEntry& aServerEntry,
							 			  CImapSettings& aImapSettings,
							 			  CImapSessionManager& aSessionManager,
							 			  CImapMailStore& aImapMailStore,
							 			  const RPointerArray<CImapSession>& aSessionList,
							 			  CImapOfflineControl& aImapOfflineControl,
							 			  TBool aDoDeletes );

	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession);
	virtual void Progress( TImap4CompoundProgress& aCompoundProgress );
	
	// Support for Bearer Mobility
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession);
	
private:
	CImapCompoundDisconnect( CImapSyncManager& aSyncManager, 
							 CMsvServerEntry& aServerEntry, 
							 CImapSettings& aImapSettings,
							 CImapSessionManager& aSessionManager,
							 CImapMailStore& aImapMailStore,
							 const RPointerArray<CImapSession>& aSessionList,
							 CImapOfflineControl& aImapOfflineControl,
							 TBool aDoDeletes );
							 
	void ConstructL();

	TBool DoRunLoopL();
	void DoCancel();
					 
	virtual TInt ProcessNegativeServerResponse();	

private:
	CImapSessionManager& iSessionManager;
	const RPointerArray<CImapSession>& iSessionList;
	CImapCompoundSyncService* iCompoundSync;
	CImapMailStore& iImapMailStore;
	CImapOfflineControl& iImapOfflineControl;
	TBool iDoDeletes;
	};

#endif // __CIMAPCOMPOUNDDISCONNECT_H__
