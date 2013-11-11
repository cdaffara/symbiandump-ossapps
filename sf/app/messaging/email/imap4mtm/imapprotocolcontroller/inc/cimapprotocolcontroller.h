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

#ifndef __CIMAPPROTOCOLCONTROLLER_H__
#define __CIMAPPROTOCOLCONTROLLER_H__

#include <e32std.h>
#include <imapset.h>
#include <msvstd.h>

#include "cimapidlecontroller.h"
#include "mimapopbackgroundsyncobserver.h"
#include "mimmobileserver.h"
#include "cimmobilitymanager.h"

/**
package buffer for progress info.
@internalComponent
@prototype
*/
typedef TPckgBuf<TImap4CompoundProgress> TImap4ProgressBuf;

// Forward Declarations
class CImapCompoundBase;
class CImapSessionManager;
class CImapSession;
class CImapSettings;
class CImapMailStore;
class CImapSyncManager;
class CMsgActive;
class TImImap4GetPartialMailInfo;
class CImapOpBackgroundSync;
class CImapOfflineControl;

/**
Provides an API for connecting/disconnecting to a remote IMAP server and for 
connected IMAP operations.
Manages background online activities: synchronisation after connection
and the usage of IMAP idle between requested operations.
Uses instance of class CImapSyncManager to manage synchronisation of folders 
on the remote IMAP server and the headers contained within subscribed folders.
Uses instances of CImapCompoundBase derived classes to perform requested
operations.
@internalTechnology
@prototype
*/
class CImapProtocolController : public CMsgActive, 
                                public MImapOpBackgroundSyncObserver, 
                                public MImapIdleControllerObserver,
                                public MImMobileServer
	{
public:

	typedef RPointerArray<CImapSession> RImapSessionArray;

	virtual ~CImapProtocolController();
	IMPORT_C static CImapProtocolController* NewL(CMsvServerEntry& aEntry,CImapOfflineControl& aImapOfflineControl);

	// Connection APIs
	IMPORT_C TBool Connected() const;
	IMPORT_C void ConnectL( TRequestStatus& aStatus, 
							CMsvEntrySelection& aSelection );
	IMPORT_C void ConnectAndSynchroniseL( TRequestStatus& aStatus, 
										  CMsvEntrySelection& aSelection );
	
	IMPORT_C void DisconnectL(TRequestStatus& aStatus);
	IMPORT_C TBool Busy() const;
	IMPORT_C TInt GetAccessPointIdForConnection(TUint32& aAccessPointId) const;
	
	// Account Synchronisation APIs
	IMPORT_C TBool BackgroundSyncInProgress() const;
	IMPORT_C void CancelBackgroundSync(TRequestStatus& aStatus);
	IMPORT_C void WaitForBackground(TRequestStatus& aStatus);
	IMPORT_C void SynchroniseTreeL(TRequestStatus& aStatus);
	IMPORT_C void SynchroniseAllL(TRequestStatus& aStatus);
	
	// from MImapOpBackgroundSyncObserver
	void BackgroundSyncComplete(TInt aError);
	
	// from MImapIdleControllerObserver
	void OnIdleError(TInt aError);
	
	// Message/Folder APIs
	IMPORT_C void CopyToLocalL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination);				   
	IMPORT_C void PopulateL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, TImImap4GetPartialMailInfo aGetPartialMailInfo);
	IMPORT_C void MoveToLocalL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination);
	IMPORT_C void CopyWithinServiceL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination);
	IMPORT_C void MoveWithinServiceL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination);
	IMPORT_C void CopyFromLocalL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination);
	IMPORT_C void MoveFromLocalL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel, const TMsvId aDestination);
	IMPORT_C void DeleteL(TRequestStatus& aStatus, const CMsvEntrySelection& aSourceSel);
	IMPORT_C void DeleteFolderL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
	IMPORT_C void NewOnlySyncFolderL(TRequestStatus& aStatus);
	IMPORT_C void FullSyncFolderL(TRequestStatus& aStatus, const TMsvId aFolder);
	IMPORT_C void SelectL(TRequestStatus& aStatus, const TMsvId aFolder);
	IMPORT_C void FullSyncSelectedFolderL(TRequestStatus& aStatus);
	IMPORT_C void CreateL(TRequestStatus& aStatus, const TMsvId aParent, const TDesC& aLeafName, const TBool aFolder );
	IMPORT_C void RenameL(TRequestStatus& aStatus, const TMsvId aTarget, const TDesC& aNewName);

	IMPORT_C TInt SetLocalSubscription(const TMsvId aFolder, TBool aSubscribed);
										
	// Progress and Cancel APIs
	IMPORT_C TImap4CompoundProgress Progress();
	IMPORT_C void CancelAndCleanup();
	IMPORT_C void UpdateFlagL(TRequestStatus& aStatus);
	IMPORT_C void UpdateEntriesFlagL(TRequestStatus& aStatus,const CMsvEntrySelection& aSourceSel, TBool aFlagChanged);

	// mobility API from MImMobileServer
	void PrepareForNewCarrier(TImMobilityAction aAction, TBool aIsSeamless);
	void CarrierLost();
	void NewCarrierActive(TAccessPointInfo aNewAp, TBool aIsSeamless);
	void MobilityError(TUint aError);
	const TDesC8& MobilityProgress(); 


