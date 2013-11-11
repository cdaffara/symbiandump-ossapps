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
// popservermtm module header
// 
//

/**
 @file
 @internalComponent
 @released
*/

#if !defined(__POPSMTM_H__)
#define __POPSMTM_H__

#include <e32base.h>

#include <mtsr.h>
#include <offop.h>
#include <pop3set.h>

#include <msvstd.h>
#include <pops.h>
#include "mimmobileserver.h"
#include <tnonoperationmtmdata.h>

// Forward declarations
class CImPop3RefreshMailBox;
class CImPop3CopyMove;
class CImPop3Delete;
class CImPop3TopPopulate;
class CPopSessionManager;
class CImMobilityManager;

#if (defined SYMBIAN_USER_PROMPT_SERVICE) 
class CPopUpsResponseWaiter;
#endif

//
// CImppServerMtm
//
// 
// POP3 server MTM
// package buffer for progress info.
typedef TPckgBuf<TPop3Progress> TPop3ProgressBuf;

_LIT(KImPopsMTMResourcePath,"c:\\system\\data\\imcv.r01");

//
// Forward declarations
class CImPop3Settings;
class CImOffLineOperation;
class CImPop3OfflineOperationFinder;
class CImPop3SetOfflineOps;
class CImLogMessage;
//
//

//
// CImppServerMtm
//
class CImppServerMtm : public CBaseServerMtm, public MImMobileServer
{
public:
	enum TPopOpStopType
		{
		EPopStopOpBearerLost,
		EPopStopOpImmediate,
		EPopStopOpNormal
		};
		
	IMPORT_C static CImppServerMtm* NewL(CRegisteredMtmDll& aPopServerMtmDll,
												  CMsvServerEntry* aEntry);
	// mobility API from MImMobileServer
	void PrepareForNewCarrier(TImMobilityAction aAction, TBool aIsSeamless);
	void CarrierLost();
	void NewCarrierActive(TAccessPointInfo aNewAp, TBool aIsSeamless);
	void MobilityError(TUint aError);
	virtual const TDesC8& MobilityProgress();

	~CImppServerMtm();
	//
	void CopyFromLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus);
	void CopyToLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus);
	void CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	void CreateL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	void ChangeL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	//
	void StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus);
	//
	const TDesC8& Progress();
	
	TBool CommandExpected();
	//
	void MoveToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);

protected:
	TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);
	
