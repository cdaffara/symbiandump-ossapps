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

#ifndef __CIMAPOPBACKGROUNDSYNC_H__
#define __CIMAPOPBACKGROUNDSYNC_H__

class CActive;
class CImapFolder;
class CImapSyncManager;
class CImapSession;
class MImapOpBackgroundSyncObserver;
class CImapSettings;
class CImapCompoundSyncService;
class CMsvServerEntry;
class TImap4CompoundProgress;
class CImapMailStore;
class CImapOfflineControl;
class CMsvEntrySelection;

/**
The CImapOpBackgroundSync class encapsulates the management of performing
a background synchronisation operation. It utilises CImapCompoundBase
derived operation classes to perform the necessary asynchronous operations
and reports completion or errors via the MImapOpBackgroundSyncObserver
API, a reference to an instance of which is passed on construction.

@internalTechnology
@prototype
*/
class CImapOpBackgroundSync : public CActive
	{
private:
	enum TBackgroundSyncState 
		{
		ENotInUse,
		EStartingSync,
		ESynchronising,
		ECancelRecoveringSession,
		ECancelRefreshingSyncManager,
		EFinished
		};

public:
	virtual ~CImapOpBackgroundSync( );
	
	static CImapOpBackgroundSync* NewL( MImapOpBackgroundSyncObserver& aBackgroundSyncObserver,
										CMsvServerEntry& aServerEntry,
										CImapSyncManager& aSyncMan,
										CImapSettings& aImapSettings,
										CImapMailStore& aImapMailStore,
										CImapOfflineControl& aImapOfflineControl );

	void StartSync(CImapSession& aSession);
	void CancelAndCleanup();
	
	void Progress(TImap4CompoundProgress& aCompoundProgress);
	
	// Support for Bearer Mobility
	void  CancelForMigrate();
	void  StopForMigrate();
	void  ResumeOperationL(CImapSession& aSession);
	TBool IsSuspendedForMigrate();
	
	// Support for sync using download rules.
	void RemoveFromSelectionL(CMsvEntrySelection& aDeleteSel);

private:
	CImapOpBackgroundSync( MImapOpBackgroundSyncObserver& aBackgroundSyncObserver, 
						   CMsvServerEntry& aServerEntry,
						   CImapSyncManager& aSyncMan, 
						   CImapSettings& aImapSettings,
						   CImapMailStore& aImapMailStore,
						   CImapOfflineControl& aImapOfflineControl );
	void ConstructL();
	
	// from CActive
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();

private:
	
	// background sync-op migration states:
	enum TImapBgMigrateState
		{
		ENotMigrating,				// normal state
		ECancellingForMigrate,      // intermediate state
		EStoppingForMigrate,		// stop requested...
		ESuspendedForMigrate        // waiting to be restarted
		};
		
	MImapOpBackgroundSyncObserver& iBackgroundSyncObserver;
	CImapSession* iSession;
	CMsvServerEntry& iEntry;
	CImapSyncManager& iSyncMan;
	CImapSettings& iImapSettings;
	CImapMailStore& iImapMailStore;
	CImapOfflineControl& iImapOfflineControl; 
	CImapCompoundSyncService* iCompoundSync;

	TBackgroundSyncState iState;
	TImapBgMigrateState  iMigrateState;
	};

#endif	//__CIMAPOPBACKGROUNDSYNC_H__