private:
	CImapProtocolController(CMsvServerEntry& aEntry,CImapOfflineControl& aImapOfflineControl);
	
	void ConstructL();
	// from CMsgActive
	virtual void DoRunL();
	void DoComplete(TInt& aErr);
	virtual void DoCancel();
	void DoCancelClientOp();

	void DoDisconnectL(TRequestStatus& aStatus);
	void DropSession(TInt aSessionId);
	void DisconnectAll();
	TBool CompleteIfBackgroundOpInProgress(TRequestStatus& aStatus);
	TBool SelectSessionL(TInt& aIndexToUse);
	void StartPrimaryOperation();
	void StartIdle();

	void MarkOnOrOfflineL(const TBool aOnline);
	void ChangeVisibilityL(TMsvId aParent, TBool aInvisible);
	void ChangeVisibilityL(TMsvId aParent, TBool aInvisible, TBool aRecurse, TUid aType);
	void PropagateInvisibleFlagL(TMsvId aId);
	void PropagateUnsubscribeL(const TMsvId aFolder);
	
	TBool IdIsLocalL(TMsvId aId);
	TMsvId ServiceOfL(TMsvId aId);
	
	TBool ProcessError(TInt aError);
	void ThranslateSessionError(TInt& errCode);
	void ResetProgress();
	
	// Migration methods
	void DoMigrateRunL();
	void CompleteCurrentForMigrate();
	void StopCurrentForMigrate();
	void CancelForMigrate();
	void DoCancelForMigrate();
	void ProcessOpCompleteForMigrate();
	void DisconnectForMigrateL();
	void NewPrimarySessionL();
	void RestartAfterMigrateL();
	
private:

	enum TImapProtocolOp
		{
	/** Idle. The default connected state. */
		EIdle,
	/** Connect operation. */
		EConnect,
	/** Disconnect operation. */
		EDisconnect, 
	/** Sync operation. */
		ESync,
	/** Select operation. */
		ESelect,		
	/** Copy messages to local operation. */
		ECopyToLocal,
	/** Copy messages within remote server operation. */
		ECopyWithinService,
	/** Copy messages from local operation. */
		ECopyFromLocal,
	/** Move messages to local operation. */
		EMoveToLocal,
	/** Move messages within remote server operation. */
		EMoveWithinService,
	/** Move messages from local operation. */
		EMoveFromLocal,
	/** Message population operation. */
		EPopulate,		
	/** Delete operation. (messages) */
		EDelete,
	/** Delete operation. (folders) */
		EDeleteFolder,
	/** Create operation. (folders) */
		ECreate,
	/** Rename operation. (folders) */
		ERename,
	/** Stop idle operation. */
		EStopIdle,
	/** Connect a secondary session. */
		EConnectSecondary,
	/** Recovering primary session following a cancel. */
		ECancelRecoverPrimary,
	/** Update read/unread message flags */
		EUpdateFlag,
		};

	// protocol controller migration states
	enum TImapPcMigrateState
		{
		ENotMigrating,				// normal state
		ESuspendingForMigrate,      // intermediate state
		EWaitingForOpToComplete,	// async wait on compound op/bground op/idle controller
		EWaitingForOpToStop,		// async wait on compound op/bground op/idle controller
		EDisconnectingForMigrate,	// async wait on compound op
		EWaitingForNewCarrier,		// async wait state on mobility engine
		EStartingReconnect,			// intermediate state
		EConnectingAfterMigrate,	// async wait on session manager
		EWaitingInitialCarrierRejected,		// async wait state on mobility engine
		EHandlingConnectError 		// Handling Initial connect error
		};

	CMsvServerEntry& iEntry;
	
	// objects the Protocol Controller owns
	CImapSettings*			iImapSettings;
	CImapMailStore* 		iImapMailStore;
	CImapSessionManager* 	iImapSessionManager;
	RImapSessionArray		iImapSessionArray;
	CImapCompoundBase* 		iImapCompound;
	CImapCompoundBase* 		iMigrateCompound;
	
	// CImapOfflineControl object is owned by the CImap4ServerMtm
	CImapOfflineControl& 	iImapOfflineControl;
	CImapSyncManager* 		iImapSyncManager;
	CImapOpBackgroundSync* 	iBackgroundSyncOp;
	CImapIdleController*    iImapIdleController;
	CImMobilityManager*		iMobilityManager;
	
	// Service ID
	TMsvId					iServiceId;
	
	// Progress
	TImap4CompoundProgress 	iProgress;

	// Current operational state
	// Requested and current foreground operation
	TImapProtocolOp			iRequestedOp;
	TImapProtocolOp			iCurrentOp;
	TInt 					iForegroundSession;
	
	TBool					iWaitForBackgroundSync;
	TBool					iFlushPrimary;
	TBool 					iCancelInProgress;
	TImapPcMigrateState		iMigrateState;
	TImap4ProgressBuf 		iProgressBuffer;
	};
							 
#endif	//__CIMAPPROTOCOLCONTROLLER_H__