private:
	enum TImppCopyMethod
		{
		EImppCopy,
		EImppMove,
		EImppPopulate
		};
	
	CImppServerMtm(CRegisteredMtmDll& aPopServerMtmDll, CMsvServerEntry* aEntry);
	void ConstructL();

	void DoCancel();
	void DoRunL();
	void DoComplete(TInt aError);
	
	void DoCancelCurrentOp();

	void CheckMailboxStateL();
	void DoConnectL(TRequestStatus& aStatus, CMsvEntrySelection& aSelection);
	void DoRefreshL();
	void DoCopyMoveL(const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus, TImppCopyMethod aCopyMethod);
	void DoQuitL(TRequestStatus& aStatus);
	void DoTopPopulateL(const CMsvEntrySelection& aSelection, TInt aLimit, TRequestStatus& aStatus);

	void GetPopDetailsL(const CMsvEntrySelection& aSel);
	void QueueRemoteCleanup();  // tidy up if anything goes wrong with refresh

	void ResetProgress();

    //  Called from DoRun() sets TMsvEntry visible flag for each POP3 child. 
    void DoShowMessagesL( TBool aHide);
	void CommandComplete( TInt aErrorCode );
	CMsvEntrySelection* StripInvalidEntriesLC(const CMsvEntrySelection& aSelection,TBool aExcludePartial=EFalse) const;
	void FindFirstOfflineOperationL(TBool aQuitting);
	inline void RunOfflineOperationL();
	inline TBool AcceptingOfflineOperationsL(const CMsvEntrySelection& aSelection);
	void AddOfflineOperationL(const CMsvEntrySelection& aSelection, TMsvId aDestination, CImOffLineOperation::TOffLineOpType aOperationType, TRequestStatus& aStatus);
	void CancelOfflineOperationsL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);

	TBool PruneMessages(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	void GetSystemProgress(TMsvSystemProgress& aSysProg);
	void GetProgress();

	TBool StartBearerMobilityL(TInt aError);
	void CancelToMigrate();
	void CompleteCurrentOpForMigration();
	void StopCurrentOpForMigration();
	TBool DoMigrationRunL();
	void DisconnectForMigrate();
	void RestartAfterMigrateL();
	void StoreConfigurationToMigrateL();
	TInt GetNonOperationMtmData(TNonOperationMtmDataType aMtmDataType, TPtrC8& aMtmDataBuffer);

private:

	enum TPopsMtmState
		{
		EPopConnecting,
		EPopRefreshing,
		EPopCopying,
		EPopMoving,
		EPopDeleting,
		EPopQuitting,
		EPopTidying,
		EPopConnectedAndIdle,
		EPopDisconnected,
		EPopPopulating,
		EPopAddingOfflineOp,
		EPopFindingFirstOfflineOp,
		EPopCancellingOfflineOps,
		EPopTopPopulating,
		EPopAuthoriseAndConnect
		};

	enum TPopMigrateState
		{
		ENotMigrating,
		EWaitingForOpToStop,
		EWaitingForOpToComplete,
		EDisconnectingForMigrate,
		EWaitingForNewCarrier,
		EWaitingCarrierRejected,
		EConnectingAfterMigrate
		};

	struct TImPop3Status
		{
		TBool			iQuitting;
		TBool			iRunningOfflineOperations;
		TPopsMtmState	iCurrentOperation;
		TPopsMtmState	iLastCurrentOperation;
		};
		
	struct TPopSavedValuesForMigration
		{
		TInt32*	iMessageArray;
		TInt	iNumMessages;
		TBool	iValuesSaved;
		};

	TImPop3Status				iState;
	TPopMigrateState			iMigrationState;
	TBool						iCancelForBMMigration;
	TPopSavedValuesForMigration iSavedValuesForMigration;

	CImPop3Session*				iPopSession;
	CImPop3RefreshMailBox*		iPopRefreshMailbox;
	CImPop3CopyMove*			iPopCopyMove;
	CImPop3Delete*				iPopDelete;
	CImPop3TopPopulate*			iPopTopPop;

	CArrayFixFlat<TMsvId>*		iMsvIdArray;		// stores any failed refresh msgs
	TInt						iArrayCtr;
	TRequestStatus*				iReportStatus;
	// Pop3 settings
	CImPop3Settings*			iPopSettings;
	CImIAPPreferences*			iIAPPreferences;

	TPop3Progress				iPopProgress;
	TPop3ProgressBuf			iPopProgressBuf;

	TBool						iConnectedToPopMbox;
	TBool						iOperationActive;

	TMsvId						iServiceId;
	TUint						iPort;
	TInt						iDoCallShowMessages;

	TInt							iCurrentOfflineOp;
	CImPop3OfflineOperationFinder*	iOfflineOpFinder;
	CImPop3SetOfflineOps*			iOfflineOpSetter;
	CImLogMessage*					iLogMessage;

	CMsvEntrySelection*			iMessagesToKeep;
	TBool						iNotConnectToPopMailBox ;
#if (defined SYMBIAN_USER_PROMPT_SERVICE) 
	/** For interaction with UPS server */
	TThreadId 					iClientThreadId;
	TBool 						iHasCapability;
	CPopUpsResponseWaiter*		iWaiter;
#endif

	CImMobilityManager*			iMobilityManager;

	CPopSessionManager* 		iSessionManager;

	TNonOperationMtmDataAccessPointIdBuffer iMtmDataAccessPointIdBuffer;

	};

#endif
